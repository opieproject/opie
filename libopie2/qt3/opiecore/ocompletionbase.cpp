/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
                             Inspired by the KDE completion classes which are
                             Copyright (C) 2000 Dawit Alemayehu <adawit@kde.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <opie2/ocompletion.h>
#include <opie2/ocompletionbase.h>

OCompletionBase::OCompletionBase()
{
    m_delegate = 0L;
    // Assign the default completion type to use.
    m_iCompletionMode = OGlobalSettings::completionMode();

    // Initialize all key-bindings to 0 by default so that
    // the event filter will use the global settings.
    useGlobalKeyBindings();

    // By default we initialize everything to false.
    // All the variables would be setup properly when
    // the appropriate member functions are called.
    setup( false, false, false );
}

OCompletionBase::~OCompletionBase()
{
    if( m_bAutoDelCompObj && m_pCompObj )
    {
        delete m_pCompObj;
    }
}

void OCompletionBase::setDelegate( OCompletionBase *delegate )
{
    m_delegate = delegate;

    if ( m_delegate ) {
        m_delegate->m_bAutoDelCompObj = m_bAutoDelCompObj;
        m_delegate->m_bHandleSignals  = m_bHandleSignals;
        m_delegate->m_bEmitSignals    = m_bEmitSignals;
        m_delegate->m_iCompletionMode = m_iCompletionMode;
        m_delegate->m_keyMap          = m_keyMap;
    }
}

OCompletion* OCompletionBase::completionObject( bool hsig )
{
    if ( m_delegate )
        return m_delegate->completionObject( hsig );
    
    if ( !m_pCompObj )
    {
        setCompletionObject( new OCompletion(), hsig );
	m_bAutoDelCompObj = true;
    }
    return m_pCompObj;
}

void OCompletionBase::setCompletionObject( OCompletion* compObj, bool hsig )
{
    if ( m_delegate ) {
        m_delegate->setCompletionObject( compObj, hsig );
        return;
    }
    
    if ( m_bAutoDelCompObj && compObj != m_pCompObj )
        delete m_pCompObj;

    m_pCompObj = compObj;

    // We emit rotation and completion signals
    // if completion object is not NULL.
    setup( false, hsig, !m_pCompObj.isNull() );
}

// BC: Inline this function and possibly rename it to setHandleEvents??? (DA)
void OCompletionBase::setHandleSignals( bool handle )
{
    if ( m_delegate )
        m_delegate->setHandleSignals( handle );
    else
        m_bHandleSignals = handle;
}

void OCompletionBase::setCompletionMode( OGlobalSettings::Completion mode )
{
    if ( m_delegate ) {
        m_delegate->setCompletionMode( mode );
        return;
    }
    
    m_iCompletionMode = mode;
    // Always sync up OCompletion mode with ours as long as we
    // are performing completions.
    if( m_pCompObj && m_iCompletionMode != OGlobalSettings::CompletionNone )
        m_pCompObj->setCompletionMode( m_iCompletionMode );
}

bool OCompletionBase::setKeyBinding( KeyBindingType item, const OShortcut& cut )
{
    if ( m_delegate )
        return m_delegate->setKeyBinding( item, cut );


    if( !cut.isNull() )
    {
        for( KeyBindingMap::Iterator it = m_keyMap.begin(); it != m_keyMap.end(); ++it )
            if( it.data() == cut )  return false;
    }
    m_keyMap.replace( item, cut );
    return true;
}

void OCompletionBase::useGlobalKeyBindings()
{

/*    

    if ( m_delegate ) {
        m_delegate->useGlobalKeyBindings();
        return;
    }
    
    m_keyMap.clear();
    m_keyMap.insert( TextCompletion, 0 );
    m_keyMap.insert( PrevCompletionMatch, 0 );
    m_keyMap.insert( NextCompletionMatch, 0 );
    m_keyMap.insert( SubstringCompletion, 0 );

*/
    
}

void OCompletionBase::setup( bool autodel, bool hsig, bool esig )
{
    if ( m_delegate ) {
        m_delegate->setup( autodel, hsig, esig );
        return;
    }
    
    m_bAutoDelCompObj = autodel;
    m_bHandleSignals = hsig;
    m_bEmitSignals = esig;
}
