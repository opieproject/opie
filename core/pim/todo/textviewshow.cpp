/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@lists.sourceforge.net>
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

#include "textviewshow.h"

using namespace Todo;

TextViewShow::TextViewShow( QWidget* parent, MainWindow* win)
    : QTextView( parent ), TodoShow(win)
{
}

TextViewShow::~TextViewShow()
{
}

QString TextViewShow::type() const
{
    return QString::fromLatin1("TextViewShow");
}

void TextViewShow::slotShow( const OPimTodo& ev )
{
    setText( ev.toRichText() );
}

QWidget* TextViewShow::widget()
{
    return this;
}

void TextViewShow::keyPressEvent( QKeyEvent* event )
{
    switch( event->key() ) {
        case Qt::Key_Up:
            if ( ( visibleHeight() < contentsHeight() ) &&
                ( verticalScrollBar()->value() > verticalScrollBar()->minValue() ) )
                scrollBy( 0, -(visibleHeight()-20) );
            else
                showPrev();
            break;
        case Qt::Key_Down:
            if ( ( visibleHeight() < contentsHeight() ) &&
                ( verticalScrollBar()->value() < verticalScrollBar()->maxValue() ) )
                scrollBy( 0, visibleHeight()-20 );
            else
                showNext();
            break;
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_F33:
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            hide();
            break;
        default:
            QTextView::keyPressEvent( event );
            break;
    }
}

void TextViewShow::hideEvent ( QHideEvent *event )
{
    escapeView();
    QTextView::hideEvent( event );
}

