/*
 *           kPPP: A pppd front end for the KDE project
 *
 * $Id: accounts.cpp,v 1.9.2.1 2003-07-15 15:29:08 tille Exp $
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

#include "accounts.h"
#include "authwidget.h"
#include "pppdata.h"
#include "edit.h"

void parseargs(char* buf, char** args);

AccountWidget::AccountWidget( PPPData *pd, QWidget *parent, const char *name )
    : QWidget( parent, name )//, _pppdata(pd)
{
    _pppdata = pd;
  QVBoxLayout *l1 = new QVBoxLayout(this, 10, 10);
  accountlist_l = new QListBox(this);

  connect(accountlist_l, SIGNAL(highlighted(int)),
	  this, SLOT(slotListBoxSelect(int)));
  connect(accountlist_l, SIGNAL(selected(int)),
	  this, SLOT(editaccount()));
  l1->addWidget(accountlist_l, 10);

  edit_b = new QPushButton(tr("&Edit..."), this);
  connect(edit_b, SIGNAL(clicked()), SLOT(editaccount()));
  QWhatsThis::add(edit_b, tr("Allows you to modify the selected account"));
  l1->addWidget(edit_b);

  new_b = new QPushButton(tr("&New..."), this);
  connect(new_b, SIGNAL(clicked()), SLOT(newaccount()));
  l1->addWidget(new_b);
  QWhatsThis::add(new_b, tr("Create a new dialup connection\n"
  			      "to the Internet"));

  copy_b = new QPushButton(tr("Co&py"), this);
  connect(copy_b, SIGNAL(clicked()), SLOT(copyaccount()));
  l1->addWidget(copy_b);
  QWhatsThis::add(copy_b,
		  tr("Makes a copy of the selected account. All\n"
		       "settings of the selected account are copied\n"
		       "to a new account, that you can modify to fit your\n"
		       "needs"));

  delete_b = new QPushButton(tr("De&lete"), this);
  connect(delete_b, SIGNAL(clicked()), SLOT(deleteaccount()));
  l1->addWidget(delete_b);
  QWhatsThis::add(delete_b,
		  tr("<p>Deletes the selected account\n\n"
		       "<font color=\"red\"><b>Use with care!</b></font>"));

  QHBoxLayout *l12 = new QHBoxLayout;
  l1->addStretch(1);
  l1->addLayout(l12);

  int currAccId = _pppdata->currentAccountID();
  qDebug("currentAccountID %i", currAccId);

  //load up account list from gppdata to the list box
  if(_pppdata->count() > 0) {
    for(int i=0; i <= _pppdata->count()-1; i++) {
      _pppdata->setAccountbyIndex(i);
      accountlist_l->insertItem(_pppdata->accname());
    }
  }
  _pppdata->setAccountbyIndex( currAccId );

  qDebug("setting listview index to %i",_pppdata->currentAccountID() );
  accountlist_l->setCurrentItem( _pppdata->currentAccountID() );
  slotListBoxSelect( _pppdata->currentAccountID() );

  l1->activate();
}



void AccountWidget::slotListBoxSelect(int idx) {
  delete_b->setEnabled((bool)(idx != -1));
  edit_b->setEnabled((bool)(idx != -1));
  copy_b->setEnabled((bool)(idx != -1));
  if(idx!=-1) {
      qDebug("setting account to %i", idx);
    QString account = _pppdata->accname();
    _pppdata->setAccountbyIndex(accountlist_l->currentItem());
 }
}

void AccountWidget::editaccount() {
  _pppdata->setAccount(accountlist_l->text(accountlist_l->currentItem()));

  int result = doTab();

  if(result == QDialog::Accepted) {
    accountlist_l->changeItem(_pppdata->accname(),accountlist_l->currentItem());
//    emit resetaccounts();
    _pppdata->save();
  }
}


void AccountWidget::newaccount() {

    if(accountlist_l->count() == MAX_ACCOUNTS) {
        QMessageBox::information(this, "sorry",
                                 tr("Maximum number of accounts reached."));
        return;
    }

    int result;
    if (_pppdata->newaccount() == -1){
        qDebug("_pppdata->newaccount() == -1");
        return;
    }
    result = doTab();

    if(result == QDialog::Accepted) {
        accountlist_l->insertItem(_pppdata->accname());
        accountlist_l->setSelected(accountlist_l->findItem(_pppdata->accname()),true);

        _pppdata->save();
    } else
        _pppdata->deleteAccount();
}


void AccountWidget::copyaccount() {
  if(accountlist_l->count() == MAX_ACCOUNTS) {
    QMessageBox::information(this, "sorry", tr("Maximum number of accounts reached."));
    return;
  }

  if(accountlist_l->currentItem()<0) {
    QMessageBox::information(this, "sorry", tr("No account selected."));
    return;
  }

  _pppdata->copyaccount(accountlist_l->currentItem());

  accountlist_l->insertItem(_pppdata->accname());
//  emit resetaccounts();
  _pppdata->save();
}


void AccountWidget::deleteaccount() {

  QString s = tr("Are you sure you want to delete\nthe account \"%1\"?")
    .arg(accountlist_l->text(accountlist_l->currentItem()));

  if(QMessageBox::warning(this,tr("Confirm"),s,
                          QMessageBox::Yes,QMessageBox::No
                          ) != QMessageBox::Yes)
    return;

  if(_pppdata->deleteAccount(accountlist_l->text(accountlist_l->currentItem())))
    accountlist_l->removeItem(accountlist_l->currentItem());

  emit resetaccounts();
  _pppdata->save();

  slotListBoxSelect(accountlist_l->currentItem());

}


int AccountWidget::doTab(){
    QDialog *dlg = new QDialog( 0, "newAccount", true );
    QVBoxLayout *layout = new QVBoxLayout( dlg );
    layout->setSpacing( 0 );
    layout->setMargin( 1 );

    tabWindow = new QTabWidget( dlg, "tabWindow" );
    layout->addWidget( tabWindow );

    bool isnewaccount;

    if(_pppdata->accname().isEmpty()) {
        dlg->setCaption(tr("New Account"));
        isnewaccount = true;
    } else {
        QString tit = tr("Edit Account: ");
        tit += _pppdata->accname();
        dlg->setCaption(tit);
        isnewaccount = false;
    }

//   // DIAL WIDGET
    dial_w = new DialWidget( _pppdata, tabWindow, isnewaccount, "Dial Setup");
    tabWindow->addTab( dial_w, tr("Dial") );

//   // AUTH WIDGET
   auth_w = new AuthWidget( _pppdata, tabWindow, isnewaccount, tr("Edit Login Script"));
   tabWindow->addTab( auth_w, tr("Authentication") );

//   // IP WIDGET
    ip_w = new IPWidget( _pppdata, tabWindow, isnewaccount, tr("IP Setup"));
    tabWindow->addTab( ip_w, tr("IP") );

//   // GATEWAY WIDGET
    gateway_w = new GatewayWidget( _pppdata, tabWindow, isnewaccount, tr("Gateway Setup"));
    tabWindow->addTab( gateway_w, tr("Gateway") );

//   // DNS WIDGET
    dns_w = new DNSWidget( _pppdata, tabWindow, isnewaccount, tr("DNS Servers") );
    tabWindow->addTab( dns_w, tr("DNS") );

//   // EXECUTE WIDGET
   ExecWidget *exec_w = new ExecWidget( _pppdata, tabWindow, isnewaccount, tr("Execute Programs"));
   tabWindow->addTab( exec_w, tr("Execute") );

    int result = 0;
    bool ok = false;

    while (!ok){
        dlg->showMaximized();
        result = dlg->exec();
        ok = true;

        if(result == QDialog::Accepted) {
            if (!auth_w->check()){
                ok = false;
            } else if(!dial_w->save()) {
                QMessageBox::critical(this, "error", tr( "You must enter a unique account name"));
                ok = false;
            }else{
                ip_w->save();
                dns_w->save();
                gateway_w->save();
                auth_w->save();
                exec_w->save();
            }
        }
    }

    delete dlg;

    return result;
}


QString AccountWidget::prettyPrintVolume(unsigned int n) {
  int idx = 0;
  const QString quant[] = {tr("Byte"), tr("KB"),
		   tr("MB"), tr("GB"), QString::null};

  float n1 = n;
  while(n >= 1024 && quant[idx] != QString::null) {
    idx++;
    n /= 1024;
  }

  int i = idx;
  while(i--)
    n1 = n1 / 1024.0;

  QString s = QString::number( n1, 'f', idx==0 ? 0 : 1 );
  s += " " + quant[idx];
  return s;
}


/////////////////////////////////////////////////////////////////////////////
//
// Queries the user what to reset: costs, volume or both
//
/////////////////////////////////////////////////////////////////////////////
// QueryReset::QueryReset(QWidget *parent) : QDialog(parent, 0, true) {
// //  KWin::setIcons(winId(), kapp->icon(), kapp->miniIcon());
//   setCaption(tr("Reset Accounting"));

//   QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
//   QVGroupBox *f = new QVGroupBox(tr("What to Reset"), this);

//   QVBoxLayout *l1 = new QVBoxLayout(this, 10, 10);
// //   costs = new QCheckBox(tr("Reset the accumulated phone costs"), f);
// //   costs->setChecked(true);
// //   l1->addWidget(costs);
// //   QWhatsThis::add(costs, tr("Check this to set the phone costs\n"
// // 			      "to zero. Typically you'll want to\n"
// // 			      "do this once a month."));

// //   volume = new QCheckBox(tr("Reset volume accounting"), f);
// //   volume->setChecked(true);
// //   l1->addWidget(volume);
// //   QWhatsThis::add(volume, tr("Check this to set the volume accounting\n"
// // 			       "to zero. Typically you'll want to do this\n"
// // 			       "once a month."));

//   l1->activate();

//   // this activates the f-layout and sets minimumSize()
//   f->show();

//   tl->addWidget(f);

//   QButtonGroup *bbox = new QButtonGroup(this);
// //  bbox->addStretch(1);
//   QPushButton *ok = new QPushButton( bbox, tr("OK") );
//   bbox->insert(ok);
//   ok->setDefault(true);
//   QPushButton *cancel = new QPushButton( bbox, tr("Cancel") );
//   bbox->insert(cancel);

//   connect(ok, SIGNAL(clicked()),
// 	  this, SLOT(accepted()));
//   connect(cancel, SIGNAL(clicked()),
// 	  this, SLOT(reject()));

//   bbox->layout();
//   tl->addWidget(bbox);

// }


// void QueryReset::accepted() {
//   int result = costs->isChecked() ? COSTS : 0;
//   result += volume->isChecked() ? VOLUME : 0;

//   done(result);
// }


