#ifndef MINIKDE_KCONFIG_H
#define MINIKDE_KCONFIG_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qcolor.h>
#include <qfont.h>
#include <qmap.h>

class KConfig
{
  public:
    KConfig( const QString & );
    ~KConfig();
  
    void setGroup( const QString & );
  
    bool hasGroup( const QString &) {return false;}
    
    QValueList<int> readIntListEntry( const QString & );
    int readNumEntry( const QString &, int def=0 );
    QString readEntry( const QString &, const QString &def=QString::null );
    QStringList readListEntry( const QString & );
    bool readBoolEntry( const QString &, bool def=false );
    QColor readColorEntry( const QString &, QColor * );
    QFont readFontEntry( const QString &, QFont * );
    
    void writeEntry( const QString &, QValueList<int> );
    void writeEntry( const QString &, int );
    void writeEntry( const QString &, const QString & );
    void writeEntry( const QString &, const QStringList & );
    void writeEntry( const QString &, bool );
    void writeEntry( const QString &, const QColor & );
    void writeEntry( const QString &, const QFont & );
    
    void load();
    void sync();

  private:  
    static QString mGroup;
    
    QString mFileName;
    
    QMap<QString,bool> mBoolMap;
    QMap<QString,QString> mStringMap;
    
    bool mDirty;
};

#endif
