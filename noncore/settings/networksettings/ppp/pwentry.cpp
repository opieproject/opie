/*
 *
 *            Kppp: A pppd front end for the KDE project
 *
 * $Id: pwentry.cpp,v 1.2 2003-08-09 17:14:56 kergoth Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
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



#include <stdio.h>
#include <qapplication.h>
#include "pwentry.h"

PWEntry::PWEntry( QWidget *parent, const char *name )
	: QWidget(NULL, name) {

   if(parent){

     QPoint point = this->mapToGlobal (QPoint (0,0));
     QRect pos = this->geometry();

     setGeometry(point.x() + pos.width()/2  - 300/2,
		 point.y() + pos.height()/2 - 90/2,
		 300,
		 90);
   }
//    else{
//      int scnum = QApplication::desktop()->screenNumber(parent);
//      QRect desk = QApplication::desktop()->screenGeometry(scnum);
//      setGeometry( desk.center().x() - 150, desk.center().y() - 50, 300, 90 );
//    }

   frame = new QGroupBox(name, this );

   this->setFocusPolicy( QWidget::StrongFocus );

   pw = new QLineEdit( this, "le" );
   pw->setEchoMode( QLineEdit::Password );
   connect( pw, SIGNAL(returnPressed()), this, SLOT(hide()) );

   isconsumed = TRUE;
}

QString PWEntry::text() { return (pw->text()); }

void PWEntry::focusInEvent( QFocusEvent *){

    pw->setFocus();

}

void PWEntry::setEchoModeNormal() {

  pw->setEchoMode(QLineEdit::Normal);

}

void PWEntry::setEchoModePassword() {

  pw->setEchoMode(QLineEdit::Password);

}

void PWEntry::setPrompt(const QString &p) {

    frame->setTitle(p);

}

void PWEntry::resizeEvent(QResizeEvent* ){

    pw->setGeometry( 15,35, this->width() - 30, 25 );
    frame->setGeometry(5,5,this->width() - 10, this->height() - 10 );

}


void PWEntry::show() {

    pw->setText("");
    isconsumed = FALSE;
    QWidget::show();
}

bool PWEntry::Consumed() {
    return(isconsumed);
}

void PWEntry::setConsumed() {
    isconsumed = TRUE;
}

void PWEntry::hide() {
  QWidget::hide();
  return;
}

//#include "pwentry.moc"
