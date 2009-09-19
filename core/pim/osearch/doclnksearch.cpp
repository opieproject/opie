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


#include "doclnkitem.h"
#include "doclnksearch.h"

#include <opie2/oresource.h>
#include <opie2/owait.h>

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include <qaction.h>
#include <qfile.h>
#include <qpopupmenu.h>
#include <qtextstream.h>


DocLnkSearch::DocLnkSearch(QListView* parent, QString name)
: AppLnkSearch(parent, name), _popupMenu(0)
{
    setPixmap( 0, Opie::Core::OResource::loadPixmap( "osearch/documents", Opie::Core::OResource::SmallIcon ) );

    actionSearchInFiles = new QAction( QObject::tr("Search content"), QString::null, 0, 0, 0, true );
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

bool DocLnkSearch::load()
{
    _apps = new DocLnkSet(QPEApplication::documentDir());
    return true;
}

bool DocLnkSearch::searchFile( AppLnk *app )
{
    if (!actionSearchInFiles->isOn())
        return false;
    
    DocLnk *doc = (DocLnk*)app;
    bool found = false;
    if ( doc->type().contains( "text" ) ) {
#ifdef NEW_OWAIT
        QString ouput = QObject::tr("searching %1").arg( doc->file() );
        OWait( output );
#endif
        QFile f(doc->file());
        if ( f.open(IO_ReadOnly) ) {
            QTextStream t( &f );
            while ( !t.eof() ) {
                if (m_search.match( t.readLine()) != -1) {
                    found = true;
                    break;
                }
            }
        }
        f.close();
    }
    return found;
}

void DocLnkSearch::insertItem( void *rec )
{
    (void)new DocLnkItem( this, (DocLnk*)rec );
    m_resultCount++;
}


QPopupMenu* DocLnkSearch::popupMenu()
{
    if (!_popupMenu){
        _popupMenu = new QPopupMenu( 0 );
        actionSearchInFiles->addTo( _popupMenu );
    }
    return _popupMenu;
}
