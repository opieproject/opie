#include "otimepicker.h"

#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qstring.h>
#include <stdio.h>
#include <qlineedit.h>

OTimePicker::OTimePicker(QWidget* parent, const char* name, 
		       WFlags fl) :
  QWidget(parent,name,fl)
{
  QVBoxLayout *vbox=new QVBoxLayout(this);

  OClickableLabel *r;
  QString s;

  // Hour Row
  QWidget *row=new QWidget(this);
  QHBoxLayout *l=new QHBoxLayout(row);
  vbox->addWidget(row);
  
  
  for (int i=0; i<24; i++) {
    r=new OClickableLabel(row);
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
    r=new OClickableLabel(row);
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

void OTimePicker::slotHour(bool b) {

  OClickableLabel *r = (OClickableLabel *) sender();

  if (b) {
    QValueListIterator<OClickableLabel *> it;
    for (it=hourLst.begin(); it!=hourLst.end(); it++) {
      if (*it != r) (*it)->setOn(false);
      else tm.setHMS((*it)->text().toInt(), tm.minute(), 0);
    }
    emit timeChanged(tm);
  } else {
    r->setOn(true);
  }

}

void OTimePicker::slotMinute(bool b) {

  OClickableLabel *r = (OClickableLabel *) sender();

  if (b) {
    QValueListIterator<OClickableLabel *> it;
    for (it=minuteLst.begin(); it!=minuteLst.end(); it++) {
      if (*it != r) (*it)->setOn(false);
      else tm.setHMS(tm.hour(),(*it)->text().toInt(), 0);
    }
    emit timeChanged(tm);
  } else {
    r->setOn(true);
  }

}

void OTimePicker::setMinute(int m) {

  QString minute;
  minute.sprintf("%.2d",m);

  QValueListIterator<OClickableLabel *> it;
  for (it=minuteLst.begin(); it!=minuteLst.end(); it++) {
    if ((*it)->text() == minute) (*it)->setOn(true);
    else (*it)->setOn(false);
  }  

  tm.setHMS(tm.hour(),m,0);
}

void OTimePicker::setHour(int h) {

  QString hour;
  hour.sprintf("%.2d",h);

  QValueListIterator<OClickableLabel *> it;
  for (it=hourLst.begin(); it!=hourLst.end(); it++) {
    if ((*it)->text() == hour) (*it)->setOn(true);
    else (*it)->setOn(false);
  }  
  tm.setHMS(h,tm.minute(),0);
}


OTimePickerDialog::OTimePickerDialog ( QWidget* parent, const char* name, WFlags fl )
	: OTimePickerDialogBase (parent , name, true , fl)
{

	connect ( m_timePicker, SIGNAL( timeChanged( const QTime& ) ),
		  this, SLOT( setTime ( const QTime& ) ) );
	connect ( minuteField, SIGNAL( textChanged ( const QString& ) ),
		  this, SLOT ( setMinute ( const QString& ) ) );
	connect ( hourField, SIGNAL( textChanged ( const QString& ) ),
		  this, SLOT ( setHour ( const QString& ) ) );

}

QTime& OTimePickerDialog::time()
{
	return m_time;
}
void OTimePickerDialog::setTime( const QTime& time )
{
	m_time = time;

	m_timePicker->setHour ( time.hour() );
	m_timePicker->setMinute( time.minute() );

	// Set Textfields
	if ( time.hour() < 10 )
		hourField->setText( "0" + QString::number( time.hour() ) );
	else
		hourField->setText( QString::number( time.hour() ) );

	if ( time.minute() < 10 )
		minuteField->setText( "0" + QString::number( time.minute() ) );
	else
		minuteField->setText( QString::number( time.minute() ) );
	
}

void OTimePickerDialog::setHour ( const QString& hour )
{
	if ( QTime::isValid ( hour.toInt(),  m_time.minute() , 00 ) ){
		m_time.setHMS ( hour.toInt(),  m_time.minute() , 00 );
		setTime ( m_time );
	}

}

void OTimePickerDialog::setMinute ( const QString& minute )
{
	if ( QTime::isValid ( m_time.hour(), minute.toInt(), 00 ) ){
		m_time.setHMS ( m_time.hour(), minute.toInt(), 00 );
		setTime ( m_time );
	}
}
