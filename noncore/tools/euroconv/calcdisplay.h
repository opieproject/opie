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
 * $Id: calcdisplay.h,v 1.1 2003-02-15 11:45:26 groucho Exp $
 *
 *
 ***************************************************************************/
#ifndef CALCDISPLAY_H
#define CALCDISPLAY_H

#include <qhbox.h>
#include <qlcdnumber.h>
#include <qvgroupbox.h>
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
    QVGroupBox  *grpbxTop;
    QComboBox   *cbbxTop;
    QLCDNumber  *lcdTop;
    
    QVGroupBox  *grpbxBottom;
    QComboBox   *cbbxBottom;
    QLCDNumber  *lcdBottom;
    
    QPushButton *btnSwap;

    int iCurrentLCD; // 0=top, 1=bottom

    double  Euro2x(int iIndex, double dValue);
    double  x2Euro(int iIndex, double dValue);

    
};

#endif // CALCDISPLAY_H
