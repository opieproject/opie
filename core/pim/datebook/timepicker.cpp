#include "timepicker.h"

#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include "clickablelabel.h"
#include <qstring.h>

TimePicker::TimePicker(QWidget* parent = 0, const char* name = 0, 
		       WFlags fl = 0) :
  QWidget(parent,name,fl)
{
  QVBoxLayout *vbox=new QVBoxLayout(this);

  ClickableLabel *r;
  QString s;

  // Hour Row
  QWidget *row=new QWidget(this);
  QHBoxLayout *l=new QHBoxLayout(row);
  vbox->addWidget(row);
  
  
  for (int i=0; i<24; i++) {
    r=new ClickableLabel(row);
    hourLst.append(r);
    s.sprintf("%.2d",i);
    r->setText(s);
    r->setToggleButton(true);
    r->setAlignment(AlignHCenter | AlignVCenter);
    l->addWidget(r);
    connect(r, SIGNAL(toggled(bool)),
	    this, SLOT(slotHour(bool)));

    if (i==11) { // Second row
      row=new QWidget(this);
      l=new QHBoxLayout(row);
      vbox->addWidget(row);
    }
  }

  // Minute Row
  row=new QWidget(this);
  l=new QHBoxLayout(row);
  vbox->addWidget(row);
  
  for (int i=0; i<60; i+=5) {
    r=new ClickableLabel(row);
    minuteLst.append(r);
    s.sprintf("%.2d",i);
    r->setText(s);
    r->setToggleButton(true);
    r->setAlignment(AlignHCenter | AlignVCenter);
    l->addWidget(r);
    connect(r, SIGNAL(toggled(bool)),
	    this, SLOT(slotMinute(bool)));
  }
}

void TimePicker::slotHour(bool b) {

  ClickableLabel *r = (ClickableLabel *) sender();

  if (b) {
    QValueListIterator<ClickableLabel *> it;
    for (it=hourLst.begin(); it!=hourLst.end(); it++) {
      if (*it != r) (*it)->setOn(false);
      else tm.setHMS((*it)->text().toInt(), tm.minute(), 0);
    }
    emit timeChanged(tm);
  } else {
    r->setOn(true);
  }

}

void TimePicker::slotMinute(bool b) {

  ClickableLabel *r = (ClickableLabel *) sender();

  if (b) {
    QValueListIterator<ClickableLabel *> it;
    for (it=minuteLst.begin(); it!=minuteLst.end(); it++) {
      if (*it != r) (*it)->setOn(false);
      else tm.setHMS(tm.hour(),(*it)->text().toInt(), 0);
    }
    emit timeChanged(tm);
  } else {
    r->setOn(true);
  }

}

void TimePicker::setMinute(int m) {

  QString minute;
  minute.sprintf("%.2d",m);

  QValueListIterator<ClickableLabel *> it;
  for (it=minuteLst.begin(); it!=minuteLst.end(); it++) {
    if ((*it)->text() == minute) (*it)->setOn(true);
    else (*it)->setOn(false);
  }  

  tm.setHMS(tm.hour(),m,0);
}

void TimePicker::setHour(int h) {

  QString hour;
  hour.sprintf("%.2d",h);

  QValueListIterator<ClickableLabel *> it;
  for (it=hourLst.begin(); it!=hourLst.end(); it++) {
    if ((*it)->text() == hour) (*it)->setOn(true);
    else (*it)->setOn(false);
  }  
  tm.setHMS(h,tm.minute(),0);
}
