/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#ifndef OPIE_PIM_MAINWINDOW_H
#define OPIE_PIM_MAINWINDOW_H

#include <opie2/opimrecord.h>

#include <qpe/categories.h>
#include <qpe/config.h>

#include <qmainwindow.h>

class QAction;
class QActionGroup;
class QCopChannel;
class QDateTime;
class QPopupMenu;

namespace Opie {
/**
 * This is a common Opie PIM MainWindow
 * it takes care of the QCOP internals
 * and implements some  functions
 * for the URL scripting schema
 */
/*
 * due Qt and Templates with signal and slots
 * do not work that good :(
 * (Ok how to moc a template ;) )
 * We will have the mainwindow which calls a struct which
 * is normally reimplemented as a template ;)
 */

class OPimMainWindow : public QMainWindow {
    Q_OBJECT
public:
    enum TransPort { BlueTooth=0,
                     IrDa };

    OPimMainWindow( const QString &serviceName,
                    const QString &appName = QString::null,
                    const QString &catName = QString::null,
                    const QString &itemName = QString::null,
                    const QString &configName = QString::null,
                    QWidget *parent = 0l,  const char* name = 0l, WFlags f = WType_TopLevel );
    virtual ~OPimMainWindow();


protected slots:
    /*
     * called when a setDocument
     * couldn't be handled by this window
     */
    virtual void doSetDocument( const QString& );
    /* for syncing */
    virtual void flush()  = 0;
    virtual void reload() = 0;

    /** create a new Records and return the uid */
    virtual int create() = 0;
    /** remove a record with UID == uid */
    virtual bool remove( int uid ) = 0;
    /** beam the record with UID = uid */
    virtual void beam( int uid ) = 0;

    /** show the record with UID == uid */
    virtual void show( int uid ) = 0;
    /** edit the record */
    virtual void edit( int uid ) = 0;

    /** make a copy of it! */
    virtual void add( const OPimRecord& ) = 0;

    virtual void doAlarm( const QDateTime&, int uid );

    QCopChannel* channel();

    /** UI-related slots */
    virtual void slotItemNew() = 0;
    virtual void slotItemEdit() = 0;
    virtual void slotItemDuplicate() = 0;
    virtual void slotItemDelete() = 0;
    virtual void slotItemBeam() = 0;
    virtual void slotItemFind() = 0;
    virtual void slotConfigure() = 0;

protected:
    /**
     * start to play soundAlarm()
     * @param count How many times the alarm is played
     */
    void startAlarm( int count = 10 );
    void killAlarm();
    void timerEvent( QTimerEvent* );

    /** UI-related functions */
    QPopupMenu *itemContextMenu();
    
    void insertItemMenuItems( QActionGroup *items );
    void insertViewMenuItems( QActionGroup *items );

    void setViewCategory( const QString &category );
    void reloadCategories();
    
private slots:
    void appMessage( const QCString&, const QByteArray& );
    void setDocument( const QString& );

    /** UI-related slots */
    void slotViewCategory( QAction *category );
    
private:
    class Private;
    Private* d;

    int m_rtti;
    QCopChannel* m_channel;
    QString m_service;
    QCString m_str;
    OPimRecord* m_fallBack;
    int m_alarmCount;
    int m_playedCount;
    int m_timerId;

    QString    m_appName;      // Name of application for title bar
    QString    m_catGroupName; // Name of category group
    Config     m_config;       // Configuration settings file

    // Menu bar data
    QPopupMenu   *m_itemContextMenu;     // Pointer to context-sensitive menu
    QPopupMenu   *m_itemMenu;            // Pointer to item menu
    QPopupMenu   *m_viewMenu;            // Pointer to view menu
    QActionGroup *m_itemMenuGroup1;      // Action group containing default items in Item menu
    QActionGroup *m_itemMenuGroup2;      // Action group containing default items in Item menu
    QActionGroup *m_viewMenuAppGroup;    // Action group containing application items in View menu
    QActionGroup *m_viewMenuGroup;       // Action group containing default items in View menu
    QActionGroup *m_viewMenuCategories;  // Pointer to action group containing categories in View menu
    QAction      *m_itemEditAction;      // Edit current item action
    QAction      *m_itemDuplicateAction; // Duplicate current item action
    QAction      *m_itemDeleteAction;    // Delete current item action

    /* I would love to do this as a template
     * but can't think of a right way
     * because I need signal and slots -zecke
     */
    virtual OPimRecord* record( int rtti, const QByteArray& ) ;
    int service();

    /** UI-related functions */
    void initBars( const QString &itemName = tr( "Item" ) );

signals:
    void categorySelected( const QString &cat );
};

}

#endif
