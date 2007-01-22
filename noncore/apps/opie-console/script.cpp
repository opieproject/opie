#include <qfile.h>
#include "script.h"

Script::Script() {
}

Script::Script(const QString fileName) {
    QFile file(fileName);
    if ( !file.open(IO_ReadOnly ) )
	return;
    m_script = file.readAll();
}

void Script::saveTo(const QString fileName) const {
    QFile file(fileName);
    if ( !file.open(IO_WriteOnly) )
	return;
    file.writeBlock(m_script);
    file.close();
}


void Script::append(const QByteArray &data) {
    int size = m_script.size();
    m_script.resize(size + data.size());
    memcpy(m_script.data() + size, data.data(), data.size());
}

QByteArray Script::script() const {
    return m_script;
}
