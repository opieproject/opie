#include "KeycfgErrorHandler.h"

void KeycfgErrorHandler::setErrorString(const QString& header,
	const QXmlParseException& exception)
{
	m_errstr = header;
	m_errstr.append(": ");
	m_errstr.append(exception.message());
	m_errstr.append(" [");
	m_errstr.append(QString::number(exception.lineNumber()));
	m_errstr.append(", ");
	m_errstr.append(QString::number(exception.columnNumber()));
	m_errstr.append("]");
	qWarning(m_errstr.latin1());
}

bool KeycfgErrorHandler::warning(const QXmlParseException& exception)
{
	setErrorString("warning", exception);
	return(true);
}

bool KeycfgErrorHandler::error(const QXmlParseException& exception)
{
	setErrorString("error", exception);
	return(false);
}

bool KeycfgErrorHandler::fatalError(const QXmlParseException& exception)
{
	setErrorString("fatal", exception);
	return(false);
}

QString KeycfgErrorHandler::errorString()
{
	return(m_errstr);
}
