/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael 'Mickey' Lauer
                             <mickey@tm.informatik.uni-frankfurt.de>
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

#ifndef OLISTVIEW_H
#define OLISTVIEW_H

#include <qlistview.h>
#include <qcolor.h>
#include <qpen.h>

/**
 * A @ref QListView variant featuring visual enhancements
 * like an alternate background for odd rows and an autostretch
 * mode for the width of the widget.
 *
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 * @short OListView list/tree widget.
 */
 class OListView: public QListView
{
  public:
   /**
    * Constructor.
    *
    * The parameters @p parent and @p name are handled by
    * @ref QListView, as usual.
    */
    OListView ( QWidget *parent = 0, const char *name = 0 );

   /**
    * Destructor.
    */
    virtual ~OListView();

   /**
    * Let the last column fit exactly all the available width.
    */
    void setFullWidth( bool fullWidth );

   /**
    * Returns whether the last column is set to fit the available width.
    */
    bool fullWidth() const;

   /**
    * Reimplemented for full width support
    */
    virtual int addColumn( const QString& label, int width = -1 );

   /**
    * Reimplemented for full width support
    */
    virtual int addColumn( const QIconSet& iconset, const QString& label, int width = -1 );

   /**
    * Reimplemented for full width support
    */
    virtual void removeColumn(int index);

   /**
    * sets the alternate background background color.
    * This only has an effect if the items are OListViewItems
    *
    * @param c the color to use for every other item. Set to an invalid
    *        color to disable alternate colors.
    */
    void setAlternateBackground( const QColor &c );

   /**
    * sets the column separator pen.
    *
    * @param p the pen used to draw the column separator.
    */
    void setColumnSeparator( const QPen &p );

   /**
    * @return the alternate background color
    */
    const QColor& alternateBackground() const;

   /**
    * @return the column separator pen
    */
    const QPen& columnSeparator() const;

  private:
    QColor m_alternateBackground;
    bool m_fullWidth;
    QPen m_columnSeparator;

};

//****************************** OListViewItem ******************************************************************

class OListViewItem: public QListViewItem
{
  public:
    OListViewItem( QListView * parent );
    OListViewItem( QListViewItem * parent );
    OListViewItem( QListView * parent, QListViewItem * after );
    OListViewItem( QListViewItem * parent, QListViewItem * after );

    OListViewItem( QListView * parent,
        QString,     QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null );

    OListViewItem( QListViewItem * parent,
        QString,     QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null );

    OListViewItem( QListView * parent, QListViewItem * after,
        QString,     QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null );
    
    OListViewItem( QListViewItem * parent, QListViewItem * after,
        QString,     QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null );
    
    virtual ~OListViewItem();
    
    const QColor& backgroundColor();
    bool isAlternate();
    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment );
    void init();

  private:
    bool m_known;
    bool m_odd;
};

#endif
