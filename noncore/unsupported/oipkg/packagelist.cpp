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

void PackageList::filterPackages( QString f )
{	
  packageList.clear();
  QDictIterator<Package> filterIter( origPackageList );
  filterIter.toFirst();
  Package *pack= filterIter.current() ;
  while ( pack )
    {	
      if (
      			((aktSection=="All")||(pack->section()==aktSection)) &&
	   				((aktSubSection=="All")||(pack->subSection()==aktSubSection)) &&
        		 pack->name().contains( f )
    			)
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
  QString s = pack->section();
  if ( s.isEmpty() || s == "") return;
  if ( sections.contains(s) )  return;
  sections += s;
  QString ss = pack->subSection();
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
  QStringList destnames = settings->getDestinationNames();
  QStringList::Iterator name = destnames.begin();
  for ( QStringList::Iterator dir = dests.begin(); dir != dests.end(); ++dir )
    {
      pvDebug( 2,"Status: "+*dir+statusDir+"/status");
      readFileEntries( *dir+statusDir+"/status", *name );
      ++name;
    };
}

void PackageList::parseList()
{
  QStringList srvs = settings->getActiveServers();
	
  for ( QStringList::Iterator it = srvs.begin(); it != srvs.end(); ++it )
    {
      pvDebug( 2, "List: "+listsDir+"/"+*it);
      readFileEntries( listsDir+"/"+*it, "" );  	
    }
}

void PackageList::readFileEntries( QString filename, QString dest )
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
	      Package *p = new Package( packEntry, settings );
       	p->setDest( dest );
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
  pvDebug( 2, "parseStatus");
  parseStatus();
  pvDebug( 2, "parseList");
  parseList();
  pvDebug( 2, "finished parsing");
}

void PackageList::setSettings( PackageManagerSettings *s )
{
  settings = s;
}

Package* PackageList::getByName( QString n )
{
	origPackageList[n];
}

void PackageList::clear()
{
	origPackageList.clear();
	packageList.clear();
}

void PackageList::allPackages()
{
	packageList.clear();
  QDictIterator<Package> filterIter( origPackageList );
  filterIter.toFirst();
  Package *pack= filterIter.current() ;
  while ( pack )
    {	
	  	packageList.insert( pack->name(), pack );
      ++filterIter;
      pack = filterIter.current();
    }
}
