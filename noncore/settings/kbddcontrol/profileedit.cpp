/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2009 Opie Developer Team <opie-devel@lists.sourceforge.net>
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

#include "profileedit.h"

/* Opie */
#include <qpe/qpedialog.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>

/* Qt */
#include <qcombobox.h>
#include <qlineedit.h>
#include <qfile.h>
#if QT_VERSION >= 0x030000
#include <qstylefactory.h>
#endif

KbddProfileEditor::KbddProfileEditor( QWidget* parent, const char* name, WFlags fl )
        : KbddProfileEditorBase( parent, name, TRUE, fl ) {
    leName->setText(tr("Keyboard"));

    // FIXME we need to do better than this
    // (some way of getting a list from the application perhaps?)
    cbKeybType->insertItem("foldable");
    cbKeybType->insertItem("snapntype");
    cbKeybType->insertItem("stowaway");
    cbKeybType->insertItem("stowawayxt");
    cbKeybType->insertItem("hpslim");
    cbKeybType->insertItem("smartbt");
    cbKeybType->insertItem("lirc");
    cbKeybType->insertItem("belkinir");
    cbKeybType->insertItem("flexis");
    cbKeybType->insertItem("g250");
    cbKeybType->insertItem("pocketvik");
    cbKeybType->insertItem("microfold");
    cbKeybType->insertItem("micropad");
    cbKeybType->insertItem("microkbd");
    cbKeybType->insertItem("targusir");
    cbKeybType->insertItem("btfoldable");
    cbKeybType->insertItem("snapntypebt");
    cbKeybType->insertItem("freedom");

    // Add all usually used devices
    cbDevice->insertItem("/dev/tts/0");
    cbDevice->insertItem("/dev/tts/1");
    cbDevice->insertItem("/dev/tts/2");
    cbDevice->insertItem("/dev/tts/3");
    cbDevice->insertItem("/dev/ttySA0");
    cbDevice->insertItem("/dev/ttySA1");
    cbDevice->insertItem("/dev/ttySA2");
    cbDevice->insertItem("/dev/ttySB0");
    cbDevice->insertItem("/dev/ttyS0");
    cbDevice->insertItem("/dev/ttyS1");
    cbDevice->insertItem("/dev/ttyS2");
    cbDevice->insertItem("/dev/ttyS3");
    cbDevice->insertItem("/dev/rfcomm0");
    cbDevice->insertItem("/dev/ircomm0");

    // Now remove all items that don't exist on this machine
    for(int i=cbDevice->count();i>=0;i--) {
        if(!QFile::exists(cbDevice->text(i))) {
            cbDevice->removeItem(i);
        }
    }
}

KbddProfileEditor::~KbddProfileEditor() {

}

void KbddProfileEditor::accept() {
    Config cfg("kbddcontrol");
    cfg.setGroup(m_profileid);
    cfg.writeEntry("name", leName->text());
    cfg.writeEntry("keybtype", cbKeybType->currentText());
    cfg.writeEntry("device", cbDevice->currentText());
    cfg.writeEntry("keymap", leKeymap->text());

    m_name = leName->text();
    QDialog::accept();
}

void KbddProfileEditor::setProfileId(const QString &profileid) {
    m_profileid = profileid;
}

void KbddProfileEditor::loadProfile(void) {
    Config cfg("kbddcontrol");
    cfg.setGroup(m_profileid);
    leName->setText(cfg.readEntry("name"));
    cbKeybType->lineEdit()->setText(cfg.readEntry("keybtype"));
    cbDevice->lineEdit()->setText(cfg.readEntry("device"));
    leKeymap->setText(cfg.readEntry("keymap"));
}

const QString &KbddProfileEditor::getProfileName(void) {
    return m_name;
}
