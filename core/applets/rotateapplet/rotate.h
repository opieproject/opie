/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2003 Maximilian Reiss <harlekin@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <_,   >  .   <=          redistribute it and/or  modify it under
:=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=           this library; see the file COPYING.LIB.
	                       If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
                                                       
*/
                                                                                       
                                                                                       

#ifndef __OPIE_ROTATE_APPLET_H__
#define __OPIE_ROTATE_APPLET_H__

#include <qpe/menuappletinterface.h>

class RotateApplet : public QObject, public MenuAppletInterface
{
public:
    RotateApplet ( );
    virtual ~RotateApplet ( );

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual int position() const;
    
    virtual QString name ( ) const;
    virtual QIconSet icon ( ) const;
    virtual QString text ( ) const;
    virtual QString tr( const char* ) const;
    virtual QString tr( const char*, const char* ) const;
    virtual QPopupMenu *popup ( QWidget *parent ) const;
    
    virtual void activated ( );

private:
    bool m_native;
    int m_startupRot;
    ulong ref;
};

#endif
