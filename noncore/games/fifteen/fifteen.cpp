/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#include "fifteen.h"

#include <qpe/resource.h>
#include <qpe/config.h>

#include <qvbox.h>
#include <qaction.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpemenubar.h>
#include <qstringlist.h>
#include <qapplication.h>

#include <stdlib.h>
#include <time.h>

FifteenMainWindow::FifteenMainWindow(QWidget *parent, const char* name)
  : QMainWindow( parent, name )
{
  // random seed
  srand(time(0));

  setToolBarsMovable( FALSE );
  QVBox *vbox = new QVBox( this );
  PiecesTable *table = new PiecesTable( vbox );
  setCentralWidget(vbox);

  QPEToolBar *toolbar = new QPEToolBar(this);
  toolbar->setHorizontalStretchable( TRUE );
  addToolBar(toolbar);

  QPEMenuBar *menubar = new QPEMenuBar( toolbar );
  menubar->setMargin(0);

  QPopupMenu *game = new QPopupMenu( this );

  QWidget *spacer = new QWidget( toolbar );
  spacer->setBackgroundMode( PaletteButton );
  toolbar->setStretchableWidget( spacer );

  QAction *a = new QAction( tr( "Randomize" ), Resource::loadPixmap( "new" ),
			    QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), table, SLOT( slotRandomize() ) );
  a->addTo( game );
  a->addTo( toolbar );

  a = new QAction( tr( "Solve" ), Resource::loadPixmap( "repeat" ),
		   QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), table, SLOT( slotReset() ) );
  a->addTo( game );
  a->addTo( toolbar );

  menubar->insertItem( tr( "Game" ), game );
}

PiecesTable::PiecesTable(QWidget* parent, const char* name )
  : QTableView(parent, name), _menu(0), _randomized(false)
{
  // setup table view
  setFrameStyle(StyledPanel | Sunken);
  setBackgroundMode(NoBackground);
  setMouseTracking(true);

  setNumRows(4);
  setNumCols(4);

  // init arrays
  initMap();
  readConfig();
  initColors();

  // set font
  QFont f = font();
  f.setPixelSize(18);
  f.setBold( TRUE );
  setFont(f);
}

PiecesTable::~PiecesTable()
{
  writeConfig();
}

void PiecesTable::writeConfig()
{
  Config cfg("Fifteen");
  cfg.setGroup("Game");
  QStringList map;
  for (unsigned int i = 0; i < 16; i++)
    map.append( QString::number( _map[i] ) );
  cfg.writeEntry("Map", map, '-');
  cfg.writeEntry("Randomized", _randomized );
}

void PiecesTable::readConfig()
{
  Config cfg("Fifteen");
  cfg.setGroup("Game");
  QStringList map = cfg.readListEntry("Map", '-');
  _randomized = cfg.readBoolEntry( "Randomized", FALSE );
  unsigned int i = 0;
  for ( QStringList::Iterator it = map.begin(); it != map.end(); ++it ) {
    _map[i] = (*it).toInt();
    i++;
    if ( i > 15 ) break;
  }
}

void PiecesTable::paintCell(QPainter *p, int row, int col)
{
  int w = cellWidth();
  int h = cellHeight();
  int x2 = w - 1;
  int y2 = h - 1;

  int number = _map[col + row * numCols()] + 1;

  // draw cell background
  if(number == 16)
    p->setBrush(colorGroup().background());
  else
    p->setBrush(_colors[number-1]);
  p->setPen(NoPen);
  p->drawRect(0, 0, w, h);

  // draw borders
  if (height() > 40) {
    p->setPen(colorGroup().text());
    if(col < numCols()-1)
      p->drawLine(x2, 0, x2, y2); // right border line

    if(row < numRows()-1)
      p->drawLine(0, y2, x2, y2); // bottom boder line
  }

  // draw number
  if (number == 16) return;
  p->setPen(black);
  p->drawText(0, 0, x2, y2, AlignHCenter | AlignVCenter, QString::number(number));
}

void PiecesTable::resizeEvent(QResizeEvent *e)
{
  QTableView::resizeEvent(e);

  setCellWidth(contentsRect().width()/ numRows());
  setCellHeight(contentsRect().height() / numCols());
}

void PiecesTable::initColors()
{
  _colors.resize(numRows() * numCols());
  for (int r = 0; r < numRows(); r++)
    for (int c = 0; c < numCols(); c++)
      _colors[c + r *numCols()] = QColor(255 - 70 * c,255 - 70 * r, 150);
}

void PiecesTable::initMap()
{
  _map.resize(16);
  for (unsigned int i = 0; i < 16; i++)
    _map[i] = i;

  _randomized = false;
}

void PiecesTable::randomizeMap()
{
  initMap();
  _randomized = true;
  // find the free position
  int pos = _map.find(15);

  int move = 0;
  while ( move < 333 ) {

    int frow = pos / numCols();
    int fcol = pos - frow * numCols();

    // find click position
    int row = rand()%4;
    int col = rand()%4;

    // sanity check
    if ( row < 0 || row >= numRows() ) continue;
    if ( col < 0 || col >= numCols() ) continue;
    if ( row != frow && col != fcol ) continue;

    move++;

    // rows match -> shift pieces
    if(row == frow) {

      if (col < fcol) {
	for(int c = fcol; c > col; c--) {
	  _map[c + row * numCols()] = _map[ c-1 + row *numCols()];
	}
      }
      else if (col > fcol) {
	for(int c = fcol; c < col; c++) {
	  _map[c + row * numCols()] = _map[ c+1 + row *numCols()];
	}
      }
    }
    // cols match -> shift pieces
    else if (col == fcol) {

      if (row < frow) {
	for(int r = frow; r > row; r--) {
	  _map[col + r * numCols()] = _map[ col + (r-1) *numCols()];
	}
      }
      else if (row > frow) {
	for(int r = frow; r < row; r++) {
	  _map[col + r * numCols()] = _map[ col + (r+1) *numCols()];
	}
      }
    }
    // move free cell to click position
    _map[pos=(col + row * numCols())] = 15;
    repaint();
  }
}

void PiecesTable::checkwin()
{
  if(!_randomized) return;

  int i;
  for (i = 0; i < 16; i++)
    if(i != _map[i])
      break;

  if (i == 16) {
    QMessageBox::information(this, tr("Fifteen Pieces"),
			     tr("Congratulations!\nYou win the game!"));
    _randomized = FALSE;
  }

}

void PiecesTable::slotRandomize()
{
  randomizeMap();
}

void PiecesTable::slotReset()
{
  initMap();
  repaint();
}

void PiecesTable::mousePressEvent(QMouseEvent* e)
{
  QTableView::mousePressEvent(e);

  if (e->button() == RightButton) {

    // setup RMB pupup menu
    if(!_menu) {
      _menu = new QPopupMenu(this);
      _menu->insertItem(tr("R&andomize Pieces"), mRandomize);
      _menu->insertItem(tr("&Reset Pieces"), mReset);
      _menu->adjustSize();
    }

    // execute RMB popup and check result
    switch(_menu->exec(mapToGlobal(e->pos()))) {
    case mRandomize:
      randomizeMap();
      break;
    case mReset:
      initMap();
      repaint();
      break;
    default:
      break;
    }
  }
  else {
    // GAME LOGIC

    // find the free position
    int pos = _map.find(15);
    if(pos < 0) return;

    int frow = pos / numCols();
    int fcol = pos - frow * numCols();

    // find click position
    int row = findRow(e->y());
    int col = findCol(e->x());

    // sanity check
    if (row < 0 || row >= numRows()) return;
    if (col < 0 || col >= numCols()) return;
    if ( row != frow && col != fcol ) return;

    // valid move?
    if(row != frow && col != fcol) return;

    // rows match -> shift pieces
    if(row == frow) {

      if (col < fcol) {
	for(int c = fcol; c > col; c--) {
	  _map[c + row * numCols()] = _map[ c-1 + row *numCols()];
	  updateCell(row, c, false);
	}
      }
      else if (col > fcol) {
	for(int c = fcol; c < col; c++) {
	  _map[c + row * numCols()] = _map[ c+1 + row *numCols()];
	  updateCell(row, c, false);
	}
      }
    }
    // cols match -> shift pieces
    else if (col == fcol) {

      if (row < frow) {
	for(int r = frow; r > row; r--) {
	  _map[col + r * numCols()] = _map[ col + (r-1) *numCols()];
	  updateCell(r, col, false);
	}
      }
      else if (row > frow) {
	for(int r = frow; r < row; r++) {
	  _map[col + r * numCols()] = _map[ col + (r+1) *numCols()];
	  updateCell(r, col, false);
	}
      }
    }
    // move free cell to click position
    _map[col + row * numCols()] = 15;
    updateCell(row, col, false);

    // check if the player wins with this move
    checkwin();
  }
}
