#include <qfile.h>
#include <qtextstream.h>
#include <opie2/odebug.h>

#include "logger.h"


Logger::Logger() {}

Logger::Logger(const QString fileName) {
	m_file.setName(fileName);
	if ( !m_file.open(IO_ReadWrite) )
	    owarn << "failed to open " << m_file.name() << oendl;
}

Logger::~Logger() {
	m_file.close();
}

void Logger::append(QByteArray ar) {
	m_file.writeBlock(ar);
}
