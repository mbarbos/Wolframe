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
#include "WidgetRequest.hpp"
#include "WidgetVisitor.hpp"
#include "DebugTerminal.hpp"
#include <QXmlStreamWriter>
#include <QVariant>

QByteArray getWigdetRequest( QWidget* widget, QHash<QByteArray, QVariant>* globals, bool debugmode)
{
	QList<QByteArray> props = widget->dynamicPropertyNames();
	QString docType,rootElement;
	bool isStandalone = true;
	if (props.contains("doctype"))
	{
		docType = widget->property( "doctype").toString();
		isStandalone = false;
	}
	if (props.contains("rootelement"))
	{
		rootElement = widget->property( "rootelement").toString();
		isStandalone = false;
	}
	if (!isStandalone && rootElement.isEmpty())
	{
		rootElement = docType;
	}
	if (!isStandalone && docType.isEmpty())
	{
		docType = rootElement;
	}
	QByteArray rt;
	QXmlStreamWriter xml( &rt);
	WidgetVisitor visitor( widget, globals);
	QList<WidgetVisitor::Element> elements = visitor.elements();

	if (debugmode) {
		xml.setAutoFormatting( true);
		xml.setAutoFormattingIndent( 2);
	}
	xml.writeStartDocument( "1.0", isStandalone);
	if (!isStandalone)
	{
		if (rootElement == docType)
		{
			xml.writeDTD( QString( "<!DOCTYPE %1>").arg( docType));
		}
		else
		{
			xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>").arg( rootElement).arg( docType));
		}
	}

	xml.writeStartElement( rootElement);
	QList<WidgetVisitor::Element>::const_iterator ie = elements.begin(), ee = elements.end();
	for (; ie != ee; ++ie)
	{
		QVariant attribute;
		switch (ie->type())
		{
			case WidgetVisitor::Element::OpenTag:
				xml.writeStartElement( rootElement);

			case WidgetVisitor::Element::CloseTag:
				xml.writeEndElement();

			case WidgetVisitor::Element::Attribute:
				attribute = ie->value();
				++ie;
				if (ie == ee || ie->type() != WidgetVisitor::Element::Value)
				{
					qCritical() << "producing illegal XML";
					return QByteArray();
				}
				xml.writeAttribute( attribute.toString(), ie->value().toString());
				break;

			case WidgetVisitor::Element::Value:
				xml.writeCharacters( ie->value().toString());
				break;
		}
	}

	xml.writeEndElement();
	xml.writeEndDocument();
	return rt;
}

struct WidgetAnswerStackElement
{
	QByteArray name;
	QString tok;
	bool istag;

	explicit WidgetAnswerStackElement( const QXmlStreamReader& xml, bool istag_)
		:name(xml.name().toString().toAscii())
		,tok()
		,istag(istag_)
	{}

	WidgetAnswerStackElement( const WidgetAnswerStackElement& o)
		:name( o.name)
		,tok( o.tok)
		,istag(o.istag)
	{}
};

static void XMLERROR( const QXmlStreamReader& xml, const QList<WidgetAnswerStackElement>& stk, const QString& message)
{
	QXmlStreamAttributes attributes;
	QString path;
	QList<WidgetAnswerStackElement>::const_iterator pi = stk.begin(), pe = stk.end();
	for (; pi != pe; ++pi)
	{
		path.append( "/");
		path.append( pi->name);
	}
	qCritical() << (message.isEmpty()?xml.errorString():message)
		<< "in XML at line " << xml.lineNumber() << ", column " << xml.columnNumber()
		<< " path " << path;
}


bool setWidgetAnswer( QWidget* widget, QHash<QByteArray, QVariant>* globals, const QByteArray& answer)
{
	QList<WidgetAnswerStackElement> stk;
	WidgetVisitor visitor( widget, globals);
	QXmlStreamReader xml( answer);

	qDebug( ) << "feeding widget " << widget->objectName() << " with XML:\n" << answer;

	for (xml.readNext(); !xml.atEnd(); xml.readNext())
	{
		if (xml.isStartElement())
		{
			QString tagname = xml.name().toString();
			if (!stk.last().istag)
			{
				XMLERROR( xml, stk, QString( "element not defined: '") + stk.last().name + "/" + tagname + "'");
				return false;
			}
			bool istag = visitor.enter( tagname);
			stk.push_back( WidgetAnswerStackElement( xml, istag));
			QXmlStreamAttributes attributes = xml.attributes();
			if (istag)
			{
				QXmlStreamAttributes::const_iterator ai = attributes.begin(), ae = attributes.end();
				for (; ai != ae; ++ai)
				{
					if (!visitor.setProperty( ai->name().toString(), ai->value().toString()))
					{
						XMLERROR( xml, stk, QString( "failed to set property '") + ai->name().toString() + "'");
					}
				}
			}
			else
			{
				if (!attributes.isEmpty())
				{
					XMLERROR( xml, stk, QString( "substructure not defined: '") + tagname + "'");
				}
			}
		}
		else if (xml.isEndElement())
		{
			QString::const_iterator ti = stk.last().tok.begin(), te = stk.last().tok.end();
			for (; ti != te && ti->isSpace(); ++ti);
			bool tokIsEmpty = (ti == te);
			if (stk.last().istag)
			{
				if (!tokIsEmpty && !visitor.setProperty( QByteArray(""), stk.last().tok))
				{
					XMLERROR( xml, stk, "failed to set content element");
				}
				visitor.leave();
			}
			else
			{
				if (!tokIsEmpty && !visitor.setProperty( stk.last().name, stk.last().tok))
				{
					XMLERROR( xml, stk, QString( "failed to set property '") + stk.last().name + "'");
				}
			}
			stk.removeLast();
		}
		else if (xml.isCDATA() || xml.isCharacters() || xml.isEntityReference() || xml.isWhitespace())
		{
			stk.last().tok.append( xml.tokenString());
		}
	}
	return true;
}



