#include <qfile.h>
#include <qtextstream.h>
#include "script.h"

Script::Script() {
}

Script::Script(const QString fileName) {
    QFile file(fileName);
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        appendString(stream.readLine());
    }
}

void Script::saveTo(const QString fileName) const {
    QFile file(fileName);
    file.open(IO_WriteOnly);
    file.writeBlock(m_script.ascii(), m_script.length());
    file.close();
}


void Script::appendString(const QString string) {
    m_script += string;
}

QString Script::script() const {
    return m_script;
}
