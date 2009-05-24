/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2009 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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

#include "profilelist.h"
#include "profileedit.h"
#include "kbddhandler.h"

/* Opie */
#include <qpe/qpedialog.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>

/* Qt */
#include <qwidget.h>
#include <qlistbox.h>
#include <qstrlist.h>
#include <qmessagebox.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

/* Std */
#include <stdio.h>

KbddProfileList::KbddProfileList( QWidget* parent, const char* name, WFlags fl )
        : KbddProfileListBase( parent, name, TRUE, fl )
{
    kbdd.readConfig();
    const QString &currport = kbdd.getPort();
    const QString &currtype = kbdd.getType();

    Config cfg ("kbddcontrol");
    m_profiles = cfg.groupList();
    for (QStringList::Iterator it = m_profiles.begin(); it != m_profiles.end(); ++it ) {
        cfg.setGroup(*it);
        lbProfiles->insertItem(cfg.readEntry("name"));
        if(currport == cfg.readEntry("device") && currtype == cfg.readEntry("keybtype"))
            lbProfiles->setCurrentItem(lbProfiles->count() - 1);
    }
}

KbddProfileList::~KbddProfileList() {

}

void KbddProfileList::slotAddProfile() {
    QString profileid;

    for(int i=0;;i++) {
        profileid = "profile" + QString::number(i);
        if(m_profiles.findIndex(profileid) == -1)
            break;
    }

    KbddProfileEditor *pe = new KbddProfileEditor( this, "blar", WType_Modal | WStyle_NormalBorder );
    pe->setProfileId(profileid);
    pe->showMaximized();
    pe->exec();
    if(pe->result()) {
        lbProfiles->insertItem(pe->getProfileName());
        m_profiles.append(profileid);
    }
    delete pe;
}

void KbddProfileList::slotEditProfile() {
    if(lbProfiles->currentItem() > -1) {
        KbddProfileEditor *pe = new KbddProfileEditor( this, "blar", WType_Modal | WStyle_NormalBorder );
        pe->setProfileId(m_profiles[lbProfiles->currentItem()]);
        pe->loadProfile();
        pe->showMaximized();
        pe->exec();
        if(pe->result())
            lbProfiles->changeItem(pe->getProfileName(), lbProfiles->currentItem());
        delete pe;
    }
}

void KbddProfileList::slotDeleteProfile() {
    if(lbProfiles->currentItem() > -1) {
    if (QMessageBox::information(this, tr("Delete Profile"), tr("Are you sure you want to delete %s?", lbProfiles->currentText()), QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes) {
            Config cfg("kbddcontrol");
            QStringList::Iterator it = m_profiles.at(lbProfiles->currentItem());
            cfg.setGroup(*it);
            cfg.removeGroup();
            m_profiles.remove(it);
            lbProfiles->removeItem(lbProfiles->currentItem());
        }
    }
}

void KbddProfileList::accept() {
    if(lbProfiles->currentItem() > -1) {
        Config cfg("kbddcontrol");
        cfg.setGroup(m_profiles[lbProfiles->currentItem()]);
        kbdd.setPort(cfg.readEntry("device"));
        kbdd.setType(cfg.readEntry("keybtype"));
        kbdd.writeConfig();
        kbdd.restart();
    }
    QDialog::accept();
}

void KbddProfileList::done(int r)
{
    QDialog::done(r);
    close();
}
