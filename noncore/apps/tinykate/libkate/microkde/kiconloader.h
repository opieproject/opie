#ifndef MINIKDE_KICONLOADER_H
#define MINIKDE_KICONLOADER_H

#include <qpixmap.h>
#include <qstring.h>

class KIcon
{
  public:
    enum Group { NoGroup=-1, Desktop=0, Toolbar, MainToolbar, Small,
	         Panel, LastGroup, User };
    enum StdSizes { SizeSmall=16, SizeMedium=32, SizeLarge=48 };
};

class KIconLoader
{
  public:    
    QPixmap loadIcon( const QString &name, int ); 

    QString iconPath( const QString &, int );
};

QPixmap BarIcon(const QString& name);

QPixmap DesktopIcon(const QString& name, int);

QPixmap SmallIcon(const QString& name);

QPixmap SmallIconSet( const QString &name );

#endif
