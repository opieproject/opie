#include <qfile.h>
#include <qtextstream.h>

#include "logger.h"


Logger::Logger() {}

Logger::Logger(const QString fileName) {
	m_file.setName(fileName);
	m_file.open(IO_ReadWrite);
}

Logger::~Logger() {
	m_file.close();
}

void Logger::append(QByteArray ar) {
	m_file.writeBlock(ar);
}
