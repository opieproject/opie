/***************************************************************************
                          linesboard.cpp  -  description
                             -------------------
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qpainter.h>
#include <stdlib.h>

#include "linesboard.h"

/*
   Constructs a LinesBoard widget.
*/

LinesBoard::LinesBoard( BallPainter * abPainter, QWidget* parent, const char* name )
    : Field( parent, name )
{
  anim = ANIM_NO;
//  waypos = 0;
//  waylen = 0;
//  jumpingRow = -1;
//  jumpingCol = -1;
  painting = 0;
  way = new Waypoints[NUMCELLSW*NUMCELLSH];

  bPainter = abPainter;

  setFocusPolicy( NoFocus );
  setBackgroundColor( gray );

  setMouseTracking( FALSE );
  setFixedSize(wHint(), hHint());

  timer = new QTimer(this);
  connect( timer, SIGNAL(timeout()), SLOT(timerSlot()) );
  timer->start( TIMERCLOCK, FALSE );

}

/*
   Destructor: deallocates memory for contents
*/

LinesBoard::~LinesBoard()
{
  // debug("stop");
  timer->stop();
  delete timer;
  delete []way;
}


void LinesBoard::placeBalls(int pnextBalls[BALLSDROP])
{
    debug("LinesBoard::placeBalls( )");
    for(int i=0; i < BALLSDROP; i++){
      nextBalls[i] = pnextBalls[i];
    }
    nextBallToPlace = 0;
    placeBall();
    debug("LinesBoard::placeBalls End ");
}
void LinesBoard::placeBall(  )
{
		int color = nextBalls[nextBallToPlace];
    debug("LinesBoard::placeBall( ) color=%i, nextBallToPlace = %i", color, nextBallToPlace);
    char* xx = (char*)malloc( sizeof(char)*NUMCELLSW*NUMCELLSH );
    char* yy = (char*)malloc( sizeof(char)*NUMCELLSW*NUMCELLSH );
//    int nb=3;
//    if( freeSpace() < 3) nb = freeSpace();
    int empty=0;
    for(int y=0; y<NUMCELLSH; y++)
      for(int x=0; x<NUMCELLSW; x++)
        if( getBall(x,y) == NOBALL )
        {
          xx[empty] = x;
          yy[empty] = y;
          empty++;
        };
//      debug("empty = %i",empty);
    if ( empty >  0)
    {
      int pos = rand()%empty;
      putBall( xx[pos], yy[pos], color );
      clearAnim();
      setAnim( xx[pos], yy[pos], ANIM_BORN );
			nextBallToPlace++;
      AnimStart(ANIM_BORN);
      free(xx);
      free(yy);
    }
    else
    {
      free(xx);
      free(yy);
      emit endGame();
    }
    debug("LinesBoard::placeBall END");
}


/*id LinesBoard::doAfterBalls() {
  erase5Balls();
  repaint(FALSE);
}
*/
/*
   Sets the size of the table
*/

int LinesBoard::width() { return CELLSIZE * NUMCELLSW; }
int LinesBoard::height() { return CELLSIZE * NUMCELLSH; }
int LinesBoard::wHint() { return width(); }
int LinesBoard::hHint() { return height(); }



void LinesBoard::paintEvent( QPaintEvent* )
{
//  debug("LinesBoard::paintEvent ");
	QPixmap pixmap(width(), height());
	QPainter paint(&pixmap, this);

    for( int y=0; y < NUMCELLSH; y++ ){
        for( int x=0; x < NUMCELLSW; x++ ){
          if( getBall(x,y) == NOBALL )
					{
//            debug("draw empty  %i %i", x, y );
            paint.drawPixmap(x*CELLSIZE, y*CELLSIZE, *(bPainter->GetBackgroundPix()) );
					}
          else
          {
//            debug("draw empty  %i %i %c", x, y, getBall(x,y) );
            paint.drawPixmap(x*CELLSIZE, y*CELLSIZE,
              *(bPainter->GetBall(getBall(x,y),animstep,getAnim(x,y))));
          }
        }
    }

	bitBlt(this, 0,0, &pixmap, 0,0, width(), height(), CopyROP);
}

/*
   Handles mouse press events for the LinesBoard widget.
*/
void LinesBoard::mousePressEvent( QMouseEvent* e )
{
  debug("LinesBoard::mousePressEvent START");
  int curRow = e->y() / CELLSIZE;
  int curCol = e->x() / CELLSIZE;
  //debug
  debug("Mouse pressed: curRow=%i, curCol=%i", curRow, curCol);

  //check range
  if (!checkBounds( curCol, curRow ) )
    return;
//    if( running || anim != ANIM_NO )  return;
  if(anim != ANIM_JUMP && anim != ANIM_NO) return;
  if ( anim == ANIM_JUMP )
  {
    if ( getBall(curCol,curRow) == NOBALL )
    {
      if(existPath(jumpingCol, jumpingRow, curCol, curRow))
			{
				saveUndo();
        AnimStart(ANIM_RUN);
			}
    }
    else
      AnimJump(curCol,curRow);
  }
  else
    AnimJump(curCol,curRow);
  debug("LinesBoard::mousePressEvent END");
}
void LinesBoard::AnimJump(int x, int y ) {
  debug("LinesBoard::AnimJump( %i,%i)", x,y );
  if ( getBall(x,y) != NOBALL )
    if (!( anim == ANIM_JUMP &&  jumpingCol == x && jumpingRow == y ))
      if ( AnimEnd() )
      {
        clearAnim();
        setAnim(x,y,ANIM_JUMP);
        jumpingCol = x;
        jumpingRow = y;
        AnimStart(ANIM_JUMP);
      }
  debug("LinesBoard::AnimJump END");
}
void LinesBoard::AnimStart(int panim) {
  debug("LinesBoard::AnimStart( %i )", panim);
  if (anim != ANIM_NO)
    AnimEnd();
  animstep = 0;
  animdelaystart = 1;
  switch(panim) {
    case ANIM_NO:
        break;
    case ANIM_BORN:
        debug("LinesBoard::AnimStart( ANIM_BORN )");
        animdelaystart=1;
        animmax = BOOMBALLS;
        break;
    case ANIM_JUMP:
        direction = -1;
        animstep = 4;
        animmax = PIXTIME -1;
        break;
    case ANIM_RUN:
        animdelaystart=3;
        // do first step on next timer;
        animdelaycount = 0;
        // animmax already set
        break;
    case ANIM_BURN:
        animdelaystart=1;
        animmax = FIREBALLS + FIREPIX - 1;
        break;
    default:
      ;
  }
  anim = panim;
  animdelaycount = animdelaystart;
  debug("LinesBoard::AnimStart END");
}
int LinesBoard::AnimEnd( )
{
  debug("LinesBoard::AnimEnd( %i )",anim );
  if (anim == ANIM_NO ) return true;
  int oldanim = anim;
  anim = ANIM_NO;
  if (oldanim == ANIM_RUN) {
    if (animstep != animmax) {
      moveBall(way[animstep].x,way[animstep].y,way[animmax].x,way[animmax].y);
 	  }
 	  if ( erase5Balls() == 0 ) {
 	    // debug("end turn");
 	    emit endTurn();
		  debug("LinesBoard::AnimEnd END true 1");
 	    return true;
 	  }
 	  else
		  debug("LinesBoard::AnimEnd END false 2");
 	    return false;
 	} else if ( oldanim == ANIM_BURN )
 	{
    emit eraseLine( deleteAnimatedBalls() );
    repaint(FALSE);
    if ( nextBallToPlace < BALLSDROP )
    {
      placeBall();
      // continue with born
		  debug("LinesBoard::AnimEnd END false 3");
      return false;
    }
    else
    {
      // emit endTurn();
		  debug("LinesBoard::AnimEnd END true 4");
      return true;
    }
  } else if ( oldanim == ANIM_BORN )
  {
 	  if ( erase5Balls() == 0 )
 	  {
 	    if ( freeSpace() > 0)
 	    {
        if ( nextBallToPlace < BALLSDROP )
        {
          placeBall();
  			  debug("LinesBoard::AnimEnd END false 5");
          return false;
        }
        else
        {
  			  debug("LinesBoard::AnimEnd END true 6");
          return true;
        }
      }
      else
      {
 			  debug("emit endGame");
        emit endGame();
 			  debug("LinesBoard::AnimEnd END false 7");
        return false;
      }
    }
    else
    {
      // wait for user input
		  debug("LinesBoard::AnimEnd END true 8");
      return true;
    }
  }
  else
  {
	  debug("LinesBoard::AnimEnd END true");
    return true;
  }
  return true;
}
void LinesBoard::AnimNext() {
  if ( anim != ANIM_NO )
  {
    debug("LinesBoard::AnimNext( ) anim %i animstep %i",anim,animstep);
    if ( anim == ANIM_JUMP ) {
      if ( (direction > 0 && animstep == animmax) || ( direction < 0 && animstep == 0))
        direction = -direction;
      animstep += direction;
      repaint(FALSE);
    } else {
      if ( animstep >= animmax )
        AnimEnd();
      else {
        animdelaycount--;
        if (animdelaycount <= 0) {
          debug("LinesBoard::AnimNext step %i", animstep);
          if ( anim == ANIM_RUN )
            moveBall(way[animstep].x,way[animstep].y,way[animstep+1].x,way[animstep+1].y);
          animstep++;
          animdelaycount = animdelaystart;
          repaint( FALSE );
        }
      }
    }
    debug("LinesBoard::AnimNext END");
  }
}
int LinesBoard::getAnim( int x, int y )
{
//   debug("LinesBoard::getAnim( %i,%i )",x,y);
  return (( Field::getAnim(x,y) != ANIM_NO )? anim : ANIM_NO);
}

void LinesBoard::timerSlot()
{
//  debug("LinesBoard::Timer()  anim = %i",anim );
  AnimNext();
}

int LinesBoard::erase5Balls()
{
    //debug
    debug("LinesBoard::erase5Balls()");

    int nb=5;  // minimum balls for erasure

    bool bit_erase[NUMCELLSH][NUMCELLSW]; //bool array for balls, that must be erased
    for(int y=0; y<NUMCELLSH; y++)
      for(int x=0; x<NUMCELLSW; x++)
        bit_erase[y][x] = false;

    int color,newcolor;
    int count;
    //for horisontal
    //debug("entering to horiz");

    for(int y=0; y<NUMCELLSH; y++) {
      count = 1;
      color = NOBALL;
      for(int x=0; x<NUMCELLSW; x++){
        if ( (newcolor = getBall(x,y)) == color) {
            if ( color != NOBALL) {
                count++;
                if ( count >= nb )
                    if ( count == nb )
                        for (int i = 0; i < nb; i++)
                            bit_erase[y][x-i] = true;
                    else bit_erase[y][x] = true;
            }
        } else {
            color = newcolor;
            count = 1;
        }
      }
    }

    //for vertical
    //debug("entering to vert");
    for(int x=0; x<NUMCELLSW; x++) {
      count = 0;
      color = NOBALL;
      for(int y=0; y<NUMCELLSH; y++){
        if ( (newcolor = getBall(x,y)) == color) {
            if ( color != NOBALL) {
                count++;
                if ( count >= nb )
                    if ( count == nb )
                        for (int i = 0; i < nb; i++)
                            bit_erase[y-i][x] = true;
                    else bit_erase[y][x] = true;
            }
        } else {
            color = newcolor;
            count = 1;
        }
      }
    }


    //  debug("entering to diag1");
    //for left-down to rigth-up diagonal
    for ( int xs = NUMCELLSW-1,ys = NUMCELLSH-nb; xs >= nb-1; ) {
        count = 0;
        color = NOBALL;
        for ( int x = xs, y = ys; x >= 0 && y < NUMCELLSH; x--, y++ ) {
            if ( (newcolor = getBall(x,y)) == color) {
                if ( color != NOBALL) {
                    count++;
                    if ( count >= nb )
                        if ( count == nb )
                            for (int i = 0; i < nb; i++)
                                bit_erase[y-i][x+i] = true;
                        else bit_erase[y][x] = true;
                }
            } else {
                color = newcolor;
                count = 1;
            }
        }
        if ( ys > 0 ) ys--; else xs--;
    }

    //debug("entering to diag2");

    //for left-up to rigth-down diagonal
    for ( int xs = 0,ys = NUMCELLSH-nb; xs <= NUMCELLSW-nb; )
    {
        count = 0;
        color = NOBALL;
        for ( int x = xs, y = ys; x < NUMCELLSW && y < NUMCELLSH; x++, y++ )
        {
            if ( (newcolor = getBall(x,y)) == color)
            {
                if ( color != NOBALL)
                {
                    count++;
                    if ( count >= nb )
                        if ( count == nb )
                            for (int i = 0; i < nb; i++)
                                bit_erase[y-i][x-i] = true;
                        else
                          bit_erase[y][x] = true;
                }
            }
            else
            {
                color = newcolor;
                count = 1;
            }
        }
        if ( ys > 0 ) ys--; else xs++;
    }

    //remove all lines balls, that more than nb
    int cb=0;
    for(int y=0; y < NUMCELLSH; y++)
      for(int x=0; x < NUMCELLSW; x++)
      {
				if (bit_erase[y][x])
				{
        	setAnim(x,y,ANIM_YES);
        	cb++;
				}
				else
				{
          	setAnim(x,y,ANIM_NO);
        }
      }
    //debug
    debug("LinesBoard::erase5Balls marked %i balls", cb);
    if ( cb > 0 )
    {
      AnimStart(ANIM_BURN);
      //emit eraseLine(cb);
    }

    //debug
    debug("LinesBoard::erase5Balls END");
    return cb;
}


#define GO_EMPTY    4
#define GO_A        5
#define GO_B        6
#define GO_BALL     7

bool LinesBoard::existPath(int bx, int by, int ax, int ay)
{
  debug("LinesBoard::existPath( %i, %i, %i, %i )", bx, by, ax, ay);
  int dx[4]={1,-1, 0, 0};
  int dy[4]={0, 0, 1,-1};

  // debug("path %i %i %i %i",bx,by,ax,ay);
  if (getBall(ax,ay) != NOBALL)
    return false;

  char pf[NUMCELLSH][NUMCELLSW];
  for(int y=0; y<NUMCELLSH; y++)
    for(int x=0; x<NUMCELLSW; x++)
      pf[y][x] = (getBall(x,y) == NOBALL) ? GO_EMPTY:GO_BALL;

  Waypoints lastchanged[2][NUMCELLSH*NUMCELLSW];

  int lastChangedCount[2];
  int currentchanged = 0;
  int nextchanged = 1;
  lastchanged[currentchanged][0].x = ax;
  lastchanged[currentchanged][0].y = ay;
  lastChangedCount[currentchanged] = 1;
  pf[ay][ax]=GO_A;
  pf[by][bx]=GO_B;

  // int expanded;
  bool B_reached = false;

  do
  {
    lastChangedCount[nextchanged] = 0;
    for(int dir=0; dir<4; dir++)
    {
      for ( int i = 0 ; i < lastChangedCount[currentchanged]; i++ )
      {
        int nx = lastchanged[currentchanged][i].x + dx[dir];
        int ny = lastchanged[currentchanged][i].y + dy[dir];
        if( (nx>=0) && (nx<NUMCELLSW) && (ny>=0) && (ny<NUMCELLSH) )
        {
          if( pf[ny][nx]==GO_EMPTY ||( nx==bx && ny==by ))
          {
            pf[ny][nx] = dir;
            lastchanged[nextchanged][lastChangedCount[nextchanged]].x = nx;
            lastchanged[nextchanged][lastChangedCount[nextchanged]].y = ny;
            lastChangedCount[nextchanged]++;
          };
          if( (nx==bx) && (ny==by) )
          {
            B_reached = true;
            break;
          }
        }
      }
    }
    nextchanged = nextchanged ^ 1;
    currentchanged = nextchanged ^ 1;
    //      debug("currentchanged %i lastChangedCount[currentchanged] %i",
    //      currentchanged,lastChangedCount[currentchanged]);
  } while(!B_reached && lastChangedCount[currentchanged] != 0);
  //debug("B_reached %i ", B_reached );

  if (B_reached) {
    AnimStart( ANIM_BLOCK);
    animmax = 0;
//    waypos = 0;
    int x, y,dir;
    for( x = bx, y = by,dir; (dir = pf[y][x]) != GO_A;x -=dx[dir],y -= dy[dir]) {
     	way[animmax].x = x;
     	way[animmax].y = y;
    	//debug("%i %i %i %i",dir,waylen,x,y);
    	animmax++;
    }
    way[animmax].x = x;
    way[animmax].y = y;
  }
  debug("LinesBoard::existPath END %s", B_reached?"true":"false" );
  return B_reached;
}

void LinesBoard::undo()
{
	AnimEnd();
	restoreUndo();
	repaint( FALSE );
}

