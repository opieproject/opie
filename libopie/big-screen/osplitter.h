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

#ifndef OPIE_SPLITTER_H
#define OPIE_SPLITTER_H

#include <qstring.h>

#include <qframe.h>
#include <qvaluelist.h>
#include <qlist.h>

#include "obigscreen_p.h"

/* forward declarations */
class OTabWidget;
class QHBox;
//template class QValueList<Opie::OSplitterContainer>;

/*
 * TODO
 * -check API docu
 * -one more example
 * -allow inserting at a position
 */

/**
 *
 * If you've widgets that could be placed side by side but you think
 * on small resolutions is not enough place but it would really make sense
 * on bigger resolutions this class will help you.
 * You can add as many widgets you want to it. Set a poliy on which width/height it
 * should switch the layout.
 * You can either say to place widgets vertical or horizontal.
 * This class uses QHBox, QVBox and QTAbWidget internally.
 * OSplitter takes ownership of the widgets
 *
 * @since 1.2
 *
 * @short a small dynamically changing its layout to store two or more widgets side by side
 * @version 0.1
 * @author zecke
 */
class OSplitter : public QFrame{
    Q_OBJECT
public:
    typedef QValueList<Opie::OSplitterContainer> ContainerList;
    OSplitter( Qt::Orientation = Horizontal,  QWidget *parent = 0,
               const char* name = 0, WFlags fl = 0 );
    ~OSplitter();

    void setLabel( const QString& name );
    void setIconName( const QString& name );
    QString label()const;
    QString iconName()const;

    void setSizeChange( int width_height );

    void addWidget( OSplitter* splitter );
    void addWidget( QWidget* wid, const QString& icon, const QString& label );
    void removeWidget( QWidget* );
    void removeWidget( OSplitter* );

    void setCurrentWidget( QWidget* );
    void setCurrentWidget( const QString& label );
    void setCurrentWidget( int );
    QWidget* currentWidget()const;


signals:
    /**
     * Emitted if in tab  and comes directly from the tab widget
     *
     */
    void currentChanged( QWidget* );

    /**
     * emitted whenever a border is crossed
     * true if in small screen mode
     * false if in bigscreen
     * this signal is emitted after the layout switch
     * @param b The layout mode
     * @param ori The orientation
     */
    void sizeChanged( bool b, Orientation ori);
public:
//    QSize sizeHint()const;
//    QSize minimumSizeHint()const;

protected:
    void resizeEvent( QResizeEvent* );

private:
    /* true if OTabMode */
    bool layoutMode()const;
//    void reparentAll();
    void setTabWidget( OTabWidget*);
    void addToTab( const Opie::OSplitterContainer& );
    void addToBox( const Opie::OSplitterContainer& );
    void removeFromTab( QWidget* );
    void changeTab();
    void changeHBox();
    void changeVBox();
    void commonChangeBox();
    QHBox *m_hbox;
    OTabWidget *m_tabWidget;
    OTabWidget *m_parentTab;
    Orientation m_orient;
    int m_size_policy;

    ContainerList m_container;
    QList<OSplitter> m_splitter;

    QString m_icon, m_name;

    struct Private;
    Private *d;
};


#endif
