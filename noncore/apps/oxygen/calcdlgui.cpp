/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "oxygen.h"

#include "calcdlgui.h"
#include "kmolcalc.h"
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>

calcDlgUI::calcDlgUI() : CalcDlg()
{
    this->showMaximized();
    kmolcalc = new KMolCalc;
    connect( calculate, SIGNAL( clicked() ), this, SLOT( calc() ) );
    connect( clear_fields, SIGNAL( clicked() ), this, SLOT( clear() ) );
    result->setReadOnly( true );
}

void calcDlgUI::calc()
{
    QString compound( formula->text() );
    if ( compound.isEmpty() ) {
        clear();
        return;
    }
    QString errors( kmolcalc->readFormula( compound ) );
    QString mw, ea;
    double weight = kmolcalc->getWeight();
    if ( errors == "OK" ) {
        mw.setNum( weight );
        ea = kmolcalc->getEmpFormula() + " :\n" + kmolcalc->getEA();
    } else {
        mw = "???";
        ea = tr( "ERROR: \n" ) + errors + "\n";
    }
    result->setText( mw );
    anal_display->setText( ea );
}

/**
 * * Clear all text entry / result fields.
 * */
void calcDlgUI::clear()
{
    formula->clear();
    result->clear();
    anal_display->clear();
}


