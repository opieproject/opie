#include "package.h"

#include <qpe/process.h>
#include <qpe/stringutil.h>

#include "debug.h"

Package::~Package()
{
}

Package::Package()
{
  _size = "";
  _section = "";
  _subsection = "";
  _shortDesc = "";
  _desc = "";
  _name = "";
  _toProcess = true;
  _status = "";
}

Package::Package( QStringList pack )
{	
  Package();
  parsePackage( pack );
  _toProcess = false;
}

Package::Package( QString n )
{	
  Package();
  _name = QString( n );
  _toProcess = false;
}

Package::Package( Package *pi )
{
  Package();
  copyValues( pi );
  _toProcess = false;
}


void Package::setValue( QString n, QString t )
{
  if ( n == "Status" && installed() ) return;
  if ( n == "Package" )
    {
      _name = QString( t );
    }
  if ( n == "Installed-Size" )
    {
      _size = t;
    }
  if ( n == "Priority")
    {

    }
  if ( n == "Section")
    {
      setSection( t );
    }
  if ( n == "Maintainer")
    {

    }
  if ( n == "Architecture")
    {

    }
  if ( n == "Version")
    {

    }
  if ( n == "Pre-Depends")
    {

    }
  if ( n == "Depends")
    {

    }else if ( n == "Filename")
      {

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
  _toProcess = !(_toProcess);
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

QString Package::getSection()
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

QString Package::getSubSection()
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

