#ifndef SKIN_H
#define SKIN_H

#include <qstring.h>
#include <qimage.h>

class Skin
{
public:
    Skin( const QString &name, const QString &fileNameInfix );

    QImage backgroundImage() const;

private:
    QString m_name;
    QString m_fileNameInfix;
    QString m_skinPath;
};

#endif // SKIN_H
/* vim: et sw=4 ts=4
 */
