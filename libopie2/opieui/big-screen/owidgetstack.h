/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2003 hOlgAr <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#ifndef OWIDGETSTACK_H
#define OWIDGETSTACK_H

/* QT*/
#include <qframe.h>
#include <qmap.h>

class QWidgetStack;

namespace Opie {
namespace Ui {
/**
 *
 * OWidgetStack is the answer to the problem of using Opie at different screen
 * sizes and to have a different behaviour. Most applications use a QWidgetStack
 * to supply a view on click. And by clicking the (X) you go back but this
 * behaviour feels strange on bigger screens. It's ok on smaller one because
 * one can't determine the difference.
 * This stack reads the default out of the size of the desktop widget but
 * can be forced to have either the one or the other behaviour.
 * The first widget added is considered the 'main' widget and its
 * sizeHint will be taking if in BigScreen  mode.
 * In small screen mode this widget behaves exactly like a QWidgetStack and in BigScreen
 * mode it'll use the MainWindow as child of this widget and arranges the others as
 * hidden top level widgets.
 *
 * @version 0.1
 * @author hOlgAr F.
 * @short Either a true stack or a list of top Level widgets
 */
class OWidgetStack : public QFrame {
    Q_OBJECT
public:
    enum Mode { SmallScreen, BigScreen, NoForce };
    OWidgetStack( QWidget* parent, const char* name = 0, WFlags fl = 0 );
    ~OWidgetStack();

    enum Mode mode()const;
    void forceMode( enum Mode );

    void addWidget( QWidget* , int );
    void removeWidget( QWidget* );

//    QSizeHint sizeHint()const;
//    QSizeHint minimumSizeHint()const;

    QWidget *widget( int )const;
    int id( QWidget* )const;



    QWidget* visibleWidget() const;

    bool eventFilter( QObject*, QEvent* );
signals:
    /**
     * OWidgetStack monitors the Desktop Widget for
     * size changes if it recignizes a change size it'll
     * send a signal and adjust its mode. After the signal
     * was emitted. During the signal a call to mode() the
     * old mode will be returned. Note that if a size change happens
     * but no modeChange no signal will be emitted
     *
     *
     * @param mode The new mode of the desktop
     */
    void modeChanged( enum Mode mode);

    /**
     * These two signals are emitted whenever we're about to
     * show one of the widgets
     */
    void aboutToShow( QWidget* );
    void aboutToShow( int );

public slots:
    void raiseWidget( int );
    void raiseWidget( QWidget* );
    void hideWidget( int );
    void hideWidget( QWidget* );
    void setMainWindow( QWidget* );
    void setMainWindow( int );

protected:
    void resizeEvent( QResizeEvent* );

private:
    void switchStack();
    void switchTop();
    QMap<int, QWidget*> m_list;
    QWidgetStack *m_stack;
    QWidget *m_mWidget;
    QWidget *m_last;

    enum Mode m_mode;
    bool m_forced : 1;

    struct Private;
    Private *d;
};

}
}

#endif
