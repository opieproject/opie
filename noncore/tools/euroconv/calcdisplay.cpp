/****************************************************************************
 *
 * File:        calcdisplay.cpp
 *
 * Description:  
 *              
 *               
 * Authors:     Eric Santonacci <Eric.Santonacci@talc.fr>
 *
 * Requirements:    Qt
 *
 * $Id: calcdisplay.cpp,v 1.4 2003-04-15 07:42:19 eric Exp $
 *
 ***************************************************************************/

#include <stdio.h>
#include <qvbox.h>
#include <qpixmap.h>

#include "currency.h"
#include "calcdisplay.h"


LCDDisplay::LCDDisplay( QWidget *parent, const char *name )
        : QHBox( parent, name ){



this->setMargin(5);
this->setSpacing(5);

// Create display
QVBox *vbxlayout    = new QVBox (this);

/***************    Top LCD   ***********************/
grpbxTop    = new QHGroupBox(vbxlayout, "grpbxTop");
grpbxStyle  = grpbxTop->frameStyle();
grpbxTop->setMaximumHeight(48);

cbbxTop     = new QComboBox(grpbxTop, "cbbxTop");
cbbxTop->setMaximumWidth(50);
cbbxTop->insertStrList(aCurrency);

lcdTop      = new QLCDNumber(10, grpbxTop, "lcdTop");
lcdTop->setMode( QLCDNumber::DEC );
lcdTop->setSmallDecimalPoint(false);
lcdTop->setSegmentStyle(QLCDNumber::Flat);

/**************   Bottom LCD  ************************/
grpbxBottom = new QHGroupBox(vbxlayout, "grpbxBottom");
grpbxBottom->setMaximumHeight(46);
grpbxBottom->setFrameStyle(0);
grpbxBottom->setFrameShadow(QFrame::MShadow);

cbbxBottom  = new QComboBox(grpbxBottom, "cbbxBottom");
cbbxBottom->setMaximumWidth(50);
cbbxBottom->insertStrList(aCurrency);

lcdBottom   = new QLCDNumber(10, grpbxBottom, "lcdBottom");
lcdBottom->setMode( QLCDNumber::DEC );
lcdBottom->setSmallDecimalPoint(false);
lcdBottom->setSegmentStyle(QLCDNumber::Flat);

// set combo box signals
connect(cbbxTop, SIGNAL(activated(int)), this, SLOT(cbbxChange()));
connect(cbbxBottom, SIGNAL(activated(int)), this, SLOT(cbbxChange()));

btnSwap     = new QPushButton(this, "swap");
QPixmap imgSwap((const char**) swap_xpm);
btnSwap->setPixmap(imgSwap);
btnSwap->setFixedSize(20,40);
// set signal
connect(btnSwap, SIGNAL(clicked()), this, SLOT(swapLCD()));

// set default LCD to top
iCurrentLCD = 0;

}

/***********************************************************************
 * SLOT: Display value in the correct LCD
 **********************************************************************/
void LCDDisplay::setValue(double dSrcValue){

double  dDstValue=0;

int     iSrcIndex;
int     iDstIndex;


// get item index of the focused
if(!iCurrentLCD){
    iSrcIndex = cbbxTop->currentItem();
    iDstIndex = cbbxBottom->currentItem();
}else{
    iSrcIndex = cbbxBottom->currentItem();
    iDstIndex = cbbxTop->currentItem();
}

if(iSrcIndex == iDstIndex)
    dDstValue = dSrcValue;
else{
    if(iSrcIndex){
        // we are NOT in Euro as iDstIndex <> 0
        // Convert to Euro
        dDstValue = x2Euro(iSrcIndex, dSrcValue);
        dDstValue = Euro2x(iDstIndex, dDstValue);
    }else
        // We are in Euro
        dDstValue = Euro2x(iDstIndex, dSrcValue);
}


if(!iCurrentLCD){
    lcdTop->display(dSrcValue);
    lcdBottom->display(dDstValue);
}else{
    lcdBottom->display(dSrcValue);
    lcdTop->display(dDstValue);
}

}

/***********************************************************************
 * SLOT: Swap output keypad between LCD displays
 **********************************************************************/
void LCDDisplay::swapLCD(void){

double dCurrentValue;

// get current value
if(!iCurrentLCD){
    // iCurrentLCD = 0, lcdTop has current focus and is going to loose
    // it
    dCurrentValue = lcdTop->value();
    iCurrentLCD = 1;
    grpbxTop->setFrameStyle(0);
    grpbxBottom->setFrameStyle(grpbxStyle);
}else{
    dCurrentValue = lcdBottom->value();
    iCurrentLCD = 0;
    grpbxTop->setFrameStyle(grpbxStyle);
    grpbxBottom->setFrameStyle(0);
}

setValue(dCurrentValue);
}

/***********************************************************************
 * SLOT: Currency change
 **********************************************************************/
void LCDDisplay::cbbxChange(void){

double dCurrentValue;

// get current value
if(!iCurrentLCD){
    dCurrentValue = lcdTop->value();
}else{
    dCurrentValue = lcdBottom->value();
}

setValue(dCurrentValue);
}


/***********************************************************************
 * Euro2x converts dValue from Euro to the currency which combo box
 * index is provided in iIndex.
 **********************************************************************/
double LCDDisplay::Euro2x(int iIndex, double dValue){

switch (iIndex){
    case 0: // Euro
        return(dValue);
        break;

    case 1: // FF: French Francs
        return(dValue*FRF);
        break;

    case 2: // DM: Deutch Mark
        return(dValue*DEM);
        break;

    case 3: // BEL Belgium Francs
        return(dValue*BEF);
        break;

    case 4: // ITL Itialian Lire
        return(dValue*ITL);
        break;

    case 5: // LUF Luxemburg
        return(dValue*LUF);
        break;

    case 6: // IEP Irish Pound
        return(dValue*IEP);
        break;

    default:
        return 0;
}//switch (iIndex)
}// fct Eur2x



/***********************************************************************
 * x2Euro converts dValue to Euro from the currency which combo box
 * index is provided in iIndex.
 **********************************************************************/
double LCDDisplay::x2Euro(int iIndex, double dValue){

switch (iIndex){
    case 0: // Euro
        return(dValue);
        break;

    case 1: // FF: French Francs
        return(dValue/FRF);
        break;

    case 2: // DM: Deutch Mark
        return(dValue/DEM);
        break;

    case 3: // BEL Belgium Francs
        return(dValue/BEF);
        break;

    case 4: // ITL Itialian Lire
        return(dValue/ITL);
        break;

    case 5: // LUF Luxemburg
        return(dValue/LUF);
        break;

    case 6: // IEP Irish Pound
        return(dValue/IEP);
        break;
}//switch (iIndex)

// we shouldn't come here
return 0;

}// fct x2Euro
