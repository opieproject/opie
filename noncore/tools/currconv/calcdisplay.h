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
 ***************************************************************************/
#ifndef CALCDISPLAY_H
#define CALCDISPLAY_H

#include "currency.h"

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

    CurrencyConverter m_curr;

    void refreshRates();
    void updateRateCombo( QComboBox *combo, const QStringList &lst );
};

#endif // CALCDISPLAY_H
