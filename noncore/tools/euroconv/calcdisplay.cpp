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
 * $Id: calcdisplay.cpp,v 1.1 2003-02-15 11:45:26 groucho Exp $
 *
 ***************************************************************************/

#include <stdio.h>
#include <qvbox.h>

#include "currency.h"
#include "calcdisplay.h"


LCDDisplay::LCDDisplay( QWidget *parent, const char *name )
        : QHBox( parent, name ){


// Create display
QVBox *vbxlayout    = new QVBox (this);

grpbxTop    = new QVGroupBox(vbxlayout, "grpbxTop");
cbbxTop     = new QComboBox(grpbxTop, "cbbxTop");
lcdTop      = new QLCDNumber(10, grpbxTop, "lcdTop");
lcdTop->setMode( QLCDNumber::DEC );
lcdTop->setSmallDecimalPoint(true);
lcdTop->setSegmentStyle(QLCDNumber::Flat);
cbbxTop->insertStrList(aCurrency);

grpbxBottom = new QVGroupBox(vbxlayout, "grpbxBottom");
cbbxBottom  = new QComboBox(grpbxBottom, "cbbxBottom");
lcdBottom   = new QLCDNumber(10, grpbxBottom, "lcdBottom");
lcdBottom->setMode( QLCDNumber::DEC );
lcdBottom->setSmallDecimalPoint(true);
lcdBottom->setSegmentStyle(QLCDNumber::Flat);
cbbxBottom->insertStrList(aCurrency);

// set combo box signals
connect(cbbxTop, SIGNAL(activated(int)), this, SLOT(cbbxChange()));
connect(cbbxBottom, SIGNAL(activated(int)), this, SLOT(cbbxChange()));

btnSwap     = new QPushButton("S",this, "swap");
btnSwap->setMaximumSize(20,50);
btnSwap->setMinimumSize(20,50);
// set signal
connect(btnSwap, SIGNAL(clicked()), this, SLOT(swapLCD()));

// set default LCD to top
iCurrentLCD = 0;

//setValue(123.456);

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
    dCurrentValue = lcdTop->value();
    iCurrentLCD = 1;
}else{
    dCurrentValue = lcdBottom->value();
    iCurrentLCD = 0;
}

setValue(dCurrentValue);
}

/***********************************************************************
 * SLOT: Currency change
 **********************************************************************/
void LCDDisplay::cbbxChange(void){

double dCurrentValue;

printf("combo changes...\n");

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
        return(dValue*FF);
        break;

    case 2: // DM: Deutch Mark
        return(dValue*DM);
        break;
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
        return(dValue/FF);
        break;

    case 2: // DM: Deutch Mark
        return(dValue/DM);
        break;
}//switch (iIndex)
}// fct x2Euro
