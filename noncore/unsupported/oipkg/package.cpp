#include "package.h"

#include <qpe/process.h>
#include <qpe/stringutil.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"

//Package::~Package()
//{
//}

Package::Package( PackageManagerSettings *s )
{
	init(s);
}

void Package::init( PackageManagerSettings *s )
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
  _status = "";
  _dest = settings->getDestinationName();
  _link = settings->createLinks();
}

Package::Package( QStringList pack, PackageManagerSettings *s  )
{	
  init(s);
  parsePackage( pack );
}

Package::Package( QString n, PackageManagerSettings *s )
{	
  init(s);
 	if ( !QFile::exists( n ) )
  {
	  _name = QString( n );
  }else{
   	parseIpkgFile( n );
    _toProcess = true;
    _useFileName = true;
    _fileName = QString( n );
  }
}

Package::Package( Package *pi )
{
  init(pi->settings);
  copyValues( pi );
}


void Package::setValue( QString n, QString t )
{
  if ( n == "Package" )
  {
      _name = QString( t );
  }else if ( n == "Installed-Size" )
  {
      _size = t;
  }else if ( n == "Priority")
  {

  }else if ( n == "Section")
  {
      setSection( t );
  }else if ( n == "Maintainer")
  {

  }else if ( n == "Architecture")
  {

  }else if ( n == "Version")
  {

  }else if ( n == "Pre-Depends")
  {

  }else if ( n == "Depends")
  {

  }else if ( n == "Filename")
  {
    _fileName = t;
  }else if ( n == "Size")
 	{
  	
	}else if ( n == "MD5Sum")
	{

	}
  if ( n == "Description")
  {
      setDesc( t );
  }
  if ( n == "Status")
  {
      if ( installed() ) return;
      _status = t;
  }
  if ( t == "Essential")
  {

  }
};

QString Package::name()
{
  return _name;
}


QString Package::installName()
{
	if (_useFileName) return _fileName;
  else return _name;
}

bool Package::installed()
{
  return _status.contains("installed");
}

void Package::setDesc( QString s )
{
  _desc = s;
  _shortDesc = s.left( s.find("\n") );
}

QString Package::desc()
{
  return _desc;
}

QString Package::shortDesc()
{
  return _shortDesc;
}

QString Package::size()
{
  return _size;
}

QString Package::sizeUnits()
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

bool Package::toProcess()
{
  return _toProcess;
}

bool Package::toRemove()
{
  if ( _toProcess && installed() ) return true;
  else return false;
}

bool Package::toInstall()
{
  if ( _toProcess && !installed() ) return true;
  else return false;
}

void Package::toggleProcess()
{
  _toProcess = ! _toProcess;
}



void Package::copyValues( Package* pack )
{
  if (_size.isEmpty()      && !pack->_size.isEmpty()) _size = QString( pack->_size );
  if (_section.isEmpty()   && !pack->_section.isEmpty()) _section = QString( pack->_section );
  if (_subsection.isEmpty()&& !pack->_subsection.isEmpty()) _subsection = QString( pack->_subsection );
  if (_shortDesc.isEmpty() && !pack->_shortDesc.isEmpty()) _shortDesc = QString( pack->_shortDesc );
  if (_desc.isEmpty()      && !pack->_desc.isEmpty()) _desc = QString( pack->_desc );
  if (_name.isEmpty()      && !pack->_name.isEmpty()) _name = QString( pack->_name );
  if (!installed() && _status.isEmpty()    && !pack->_status.isEmpty()) _status = QString( pack->_status );
}

QString Package::section()
{
  return _section;
}

void Package::setSection( QString s)
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

QString Package::subSection()
{
  return _subsection;
}

void Package::parsePackage( QStringList pack )
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

QString Package::details()
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

void Package::processed()
{
	_toProcess = false;
 //hack, but we're not writing status anyway...
	if ( installed() ) _status = "install";
 	else _status = "installed";
}

QString Package::dest()
{
	if ( installed()||(!installed() && _toProcess) )
		return _dest!=""?_dest:settings->getDestinationName();
  else return "";
}

void Package::setDest( QString d )
{
	_dest = d;
}

void Package::setOn()
{
	_toProcess = true;	
}

bool Package::link()
{
	if ( _dest == "root" || (!installed() && !_toProcess) ) return false;
	return _link;
}

void Package::setLink(bool b)
{
	_link = b;
}

void Package::parseIpkgFile( QString file)
{
	system("tar xzf "+file+" -C /tmp");
 	system("tar xzf /tmp/control.tar.gz -C /tmp");
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

//QString Package::getPackageName()
//{
//	if ( _packageName.isEmpty() ) return _name;
//	else return _packageName;
//}

void Package::instalFromFile(bool iff)
{
	_useFileName = iff;
}
