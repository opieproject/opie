#ifndef OPIE_PROFILE_H
#define OPIE_PROFILE_H

#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
/**
 * A session will be generated from a saved
 * profile. A profile contains the iolayername
 * a name.
 * We can generate a Session from a Profile
 * No configuration is contained here....
 */
class Profile {
public:
    typedef QValueList<Profile> ValueList;
    enum Color { Black = 0,
                 White,
                 Gray };
    enum Terminal {VT102 = 0 };
    enum Font { Micro = 0,  Small, Medium };
    Profile();
    Profile( const QString& name,
             const QString& iolayerName,
             int background,
             int foreground,
             int terminal);
    Profile( const Profile& );
    Profile &operator=( const Profile& );
    bool operator==( const Profile& prof );

    ~Profile();
    QString name()const;
    QString ioLayerName()const;
    int foreground()const;
    int background()const;
    int terminal()const;

    /*
     * config stuff
     */
    QMap<QString, QString> conf();
    void clearConf();
    void writeEntry( const QString& key,  const QString& value );
    void writeEntry( const QString& key,  int num );
    void writeEntry( const QString& key, bool b );
    void writeEntry( const QString& key, const QStringList&, const QChar& );
    QString readEntry( const QString& key, const QString& deflt = QString::null)const;
    int readNumEntry( const QString& key, int = -1 )const;
    bool readBoolEntry( const QString& key,  bool = FALSE )const;

    void setName( const QString& );
    void setIOLayer( const QString& );
    void setBackground( int back );
    void setForeground( int fore );
    void setTerminal( int term );
private:
    QMap<QString, QString> m_conf;
    QString m_name;
    QString m_ioLayer;
    int m_back;
    int m_fore;
    int m_terminal;
};

#endif
