/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef NAVBAR_H
#define NAVBAR_H

#include <qspinbox.h>
#include <qtoolbar.h>

class QAction;
class QLineEdit;

class NavBar : public QToolBar
{
    Q_OBJECT

public:
    NavBar( QMainWindow *parent = 0x0 );

    void      navBtnsEnable( bool enabled );
    void      setKey( const QString &newKey );
    void      setAutoScrollRate( int scrollRate );
    const int autoScrollRate() { return m_scrollRate->value(); }

private:
    QAction   *m_actionPrevPage;    // Action for going back 1 page
    QAction   *m_actionPrevVerse;   // Action for going back 1 verse
    QLineEdit *m_key;               // Edit box to enter key to goto
    QAction   *m_actionNextVerse;   // Action for going forward 1 verse
    QAction   *m_actionNextPage;    // Action for going forward 1 page
    QSpinBox  *m_scrollRate;        // Spin box to adjust rate of auto-scrolling
    QAction   *m_actionScroll;      // Action to start/stop auto-scrolling

signals:
    void prevPage();
    void prevVerse();
    void keyChanged( const QString &newKey );
    void nextVerse();
    void nextPage();
    void autoScroll( bool enabled );
    void scrollRateChanged( int newRate );
};

#endif
