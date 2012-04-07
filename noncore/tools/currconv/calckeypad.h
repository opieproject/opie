/****************************************************************************
 *
 * File:        calckeypad.h
 *
 * Description: Header file for the class KeyPad
 *
 *
 * Authors:     Eric Santonacci <Eric.Santonacci@talc.fr>
 *
 * Requirements:    Qt
 *
 * $Id: calckeypad.h,v 1.2 2003-02-21 10:39:30 eric Exp $
 *
 *
 ***************************************************************************/
#ifndef CALCKEYPAD_H
#define CALCKEYPAD_H

#include <qpushbutton.h>
#include <qgrid.h>
#include <qbuttongroup.h>
#include <qintdict.h>

#include "calcdisplay.h"

class KeyPad : public QGrid {

Q_OBJECT
public:
    KeyPad(LCDDisplay *LCD, QWidget *parent=0, const char *name=0 );

public slots:
    void enterDigits(int);
    void operatorPressed(int);
    void clearLCD(void);
    void backspace(void);
    void aboutDialog(void);
    void gotoDec(void);
    void slotKeyPressed(int);
    void slotKeyReleased(int);

private:
    LCDDisplay  *display;
    double      dCurrent;
    int         iLenCurrent;
    bool        bIsDec;
    double      dDecCurrent;
    int         iPreviousOperator;
    double      dPreviousValue;

    QPushButton *btn0;
    QPushButton *btn1;
    QPushButton *btn2;
    QPushButton *btn3;
    QPushButton *btn4;
    QPushButton *btn5;
    QPushButton *btn6;
    QPushButton *btn7;
    QPushButton *btn8;
    QPushButton *btn9;
    QButtonGroup *grpbtnDigits;

    QPushButton *btnDot;

    QPushButton *btnClear;

    QPushButton *btnPlus;
    QPushButton *btnMinus;
    QPushButton *btnMultiply;
    QPushButton *btnDivide;
    QPushButton *btnEqual;
    QPushButton *btnPercent;
    QPushButton *btnBackspace;
    QPushButton *btnAbout;

    QButtonGroup *grpbtnOperators;

    QIntDict<QPushButton> m_buttonKeys;

    void setAppearance(void);

};

#endif // CALCKEYPAD_H
