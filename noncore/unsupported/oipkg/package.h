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
  //	Q_OBJECT
 public:
  Package(PackageManagerSettings *);
  ~Package();
  Package( QStringList, PackageManagerSettings * );
  Package( QString, PackageManagerSettings * );
  Package( Package* );
 	
  void    setValue( QString, QString );
  void    copyValues( Package* );

  QString name() ;
  bool    installed();

  void    setDesc( QString );
  QString shortDesc();
  QString desc();
  QString size();
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
public slots:
  void toggleProcess();

private:
  PackageManagerSettings *settings;
  QString _name;
  bool    _toProcess;
  bool    _link;
  QString _status;
  QString _size;
  QString _section;
  QString _subsection;
  QString _shortDesc;
  QString _desc;
  QString _dest;
  void parsePackage( QStringList );
  void init(PackageManagerSettings *);
};


#endif
