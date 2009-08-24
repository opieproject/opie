/****************************************************************************
 *
 * File:        calcdisplay.h
 *
 * Description: Header file for the class LCDDisplay
 *
 *
 * Authors:     Eric Santonacci <Eric.Santonacci@talc.fr>
 *
 * Requirements:    Qt
 *
 * $Id: calcdisplay.h,v 1.3 2004-09-10 11:18:05 zecke Exp $
 *
 *
 ***************************************************************************/
#ifndef CALCDISPLAY_H
#define CALCDISPLAY_H

#include <qhbox.h>
#include <qlcdnumber.h>
#include <qhgroupbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>



class LCDDisplay : public QHBox{

Q_OBJECT
public:
    LCDDisplay( QWidget *parent=0, const char *name=0 );

public slots:
    void setValue(double);
    void swapLCD(void);
    void cbbxChange(void);

//signals:
//    void valueChanged( int );
private:
    int         grpbxStyle;

    QHGroupBox  *grpbxTop;
    QComboBox   *cbbxTop;
    QLCDNumber  *lcdTop;

    QHGroupBox  *grpbxBottom;
    QComboBox   *cbbxBottom;
    QLCDNumber  *lcdBottom;

    QPushButton *btnSwap;

    int iCurrentLCD; // 0=top, 1=bottom

    double  Euro2x(int iIndex, double dValue);
    double  x2Euro(int iIndex, double dValue);


};

#endif // CALCDISPLAY_H
