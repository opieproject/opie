#include "vumeter.h"
#include "qtrec.h"
#include <qtimer.h>
#include <math.h>
#include <qdrawutl.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

//************************************************************
//===========================================================-VUMeter

/////     copyright            : (C) 1999 bY Martin Lorenz
//////    email                : lorenz@ch.tum.de

VUMeter::VUMeter(QWidget *parent, const char *name, const int tr)
		: QWidget( parent, name )
{
//    qDebug("Making VUMeter");
	int tracks = tr;
	int i;

  qWarning("initialize peakvalues");
	for(i=0;i < tracks+2;i++) {
		peak[i] = hold[i] = 32768;
//		i_peak[i]=i_hold[i]=32768; 
		holdTime[i] = 20;
	}

	para.onOff = true;
	para.hold = 20;
	para.update = 25; //timer
	para.leds = 17;
	para.reso = 3;
	para.resoFactor = pow(2, para.reso/6.0);

	for( i = 0; i < para.leds; i++) color[i] = green;
	color[0] = color[1] = red;
	color[2] = color[3] = color[4] = color[5] = yellow;

	buffer = new QPixmap();
	setBackgroundMode(NoBackground);
	vuTimer = new QTimer(this,"vu timer");


//      vuProp= new QPopupMenu(NULL,"vu popoup");
//      vuProp->insertItem(("O&n"),this,SLOT(slotOn()),0,0 );
//      vuProp->setItemEnabled(0, FALSE);
//      vuProp->insertItem(("O&ff"),this,SLOT(slotOff()), 0,1);
//      vuProp->insertSeparator();
//      vuProp->insertItem(("&Properties..."),this,SLOT(slotProps()) );

//      dia=new VUMeterDialog(&para);
//      connect(dia, SIGNAL(accepted()),  this, SLOT(update()) );
    //    QtRec *qtrecPtr;

//      if(qtrecPtr->tab_2->isActiveWindow())
//      vuTimer->start( 50 /*para.update*/, FALSE);
//      startTimer();
	connect(vuTimer, SIGNAL(timeout()), this , SLOT(timeSlot()));
//	update();
}

VUMeter::~VUMeter(){
//  delete vuProp;
}

void VUMeter::update(){
		qWarning("vumeter update");
	vuTimer->start(para.update, FALSE);
	if (para.onOff) {
//          vuProp->setItemEnabled(0, FALSE);
//          vuProp->setItemEnabled(1, TRUE);
		disconnect(vuTimer, SIGNAL(timeout()), this , SLOT(timeSlot()));
		connect(vuTimer, SIGNAL(timeout()), this , SLOT(timeSlot()));
	} else {
//          vuProp->setItemEnabled(0, TRUE);
//          vuProp->setItemEnabled(1, FALSE);
		disconnect(vuTimer, SIGNAL(timeout()), this , SLOT(timeSlot()));
	}
	resize();
}

void VUMeter::slotOn() {
	connect(vuTimer, SIGNAL(timeout()), this , SLOT(timeSlot()));
	para.onOff=true;
//    dia->updatePara();
//      vuProp->setItemEnabled(0, FALSE);
//      vuProp->setItemEnabled(1, TRUE);
}

void VUMeter::slotOff() {
	disconnect(vuTimer, SIGNAL(timeout()), this , SLOT(timeSlot()));
	para.onOff=false;
//      dia->updatePara();
//      vuProp->setItemEnabled(0, TRUE);
//      vuProp->setItemEnabled(1, FALSE);
}

void VUMeter::slotProps() {
	qDebug("VU-Dialog");
//    dia->show();
}

void VUMeter::paintEvent(QPaintEvent* e) {
   Q_UNUSED(e);
	bitBlt(this, 0, 0, buffer);
}


void VUMeter::mousePressEvent(QMouseEvent* e) {
   Q_UNUSED(e);
//      if (e->button() == RightButton)
//          vuProp->popup(QCursor::pos() );
}


void VUMeter::resizeEvent(QResizeEvent* event) {
	buffer->resize(event->size());
	resize();
}

void VUMeter::resize() {
	qWarning("resize VUMeter painting");
	if(buffer == 0)
		qDebug("Dude NULL pixmap buffer!");

	buffer->fill(black);

	x=width()-7; y=height()-12;
	dx=x/(tracks+2); dy=y/(para.leds+2); // size of one LED + black frame
	ox=dx/6+4+(x-(tracks+2)*dx)/2;
	oy=dy/5+5+(y-(para.leds+2)*dy)/2; //offsets
	sx=(4*dx)/6; sy=(4*dy)/5;         //size of one LED

	QPainter painter; QString str; QFont font;
	int i, textOffset=0;

	if( painter.begin(buffer) ==FALSE)
		qWarning("Painting pixmap did not work!");
	else {
//          QColor c;
			//c.setHsv( (x * 255)/w, 255, 255 );// rainbow effect
			//                 c.setRgb( 255, 0, 255);
//             p.setPen(c);
		painter.setPen(green);
		qDrawShadePanel ( &painter, 0,0, width(),height(), colorGroup(), TRUE, 2, 0);
		if (2*dy-2 == 10) textOffset=1;
		font=painter.font(); font.setPointSize(2*dy-2);
		painter.setFont(font);
		for(i=0;i<tracks+2;i++) {
			painter.setPen(green); painter.setBrush(green);
			str.sprintf("%d",i+1);
			if (i==tracks) str.sprintf("L");
			if (i==tracks+1) str.sprintf("R");

              painter.drawRect(ox+dx*i,oy+dy*(para.leds)-2,sx,2*dy-1);
//			painter.drawRect(ox+dx*i,oy+dy*(15)+1,sx,2*dy-1);
			painter.setPen(black);
              painter.drawText(textOffset+ox+dx*i,oy+dy*(para.leds)-2,sx,2*dy,AlignCenter,str);
//			painter.drawText(textOffset+ox+dx*i,oy+dy*(15),sx,2*dy,AlignCenter,str);
		}
		painter.end();
		paint();
	}
}

void VUMeter::timeSlot() {
	int i;
// getting stuck here
//    qDebug("calling paint() from timeSlot()\n");
	paint();
	for(i=0;i<tracks+2;i++) {
		peak[i] /= para.resoFactor;
//		i_peak[i] /= (int)para.resoFactor;
	}
//cout <<"VU "<<peak[0]<<"\n";

}

void VUMeter::paint() {
	int i, k;
	float p, h ;
//	int i_p, i_h;
//draw background in resizeEvent
//cout <<x <<" "<<y <<" "<<dx<<" " <<dy <<" vumeter.paint\n";
//    qDebug("paint()\n");
	QPainter painter;
	painter.begin(buffer);
	int c;

	for(i=0;i<tracks+2;i++)	{
//			i_p=i_peak[i]; i_h=i_hold[i];
		p=peak[i]; h=hold[i];
		if (p>=32767) p=32768;
//			if (i_p>=32767) i_p=32768;
		if (h>=32767) h=32768;
//			if (i_h>=32767) i_h=32768;
			for(k=para.leds+1; k>=2; k--)	{
					c=para.leds+1-k;
			if (p>=32768) {  //LED on
//					if (i_p>=32768)	{  //LED on
							painter.setBrush(color[c]);
						} else { //LED off
								//painter.setPen(color[c].dark(300));
							painter.setBrush(color[c].dark(300));
						}
			if (h>=32768) {  //LED-Frame on
//					if (i_h>=32768)	{  //LED-Frame on
							painter.setPen(color[c]);
						}	else	{ //LED off
							painter.setPen(color[c].dark(300));
						}
					painter.drawRect(ox+dx*i,oy+dy*c,sx,sy);
			p*=para.resoFactor; h*=para.resoFactor;
//					i_p*=(int)para.resoFactor; i_h*=(int)para.resoFactor;
				}
		if (--holdTime[i]<=0) hold[i]=peak[i];
//			if (--holdTime[i]<=0) i_hold[i]=i_peak[i];
		}
	painter.end();
	bitBlt(this, 0, 0, buffer);
}

void VUMeter::setPeak(int a[]) {
	int i;
//	qDebug("set peak int");
//    cerr<<"setting peak\n";
	for(i=0;i<tracks+2;i++) {
		if (a[i] > i_peak[i]) i_peak[i]= a[i];
		if (a[i] > i_hold[i]) {i_hold[i]=a[i]; holdTime[i]=para.hold;}
	}
	paint();
}

void VUMeter::setPeak(float a[]) {
	int i;
//	owarn <<"set peak float"<< a[0]<< oendl;

	for(i=0;i<tracks+2;i++) {
		if (a[i] > peak[i]) peak[i]= a[i];
		if (a[i] > hold[i]) {hold[i]= a[i]; holdTime[i]=para.hold;}
	}
	paint();
}

void VUMeter::startTimer() {
	vuTimer->start(para.update, FALSE);
}

void VUMeter::stopTimer() {
	vuTimer->stop();
}

void VUMeter::readConf() {
//       Config config("QtRec");
//       config->setGroup("VU-Meter");

//      para.onOff=config->readBoolEntry("OnOff", true ) ;
//      para.update=config->readNumEntry("Update", 50 );
//      para.hold=config->readNumEntry("Hold", 20 );
//      para.reso=config->readNumEntry("Resolution", 3 );
//      para.leds=config->readNumEntry("LEDs", 16 );
//      para.resoFactor=pow(2,para.reso/6.0);

//        update();
//      dia->updatePara();
}


void VUMeter::writeConf() {
//       Config config("QtRec");
//       config->setGroup("VU-Meter");

//      config->writeEntry("OnOff" ,para.onOff );
//      config->writeEntry("Update" ,para.update );
//      config->writeEntry("Hold" ,para.hold );
//      config->writeEntry("Resolution" , para.reso );
//      config->writeEntry("LEDs" , para.leds);

}
