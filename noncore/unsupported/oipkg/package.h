#ifndef PK_ITEM_H
#define PK_ITEM_H

#include <qstring.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdict.h>
#include <qobject.h>

#include "pksettings.h"

class Package //: public QObject
{
//  	Q_OBJECT
 public:
  Package(PackageManagerSettings *);
 // ~Package();
  Package( QStringList, PackageManagerSettings * );
  Package( QString, PackageManagerSettings * );
  Package( Package* );
 	
  void    setValue( QString, QString );
  void    copyValues( Package* );

  QString name() ;
  QString installName() ;
  bool    installed();

  void    setDesc( QString );
  QString shortDesc();
  QString desc();
  QString size();
  QString sizeUnits();
  void    setSection( QString );
  QString section();
  QString subSection();
  QString details();
  bool    toProcess();
  bool    toInstall();
  bool    toRemove();
  void processed();
  QString dest();
  void setDest( QString d );
  void setOn();
  bool link();
  void setLink(bool);
  void parseIpkgFile( QString );
  void instalFromFile(bool iff=true);
public slots:
//  QString getPackageName();
  void toggleProcess();

private:
  PackageManagerSettings *settings;
//  QString _packageName;
  QString _name;
  QString _fileName;
  bool    _toProcess;
  bool    _link;
  QString _status;
  QString _size;
  QString _section;
  QString _subsection;
  QString _shortDesc;
  QString _desc;
  QString _dest;
  bool _useFileName;
  void parsePackage( QStringList );
  void init(PackageManagerSettings *);
};


#endif
