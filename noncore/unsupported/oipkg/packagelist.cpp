/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#include "packagelist.h"

#include <assert.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#include "debug.h"

static QDict<Package>  *packageListAll;
static int packageListAllRefCount = 0;

PackageList::PackageList(QObject *parent, const char *name)
  : QObject(parent,name), packageIter( packageList )
{
  empty=true;
  if (!packageListAll) packageListAll = new QDict<Package>();
  packageListAllRefCount++;
  sections << "All";
  subSections.insert("All", new QStringList() );
  QStringList *ss = subSections["All"];
  *ss << "All";
  aktSection = "All";
  aktSubSection = "All";
}

PackageList::PackageList( PackageManagerSettings* s, QObject *parent, const char *name)
  : QObject(parent,name), packageIter( packageList )
{
  settings = s;
  PackageList(parent, name);
}

PackageList::~PackageList()
{
	if (--packageListAllRefCount < 1 ) delete packageListAll;
}

/** Inserts a package into the list */
void PackageList::insertPackage( Package* pack )
{
	if (!pack) return;
  Package* p = packageListAll->find( pack->name() );
  if ( p )
    {
      if ( (p->version() == pack->version())
//      		&& (p->dest() == pack->dest())
        	)
      {
	      p->copyValues( pack );
  	    delete pack;
    	  pack = p;
      } else {
      	QDict<Package> *packver = p->getOtherVersions();
//       	p->setName( pack->name()+"["+p->version()+"]" );
				if (!packver)
    		{
    				packver = new QDict<Package>();    	
		       	packver->insert( pack->name(), p );
          	p->setOtherVersions( packver );
        }
       	pack->setName( pack->name()+"["+pack->version()+"]" );
       	pack->setOtherVersions( packver );
       	packver->insert( pack->name(), pack );
				packageListAll->insert( pack->name(), pack );
				packageList.insert( pack->name(), pack );
      	origPackageList.insert( pack->name(), pack );
      }
    }else{
      packageListAll->insert( pack->name(), pack );
      packageList.insert( pack->name(), pack );
      origPackageList.insert( pack->name(), pack );
    };
  empty=false;
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


void PackageList::readFileEntries( QString filename, QString dest )
{ 	
	pvDebug(5,"PackageList::readFileEntries "+filename+" dest "+dest);
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
	    	  if ( p )
					{
			      p->setDest( dest );
		  			insertPackage( p );
					  packEntry.clear();
					}
	   		 }
			}else{
	  		packEntry << line;
			};
    }
 	delete statusStream;
  return;
}

void PackageList::setSettings( PackageManagerSettings *s )
{
  settings = s;
}

Package* PackageList::getByName( QString n )
{
	return origPackageList[n];
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
