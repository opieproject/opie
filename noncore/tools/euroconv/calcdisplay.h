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
 * $Id: calcdisplay.h,v 1.2 2003-02-21 10:39:29 eric Exp $
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


/* XPM */
static char *swap_xpm[] = {
/* width height num_colors chars_per_pixel */
"    13    18        2            1",
/* colors */
". c None",
"# c #000000",
/* pixels */
"..#######....",
"..#####......",
"..######.....",
"..#...###....",
"........##...",
".........##..",
"..........##.",
"...........##",
"...........##",
"...........##",
"...........##",
"..........##.",
".........##..",
"........##...",
"..#...###....",
"..######.....",
"..#####......",
"..#######....",
};


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
