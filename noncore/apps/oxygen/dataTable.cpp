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

#include <qpe/config.h>

#include "dataTable.h"
#include <qwidget.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qfont.h>
#include <qlayout.h>
#include <qpe/qpeapplication.h>

#include <qstringlist.h>

OxydataWidget::OxydataWidget(QWidget *parent, const char *name, const QStringList &list ) : QWidget( parent,name ), names( list )
{
    QGridLayout *qgrid = new QGridLayout( this, 2,1 );

    QHBox *hbox = new QHBox( this );
    left = new QLabel( hbox );
    middle = new QLabel( hbox );
    right = new QLabel( hbox );

    right->setAlignment( AlignRight );
    middle->setAlignment( AlignHCenter );

    QFont bf;
    bf.setBold( true );
    bf.setPointSize( bf.pointSize()+2 );
    middle->setFont( bf );

    DataTable = new OxydataTable( 9,2, this );
    setTable();

    qgrid->addWidget( hbox,0,0 );
    qgrid->addWidget( DataTable,1,0 );
}

void OxydataWidget::setElement( int el )
{
	QStringList::ConstIterator it = names.at(el);
    Config configobj( QPEApplication::qpeDir() +"share/oxygen/oxygendata", Config::File );

    configobj.setGroup( QString::number( el+1 ));

    left->setText( configobj.readEntry( "Symbol" ) );
    middle->setText( *it  );
    right->setText( QString::number( el+1 ) );


    DataTable->setText( 0,1,tr( "%1 u" ).arg( configobj.readEntry( "Weight" ) ) );
    DataTable->setText( 1,1,configobj.readEntry( "Block" ) );
    DataTable->setText( 2,1,configobj.readEntry( "Group" ) );
    DataTable->setText( 3,1,configobj.readEntry( "EN" ) );
    DataTable->setText( 4,1,tr( "%1 pm" ).arg( configobj.readEntry( "AR" ) ) ) ;
    DataTable->setText( 5,1,tr( "%1 J" ).arg( configobj.readEntry( "IE" ) ) );
    DataTable->setText( 6,1,tr( "%1 g/cm^3" ).arg( configobj.readEntry( "Density" ) ) );
    DataTable->setText( 7,1,tr( "%1 K" ).arg( configobj.readEntry( "BP" ) ) );
    DataTable->setText( 8,1,tr( "%1 K" ).arg( configobj.readEntry( "MP" ) ) );
}

void OxydataWidget::setTable() const
{
    DataTable->setText( 0,0, tr( "Weight:" ) );
    DataTable->setText( 1,0, tr( "Block" )) ;
    DataTable->setText( 2,0, tr( "Group" )) ;
    DataTable->setText( 3,0, tr( "Electronegativity" )) ;
    DataTable->setText( 4,0, tr( "Atomic radius" )) ;
    DataTable->setText( 5,0, tr( "Ionizationenergie" )) ;
    DataTable->setText( 6,0, tr( "Density" )) ;
    DataTable->setText( 7,0, tr( "Boilingpoint" ) );
    DataTable->setText( 8,0, tr( "Meltingpoint" ) );
}

void OxydataWidget::setLayout()
{
#warning this is not working and I have no idea why!
//    DataTable->setColumnWidth ( 0 , this->width()/2 );
//    DataTable->setColumnWidth ( 1 , this->width()/2 );

//X     DataTable->setColumnWidth ( 0 , 110 );
//X     DataTable->setColumnWidth ( 1 , 110 );
}

OxydataTable::OxydataTable(int numRows, int numCols, QWidget *parent,
        const char *name) : QTable(numRows, numCols,parent, name)
{

    for (int zeile = 0; zeile < numRows; zeile++)
        for ( int spalte = 0; spalte < numCols; spalte++ )
        {
            OxydataQTI *testus = new OxydataQTI (this, OxydataQTI::Never, "hm" );
            setItem(zeile, spalte, (QTableItem*)testus);
        }


    this->setShowGrid( false );
    this->setHScrollBarMode(QScrollView::AlwaysOff);
    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();
    this->setTopMargin( 0 );
    this->setLeftMargin( 0 );
}

void OxydataTable::paintCell(  QPainter *p, int row, int col, const QRect &cr, bool selected)
{
    if ( cr.width() == 0 || cr.height() == 0 )
        return;
    selected = FALSE;

    QTableItem *itm = item( row, col );
    QColorGroup colgrp = colorGroup();
    if ( itm )
    {
        if ( row%2 )
            colgrp.setColor(  QColorGroup::Base, QColor( 180,200,210 ) );
        else
            colgrp.setColor(  QColorGroup::Base, QColor( 230,235,235 ) );
        p->save();
        itm->paint( p, colgrp, cr, selected );
        p->restore();
    }
}

OxydataQTI::OxydataQTI(QTable * table, EditType et, const QString & text )
    : QTableItem ( table, et, text )
{
}

int OxydataQTI::alignment() const
{
    if ( col()%2 )
    {
        return AlignRight | AlignVCenter;
    }else return AlignLeft | AlignVCenter;
};

