/***************************************************************************
   application:             : Oxygen

   begin                    : September 2002
   copyright                : ( C ) 2002 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "oxygen.h"

#include <qapplication.h>
#include <qtabwidget.h>

#include "calcdlg.h"
#include "calcdlgui.h"
#include "datawidgetui.h"
#include "psewidget.h"


Oxygen::Oxygen() : QMainWindow()
{
    loadNames();
    calcDlgUI *CalcDlgUI = new calcDlgUI();
    PSEWidget *pse = new PSEWidget(names);
    dataWidgetUI *DataWidgetUI = new dataWidgetUI(names);
    
    this->setCaption( tr( "Oxygen" ) );
    
    QTabWidget *tabw = new QTabWidget( this , "qtab" );
    tabw->addTab( pse, tr( "PSE" ));
    tabw->addTab( DataWidgetUI , tr( "Data" ) );
    tabw->addTab( CalcDlgUI, tr( "Calculations" ) );
    setCentralWidget( tabw );
}

void Oxygen::loadNames()
{
	names.clear();
	names.append( tr("Hydrogen") );
	names.append( tr("Helium") );
	names.append( tr("Lithium") );
	names.append( tr("Beryllium") );
	names.append( tr("Boron") );
	names.append( tr("Carbon") );
	names.append( tr("Nitrogen") );
	names.append( tr("Oxygen") );
	names.append( tr("Fluorine") );
	names.append( tr("Neon") );
	names.append( tr("Sodium") );
	names.append( tr("Magnesium") );
	names.append( tr("Aluminum") );
	names.append( tr("Silicon") );
	names.append( tr("Phosphorus") );
	names.append( tr("Sulfur") );
	names.append( tr("Chlorine") );
	names.append( tr("Argon") );
	names.append( tr("Potassium") );
	names.append( tr("Calcium") );
	names.append( tr("Scandium") );
	names.append( tr("Titanium") );
	names.append( tr("Vanadium") );
	names.append( tr("Chromium") );
	names.append( tr("Manganese") );
	names.append( tr("Iron") );
	names.append( tr("Cobalt") );
	names.append( tr("Nickel") );
	names.append( tr("Copper") );
	names.append( tr("Zinc") );
	names.append( tr("Gallium") );
	names.append( tr("Germanium") );
	names.append( tr("Arsenic") );
	names.append( tr("Selenium") );
	names.append( tr("Bromine") );
	names.append( tr("Krypton") );
	names.append( tr("Rubidium") );
	names.append( tr("Strontium") );
	names.append( tr("Yttrium") );
	names.append( tr("Zirconium") );
	names.append( tr("Niobium") );
	names.append( tr("Molybdenum") );
	names.append( tr("Technetium") );
	names.append( tr("Ruthenium") );
	names.append( tr("Rhodium") );
	names.append( tr("Palladium") );
	names.append( tr("Silver") );
	names.append( tr("Cadmium") );
	names.append( tr("Indium") );
	names.append( tr("Tin") );
	names.append( tr("Antimony") );
	names.append( tr("Tellurium") );
	names.append( tr("Iodine") );
	names.append( tr("Xenon") );
	names.append( tr("Cesium") );
	names.append( tr("Barium") );
	names.append( tr("Lanthanum") );
	names.append( tr("Cerium") );
	names.append( tr("Praseodymium") );
	names.append( tr("Neodymium") );
	names.append( tr("Promethium") );
	names.append( tr("Samarium") );
	names.append( tr("Europium") );
	names.append( tr("Gadolinium") );
	names.append( tr("Terbium") );
	names.append( tr("Dysprosium") );
	names.append( tr("Holmium") );
	names.append( tr("Erbium") );
	names.append( tr("Thulium") );
	names.append( tr("Ytterbium") );
	names.append( tr("Lutetium") );
	names.append( tr("Hafnium") );
	names.append( tr("Tantalum") );
	names.append( tr("Tungsten") );
	names.append( tr("Rhenium") );
	names.append( tr("Osmium") );
	names.append( tr("Iridium") );
	names.append( tr("Platinum") );
	names.append( tr("Gold") );
	names.append( tr("Mercury") );
	names.append( tr("Thallium") );
	names.append( tr("Lead") );
	names.append( tr("Bismuth") );
	names.append( tr("Polonium") );
	names.append( tr("Astatine") );
	names.append( tr("Radon") );
	names.append( tr("Francium") );
	names.append( tr("Radium") );
	names.append( tr("Actinium") );
	names.append( tr("Thorium") );
	names.append( tr("Protactinium") );
	names.append( tr("Uranium") );
	names.append( tr("Neptunium") );
	names.append( tr("Plutonium") );
	names.append( tr("Americium") );
	names.append( tr("Curium") );
	names.append( tr("Berkelium") );
	names.append( tr("Californium") );
	names.append( tr("Einsteinium") );
	names.append( tr("Fermium") );
	names.append( tr("Mendelevium") );
	names.append( tr("Nobelium") );
	names.append( tr("Lawrencium") );
	names.append( tr("Rutherfordium") );
	names.append( tr("Dubnium") );
	names.append( tr("Seaborgium") );
	names.append( tr("Bohrium") );
	names.append( tr("Hassium") );
	names.append( tr("Meitnerium") );
}

