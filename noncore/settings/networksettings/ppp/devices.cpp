/*
 *           kPPP: A pppd front end for the KDE project
 *
 * $Id: devices.cpp,v 1.1.4.6 2003-07-31 12:38:15 harlekin Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qdir.h>
#include <stdlib.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qtabdialog.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qvgroupbox.h>

#include "interfaceppp.h"
#include "devices.h"
#include "authwidget.h"
#include "pppdata.h"
#include "edit.h"
#include "general.h"

void parseargs(char* buf, char** args);

DevicesWidget::DevicesWidget( InterfacePPP* ip, QWidget *parent, const char *name, WFlags f )
    : ChooserWidget(ip->data(), parent, name, f)
{
  _ifaceppp = ip;
  QWhatsThis::add(edit_b, tr("Allows you to modify the selected device"));
  QWhatsThis::add(new_b, tr("Create a new device") );

  QWhatsThis::add(copy_b,
		  tr("Makes a copy of the selected device. All\n"
		       "settings of the selected device are copied\n"
		       "to a new device, that you can modify to fit your\n"
		       "needs"));
  QWhatsThis::add(delete_b,
		  tr("<p>Deletes the selected device\n\n"
		       "<font color=\"red\"><b>Use with care!</b></font>"));

  copy_b->setEnabled( false ); //FIXME
//  delete_b->setEnabled( false ); //FIXME

  QStringList tmp = _pppdata->getDevicesNamesList();
  qDebug("DevicesWidget::DevicesWidget got devices %s",tmp.join("--").latin1());
  listListbox->insertStringList(tmp);

  for (uint i = 0; i < listListbox->count(); i++){
      qDebug("listListbox->text(i) %s == _pppdata->devname() %s",listListbox->text(i).latin1(), _pppdata->devname().latin1());
      if ( listListbox->text(i) == _pppdata->devname() )
          listListbox->setCurrentItem( i );
  }
}



void DevicesWidget::slotListBoxSelect(int idx) {
    bool ok = _pppdata->setDevice( listListbox->text(idx) );
    delete_b->setEnabled((bool)(idx != -1));
    edit_b->setEnabled((bool)(idx != -1));
//FIXME  copy_b->setEnabled((bool)(idx != -1));
}

void DevicesWidget::edit() {
  _pppdata->setDevice(listListbox->text(listListbox->currentItem()));

  int result = doTab();

  if(result == QDialog::Accepted) {
      listListbox->changeItem(_pppdata->devname(),listListbox->currentItem());
      _pppdata->save();
  }
}


void DevicesWidget::create() {

//     if(listListbox->count() == MAX_ACCOUNTS) {
//         QMessageBox::information(this, "sorry",
//                                  tr("Maximum number of accounts reached."));
//         return;
//     }

    int result;
    if (_pppdata->newdevice() == -1){
        return;
    }
    result = doTab();

    if(result == QDialog::Accepted) {
        listListbox->insertItem(_pppdata->devname());
        listListbox->setSelected(listListbox->findItem(_pppdata->devname()),true );

        _pppdata->save();
    } else
        _pppdata->deleteDevice();
}


void DevicesWidget::copy() {
//   if(listListbox->count() == MAX_ACCOUNTS) {
//     QMessageBox::information(this, "sorry", tr("Maximum number of accounts reached."));
//     return;
//   }

  if(listListbox->currentItem()<0) {
    QMessageBox::information(this, "sorry", tr("No devices selected."));
    return;
  }

  _pppdata->copydevice(listListbox->currentText());

  listListbox->insertItem(_pppdata->devname());
  _pppdata->save();
}


void DevicesWidget::remove() {

  QString s = tr("Are you sure you want to delete\nthe device \"%1\"?")
    .arg(listListbox->text(listListbox->currentItem()));

  if(QMessageBox::warning(this,tr("Confirm"),s,
                          QMessageBox::Yes,QMessageBox::No
                          ) != QMessageBox::Yes)
    return;

  if(_pppdata->deleteDevice(listListbox->text(listListbox->currentItem())))
    listListbox->removeItem(listListbox->currentItem());


//  _pppdata->save();


  slotListBoxSelect(listListbox->currentItem());

}


int DevicesWidget::doTab(){
    QDialog *dlg = new QDialog( 0, "newDevice", true, Qt::WStyle_ContextHelp );
    QVBoxLayout *layout = new QVBoxLayout( dlg );
    layout->setSpacing( 0 );
    layout->setMargin( 1 );

    QTabWidget *tabWindow = new QTabWidget( dlg, "tabWindow" );
    layout->addWidget( tabWindow );

    bool isnew;

    if(_pppdata->devname().isEmpty()) {
        dlg->setCaption(tr("New Device"));
        isnew = true;
    } else {
        QString tit = tr("Edit Device: ");
        tit += _pppdata->devname();
        dlg->setCaption(tit);
        isnew = false;
    }

   modem1 = new ModemWidget( _pppdata, tabWindow, "modem1" );
   tabWindow->addTab( modem1, tr("&Device") );
   modem2 = new ModemWidget2( _pppdata, _ifaceppp, tabWindow, "modem2" );
   tabWindow->addTab( modem2, tr("&Modem") );

    int result = 0;
    bool ok = false;

    while (!ok){
        dlg->showMaximized();
        result = dlg->exec();
        ok = true;

        if(result == QDialog::Accepted) {
            if (!modem1->save()){
                QMessageBox::critical(this, "error", tr( "You must enter a unique device name"));
                ok = false;
            }else{
                 modem2->save();
            }
        }
    }

    delete dlg;

    return result;
}

