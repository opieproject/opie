/*
                            This file is part of the OPIE Project

               =.            Copyright (c)  2003 Dan Williams <drw@handhelds.org>
             .=l.
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "promptdlg.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidgetlist.h>

#include <qpe/qpeapplication.h>

PromptDlg::PromptDlg( const QString &caption, const QString &text, const QString &btn1, const QString &btn2,
                      QWidget *parent )
    : QWidget( parent, QString::null, WType_Modal | WType_TopLevel | WStyle_Dialog )
    , m_btnClicked( -1 )
{
    setCaption( caption );

    QGridLayout *layout = new QGridLayout( this, 2, 2, 4, 2 );
    QLabel *label = new QLabel( text, this );
    label->setAlignment( AlignCenter | WordBreak );
    layout->addMultiCellWidget( label, 0, 0, 0, 1 );

    QPushButton *btn = new QPushButton( btn1, this );
    layout->addWidget( btn, 1, 0 );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotBtn1Clicked()) );

    btn = new QPushButton( btn2, this );
    layout->addWidget( btn, 1, 1 );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotBtn2Clicked()) );
}

int PromptDlg::display()
{
    // Determine position of dialog.  Derived from QT's QDialog::show() method.
    QWidget *w = parentWidget();
    QPoint p( 0, 0 );
    int extraw = 0, extrah = 0;
    QWidget * desk = QApplication::desktop();
    if ( w )
        w = w->topLevelWidget();

    QWidgetList  *list = QApplication::topLevelWidgets();
    QWidgetListIt it( *list );
    while ( (extraw == 0 || extrah == 0) && it.current() != 0 )
    {
        int w, h;
        QWidget * current = it.current();
        ++it;
        w = current->geometry().x() - current->x();
        h = current->geometry().y() - current->y();

        extraw = QMAX( extraw, w );
        extrah = QMAX( extrah, h );
    }
    delete list;

    // sanity check for decoration frames. With embedding, we
    // might get extraordinary values
    if ( extraw >= 10 || extrah >= 40 )
        extraw = extrah = 0;

    if ( w )
    {
        // Use mapToGlobal rather than geometry() in case w might
        // be embedded in another application
        QPoint pp = w->mapToGlobal( QPoint(0,0) );
        p = QPoint( pp.x() + w->width()/2, pp.y() + w->height()/ 2 );
    }
    else
        p = QPoint( desk->width()/2, desk->height()/2 );

    p = QPoint( p.x()-width()/2 - extraw, p.y()-height()/2 - extrah );

    if ( p.x() + extraw + width() > desk->width() )
        p.setX( desk->width() - width() - extraw );
    if ( p.x() < 0 )
        p.setX( 0 );

    if ( p.y() + extrah + height() > desk->height() )
        p.setY( desk->height() - height() - extrah );
    if ( p.y() < 0 )
        p.setY( 0 );

    move( p );
    show();

    // Enter event loop for modality
    qApp->enter_loop();

    return m_btnClicked;
}

int PromptDlg::ask( const QString &caption, const QString &text, const QString &btn1, const QString &btn2,
                    QWidget *parent )
{
    PromptDlg *dlg = new PromptDlg( caption, text, btn1, btn2, parent );
    int rc = dlg->display();
    delete dlg;
    return rc;
}

void PromptDlg::slotBtn1Clicked()
{
    m_btnClicked = 1;
    qApp->exit_loop();
}

void PromptDlg::slotBtn2Clicked()
{
    m_btnClicked = 2;
    qApp->exit_loop();
}
