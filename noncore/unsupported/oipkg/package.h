#ifndef PK_ITEM_H
#define PK_ITEM_H

#include <qstring.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdict.h>
#include <qobject.h>

#include "pksettings.h"

class Package : public QObject
{
  	Q_OBJECT
 public:
  Package(QObject *parent=0, const char *name=0);
  Package(PackageManagerSettings *s, QObject *parent=0, const char *name=0);
 ~Package();
  Package( QStringList, PackageManagerSettings *s, QObject *parent=0, const char *name=0 );
  Package( QString, PackageManagerSettings *s, QObject *parent=0, const char *name=0 );
  Package( Package*s, QObject *parent=0, const char *name=0 );
 	
  void    setValue( QString, QString );
  void    copyValues( Package* );

  QString name();
  QString installName();
  QString packageName();
  bool    installed();
  bool    otherInstalled();

  void    setDesc( QString );
  QString shortDesc();
  QString desc();
  QString size();
  QString sizeUnits();
  QString version();
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
  bool isOld();
  bool hasVersions();
  void parseIpkgFile( QString );
  void instalFromFile(bool iff=true);
  void setName(QString);
  QDict<QString>* getFields();
  QString status();

	QDict<Package>* getOtherVersions();
	void setOtherVersions(QDict<Package>*);

public slots:
  void toggleProcess();

private:
  PackageManagerSettings *settings;
  QString _displayName;
  QString _name;
  QString _fileName;
  bool    _old;
  bool    _hasVersions;
  bool    _toProcess;
  bool    _link;
  QString _status;
  QString _size;
  QString _section;
  QString _subsection;
  QString _shortDesc;
  QString _desc;
  QString _version;
  QString _dest;
  QDict<QString> _values;
	QDict<Package> *_versions;
  bool _useFileName;
  void parsePackage( QStringList );
  void init(PackageManagerSettings *);
};


#endif
