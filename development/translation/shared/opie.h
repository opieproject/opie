#ifndef OPIE_H
#define OPIE_H

#include <qstring.h>
#include <qstringlist.h>

class OPIE {
public:
    static OPIE* self();
    /** get the list of languages */
    QStringList languageList(const QString& opiedir = QString::null)const;
    QString opieDir(const QString& opieDir)const;

private:
    OPIE();
    ~OPIE();
    static OPIE* m_self;

};

#endif
