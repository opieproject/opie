/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#include "package.h"

#include <qpe/process.h>
#include <qpe/stringutil.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"

OipkgPackage::OipkgPackage( QObject *parent, const char *name )
	: QObject(parent,name)
{

}

OipkgPackage::~OipkgPackage()
{
}

OipkgPackage::OipkgPackage( PackageManagerSettings *s, QObject *parent, const char *name )
	: QObject(parent,name)
{	
	init(s);
}

void OipkgPackage::init( PackageManagerSettings *s )
{
	settings = s;
  _size = "";
  _section = "";
  _subsection = "";
  _shortDesc = "";
  _desc = "";
  _name = "";
  _toProcess = false;
  _useFileName = false;
  _old = false;
  _status = "";
  _dest = settings->getDestinationName();
  _link = settings->createLinks();
  _versions=0;
  _version="";
}

OipkgPackage::OipkgPackage( QStringList pack, PackageManagerSettings *s , QObject *parent, const char *name )
	: QObject(parent,name)
{
  init(s);
  parsePackage( pack );
}

OipkgPackage::OipkgPackage( QString n, PackageManagerSettings *s, QObject *parent, const char *name )
	: QObject(parent,name)
{
  init(s);
 	if ( !QFile::exists( n ) )
  {
	  _name = QString( n );
  }else{
    pvDebug(4,"remote file: "+n);
   	parseIpkgFile( n );
    _useFileName = true;
    _fileName = QString( n );
  }
}

OipkgPackage::OipkgPackage( OipkgPackage *pi, QObject *parent, const char *name )
	: QObject(parent,name)
{
  init(pi->settings);
  copyValues( pi );
}


void OipkgPackage::setValue( QString n, QString t )
{
  if ( n == "Package" )
  {
      _name = QString( t );
  }else if ( n == "Installed-Size" )
  {
      _size = t;
//  }else if ( n == "Priority")
//  {

  }else if ( n == "Section")
  {
      setSection( t );
//  }else if ( n == "Maintainer")
//  {
//
//  }else if ( n == "Architecture")
//  {

  }else if ( n == "Version")
  {
     _version = t;
//  }else if ( n == "Pre-Depends")
//  {
//
//  }else if ( n == "Depends")
//  {

  }else if ( n == "Filename")
  {
    _fileName = t;
//  }else if ( n == "Size")
// 	{
//
//	}else if ( n == "MD5Sum")
//	{

	}else if ( n == "Description")
  {
      setDesc( t );
  }else if ( n == "Status")
  {
      if ( installed() ) return;
      _status = t;
//  }else if ( n == "Essential")
//  {

  }else{
  	_values.insert(n,new QString(t));
  }
}

QString OipkgPackage::name()
{
	if (_displayName.isEmpty() ) return _name;
 	else return _displayName;
}


QString OipkgPackage::installName()
{
	if (_useFileName) return _fileName;
  else return _name;
}

QString OipkgPackage::packageName()
{
	QString pn = installName();
 	pn = pn.right(pn.length()-pn.findRev("/"));
  pn = pn.left(pn.find("_"));
 	return pn;
}

bool OipkgPackage::installed()
{
  if (_status.contains("installed"))
  {
   	 if (_status.contains("not-installed"))
     {
       _toProcess = true;
       return false;
     }
     else return true;
  }
  else
	if (_versions)
  {
	  QDictIterator<OipkgPackage> other( *_versions );
		while ( other.current() )
    {
    	 if (other.current()->status().contains("installed")
      		 && other.current()->version() == version())
         	return true;
	     ++other;
 		}
  }
  return false;
}

bool OipkgPackage::otherInstalled()
{
	if (_versions)
  {
	  QDictIterator<OipkgPackage> other( *_versions );
		while ( other.current() )
    {
    	 if (other.current()->installed()) return true;
	     ++other;
 		}
  }
  return false;
}

void OipkgPackage::setDesc( QString s )
{
  _desc = s;
  _shortDesc = s.left( s.find("\n") );
}

QString OipkgPackage::desc()
{
  return _desc;
}

QString OipkgPackage::shortDesc()
{
  return _shortDesc;
}

QString OipkgPackage::size()
{
  return _size;
}


QString OipkgPackage::version()
{
  return _version;
}

QString OipkgPackage::sizeUnits()
{
	int i = _size.toInt();
 	int c = 0;
  QString ret;
 	QStringList unit;
  unit << "B" << "KB" << "MB" << "GB" << "TB"; //prepair for the future ;)
	while (i > 1)
 	{
  	ret=QString::number(i)+" "+unit[c];
   	c++;
   	i /= 1024;
  }
  return ret;
}

bool OipkgPackage::toProcess()
{
  return _toProcess;
}

bool OipkgPackage::toRemove()
{
  if ( _toProcess && installed() ) return true;
  else return false;
}

bool OipkgPackage::toInstall()
{
  if ( _toProcess && !installed() ) return true;
  else return false;
}

void OipkgPackage::toggleProcess()
{
  _toProcess = ! _toProcess;
}


void OipkgPackage::copyValues( OipkgPackage* pack )
{
  if (_size.isEmpty()      && !pack->_size.isEmpty()) _size = QString( pack->_size );
  if (_section.isEmpty()   && !pack->_section.isEmpty()) _section = QString( pack->_section );
  if (_subsection.isEmpty()&& !pack->_subsection.isEmpty()) _subsection = QString( pack->_subsection );
  if (_shortDesc.isEmpty() && !pack->_shortDesc.isEmpty()) _shortDesc = QString( pack->_shortDesc );
  if (_desc.isEmpty()      && !pack->_desc.isEmpty()) _desc = QString( pack->_desc );
  if (_name.isEmpty()      && !pack->_name.isEmpty()) _name = QString( pack->_name );
  if (_dest.isEmpty()      && !pack->_dest.isEmpty()) _dest= QString( pack->_dest );
  if (_displayName.isEmpty()&& !pack->_displayName.isEmpty()) _displayName = QString( pack->_displayName );
  if (_fileName.isEmpty()  && !pack->_fileName.isEmpty()) _fileName = QString( pack->_fileName );
  if (_version.isEmpty()      && !pack->_version.isEmpty()) _version = QString( pack->_version );
  if (_values.isEmpty()      && !pack->_values.isEmpty())_values = QDict<QString>( pack->_values );
  if (!installed() && _status.isEmpty() && !pack->_status.isEmpty()) _status = QString( pack->_status );
}

QString OipkgPackage::section()
{
  return _section;
}

void OipkgPackage::setSection( QString s)
{
  int i = s.find("/");
  if ( i > 0 )
    {
      _section    = s.left(i);
      _subsection = s.mid(i+1);
    }else{
      _section    = s;
      _subsection = "";
    }
}

QString OipkgPackage::subSection()
{
  return _subsection;
}

void OipkgPackage::parsePackage( QStringList pack )
{
  if ( pack.isEmpty() ) return;
  int count = pack.count();
  for( int i = 0; i < count; i++ )
    {
      QString line = pack[i];
      int sep = line.find( QRegExp(":[\t ]+") );
      if ( sep >= 0 )
			{
	  		QString tag = line.left(sep);
	  		QString value = line.mid(sep+2).simplifyWhiteSpace();
	  		setValue( tag, value );
  		}else{
   		}
    }
  return;
}

QString OipkgPackage::details()
{
  QString status;
  Process ipkg_status(QStringList() << "ipkg" << "info" << name() );
  QString description;
  if ( ipkg_status.exec("",status) )
    {
      QStringList lines = QStringList::split('\n',status,TRUE);
      for (QStringList::Iterator it = lines.begin(); it!=lines.end(); ++it) {
	QString line = *it;
	if ( line == " ." )
	  {
	    description.append("<p>");
	  } else
	    if ( line[0] == ' ' || line[0] == '\t' )
	      {
		// continuation
		description.append(" ");
		description.append(Qtopia::escapeString(line));
	      } else {
		int sep = line.find(QRegExp(":[\t ]+"));
		if ( sep >= 0 )
		  {
		    QString tag = line.left(sep);
		    description.append("<br>");
		    description.append("<b>");
		    description.append(Qtopia::escapeString(tag));
		    description.append(":</b> ");
		    description.append(Qtopia::escapeString(line.mid(sep+2)));
		  } else {
		    description.append(" ");
		    description.append(Qtopia::escapeString(line));
		  }
	      }
      }
    }
  return description;
}

void OipkgPackage::processed()
{
	_toProcess = false;
 //hack, but we're not writing status anyway...
	if ( installed() ) _status = "install";
 	else _status = "installed";
}

QString OipkgPackage::dest()
{
	if ( installed()||(!installed() && _toProcess) )
		return _dest!=""?_dest:settings->getDestinationName();
  else return "";
}

void OipkgPackage::setDest( QString d )
{
	if ( d == "remote") _useFileName = true;
	else _dest = d;
}

void OipkgPackage::setOn()
{
	_toProcess = true;
}

bool OipkgPackage::link()
{
	if ( _dest == "root" || (!installed() && !_toProcess) ) return false;
	return _link;
}

void OipkgPackage::setLink(bool b)
{
	_link = b;
}

void OipkgPackage::parseIpkgFile( QString file)
{
// 20020830
// a quick hack to make oipkg understand the new ipk format
// neu:  ar pf PACKAGE control.tar.gz | tar xfOz - ./control > /tmp/control
    if (system("ar pf "+file+" control.tar.gz | tar xfOz - ./control > /tmp/control") != 0)
    {
//#old tar ipk format
        system("tar xzf "+file+" -C /tmp");
        system("tar xzf /tmp/control.tar.gz -C /tmp");
    }
  QFile f("/tmp/control");
  if ( f.open(IO_ReadOnly) )
  {
  	QTextStream t( &f );
		QStringList pack;
		while ( !t.eof() )
  	{
			pack << t.readLine();
   	}
    f.close();
  	parsePackage( pack );
  }

}

//QString OipkgPackage::getPackageName()
//{
//	if ( _packageName.isEmpty() ) return _name;
//	else return _packageName;
//}

void OipkgPackage::instalFromFile(bool iff)
{
	_useFileName = iff;
}

void OipkgPackage::setName(QString n)
{
	_displayName = n;
}

QDict<QString>* OipkgPackage::getFields()
{
	return &_values;
}

QString OipkgPackage::status()
{
	return _status;
}

bool OipkgPackage::isOld()
{
	if (!_versions) return false;
	QDictIterator<OipkgPackage> other( *_versions );
	while ( other.current() ) {
     if (other.current()->version() > version() ) return true;
     ++other;
 	}
  return false;
}

bool OipkgPackage::hasVersions()
{
	if (!_versions) return false;
 	else return true;
}

QDict<OipkgPackage>* OipkgPackage::getOtherVersions()
{
	return _versions;
}

void OipkgPackage::setOtherVersions(QDict<OipkgPackage> *v)
{
	_versions=v;
}
