/****************************************************************************
 *
 * File:        calckeypad.cpp
 *
 * Description:  
 *              
 *               
 * Authors:     Eric Santonacci <Eric.Santonacci@talc.fr>
 *
 * Requirements:    Qt
 *
 * $Id: calckeypad.cpp,v 1.5 2003-04-15 07:42:19 eric Exp $
 *
 ***************************************************************************/

#include <qmessagebox.h>

#include "calckeypad.h"


KeyPad::KeyPad(LCDDisplay *lcd, QWidget *parent, const char *name )
    : QGrid(5, parent, name){

int         i;
QButton *btnButton;

// save lcd at init
display     = lcd;
// Init variable
dCurrent    = 0;
iLenCurrent = 1;
bIsDec      = false;
dDecCurrent = 0;
iPreviousOperator   = 0;
dPreviousValue      = 0;

// First line
btnClear    = new QPushButton("CE/C", this, "Clear");
btn7        = new QPushButton("7", this, "7");
btn8        = new QPushButton("8", this, "8");
btn9        = new QPushButton("9", this, "9");
btnPlus     = new QPushButton("+", this, "+");
// 2nd line
btnPercent  = new QPushButton("%", this, "percent");
btn4        = new QPushButton("4", this, "4");
btn5        = new QPushButton("5", this, "5");
btn6        = new QPushButton("6", this, "6");
btnMinus    = new QPushButton("-", this, "-");
// 3rd line
btnF1       = new QPushButton("",this, "F1");
btn1        = new QPushButton("1", this, "1");
btn2        = new QPushButton("2", this, "2");
btn3        = new QPushButton("3", this, "3");
btnMultiply = new QPushButton("X", this, "X");
// 4th line
btnAbout    = new QPushButton("?", this, "About");
btn0        = new QPushButton("0", this, "0");
btnDot      = new QPushButton(".", this, "dot");
btnEqual    = new QPushButton("=", this, "equal");
btnDivide   = new QPushButton("/", this, "divide");

// Digit key
grpbtnDigits = new QButtonGroup(0, "digits");
grpbtnDigits->insert(btn0, 0);
grpbtnDigits->insert(btn1, 1);
grpbtnDigits->insert(btn2, 2);
grpbtnDigits->insert(btn3, 3);
grpbtnDigits->insert(btn4, 4);
grpbtnDigits->insert(btn5, 5);
grpbtnDigits->insert(btn6, 6);
grpbtnDigits->insert(btn7, 7);
grpbtnDigits->insert(btn8, 8);
grpbtnDigits->insert(btn9, 9);

// set appearance of buttons
for(i=0; i<10; i++){
    btnButton = grpbtnDigits->find(i);
    btnButton->setFixedSize(30,30);
}

// Operator key
grpbtnOperators = new QButtonGroup(0, "operator");
grpbtnOperators->insert(btnPlus, 1);
grpbtnOperators->insert(btnMinus,2);
grpbtnOperators->insert(btnMultiply,3);
grpbtnOperators->insert(btnDivide,4);
grpbtnOperators->insert(btnEqual,5);
// set appearance of buttons
for(i=1; i<6; i++){
    btnButton = grpbtnOperators->find(i);
    btnButton->setFixedSize(30,30);
}
btnClear->setFixedSize(30,30);
btnClear->setPalette(QPalette( QColor(255, 99, 71) ) );
btnDot->setFixedSize(30,30);
btnPercent->setFixedSize(30,30);
btnF1->setFixedSize(30,30);
btnAbout->setFixedSize(30,30);


// SIGNALS AND SLOTS
connect(grpbtnDigits, SIGNAL(clicked(int) ), this, SLOT(enterDigits(int)));
connect(grpbtnOperators, SIGNAL(clicked(int)), this, SLOT(operatorPressed(int)));
connect(btnClear, SIGNAL(clicked()), this, SLOT(clearLCD()));
connect(btnAbout, SIGNAL(clicked()), this, SLOT(aboutDialog()));
connect(btnDot, SIGNAL(clicked()), this, SLOT(gotoDec()));
}


/***********************************************************************
 * SLOT: enterDigits calculates the value to display and display it.
 **********************************************************************/
void KeyPad::enterDigits(int i){

if(!dCurrent)
    if(bIsDec) {
        dCurrent = i*dDecCurrent;
        dDecCurrent /= 10;
    }
    else
        dCurrent = i;
else if(iLenCurrent > 9)
        return;
else if(!bIsDec){
    dCurrent *= 10;
    dCurrent += i;
    iLenCurrent++;
}else{
    dCurrent += i*dDecCurrent;
    dDecCurrent /= 10;
}

display->setValue(dCurrent);
}

/***********************************************************************
 * SLOT: Operator has been pressed
 **********************************************************************/
void KeyPad::operatorPressed(int i){

if(iPreviousOperator){
    switch(iPreviousOperator){
        case 1: // Plus +
            dCurrent += dPreviousValue;
            break;

        case 2: // Minus -
            dCurrent = dPreviousValue - dCurrent;
            break;

        case 3: // Multiply *
            dCurrent *= dPreviousValue;
            break;

        case 4: // Divide /
            dCurrent = dPreviousValue / dCurrent;
            break;
            
    }
    display->setValue(dCurrent);
}

if(i==5){
    // ensure we won't be able to enter more digits
    iLenCurrent = 100;
    iPreviousOperator   = 0;
    dPreviousValue      = 0;
}else{
    dPreviousValue      = dCurrent;
    iPreviousOperator   = i;

    // reset LCD for next digit
    dCurrent            = 0;
    iLenCurrent         = 1;
    bIsDec              = false;
    dDecCurrent         = 0;
}
}
/***********************************************************************
 * SLOT: clearLCD  CE/C has been pressed
 **********************************************************************/
void KeyPad::clearLCD(void){

dCurrent            = 0;
iLenCurrent         = 1;
bIsDec              = false;
dDecCurrent         = 0;
iPreviousOperator   = 0;
dPreviousValue      = 0;
display->setValue(0);
}

/***********************************************************************
 * SLOT: gotoDec Dot has been pressed
 **********************************************************************/
void KeyPad::gotoDec(void){

    bIsDec      = true;
    dDecCurrent = 0.1;
}



/***********************************************************************
 * SLOT: Display About Dialog...
 **********************************************************************/
void KeyPad::aboutDialog(void){

QMessageBox::about( this, "About Euroconv",
                         "Euroconv is an Euro converter\n\n"
                         "Copyright 2003 TALC Informatique.\n"
                         "Under GPL license\n\n"
                         "Written by Eric Santonacci for Opie\n"
                         "http://opie.handhelds.org\n\n"
                         "Version 0.5\n\n"
                         "Any comment or feedback to:\n"
                         "Eric.Santonacci@talc.fr\n");

}
