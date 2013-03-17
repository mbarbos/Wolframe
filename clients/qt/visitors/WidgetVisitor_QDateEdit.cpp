#include "WidgetVisitor_QDateEdit.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QDateEdit::WidgetVisitorState_QDateEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_dataEdit(qobject_cast<QDateEdit*>( widget_))
{
}

bool WidgetVisitorState_QDateEdit::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QDateEdit::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QDateEdit::clear()
{
}

QVariant WidgetVisitorState_QDateEdit::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QDateEdit::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QDateEdit::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QDateEdit::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QDateEdit::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QDateEdit::getState() const
{
	return QVariant();
}
