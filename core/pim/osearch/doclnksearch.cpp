//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qaction.h>
#include <qfile.h>
#include <qiconset.h>
#include <qpopupmenu.h>
#include <qtextstream.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <opie/owait.h>

#include "doclnkitem.h"
#include "doclnksearch.h"

DocLnkSearch::DocLnkSearch(QListView* parent, QString name)
: AppLnkSearch(parent, name), _popupMenu(0)
{
	QIconSet is = Resource::loadIconSet( "osearch/documents" );
	setPixmap( 0, is.pixmap( QIconSet::Small, true ) );

	actionSearchInFiles = new QAction( QObject::tr("search content"),QString::null,  0, 0, 0, true );
	Config cfg( "osearch", Config::User );
   	cfg.setGroup( "doclnk_settings" );
   	actionSearchInFiles->setOn( cfg.readBoolEntry( "search_content", false ) );
}


DocLnkSearch::~DocLnkSearch()
{
	Config cfg( "osearch", Config::User );
   	cfg.setGroup( "doclnk_settings" );
   	cfg.writeEntry( "search_content", actionSearchInFiles->isOn() );
}

void DocLnkSearch::load()
{
	_apps = new DocLnkSet(QPEApplication::documentDir());
}

bool DocLnkSearch::searchFile( AppLnk *app )
{
	if (!actionSearchInFiles->isOn()) return false;
	DocLnk *doc = (DocLnk*)app;
	bool found = false;
	if ( doc->type().contains( "text" ) ){
#ifdef NEW_OWAIT
            QString ouput = QObject::tr("searching %1").arg(doc->file());
		OWait( output );
#endif
	  	QFile f(doc->file());
  		if ( f.open(IO_ReadOnly) ) {
       			QTextStream t( &f );
        		while ( !t.eof() )
       	    			if (_search.match( t.readLine()) != -1) {
					found = true;
					break;
				}
 	       	}
  		f.close();
    	}
	return found;
}

void DocLnkSearch::insertItem( void *rec )
{
	(void)new DocLnkItem( this, (DocLnk*)rec );
	_resultCount++;
}


QPopupMenu* DocLnkSearch::popupMenu()
{
	if (!_popupMenu){
		_popupMenu = new QPopupMenu( 0 );
 		actionSearchInFiles->addTo( _popupMenu );
	}
	return _popupMenu;
}
