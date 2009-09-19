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
#include "searchgroup.h"

#include <opie2/odebug.h>
#include <opie2/owait.h>

#include <qapplication.h>

#ifndef NEW_OWAIT
static Opie::Ui::OWait *wait = 0;
#endif

SearchGroup::SearchGroup(QListView* parent, QString name)
    : OListViewItem(parent, name)
    , m_resultCount(0)
    , m_name(name)
    , m_loaded(false)
{
}


SearchGroup::~SearchGroup()
{
}


void SearchGroup::expand()
{
    //expanded = true;
    if (m_lastSearch != m_search)
        clearList();

    if (m_search.isEmpty())
        return;

    OListViewItem *dummy = new OListViewItem( this, "searching...");
    setOpen( true );
    repaint();
    int res_count = realSearch();
    setText(0, m_name + " - " + m_search.pattern() + " (" + QString::number( res_count ) + ")");
    delete dummy;
    repaint();
}

void SearchGroup::doSearch()
{
    if (m_lastSearch == m_search)
        return;
    
    clearList();
    
    if (m_search.isEmpty())
        return;
    
    m_resultCount = realSearch();
//    repaint();
}

void SearchGroup::setSearch(QRegExp re)
{
    if (re == m_search)
        return;
    
    setText(0, m_name+" - "+re.pattern() );
    m_search = re;
    
    if ( isOpen() )
        expand();
    else
        new OListViewItem( this, "searching...");
}

int SearchGroup::realSearch()
{
    if (m_lastSearch == m_search)
        return m_resultCount;
    
#ifndef NEW_OWAIT
    if (!wait)
        wait = new Opie::Ui::OWait( qApp->mainWidget(), "osearch" );

    wait->show();
    qApp->processEvents();
#else
    Opie:Core::odebug << "********** NEW_OWAIT *************" << oendl;
    OWait( "searching" );
#endif
    if (!m_loaded)
        load();
    
    m_resultCount = 0;
    m_resultCount = search();
    m_lastSearch = m_search;
    setText(0, m_name + " - " + m_search.pattern() + " (" + QString::number( m_resultCount ) + ")");

#ifndef NEW_OWAIT
    wait->hide();
#endif
    return m_resultCount;
}

