/*
 *            kPPP: A front end for pppd for the KDE project
 *
 * $Id: modeminfo.cpp,v 1.4 2003-08-09 17:14:55 kergoth Exp $
 *
 * Copyright (C) 1997 Bernd Johannes Wuebben
 *                    wuebben@math.cornell.edu
 *
 * This file contributed by: Markus Wuebben, mwuebben@fiwi02.wiwi.uni-tuebingen.de
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <unistd.h>
#include <qregexp.h>
#include <qlayout.h>
// #include <kwin.h>
// #include <kmessagebox.h>
// #include <kapplication.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include "modeminfo.h"
#include "modem.h"

ModemTransfer::ModemTransfer(Modem *mo, QWidget *parent, const char *name)
    : QDialog(parent, name,TRUE, WStyle_Customize|WStyle_NormalBorder),
    _modem(mo)
{
  setCaption(QObject::tr("ATI Query"));
//  KWin::setIcons(winId(), kapp->icon(), kapp->miniIcon());

  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);

  progressBar = new QProgressBar(this, "bar");
  progressBar->setTotalSteps(8);

  statusBar = new QLabel(this,"sBar");
  statusBar->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  statusBar->setAlignment(AlignCenter);

  // This is a rather complicated case. Since we do not know which
  // message is the widest in the national language, we'd to
  // search all these messages. This is a little overkill, so I take
  // the longest english message, translate it and give it additional
  // 20 percent space. Hope this is enough.
  statusBar->setText(QObject::tr("Unable to create modem lock file."));
  statusBar->setFixedWidth((statusBar->sizeHint().width() * 12) / 10);
  statusBar->setFixedHeight(statusBar->sizeHint().height() + 4);

  // set original text
  statusBar->setText(QObject::tr("Looking for modem..."));
  progressBar->setFixedHeight(statusBar->minimumSize().height());
  tl->addWidget(progressBar);
  tl->addWidget(statusBar);

  cancel = new QPushButton(QObject::tr("Cancel"), this);
  cancel->setFocus();
  connect(cancel, SIGNAL(clicked()), SLOT(cancelbutton()));

  QHBoxLayout *l1 = new QHBoxLayout;
  tl->addLayout(l1);
  l1->addStretch(1);
  l1->addWidget(cancel);

  setFixedSize(sizeHint());

  step = 0;

  ////////////////////////////////////////////////

  timeout_timer = new QTimer(this);
  connect(timeout_timer, SIGNAL(timeout()), SLOT(time_out_slot()));

  scripttimer = new QTimer(this);
  connect(scripttimer, SIGNAL(timeout()), SLOT(do_script()));

  timeout_timer->start(15000,TRUE); // 15 secs single shot
  QTimer::singleShot(500, this, SLOT(init()));

}


void ModemTransfer::ati_done() {
  scripttimer->stop();
  timeout_timer->stop();
  _modem->closetty();
  _modem->unlockdevice();
  hide();

  // open the result window
  ModemInfo *mi = new ModemInfo(this);
  for(int i = 0; i < NUM_OF_ATI; i++)
    mi->setAtiString(i, ati_query_strings[i]);
  mi->showMaximized();
  mi->exec();
  delete mi;

  accept();
}


void ModemTransfer::time_out_slot() {
  timeout_timer->stop();
  scripttimer->stop();

  QMessageBox::warning(this, tr("Error"), QObject::tr("Modem Query timed out."));
  reject();
}


void ModemTransfer::init() {

  qApp->processEvents();

  int lock = _modem->lockdevice();
  if (lock == 1) {

    statusBar->setText(QObject::tr("Modem device is locked."));
    return;
  }

  if (lock == -1) {

    statusBar->setText(QObject::tr("Unable to create modem lock file."));
    return;
  }


  if(_modem->opentty()) {
    if(_modem->hangup()) {
      usleep(100000);  // wait 0.1 secs
      _modem->writeLine("ATE0Q1V1"); // E0 don't echo the commands I send ...

      statusBar->setText(QObject::tr("Modem Ready"));
      qApp->processEvents();
      usleep(100000);  // wait 0.1 secs
      qApp->processEvents();
      scripttimer->start(1000);	 	// this one does the ati query

      // clear modem buffer
      _modem->flush();

      _modem->notify(this, SLOT(readChar(unsigned char)));
      return;
    }
  }

  // opentty() or hangup() failed
  statusBar->setText(_modem->modemMessage());
  step = 99; // wait until cancel is pressed
  _modem->unlockdevice();
}


void ModemTransfer::do_script() {
  QString msg;
  QString query;

  switch(step) {
  case 0:
    readtty();
    statusBar->setText("ATI...");
    progressBar->setProgress( progressBar->progress() + 1);
    _modem->writeLine("ATI\n");
    break;

  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    readtty();
    msg.sprintf("ATI %d ...", step);
    query.sprintf("ATI%d\n", step);
    statusBar->setText(msg);
    progressBar->setProgress( progressBar->progress() + 1);
    _modem->writeLine(query.local8Bit());
    break;

  default:
    readtty();
    ati_done();
  }
  step++;
}

void ModemTransfer::readChar(unsigned char c) {
  if(readbuffer.length() < 255)
    readbuffer += c;
}

void ModemTransfer::readtty() {

  if (step == 0)
    return;

  readbuffer.replace(QRegExp("[\n\r]")," ");         // remove stray \n and \r
  readbuffer = readbuffer.stripWhiteSpace(); // strip of leading or trailing white
                                                 // space

  if(step <= NUM_OF_ATI)
    ati_query_strings[step-1] = readbuffer.copy();

  readbuffer = "";
}


void ModemTransfer::cancelbutton() {
  scripttimer->stop();
  _modem->stop();
  timeout_timer->stop();

  statusBar->setText(QObject::tr("One moment please..."));
  qApp->processEvents();

  _modem->hangup();

  _modem->closetty();
  _modem->unlockdevice();
  reject();
}


void ModemTransfer::closeEvent( QCloseEvent *e ) {
  cancelbutton();
  e->accept();
}


ModemInfo::ModemInfo(QWidget *parent, const char* name)
  : QDialog(parent, name, TRUE, WStyle_Customize|WStyle_NormalBorder)
{
  QString label_text;

  setCaption(QObject::tr("Modem Query Results"));
  // KWin::setIcons(winId(), kapp->icon(), kapp->miniIcon());

  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);

  QGridLayout *l1 = new QGridLayout(NUM_OF_ATI, 2, 5);
  tl->addLayout(l1, 1);
  for(int  i = 0 ; i < NUM_OF_ATI ; i++) {

    label_text = "";
    if ( i == 0)
      label_text.sprintf("ATI :");
    else
      label_text.sprintf("ATI %d:", i );

    ati_label[i] = new QLabel(label_text, this);
    l1->addWidget(ati_label[i], i, 0);

    ati_label_result[i] =  new QLineEdit(this);
    ati_label_result[i]->setMinimumWidth(fontMetrics().width('H') * 24);
    l1->addWidget(ati_label_result[i], i, 1);
  }
  //tl->addSpacing(1);

  QHBoxLayout *l2 = new QHBoxLayout;
  QPushButton *ok = new QPushButton(QObject::tr("Close"), this);
  ok->setDefault(TRUE);
  ok->setFocus();

  tl->addLayout(l2);
  l2->addStretch(1);

  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  l2->addWidget(ok);

  setMinimumSize(sizeHint());
}


void ModemInfo::setAtiString(int i, QString s) {
  if(i < NUM_OF_ATI)
    ati_label_result[i]->setText(s);
}

//#include "modeminfo.moc"
