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
#include <qvbox.h>
#include <qlabel.h>
#include <qfont.h>
#include <qlayout.h>

OxydataWidget::OxydataWidget(QWidget *parent) : QWidget(parent)
{
    QGridLayout *qgrid = new QGridLayout( this, 2,1 );
    QHBox *hbox = new QHBox( this );
    left = new QLabel( hbox );
    middle = new QLabel( hbox );
    right = new QLabel( hbox );
    
    DataTable = new OxydataTable( 9,2, this );

    QFont bf;
    bf.setBold( true );
    bf.setPointSize( bf.pointSize()+2 );
    middle->setFont( bf );

    setTable();

    qgrid->addWidget( hbox,0,0 );    
    qgrid->addWidget( DataTable,1,0 );    
}

void OxydataWidget::setElement( int el )
{
    Config test( "/home/opie/Settings/oxygendata", Config::File );
    test.setGroup( QString::number( el+1 ));
    
    left->setText( test.readEntry( "Symbol" ) );
    middle->setText( test.readEntry( "Name" ) );
    right->setText( QString::number( el+1 ) );

    QString weight = test.readEntry( "Weight" );
    DataTable->setText( 0,1,weight ); 
    QString block = test.readEntry( "Block" );
    DataTable->setText( 1,1,block ); 
    QString grp = test.readEntry( "Group" );
    DataTable->setText( 2,1,grp ); 
    QString en = test.readEntry( "EN" );
    DataTable->setText( 3,1,en ); 
    QString ar = test.readEntry( "AR" );
    DataTable->setText( 4,1,ar ) ; 
    QString ion = test.readEntry( "IE" );
    DataTable->setText( 5,1,ion ); 
    QString dens = test.readEntry( "Density" );
    DataTable->setText( 6,1,dens ); 
    QString bp = test.readEntry( "BP" );
    DataTable->setText( 7,1,bp ); 
    QString mp = test.readEntry( "MP" );
    DataTable->setText( 8,1,mp ); 
}

void OxydataWidget::setTable()
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

OxydataTable::OxydataTable(int numRows, int numCols, QWidget *parent,
        const char *name) : QTable(numRows, numRows, parent, name)
{
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
            colgrp.setColor(  QColorGroup::Base, QColor( cyan ) );
        else
            colgrp.setColor(  QColorGroup::Base, QColor( white ) );
        p->save();
        itm->paint( p, colgrp, cr, selected );
        p->restore();
    }
}
