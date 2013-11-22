/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implementation of the builder of transaction input and reader of output
///\file transactionFunction.cpp
#include "types/allocators.hpp"
#include "utils/parseUtils.hpp"
#include "utils/printFormats.hpp"
#include "utils/conversions.hpp"
#include "database/transactionFunction.hpp"
#include "transactionFunctionDescription.hpp"
#include "transactionfunction/InputStructure.hpp"
#include "transactionfunction/ResultStructure.hpp"
#include "transactionfunction/ResultIterator.hpp"
#include "transactionfunction/ResultFilter.hpp"
#include "transactionfunction/DatabaseCommand.hpp"
#include "transactionfunction/PreProcessCommand.hpp"
#include "transactionfunction/Path.hpp"
#include "database/transaction.hpp"
#include "logger-v1.hpp"
#include <cstring>
#include <sstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

struct TransactionFunction::Impl
{
	typedef TransactionFunctionDescription::PrintStep PrintStep;

	ResultStructureR m_resultstruct;
	std::vector<DatabaseCommand> m_call;
	TagTable m_tagmap;
	std::vector<PreProcessCommand> m_preprocs;
	std::string m_resultfilter;

	Impl( const TransactionFunctionDescription& description, const LanguageDescription* langdescr, types::keymap<std::string>& embeddedStatementMap, const SubroutineDeclarationMap& functionmap);
	Impl( const Impl& o);

	void handlePrintStep( const PrintStep& printstep);

	std::string tostring( const utils::PrintFormat* pformat) const
	{
		std::ostringstream rt;
		std::vector<PreProcessCommand>::const_iterator pi = m_preprocs.begin(), pe = m_preprocs.end();
		for (; pi != pe; ++pi)
		{
			rt << "PRE " << pi->tostring( &m_tagmap) << pformat->newitem;
		}
		std::vector<DatabaseCommand>::const_iterator ci = m_call.begin(), ce = m_call.end();
		for (int idx=0; ci != ce; ++ci,++idx)
		{
			rt << "CMD " << idx << ":" << ci->tostring( &m_tagmap) << pformat->newitem;
		}
		rt << "RESULT " << m_resultstruct->tostring() << pformat->newitem;
		return rt.str();
	}
};


TransactionFunctionInput::TransactionFunctionInput( const TransactionFunction* func_)
	:types::TypeSignature("database::TransactionFunctionInput", __LINE__)
	,m_structure( new Structure( &func_->impl().m_tagmap))
	,m_func(func_)
	,m_lasttype( langbind::TypedInputFilter::Value){}

TransactionFunctionInput::~TransactionFunctionInput(){}

TransactionFunctionInput::TransactionFunctionInput( const TransactionFunctionInput& o)
	:types::TypeSignature("database::TransactionFunctionInput", __LINE__)
	,langbind::TypedOutputFilter(o)
	,m_structure( o.m_structure)
	,m_func(o.m_func)
	,m_lasttype(o.m_lasttype){}

bool TransactionFunctionInput::print( ElementType type, const types::VariantConst& element)
{
	LOG_DATA << "[transaction input] push element " << langbind::InputFilter::elementTypeName( type) << " '" << utils::getLogString( element) << "'' :" << element.typeName( element.type());
	switch (type)
	{
		case langbind::TypedInputFilter::OpenTag:
			m_structure->openTag( element);
		break;
		case langbind::TypedInputFilter::CloseTag:
			m_structure->closeTag();
		break;
		case langbind::TypedInputFilter::Attribute:
			m_structure->openTag( element);
		break;
		case langbind::TypedInputFilter::Value:
			m_structure->pushValue( element);
			if (m_lasttype == langbind::TypedInputFilter::Attribute)
			{
				m_structure->closeTag();
			}
		break;
	}
	m_lasttype = type;
	return true;
}

void TransactionFunctionInput::finalize( const proc::ProcessorProvider* provider)
{
	std::vector<PreProcessCommand>::const_iterator pi = m_func->impl().m_preprocs.begin(), pe = m_func->impl().m_preprocs.end();
	for (; pi != pe; ++pi)
	{
		pi->call( provider, *m_structure);
	}
	LOG_DATA << "[transaction input] after preprocess " << m_structure->tostring();
}

static void bindArguments( TransactionInput& ti, const DatabaseCommand& call, const TransactionFunctionInput* inputst, const TransactionFunctionInput::Structure::NodeVisitor& selectornode, int loopcnt)
{
	typedef TransactionFunctionInput::Structure::Node Node;
	typedef TransactionFunctionInput::Structure::NodeVisitor NodeVisitor;

	std::vector<Path>::const_iterator pi=call.arg().begin(), pe=call.arg().end();
	for (std::size_t argidx=1; pi != pe; ++pi,++argidx)
	{
		switch (pi->referenceType())
		{
			case Path::ResultIndex:
				ti.bindCommandArgAsResultReference( pi->resultReferenceIndex(), pi->resultReferenceScope());
				break;
			case Path::ResultSymbol:
				ti.bindCommandArgAsResultReference( pi->resultReferenceSymbol(), pi->resultReferenceScope());
				break;
			case Path::Constant:
				ti.bindCommandArgAsValue( pi->constantReference());
				break;
			case Path::InternalVariable:
				switch (pi->internalVariableType())
				{
					case Path::ForeachLoopIndex:
						ti.bindCommandArgAsValue( loopcnt);
						break;
				}
				break;
			case Path::Find:
			case Path::Root:
			case Path::Next:
			case Path::Up:
			{
				std::vector<NodeVisitor::Index> param;
				pi->selectNodes( inputst->structure(), selectornode, param);
				if (param.size() == 0)
				{
					ti.bindCommandArgAsNull();
				}
				else
				{
					std::vector<NodeVisitor::Index>::const_iterator gs = param.begin(), gi = param.begin()+1, ge = param.end();
					for (; gi != ge; ++gi)
					{
						if (*gs != *gi) throw std::runtime_error( "more than one node selected in db call argument");
					}
					const types::Variant* valref = inputst->structure().contentvalue( inputst->structure().node( *gs));
					if (valref)
					{
						ti.bindCommandArgAsValue( *valref);
					}
					else
					{
						ti.bindCommandArgAsNull();
					}
				}
			}
		}
	}
}

static void getOperationInput( const TransactionFunctionInput* this_, TransactionInput& rt, std::size_t startfidx, std::size_t level, std::vector<DatabaseCommand>::const_iterator ci, std::vector<DatabaseCommand>::const_iterator ce, const std::vector<TransactionFunctionInput::Structure::NodeVisitor::Index>& rootnodearray)
{
	typedef TransactionFunctionInput::Structure::Node Node;
	typedef TransactionFunctionInput::Structure::NodeVisitor NodeVisitor;
	std::size_t fidx = startfidx;
	for (; ci != ce; ++ci,++fidx)
	{
		TransactionInput::cmd_iterator ti = rt.begin(), te = rt.end();
		for (; ti != te; ++ti)
		{
			if (ti->functionidx() == fidx)
			{
				if (ci->hasNonemptyResult())
				{
					ti->setNonemptyResult();
				}
				if (ci->hasUniqueResult())
				{
					ti->setUniqueResult();
				}
			}
		}

		// Select the nodes to execute the command with:
		std::vector<NodeVisitor::Index> nodearray;
		std::vector<NodeVisitor::Index>::const_iterator ni = rootnodearray.begin(), ne = rootnodearray.end();
		for (; ni != ne; ++ni)
		{
			ci->selector().selectNodes( this_->structure(), *ni, nodearray);
		}
		std::vector<DatabaseCommand>::const_iterator ca = ci;
		if (ci->level() > level)
		{
			// call SUBROUTINE: For each selected node execute all the database commands or SUBROUTINEs:
			if (!ci->name().empty()) throw std::logic_error("passing arguments expected when calling SUBROUTINE");
			std::size_t nextfidx = fidx;
			for (++ca; ca != ce; ++ca,++nextfidx)
			{
				if (ca->level() < level || (ca->level() == level && ca->name().empty())) break;
			}
			std::vector<NodeVisitor::Index>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
			for (int loopcnt=1; vi != ve; ++vi,++loopcnt)
			{
				rt.startCommand( fidx, ci->level(), ci->name(), ci->resultsetidx());
				bindArguments( rt, *ci, this_, *vi, loopcnt);
				std::vector<NodeVisitor::Index> opnodearray;
				opnodearray.push_back( *vi);
				getOperationInput( this_, rt, fidx+1, ci->level(), ci+1, ca, opnodearray);
			}
			ci = ca;
			ci--;
			fidx = nextfidx;
		}
		else
		{
			// Call DatabaseCommand: For each selected node do expand the function call arguments:
			std::vector<NodeVisitor::Index>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
			for (int loopcnt=1; vi != ve; ++vi,++loopcnt)
			{
				rt.startCommand( fidx, ci->level(), ci->name(), ci->resultsetidx());
				bindArguments( rt, *ci, this_, *vi, loopcnt);
			}
		}
	}
}

TransactionInput TransactionFunctionInput::get() const
{
	typedef TransactionFunctionInput::Structure::NodeVisitor NodeVisitor;
	TransactionInput rt;
	std::vector<DatabaseCommand>::const_iterator ci = m_func->impl().m_call.begin(), ce = m_func->impl().m_call.end();

	std::vector<NodeVisitor::Index> nodearray;
	nodearray.push_back( structure().rootindex());
	getOperationInput( this, rt, 0, 1, ci, ce, nodearray);
	return rt;
}


static int hasOpenSubstruct( const std::vector<TransactionFunctionDescription::Block>& blocks, std::size_t idx)
{
	std::size_t ii = 0;
	for (; ii<blocks.size(); ++ii) if (idx == blocks.at(ii).startidx) return ii;
	return -1;
}

static int hasCloseSubstruct( const std::vector<TransactionFunctionDescription::Block>& blocks, std::size_t idx)
{
	std::size_t ii = 0;
	for (; ii<blocks.size(); ++ii) if (idx == blocks.at(ii).startidx + blocks.at(ii).size && blocks.at(ii).size != 0) return ii;
	return -1;
}

void TransactionFunction::Impl::handlePrintStep( const PrintStep& printstep)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call::Param Param;

	std::vector<std::string>::const_iterator pi = printstep.path_INTO.begin(), pe = printstep.path_INTO.end();
	for (; pi != pe; ++pi)
	{
		m_resultstruct->addOpenTag( *pi);
	}
	switch (printstep.argument.type)
	{
		case Param::Constant:
			m_resultstruct->addConstant( printstep.argument.value);
			break;
		case Param::NumericResultReference:
			if (printstep.argument.namspace>=0)
			{
				m_resultstruct->addMark( ResultElement::SelectResultFunction, printstep.argument.namspace);
			}
			m_resultstruct->addMark( ResultElement::SelectResultColumn, utils::touint_cast( printstep.argument.value));
			break;
		case Param::SymbolicResultReference:
			if (printstep.argument.namspace>=0)
			{
				m_resultstruct->addMark( ResultElement::SelectResultFunction, printstep.argument.namspace);
			}
			m_resultstruct->addResultColumnName( printstep.argument.value);
			break;
		case Param::InternalVariable:
			throw std::runtime_error("internal variables are not allowed as argument of PRINT");
		case Param::InputSelectorPath:
			throw std::logic_error("internal: input reference is an unexpected argument of PRINT");
	}	
	pi = printstep.path_INTO.begin(), pe = printstep.path_INTO.end();
	for (; pi != pe; ++pi)
	{
		m_resultstruct->addCloseTag();
	}
}

TransactionFunction::Impl::Impl( const TransactionFunctionDescription& description, const LanguageDescription* langdescr, types::keymap<std::string>& embeddedStatementMap, const SubroutineDeclarationMap& functionmap)
	:m_resultstruct( new ResultStructure())
	,m_tagmap(description.casesensitive)
	,m_resultfilter(description.resultfilter)
{
	typedef TransactionFunctionDescription::PreProcessingStep PreProcessingStep;
	typedef TransactionFunctionDescription::PrintStep PrintStep;
	typedef TransactionFunctionDescription::MainProcessingStep MainProcessingStep;
	typedef MainProcessingStep::Call Call;

	// Compile the preprocessing commands:
	{
		std::vector<PreProcessingStep>::const_iterator pi = description.preprocs.begin(), pe = description.preprocs.end();
		for (; pi != pe; ++pi)
		{
			std::size_t eidx = pi - description.preprocs.begin();
			try
			{
				Path selector( pi->selector_FOREACH, &m_tagmap);
				std::vector<PreProcessCommand::Argument> args;
				std::vector<PreProcessingStep::Argument>::const_iterator ai = pi->args.begin(), ae = pi->args.end();
				for (; ai != ae; ++ai)
				{
					switch (ai->type)
					{
						case PreProcessingStep::Argument::Selector:
							args.push_back( PreProcessCommand::Argument( ai->name, Path( ai->value, &m_tagmap)));
							break;
						case PreProcessingStep::Argument::Constant:
							args.push_back( PreProcessCommand::Argument( ai->name, ConstantReferencePath( ai->value)));
							break;
					}
				}
				m_preprocs.push_back( PreProcessCommand( pi->functionname, selector, args, pi->path_INTO));
			}
			catch (const std::runtime_error& e)
			{
				throw PreProcessingStep::Error( eidx, e.what());
			}
		}
	}
	// Compile the database commands:
	int blkidx;
	std::map<int,int> result_reference_relocation_tab;
	std::vector<MainProcessingStep>::const_iterator di = description.steps.begin(), de = description.steps.end();
	for (; di != de; ++di)
	{
		std::size_t eidx = di - description.steps.begin();
		try
		{
			// Update address map for relocating result references
			result_reference_relocation_tab[ eidx] = m_call.size();

			// Create substructure context for RESULT INTO instructions:
			if ((blkidx=hasCloseSubstruct( description.blocks, eidx)) >= 0)
			{
				const std::vector<std::string>& ps = description.blocks.at(blkidx).path_INTO;
				std::vector<std::string>::const_iterator pi = ps.begin(), pe = ps.end();
				for (; pi != pe; ++pi)
				{
					m_resultstruct->addCloseTag();
				}
			}
			// Handle INTO .. BEGIN open structure:
			if ((blkidx=hasOpenSubstruct( description.blocks, eidx)) >= 0)
			{
				const std::vector<std::string>& ps = description.blocks.at(blkidx).path_INTO;
				std::vector<std::string>::const_iterator pi = ps.begin(), pe = ps.end();
				for (; pi != pe; ++pi)
				{
					m_resultstruct->addOpenTag( *pi);
				}
			}

			// Handle PRINT instructions:
			std::map<std::size_t,PrintStep>::const_iterator pp = description.printsteps.find( eidx);
			if (pp != description.printsteps.end())
			{
				handlePrintStep( pp->second);
			}

			// Build Function call object for parsed function:
			Path selector( di->selector_FOREACH, &m_tagmap);
			std::vector<Path> param;
			std::vector<Call::Param>::const_iterator ai = di->call.paramlist.begin(), ae = di->call.paramlist.end();
			for (; ai != ae; ++ai)
			{
				Path pa( *ai, &m_tagmap);
				param.push_back( pa);
			}


			SubroutineDeclarationMap::const_iterator fui = functionmap.find( di->call.funcname);
			if (fui == functionmap.end())
			{
				//... not a SUBROUTINE
				if (di->call.templatearg.size())
				{
					throw MainProcessingStep::Error( eidx, "unknown subroutine template called");
				}
				DatabaseCommand cc( di->call.funcname, selector, di->resultref_FOREACH, param, di->nonempty, di->unique, 1, di->hints);
				cc.rewriteResultReferences( result_reference_relocation_tab);

				if (di->path_INTO.empty())
				{
					m_resultstruct->addIgnoreResult( m_call.size());
				}
				else
				{
					// this is just the wrapping structure, iteration is only done with the last tag
					// (othwerwise we get into definition problems)
					std::string iteratingTag = di->path_INTO.back();
					bool hasOutput = (iteratingTag != ".");

					for (std::vector<std::string>::size_type i = 0; i < di->path_INTO.size()-1; i++)
					{
						m_resultstruct->addOpenTag( di->path_INTO[i] );
					}
					m_resultstruct->addMark( ResultElement::FunctionStart, m_call.size());
					if (hasOutput)
					{
						m_resultstruct->addOpenTag( iteratingTag);
					}
					if (!di->unique)
					{
						m_resultstruct->addMark( ResultElement::IndexStart, m_call.size());
					}
					m_resultstruct->addValueReference( m_call.size());

					if (!di->unique)
					{
						m_resultstruct->addMark( ResultElement::IndexEnd, m_call.size());
					}
					if (hasOutput)
					{
						m_resultstruct->addCloseTag();
					}
					m_resultstruct->addMark( ResultElement::FunctionEnd, m_call.size());
					for (std::vector<std::string>::size_type i = 0; i < di->path_INTO.size()-1; i++)
					{
						m_resultstruct->addCloseTag( );
					}
				}
				m_call.push_back( cc);
			}
			else
			{
				if (di->call.templatearg.size() != fui->second.templateArguments.size())
				{
					throw MainProcessingStep::Error( eidx, "Number of template arguments to subroutine call does not match declaration");
				}
				std::vector<LanguageDescription::TemplateArgumentAssignment> templateArgAssignments;
				std::vector<std::string>::const_iterator vi = di->call.templatearg.begin(), ve = di->call.templatearg.end();
				std::vector<std::string>::const_iterator ti = fui->second.templateArguments.begin(), te = fui->second.templateArguments.end();
				for (; vi != ve && ti != te; ++ti,++vi)
				{
					templateArgAssignments.push_back( LanguageDescription::TemplateArgumentAssignment( *ti, *vi));
				}

				Impl* func = fui->second.function->m_impl;
				std::map<int,int> rwtab = m_tagmap.insert( func->m_tagmap);
				if (!di->hints.empty())
				{
					throw MainProcessingStep::Error( eidx, "No ON ERROR hints supported for call of SUBROUTINE");
				}
				if (di->nonempty)
				{
					throw MainProcessingStep::Error( eidx, "NONEMTY not supported for call of SUBROUTINE");
				}
				std::string iteratingTag;
				bool hasOutput = false;
				if (!di->path_INTO.empty())
				{
					iteratingTag = di->path_INTO.back();
					hasOutput = (iteratingTag != ".");

					for (std::vector<std::string>::size_type i = 0; i < di->path_INTO.size()-1; i++)
					{
						m_resultstruct->addOpenTag( di->path_INTO[i] );
					}
				}
				m_resultstruct->addMark( ResultElement::OperationStart, m_call.size());
				if (hasOutput)
				{
					m_resultstruct->addOpenTag( iteratingTag);
				}
				bool hasIndex = (!di->unique && (di->selector_FOREACH.size() || di->resultref_FOREACH > 0));
				if (hasIndex)
				{
					m_resultstruct->addMark( ResultElement::IndexStart, m_call.size()+1);
				}
				m_resultstruct->addEmbeddedResult( *func->m_resultstruct, m_call.size()+1);
				if (hasIndex)
				{
					m_resultstruct->addMark( ResultElement::IndexEnd, m_call.size()+1);
				}
				if (hasOutput)
				{
					m_resultstruct->addCloseTag();
				}

				m_resultstruct->addMark( ResultElement::OperationEnd, m_call.size());
				if (!di->path_INTO.empty())
				{
					for (std::vector<std::string>::size_type i = 0; i < di->path_INTO.size()-1; i++)
					{
						m_resultstruct->addCloseTag();
					}
				}

				DatabaseCommand paramstk( "", selector, di->resultref_FOREACH, param, false, false, 1 + 1/*level*/);
				m_call.push_back( paramstk);
				int scope_functionidx_incr = m_call.size();

				std::vector<DatabaseCommand>::const_iterator fsi = func->m_call.begin(), fse = func->m_call.end();
				for (; fsi != fse; ++fsi)
				{
					Path fselector = fsi->selector();
					fselector.rewrite( rwtab, scope_functionidx_incr);
					int fresultsetidx = -1;
					if (fsi->resultsetidx() >= 0)
					{
						fresultsetidx = fsi->resultsetidx() + scope_functionidx_incr;
					}
					std::vector<Path> fparam = fsi->arg();
					std::vector<Path>::iterator fai = fparam.begin(), fae = fparam.end();
					for (; fai != fae; ++fai) fai->rewrite( rwtab, scope_functionidx_incr);
					bool templated = false;
					if (fui->second.templateArguments.size())
					{
						types::keymap<std::string>::const_iterator ei = embeddedStatementMap.find( fsi->name());
						if (ei != embeddedStatementMap.end())
						{
							std::string stm = langdescr->substituteTemplateArguments( ei->second, templateArgAssignments);
							if (stm != ei->second)
							{
								//... subroutine main step has template arguments substituted
								templated = true;
								std::string stmname;
								stmname.append( "__T__");
								stmname.append( description.name);
								stmname.append( "_");
								stmname.append( boost::lexical_cast<std::string>( m_call.size()));
								embeddedStatementMap[ stmname] = stm;

								DatabaseCommand cc( stmname, fselector, fresultsetidx, fparam, false, false, fsi->level() + 1);
								m_call.push_back( cc);
							}
						}
					}
					if (!templated)
					{
						DatabaseCommand cc( fsi->name(), fselector, fresultsetidx, fparam, false, false, fsi->level() + 1);
						m_call.push_back( cc);
					}
				}
			}
		}
		catch (const std::runtime_error& e)
		{
			throw MainProcessingStep::Error( eidx, e.what());
		}
	}
	
	// Handle PRINT instructions:
	std::map<std::size_t,PrintStep>::const_iterator pp = description.printsteps.find( description.steps.size());
	if (pp != description.printsteps.end())
	{
		handlePrintStep( pp->second);
	}
	// Create substructure context for RESULT INTO instructions:
	if ((blkidx=hasCloseSubstruct( description.blocks, description.steps.size())) >= 0)
	{
		const std::vector<std::string>& ps = description.blocks.at(blkidx).path_INTO;
		std::vector<std::string>::const_iterator pi = ps.begin(), pe = ps.end();
		for (; pi != pe; ++pi)
		{
			m_resultstruct->addCloseTag();
		}
	}
}

TransactionFunction::Impl::Impl( const Impl& o)
	:m_resultstruct(o.m_resultstruct)
	,m_call(o.m_call)
	,m_tagmap(o.m_tagmap)
	,m_preprocs(o.m_preprocs){}


TransactionFunction::TransactionFunction( const TransactionFunctionDescription& description, const LanguageDescription* langdescr, types::keymap<std::string>& embeddedStatementMap, const SubroutineDeclarationMap& functionmap)
	:m_name(description.name)
	,m_authorization( description.auth)
	,m_impl( new Impl( description, langdescr, embeddedStatementMap, functionmap))
{
	LOG_DATA2 << "[transaction function build] " << tostring();
}

TransactionFunction::TransactionFunction( const TransactionFunction& o)
	:m_name(o.m_name)
	,m_authorization(o.m_authorization)
	,m_impl( new Impl( *o.m_impl)){}

TransactionFunction::~TransactionFunction()
{
	delete m_impl;
}

const char* TransactionFunction::getErrorHint( const std::string& errorclass, int functionidx) const
{
	if (functionidx < 0 || functionidx >= (int)m_impl->m_call.size()) return 0;
	return m_impl->m_call.at( functionidx).getErrorHint( errorclass);
}

TransactionFunctionInput* TransactionFunction::getInput() const
{
	LOG_DATA << "[transaction function execute] " << tostring();
	return new TransactionFunctionInput( this);
}

langbind::TypedInputFilterR TransactionFunction::getOutput( const proc::ProcessorProvider* provider, const db::TransactionOutputR& o) const
{
	if (m_impl->m_resultfilter.empty())
	{
		ResultIterator* ri = new ResultIterator( m_impl->m_resultstruct, o);
		return langbind::TypedInputFilterR( ri);
	}
	else
	{
		ResultFilter resultFilter( provider, m_impl->m_resultfilter, m_impl->m_resultstruct, o);
		return resultFilter.getOutput();
	}
}

std::string TransactionFunction::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream rt;
	rt << "FUNCTION " << m_name << pformat->endheader;
	rt << m_impl->tostring( pformat);
	return rt.str();
}

TransactionFunction* db::createTransactionFunction( const TransactionFunctionDescription& description, const LanguageDescription* langdescr, types::keymap<std::string>& embeddedStatementMap, const SubroutineDeclarationMap& functionmap)
{
	return new TransactionFunction( description, langdescr, embeddedStatementMap, functionmap);
}


