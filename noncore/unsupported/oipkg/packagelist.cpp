#include "packagelist.h"

#include <assert.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qpe/config.h>

#include "debug.h"

PackageList::PackageList()
  : packageIter( packageList )
{
  empty=true;
  {
    Config cfg( "oipkg", Config::User );
    cfg.setGroup( "Common" );
    statusDir = cfg.readEntry( "statusDir", "" );
    listsDir  = cfg.readEntry( "listsDir", "" );
    if ( statusDir=="" || ! QFileInfo(statusDir+"/status").isFile() )
      {
	statusDir="/usr/lib/ipkg/";
	listsDir="/usr/lib/ipkg/lists/";
	cfg.writeEntry( "statusDir", statusDir );
	cfg.writeEntry( "listsDir", listsDir );
      }
  }
  pvDebug( 5, "PackageList::PackageList statusDir "+statusDir);
  pvDebug( 5, "PackageList::PackageList listsDir "+listsDir);
  sections << "All";
  subSections.insert("All", new QStringList() );
  QStringList *ss = subSections["All"];
  *ss << "All";
  aktSection = "All";
  aktSubSection = "All";
}

PackageList::PackageList( PackageManagerSettings* s)
  : packageIter( packageList )
{
  settings = s;
  PackageList();
}

PackageList::~PackageList()
{
}

/** Inserts a package into the list */
void PackageList::insertPackage( Package* pack )
{
  Package* p = packageList.find( pack->name() );
  if ( p )
    {
      p->copyValues( pack );
      delete pack;
      pack = p;
    }else{
      packageList.insert( pack->name(), pack );
      origPackageList.insert( pack->name(), pack );
      empty=false;
    };
  updateSections( pack );
}

void PackageList::filterPackages()
{	
  packageList.clear();
  QDictIterator<Package> filterIter( origPackageList );
  filterIter.toFirst();
  Package *pack= filterIter.current() ;
  while ( pack )
    {	
      if ( ((aktSection=="All")||(pack->getSection()==aktSection)) &&
	   ((aktSubSection=="All")||(pack->getSubSection()==aktSubSection)) )
	{
	  packageList.insert( pack->name(), pack );
	}
      ++filterIter;
      pack = filterIter.current();
    }
}

Package* PackageList::find( QString n )
{
  return packageList.find( n );
}

Package* PackageList::first()
{ 	
  packageIter.toFirst();
  return packageIter.current();	
}

Package* PackageList::next()
{
  ++packageIter;
  return packageIter.current();
}

QStringList PackageList::getSections()
{
  sections.sort();
  return sections;
}

QStringList PackageList::getSubSections()
{
  QStringList ss;
  if ( !subSections[aktSection] ) return ss;
  ss = *subSections[aktSection];
  ss.sort();
  return ss;
}

void PackageList::setSection( QString sec )
{
  aktSection = sec;
}

void PackageList::setSubSection( QString ssec )
{
  aktSubSection = ssec;
}

void PackageList::updateSections( Package* pack )
{
  QString s = pack->getSection();
  if ( s.isEmpty() || s == "") return;
  if ( sections.contains(s) )  return;
  sections += s;
  QString ss = pack->getSubSection();
  if ( ss.isEmpty() || ss == "" ) return;
  if ( !subSections[s] ) {
    subSections.insert( s, new QStringList() );
    QStringList *subsecs = subSections[s];
    *subsecs += "All";
  }
  QStringList *subsecs = subSections[s];
  *subsecs += ss;
  if ( !subSections["All"] ) subSections.insert( "All", new QStringList() );
  subsecs = subSections["All"];
  *subsecs += ss;
}



/** No descriptions */
void PackageList::parseStatus()
{
  QStringList dests = settings->getDestinationUrls();
  for ( QStringList::Iterator it = dests.begin(); it != dests.end(); ++it )
    {
      pvDebug( 2,"Status Dir: "+*it+statusDir+"/status");
      readFileEntries( *it+statusDir+"/status" );
    };
}

void PackageList::parseList()
{
  QStringList srvs = settings->getActiveServers();
	
  for ( QStringList::Iterator it = srvs.begin(); it != srvs.end(); ++it )
    {
      pvDebug( 2, "List Dir: "+listsDir+"/"+*it);
      readFileEntries( listsDir+"/"+*it );  	
    }
}

void PackageList::readFileEntries( QString filename )
{ 	
  QStringList packEntry;
  QFile f( filename );
  if ( !f.open(IO_ReadOnly) ) return;
  QTextStream *statusStream = new QTextStream( &f );
  while ( !statusStream ->eof() )
    { 	
      QString line = statusStream->readLine();
      if ( line.find(QRegExp("[\n\t ]*")) || line == "" )
	{
	  //end of package
	  if ( ! packEntry.isEmpty() )
	    {
	      Package *p = new Package( packEntry );
	      if ( p )
		{
		  insertPackage( p );
		  packEntry.clear();
		}
	    }
	}else{
	  packEntry << line;
	};
    }
  return;
}


void PackageList::update()
{
  pvDebug( 3, "parseStatus");
  parseStatus();
  pvDebug( 3, "parseList");
  parseList();
  pvDebug( 3, "finished parsing");
}



/** No descriptions */
void PackageList::setSettings( PackageManagerSettings *s )
{
  settings = s;
}

