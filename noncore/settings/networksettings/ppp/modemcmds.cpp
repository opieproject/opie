/*
 *            kPPP: A front end for pppd for the KDE project
 *
 * $Id: modemcmds.cpp,v 1.2 2003-05-24 16:12:04 tille Exp $
 *
 * Copyright (C) 1997 Bernd Johannes Wuebben
 * wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
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
 * License along with dummyWidget program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qlayout.h>
#include <qslider.h>
#include <qbuttongroup.h>
#include <stdlib.h>
//#include <kapplication.h> // for getMiniIcon()
//#include <klocale.h>
#define i18n QObject::tr
#include "modemcmds.h"
#include "pppdata.h"
//#include <kwin.h>

#define ADJUSTEDIT(e) //e->setText("XXXXXXXXqy"); e->setMinimumSize(e->sizeHint()); /*e->setFixedHeight(e->sizeHint().height());*/ e->setText(""); e->setMaxLength(MODEMSTR_SIZE);

// a little trick to make the label look like a disabled lineedit
#define FORMATSLIDERLABEL(l) //l->setFixedWidth(l->sizeHint().width()); l->setFixedHeight(QLineEdit(dummyWidget).sizeHint().height()); l->setAlignment(AlignCenter); l->setFrameStyle(QFrame::WinPanel|QFrame::Sunken); l->setLineWidth(2);

ModemCommands::ModemCommands(QWidget *parent, const char *name)
    : QDialog(parent, name, true ) //, i18n("Edit Modem Commands") , Ok|Cancel)
{
    setCaption(i18n("Edit Modem Commands"));

  const int GRIDROWS = 22;
  int row = 0;

  // toplevel layout
  QVBoxLayout *tl = new QVBoxLayout(this, 10, 4);

  // add grid + frame
  QGridLayout *l1 = new QGridLayout(GRIDROWS, 4);
  tl->addLayout(l1);
  box = new QGroupBox(this, "box");
  l1->addMultiCellWidget(box, row++, GRIDROWS, 0, 3);

  // put slider and label into a separate H-Box
  QHBoxLayout *l2 = new QHBoxLayout;
  l1->addLayout(l2, row, 2);
  lpreinitslider = new QLabel("MMMM", this);
  FORMATSLIDERLABEL(lpreinitslider);

  QSlider *preinitslider = new QSlider(0, 300, 1, 0,
                                       QSlider::Horizontal, this);
//  preinitslider->setFixedHeight(preinitslider->sizeHint().height());
  connect(preinitslider, SIGNAL(valueChanged(int)),
	  lpreinitslider, SLOT(setNum(int)));
  l2->addWidget(lpreinitslider, 0);
  l2->addWidget(preinitslider, 1);

  lpreinit = new QLabel(i18n("Pre-init delay (sec/100):"), this);
  l1->addWidget(lpreinit, row++, 1);

  for(int i = 0; i < PPPData::NumInitStrings; i++) {
      initstr[i] = new QLineEdit(this);
      QLabel *initLabel = new QLabel(i18n("Initialization string %1:").arg(i + 1),
				    this);
      ADJUSTEDIT(initstr[i]);
      l1->addWidget(initLabel, row, 1);
      l1->addWidget(initstr[i], row++, 2);
  }

  QHBoxLayout *l3 = new QHBoxLayout;
  l1->addLayout(l3, row, 2);
  linitslider = new QLabel("MMMM", this);
  FORMATSLIDERLABEL(linitslider);
  QSlider *initslider = new QSlider(1, 300, 1, 0,
				QSlider::Horizontal, this);
//  initslider->setFixedHeight(initslider->sizeHint().height());
  connect(initslider, SIGNAL(valueChanged(int)),
	  linitslider, SLOT(setNum(int)));
  l3->addWidget(linitslider, 0);
  l3->addWidget(initslider, 1);

  label3 = new QLabel(i18n("Post-init delay (sec/100):"), this);
  l1->addWidget(label3, row++, 1);

  /* Set ATS11 (Dial tone duration) between 0-255 (Default ~ 70) */
  QHBoxLayout *l4 = new QHBoxLayout;
  l1->addLayout(l4, row, 2);
  ldurationslider = new QLabel("MMMM", this);
  FORMATSLIDERLABEL(ldurationslider);
  QSlider *durationslider = new QSlider(1, 255, 1, 0,
				QSlider::Horizontal, this);
//  durationslider->setFixedHeight(durationslider->sizeHint().height());
  connect(durationslider, SIGNAL(valueChanged(int)),
	  ldurationslider, SLOT(setNum(int)));
  l4->addWidget(ldurationslider, 0);
  l4->addWidget(durationslider, 1);

  lduration = new QLabel(i18n("Dialing speed (sec/100):"), this);
  l1->addWidget(lduration, row++, 1);


  initresp = new QLineEdit(this);
  label2 = new QLabel(i18n("Init response:"), this);
  ADJUSTEDIT(initresp);
  l1->addWidget(label2, row, 1);
  l1->addWidget(initresp, row++, 2);

  nodetectdialtone = new QLineEdit(this);
  lnodetectdialtone = new QLabel(i18n("No dial tone detection:"), this);
  ADJUSTEDIT(nodetectdialtone);
  l1->addWidget(lnodetectdialtone, row, 1);
  l1->addWidget(nodetectdialtone, row++, 2);

  dialstr = new QLineEdit(this);
  label4 = new QLabel(i18n("Dial string:"),this);
  ADJUSTEDIT(dialstr);
  l1->addWidget(label4, row, 1);
  l1->addWidget(dialstr, row++, 2);

  connectresp = new QLineEdit(this);
  label5 = new QLabel(i18n("Connect response:"), this);
  ADJUSTEDIT(connectresp);
  l1->addWidget(label5, row, 1);
  l1->addWidget(connectresp, row++, 2);

  busyresp = new QLineEdit(this);
  label6 = new QLabel(i18n("Busy response:"), this);
  ADJUSTEDIT(busyresp);
  l1->addWidget(label6, row, 1);
  l1->addWidget(busyresp, row++, 2);

  nocarrierresp = new QLineEdit(this);
  label7 = new QLabel(i18n("No carrier response:"), this);
  ADJUSTEDIT(nocarrierresp);
  l1->addWidget(label7, row, 1);
  l1->addWidget(nocarrierresp, row++, 2);

  nodialtoneresp = new QLineEdit(this);
  label8 = new QLabel(i18n("No dial tone response:"), this);
  ADJUSTEDIT(nodialtoneresp);
  l1->addWidget(label8, row, 1);
  l1->addWidget(nodialtoneresp, row++, 2);

  hangupstr = new QLineEdit(this);
  label9 = new QLabel(i18n("Hangup string:"), this);
  ADJUSTEDIT(hangupstr);
  l1->addWidget(label9, row, 1);
  l1->addWidget(hangupstr, row++, 2);

  hangupresp = new QLineEdit(this);
  label10 = new QLabel(i18n("Hangup response:"), this);
  ADJUSTEDIT(hangupresp);
  l1->addWidget(label10, row, 1);
  l1->addWidget(hangupresp, row++, 2);

  answerstr = new QLineEdit(this);
  label11 = new QLabel(i18n("Answer string:"), this);
  ADJUSTEDIT(answerstr);
  l1->addWidget(label11, row, 1);
  l1->addWidget(answerstr, row++, 2);

  ringresp = new QLineEdit(this);
  label12 = new QLabel(i18n("Ring response:"), this);
  ADJUSTEDIT(ringresp);
  l1->addWidget(label12, row, 1);
  l1->addWidget(ringresp, row++, 2);

  answerresp = new QLineEdit(this);
  label13 = new QLabel(i18n("Answer response:"), this);
  ADJUSTEDIT(answerresp);
  l1->addWidget(label13, row, 1);
  l1->addWidget(answerresp, row++, 2);

  escapestr = new QLineEdit(this);
  label14 = new QLabel(i18n("Escape string:"), this);
  ADJUSTEDIT(escapestr);
  l1->addWidget(label14, row, 1);
  l1->addWidget(escapestr, row++, 2);

  escaperesp = new QLineEdit(this);
  label15 = new QLabel(i18n("Escape response:"), this);
  ADJUSTEDIT(escaperesp);
  l1->addWidget(label15, row, 1);
  l1->addWidget(escaperesp, row++, 2);

  QHBoxLayout *l5 = new QHBoxLayout;
  l1->addLayout(l5, row, 2);
  lslider = new QLabel("MMMM", this);
  FORMATSLIDERLABEL(lslider);

  QSlider *slider = new QSlider(0, 255, 1, 0,
				QSlider::Horizontal, this);
//  slider->setFixedHeight(slider->sizeHint().height());
  connect(slider, SIGNAL(valueChanged(int)),
	  lslider, SLOT(setNum(int)));
  l5->addWidget(lslider, 0);
  l5->addWidget(slider, 1);

  label16 = new QLabel(i18n("Guard time (sec/50):"), this);
  l1->addWidget(label16, row++, 1);

  QLabel *l = new QLabel(i18n("Volume off/low/high:"), this);
  l1->addWidget(l, row, 1);
  QHBoxLayout *l6 = new QHBoxLayout;
  l1->addLayout(l6, row++, 2);
  volume_off = new QLineEdit(this);
//  volume_off->setFixedHeight(volume_off->sizeHint().height());
  // volume_off->setMinimumWidth((int)(volume_off->sizeHint().width() / 2));
  volume_medium = new QLineEdit(this);
//  volume_medium->setFixedHeight(volume_medium->sizeHint().height());
  // volume_medium->setMinimumWidth((int)(volume_medium->sizeHint().width() / 2));
  volume_high = new QLineEdit(this);
  // volume_high->setFixedHeight(volume_high->sizeHint().height());
  // volume_high->setMinimumWidth((int)(volume_high->sizeHint().width() / 2));
  l6->addWidget(volume_off);
  l6->addWidget(volume_medium);
  l6->addWidget(volume_high);

  initstr[0]->setFocus();

  l1->addColSpacing(0, 10);
  l1->addColSpacing(3, 10);
  l1->addRowSpacing(0, 5);
  l1->addRowSpacing(GRIDROWS, 5);

  //set stuff from gpppdata
  preinitslider->setValue(PPPData::data()->modemPreInitDelay());
  lpreinitslider->setNum(PPPData::data()->modemPreInitDelay());
  for(int i = 0; i < PPPData::NumInitStrings; i++)
      initstr[i]->setText(PPPData::data()->modemInitStr(i));
  initslider->setValue(PPPData::data()->modemInitDelay());
  linitslider->setNum(PPPData::data()->modemInitDelay());
  initresp->setText(PPPData::data()->modemInitResp());

  durationslider->setValue(PPPData::data()->modemToneDuration());
  ldurationslider->setNum(PPPData::data()->modemToneDuration());

  nodetectdialtone->setText(PPPData::data()->modemNoDialToneDetectionStr());
  dialstr->setText(PPPData::data()->modemDialStr());
  connectresp->setText(PPPData::data()->modemConnectResp());
  busyresp->setText(PPPData::data()->modemBusyResp());
  nocarrierresp->setText(PPPData::data()->modemNoCarrierResp());
  nodialtoneresp->setText(PPPData::data()->modemNoDialtoneResp());

  escapestr->setText(PPPData::data()->modemEscapeStr());
  escaperesp->setText(PPPData::data()->modemEscapeResp());

  hangupstr->setText(PPPData::data()->modemHangupStr());
  hangupresp->setText(PPPData::data()->modemHangupResp());

  answerstr->setText(PPPData::data()->modemAnswerStr());
  ringresp->setText(PPPData::data()->modemRingResp());
  answerresp->setText(PPPData::data()->modemAnswerResp());

  slider->setValue(PPPData::data()->modemEscapeGuardTime());
  lslider->setNum(PPPData::data()->modemEscapeGuardTime());

  volume_off->setText(PPPData::data()->volumeOff());
  volume_medium->setText(PPPData::data()->volumeMedium());
  volume_high->setText(PPPData::data()->volumeHigh());
}


void ModemCommands::slotOk() {
  PPPData::data()->setModemPreInitDelay(lpreinitslider->text().toInt());
  for(int i = 0; i < PPPData::NumInitStrings; i++)
      PPPData::data()->setModemInitStr(i, initstr[i]->text());
  PPPData::data()->setModemInitResp(initresp->text());
  PPPData::data()->setModemInitDelay(linitslider->text().toInt());

  PPPData::data()->setModemToneDuration(ldurationslider->text().toInt());
  PPPData::data()->setModemNoDialToneDetectionStr(nodetectdialtone->text());
  PPPData::data()->setModemDialStr(dialstr->text());
  PPPData::data()->setModemConnectResp(connectresp->text());
  PPPData::data()->setModemBusyResp(busyresp->text());
  PPPData::data()->setModemNoCarrierResp(nocarrierresp->text());
  PPPData::data()->setModemNoDialtoneResp(nodialtoneresp->text());

  PPPData::data()->setModemEscapeStr(escapestr->text());
  PPPData::data()->setModemEscapeResp(escaperesp->text());
  PPPData::data()->setModemEscapeGuardTime(lslider->text().toInt());
  PPPData::data()->setModemHangupStr(hangupstr->text());
  PPPData::data()->setModemHangupResp(hangupresp->text());

  PPPData::data()->setModemAnswerStr(answerstr->text());
  PPPData::data()->setModemRingResp(ringresp->text());
  PPPData::data()->setModemAnswerResp(answerresp->text());

  PPPData::data()->setVolumeHigh(volume_high->text());
  PPPData::data()->setVolumeMedium(volume_medium->text());
  PPPData::data()->setVolumeOff(volume_off->text());

  PPPData::data()->save();
  accept();
}


void ModemCommands::slotCancel() {
  reject();
}

//#include "modemcmds.moc"
