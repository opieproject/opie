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

#include "fifteenconfigdialog.h"

#include <opie2/ofileselector.h>

#include <qtopia/resource.h>
#include <qtopia/config.h>
#include <qtopia/qpeapplication.h>

#include <qvbox.h>
#include <qaction.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qimage.h>

#include <stdlib.h>
#include <time.h>

FifteenMainWindow::FifteenMainWindow(QWidget *parent, const char* name, WFlags fl)
  : QMainWindow( parent, name, fl )
{

  // random seed
  srand(time(0));
  setCaption( tr("Fifteen Pieces") );

  QToolBar *toolbar = new QToolBar(this);
  toolbar->setHorizontalStretchable( FALSE );
  QMenuBar *menubar = new QMenuBar( toolbar );
  menubar->setMargin(0);
  QPopupMenu *game = new QPopupMenu( this );

  QWidget *spacer = new QWidget( toolbar );
  spacer->setBackgroundMode( PaletteButton );
  toolbar->setStretchableWidget( spacer );


  setToolBarsMovable( FALSE );
  QVBox *vbox = new QVBox( this );
  PiecesTable *table = new PiecesTable( vbox );
  setCentralWidget(vbox);



  QAction *a = new QAction( tr( "Randomize" ), Resource::loadPixmap( "new" ),
			    QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), table, SLOT( slotRandomize() ) );
  a->addTo( game );
  a->addTo( toolbar );


  a  = new QAction( tr("Configure"), Resource::loadPixmap( "SettingsIcon" ),
                    QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated()), table, SLOT( slotConfigure()) );
  a->addTo( game );

    /* This is pointless and confusing.
  a = new QAction( tr( "Solve" ), Resource::loadIconSet( "repeat" ),
		   QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), table, SLOT( slotReset() ) );
  a->addTo( game );
  a->addTo( toolbar );
    */
  menubar->insertItem( tr( "Game" ), game );
}




///////////////
///////  Pieces table Implementation
///////
PiecesTable::PiecesTable(QWidget* parent, const char* name )
  : QTableView(parent, name), _menu(0), _randomized(false),
    _dialog( 0l )
{
  // setup table view
  setFrameStyle(StyledPanel | Sunken);
  setBackgroundMode(NoBackground);
  setMouseTracking(true);

  setNumRows(4);
  setNumCols(4);

  // init arrays
  readConfig();
  initColors();

}


PiecesTable::~PiecesTable()
{
  writeConfig();
  clear();
}

void PiecesTable::writeConfig()
{
  Config cfg("Fifteen");
  cfg.setGroup("Game");
  QStringList map;

  int items = numRows()*numCols();

  for (int i = 0; i < items; i++)
    map.append( QString::number( _map[i] ) );

  cfg.writeEntry("Map", map, '-');
  cfg.writeEntry("Randomized", _randomized );
  cfg.writeEntry("Image", _image );
  cfg.writeEntry("Rows", numRows() );
  cfg.writeEntry("Cols", numCols() );
}

void PiecesTable::readConfig()
{
  Config cfg("Fifteen");
  cfg.setGroup("Game");
  QStringList map = cfg.readListEntry("Map", '-');
  _randomized = cfg.readBoolEntry( "Randomized", FALSE );
  _image = cfg.readEntry( "Image", QString::null );

  int rows = cfg.readNumEntry( "Rows", 4 );
  int cols = cfg.readNumEntry( "Cols", 4 );
  uint items= rows*cols;
  setNumRows( rows );
  setNumCols( cols );

  initMap();

  /* if we've more items than 'stones' don't restore the state */
  if ( items > map.count() )
      return;


  uint i = 0;
  for ( QStringList::Iterator it = map.begin(); it != map.end(); ++it ) {
    _map[i] = (*it).toInt();
    i++;
    if ( i > items ) break;
  }

}


void PiecesTable::clear() {
    /* clean up and resize */
    for (uint i = 0; i < _pixmap.count(); ++i )
        delete _pixmap[i];
    _pixmap.resize( numRows()*numCols() );
}

/*
 * Let us pre-render the tiles. Either we've a Custom Image as
 * background or we use the drawRect  to fill the background and
 * last we put the number on it
 */
void PiecesTable::slotCustomImage( const QString& _str ) {
    QString str = _str;


    /* couldn't load image fall back to plain tiles*/
    QImage img = QImage(str);
    QPixmap pix;
    if(img.isNull())
        str = QString::null;
    else{
        img = img.smoothScale( width(),height() );
        pix.convertFromImage( img );
    }

    /* initialize base point */
    uint image=0;

    /* clear the old tiles */
    clear();

    /* used variables */
    int cols = numCols();
    int rows = numRows();
    int cellW   = cellWidth();
    int cellH   = cellHeight();
    int x2      = cellW-1;
    int y2      = cellH-1;
    bool empty  = str.isEmpty();
    double bw      = empty ? 0.9 : 0.98;
    int	x_offset = cellW - int(cellW * bw);	// 10% should be enough
    int	y_offset = cellH - int(cellH * bw);

    /* border polygon calculation*/
    initPolygon(cellW, cellH, x_offset, y_offset );

    /* avoid crashes with isNull() pixmap later */
    if ( cellW == 0 || cellH == 0 ) {
        _pixmap.resize( 0 );
        return;
    }

    /* make it bold and bigger */
    QFont f = font();
    f.setPixelSize(18);
    f.setBold( TRUE );

    /* for every tile */
    for(int row = 0; row < rows; ++row ) {
        for(int col= 0; col < cols; ++col) {
            QPixmap *pip = new QPixmap(cellW, cellH );
            QPainter *p = new QPainter(pip );
            p->setFont( f );

            /* draw the tradional tile  or a part of the pixmap*/
            if(empty) {
                p->setBrush(_colors[image]);
                p->setPen(NoPen);
                p->drawRect(0,0,cellW,cellH);
            }else
                p->drawPixmap(0, 0, pix,col*cellW, row*cellH, cellW, cellH );

            // draw borders
            if (height() > 40) {
                p->setBrush(_colors[image].light(130));
                p->drawPolygon(light_border);

                p->setBrush(_colors[image].dark(130));
                p->drawPolygon(dark_border);
            }

            // draw number
            p->setPen(black);
            p->drawText(0, 0, x2, y2, AlignHCenter | AlignVCenter, QString::number(image+1));

            delete p;
            _pixmap[image++] =  pip;
        }
    }
    _image = str;
}

/*
 * Calculate 3d-effect borders
 */
void PiecesTable::initPolygon(int cell_w, int cell_h, int x_offset, int y_offset ) {
    light_border.setPoints(6,
                           0, 0,
                           cell_w, 0,
                           cell_w - x_offset, y_offset,
                           x_offset, y_offset,
                           x_offset, cell_h - y_offset,
                           0, cell_h);

    dark_border.setPoints(6,
                          cell_w, 0,
                          cell_w, cell_h,
                          0, cell_h,
                          x_offset, cell_h - y_offset,
                          cell_w - x_offset, cell_h - y_offset,
                          cell_w - x_offset, y_offset);
}

void PiecesTable::paintCell(QPainter *p, int row, int col)
{
  int w = cellWidth();
  int h = cellHeight();

  uint pos = col+row*numCols();

  /* sanity check. setNumRows()/setNumCols() calls repaint() directly */
  if ( pos >= _map.count() ) {
      p->drawRect(0, 0, w, h);
      return;
  }

  int number = _map[col + row * numCols()] + 1;

  // draw cell background
  if(number == numCols()*numRows() ) {
    p->setBrush(colorGroup().background());
    p->setPen(NoPen);
    p->drawRect(0, 0, w, h);
    return;
  }

  /* no tiles then contentRect() is not visible or too small anyway */
  if( _pixmap.count() == 0 )
      return;

  p->drawPixmap(0, 0, *(_pixmap[(number-1 )]) );
}

void PiecesTable::resizeEvent(QResizeEvent *e)
{
  /*
   * null if we faked it after the config dialog ran to
   * regenerate everything
   */
  if ( e )
      QTableView::resizeEvent(e);

  setCellWidth(contentsRect().width()/ numCols());
  setCellHeight(contentsRect().height() / numRows());


  /* update the image and calculate border*/
  slotCustomImage( _image );

}

void PiecesTable::initColors()
{
  _colors.resize(numRows() * numCols());
  for (int r = 0; r < numRows(); r++)
    for (int c = 0; c < numCols(); c++)
      _colors[c + r *numCols()] = QColor( 255 - (70 * c)%255 ,255 - (70 * r)%255, 150);
}

void PiecesTable::initMap()
{
  int items = numCols()*numRows();
  _map.resize( items );
  for ( int i = 0; i < items; i++)
    _map[i] = i;

  _randomized = false;
}

void PiecesTable::randomizeMap()
{
  initMap();
  _randomized = true;
  // find the free position
  int cols = numCols();
  int rows = numRows();
  int pos = _map.find( cols*rows -1 );

  int move = 0;
  while ( move < 333 ) {

    int frow = pos / cols;
    int fcol = pos - frow * cols;

    // find click position
    int row = rand()%rows;
    int col = rand()%cols;

    // sanity check
    if ( row < 0 || row >= rows ) continue;
    if ( col < 0 || col >= cols ) continue;
    if ( row != frow && col != fcol ) continue;

    move++;

    // rows match -> shift pieces
    if(row == frow) {

      if (col < fcol) {
	for(int c = fcol; c > col; c--) {
	  _map[c + row * cols] = _map[ c-1 + row *cols];
	}
      }
      else if (col > fcol) {
	for(int c = fcol; c < col; c++) {
	  _map[c + row * cols] = _map[ c+1 + row *cols];
	}
      }
    }
    // cols match -> shift pieces
    else if (col == fcol) {

      if (row < frow) {
	for(int r = frow; r > row; r--) {
	  _map[col + r * cols] = _map[ col + (r-1) *cols];
	}
      }
      else if (row > frow) {
	for(int r = frow; r < row; r++) {
	  _map[col + r * cols] = _map[ col + (r+1) *cols];
	}
      }
    }
    // move free cell to click position
    _map[pos=(col + row * cols)] = rows*cols-1;
  }
  repaint();
}

void PiecesTable::checkwin()
{
  if(!_randomized) return;

  int items=numCols()*numRows();
  int i;
  for (i = 0; i < items; i++)
    if(i != _map[i])
      break;

  if (i == items) {
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
    int cols = numCols();
    int rows = numRows();
    int item = cols*rows -1;

    // find the free position
    int pos = _map.find(item);
    if(pos < 0) return;

    int frow = pos / cols;
    int fcol = pos - frow * cols;

    // find click position
    int row = findRow(e->y());
    int col = findCol(e->x());

    // sanity check
    if (row < 0 || row >= rows) return;
    if (col < 0 || col >= cols) return;
    if ( row != frow && col != fcol ) return;

    // valid move?
    if(row != frow && col != fcol) return;

    // rows match -> shift pieces
    if(row == frow) {

      if (col < fcol) {
	for(int c = fcol; c > col; c--) {
	  _map[c + row * cols] = _map[ c-1 + row *cols];
	  updateCell(row, c, false);
	}
      }
      else if (col > fcol) {
	for(int c = fcol; c < col; c++) {
	  _map[c + row * cols] = _map[ c+1 + row *cols];
	  updateCell(row, c, false);
	}
      }
    }
    // cols match -> shift pieces
    else if (col == fcol) {

      if (row < frow) {
	for(int r = frow; r > row; r--) {
	  _map[col + r * cols] = _map[ col + (r-1) *cols];
	  updateCell(r, col, false);
	}
      }
      else if (row > frow) {
	for(int r = frow; r < row; r++) {
	  _map[col + r * cols] = _map[ col + (r+1) *cols];
	  updateCell(r, col, false);
	}
      }
    }
    // move free cell to click position
    _map[col + row * cols] = item;
    updateCell(row, col, false);

    // check if the player wins with this move
    checkwin();
  }
}

void PiecesTable::slotConfigure() {
    if ( !_dialog )
        _dialog = new FifteenConfigDialog(this, "Fifteen Configure Dialog", true );


    _dialog->setImageSrc( _image );
    _dialog->setGameboard( numRows(), numCols() );

    if ( QPEApplication::execDialog(_dialog) == QDialog::Accepted ) {
        /*
         * update the board grid and reinit the game if changed
         * First set new columns so the update will regenerate the
         * tiles with slotCustomImage
         */
        _image = _dialog->imageSrc();
        if (numRows() != _dialog->rows() ||
            numCols() != _dialog->columns() ) {
            setNumCols(_dialog->columns());
            setNumRows(_dialog->rows());
            slotReset();
        }
        resizeEvent( 0l );


        update();
    }
}
