#ifndef CONSOLE_LOGGER_H
#define CONSOLE_LOGGER_H

#include <qstring.h>
#include <qfile.h>

class Logger {

public:
	Logger();
	Logger(const QString fileName);
	void append(QByteArray ar);
	~Logger();

private:
	QFile m_file;
};

#endif
