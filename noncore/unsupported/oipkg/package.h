/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#ifndef PK_ITEM_H
#define PK_ITEM_H

#include <qstring.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdict.h>
#include <qobject.h>

#include "pksettings.h"

class OipkgPackage : public QObject
{
  	Q_OBJECT
 public:
  OipkgPackage(QObject *parent=0, const char *name=0);
  OipkgPackage(PackageManagerSettings *s, QObject *parent=0, const char *name=0);
 ~OipkgPackage();
  OipkgPackage( QStringList, PackageManagerSettings *s, QObject *parent=0, const char *name=0 );
  OipkgPackage( QString, PackageManagerSettings *s, QObject *parent=0, const char *name=0 );
  OipkgPackage( OipkgPackage*s, QObject *parent=0, const char *name=0 );
 	
  void    setValue( QString, QString );
  void    copyValues( OipkgPackage* );

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

	QDict<OipkgPackage>* getOtherVersions();
	void setOtherVersions(QDict<OipkgPackage>*);

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
  QDict<OipkgPackage> *_versions;
  bool _useFileName;
  void parsePackage( QStringList );
  void init();
  //  void init(PackageManagerSettings*);
};


#endif
