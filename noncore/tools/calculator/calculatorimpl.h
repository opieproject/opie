/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef CALCULATORIMPL_H
#define CALCULATORIMPL_H


#include <qlcdnumber.h>
#include "calculator.h"
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qvaluestack.h>

// mode x functions
enum Operation {
     oNop,
     oOpenBrace,
     oCloseBrace,
     oSum,
     oPoint,
     oAdd,
     oSub,
     oDiv,
     oMult,

// mode 0 functions
     oSin,
     oCos,
     oTan,
     oDivX,
     oPercent,
     oXsquared,
     oRoot,
     oLog,
     oLn,
     oChSign
};

// states
#define sStart 0
#define sNewNumber 1
#define sError 2

struct Op
{
    Op() { number = 0; operation = oNop; }
    Op( double num, Operation op ) 
    { number = num; operation = op; }
    double number;
    Operation operation;
};

class QLabel;
class CalculatorImpl : public Calculator
{
    Q_OBJECT

public:
    CalculatorImpl( QWidget * parent = 0, const char * name = 0,
                WFlags f = 0 );

public slots:
    void command_buttons(int);
    void enterNumber(int i);
    void std_buttons(int);
    void std_funcs(int);
    void do_convert(int);
    void function_button(int);

protected:
    virtual bool eventFilter( QObject *o, QEvent *e );

private:
    void clear();

    void reset_conv();
    
    void processStack( int op );

    QValueStack<Op> operationStack;
    int state;

    double acc, num, mem;
    int    numDecimals;
    bool   flPoint;
    int numOpenBraces;

    void   execOp( Operation i );
    double evalExpr( int op );
    QLabel * memMark;
    QString fake;
    
    // useful values for conversion stuff
    int current_mode, max_mode, conversion_mode_count, last_conversion;

    // make adding new modes easier for ourselves
    static const int pre_conv_modes_count = 1;
    static const int post_conv_modes_count = 0;

    // an array of pointers to the func buttons
    static const int func_button_count = 12;
	// this is an abomination
	static const int changeable_func_button_count = 10;
    QPushButton* func_buttons[func_button_count];

    QButtonGroup bgr_function, bgr_digits, bgr_std, bgr_command;
    QStringList faces, captions;

    // an array of doubles holding the conversion ratios
    double* entry_list;

    QPixmap xtopowerofy;
    QPixmap ythrootofx;
    QPixmap oneoverx;

    void display_pixmap_faces(void);
};

#endif
