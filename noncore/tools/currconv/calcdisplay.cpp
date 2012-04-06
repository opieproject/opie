/****************************************************************************
 *
 * File:        calcdisplay.cpp
 *
 * Description:
 *
 *
 * Authors:     Eric Santonacci <Eric.Santonacci@talc.fr>
 *              Paul Eggleton <bluelightning@bluelightning.org>
 *
 * Requirements:    Qt
 *
 ***************************************************************************/

#include <qvbox.h>
#include <qpixmap.h>

#include "calcdisplay.h"

/* XPM */
static const char *swap_xpm[] = {
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


LCDDisplay::LCDDisplay( QWidget *parent, const char *name )
        : QHBox( parent, name )
{
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

    m_curr.loadRates();
    refreshRates();
}

/***********************************************************************
 * SLOT: Display value in the correct LCD
 **********************************************************************/
void LCDDisplay::setValue(double dSrcValue)
{

    double  dDstValue=0;

    QString rateFrom;
    QString rateTo;


    // get item index of the focused
    if(!iCurrentLCD) {
        rateFrom = cbbxTop->currentText();
        rateTo = cbbxBottom->currentText();
    }
    else {
        rateFrom = cbbxBottom->currentText();
        rateTo = cbbxTop->currentText();
    }

    if(rateFrom == rateTo) {
        dDstValue = dSrcValue;
    }
    else {
        dDstValue = m_curr.convertRate( rateFrom, rateTo, dSrcValue );
    }


    if(!iCurrentLCD) {
        lcdTop->display(dSrcValue);
        lcdBottom->display(dDstValue);
    }
    else {
        lcdBottom->display(dSrcValue);
        lcdTop->display(dDstValue);
    }

}

/***********************************************************************
 * SLOT: Swap output keypad between LCD displays
 **********************************************************************/
void LCDDisplay::swapLCD(void)
{
    double dCurrentValue;

    // get current value
    if(!iCurrentLCD) {
        // iCurrentLCD = 0, lcdTop has current focus and is going to loose
        // it
        dCurrentValue = lcdTop->value();
        iCurrentLCD = 1;
        grpbxTop->setFrameStyle(0);
        grpbxBottom->setFrameStyle(grpbxStyle);
    }
    else {
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
void LCDDisplay::cbbxChange(void)
{
    double dCurrentValue;

    // get current value
    if(!iCurrentLCD) {
        dCurrentValue = lcdTop->value();
    }
    else {
        dCurrentValue = lcdBottom->value();
    }

    setValue(dCurrentValue);
}

void LCDDisplay::refreshRates()
{
    QStringList codes = m_curr.rateCodes();
    updateRateCombo(cbbxTop, codes);
    updateRateCombo(cbbxBottom, codes);
}

void LCDDisplay::updateRateCombo( QComboBox *combo, const QStringList &lst )
{
    QString prev = combo->currentText();
    combo->clear();
    for( QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it ) {
        combo->insertItem(*it, -1);
        if( *it == prev )
            combo->setCurrentItem( combo->count() - 1 );
    }
}
