
#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#include <klocale.h>
#endif

#include <qrect.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>

#include "board.h"
#include "bitmaps.h"

Board::Board(int size) : QArray<int> (size)
{
    sz = size;                          // set size of board

    map = "";                   
    mapName = "";                       // no map loaded so far

    init(None);                         // initialize varibales
}

void Board::init(Image image, QString levelName)
{
    prisonEntry = OUT;
    prisonExit = OUT;
    fruitHome = OUT;
    fruitPosition = OUT;
    pacmanHome = OUT;
    pacmanPosition = OUT;
    for (int m = 0; m < 8; m++) {
        monsterHome[m] = OUT;
        monsterPosition[m] = OUT;
    }
    for (int e = 0; e < 8; e++) {
        energizerPosition[e] = OUT;
    }
    for (int e = 0; e < 8; e++) {
        tunnelPosition[e] = OUT;
    }

    fill(0);
    numPoints = 0;
    numEnergizers = 0;
    numMonsters = 0;
    numTunnels = 0;

    if (!levelName.isNull() && !levelName.isEmpty())
        if (mapName == levelName)
            image = File;
        else {
            QFile levelFile(levelName);
            if (!levelFile.open(IO_ReadOnly)) {

                QString msg = "The levelmap could not be constructed.\n\n"
                                   "The file '@LEVELNAME@' does not exist,\n"
                                   "or could not be opened for reading.";
                msg.replace(QRegExp("@LEVELNAME@"), levelName);
                // QMessageBox::information(0, tr("Initialization Error"), msg);
                printf("%s\n", msg.data());
            } else {
                map.fill(' ', BoardHeight*BoardWidth);
                int height = 0; 

                QTextStream levelStream(&levelFile);
                while (!levelStream.eof() && height < BoardHeight) {
                    QString line = levelStream.readLine();
                
                    if (line.find(QRegExp("^ *;")) == -1) {     

                        line.replace(QRegExp(";.*"), "");       // strip off comments
                        line.replace(QRegExp("\" *$"), "");     // strip off trailing "
                        line.replace(QRegExp("^ *\""), "");     // strip off leading "

                        map.replace(height*BoardWidth,
                                    (line.length() > BoardWidth) ? BoardWidth : line.length(),
                                    line.data());

                        height++;
                    }
                }
                mapName = levelName;
                levelFile.close();
                image = File;
            }
        }

    switch (image) {
        case Intro : // setup(demo_bits);
                     break;
        case Demo  : setup(demo_bits);
                     break;
        case Level : setup(demo_bits);
                     break;
        case File  : setup((uchar *) map.data());
                     break;
        default    : break;
    }
}

void Board::setup(const uchar *buf)
{
    for ( int index = 0; buf[index] != 0 && index < BoardWidth*BoardHeight; index++ ) {
        switch (buf[index]) {
            case '*' : set(index, brick); break;
            case '+' : set(index, out); break;
            case '#' : set(index, prison); break;
            case '-' : set(index, gate); break;
            case 'E' : set(index, tunnel); break;
            case '.' : set(index, Point); break;
            case 'o' : set(index, energizer); break;
            case 'I' : set(index, prisonentry); break;
            case 'O' : set(index, prisonexit); break;
            case 'F' : set(index, fruithome); break;
            case 'P' : set(index, pacmanhome);  break;
            default  : if (buf[index] >= '0' && buf[index] <= '7') {
                           set(index, monsterhome, buf[index]-(uchar)'0');
                       }
        }
    }
}

bool Board::inBounds(int pos)
{
  return ( pos < 0 || pos > sz-1 ? FALSE : TRUE);
}

void Board::set(int pos, Square sq, int m)
{
    if (inBounds(pos))
        switch (sq) {
            case out         : at(pos) = OUT; break;
            case Point       : at(pos) |= pointBit; numPoints++; break;
            case tunnel      : at(pos) = sq;
                               for (int e = 0; e < numTunnels; e++) {           // if tunnel is already on board
                                   if (tunnelPosition[e] == pos)                // don't do it again.
                                       pos = OUT;
                               }
                               if (pos != OUT) {
                                   tunnelPosition[numTunnels] = pos;
                                   numTunnels++;
                               }
                               break;
            case energizer   : at(pos) |= energizerBit;
                               for (int e = 0; e < numEnergizers; e++) {
                                   if (energizerPosition[e] == pos)     
                                       pos = OUT;
                               }
                               if (pos != OUT) {
                                   energizerPosition[numEnergizers] = pos;
                                   numEnergizers++;
                               }
                               break;
            case fruit       : at(pos) |= fruitBit; fruitPosition = pos; break;
            case pacman      : at(pos) |= pacmanBit; pacmanPosition = pos; break;
            case monster     : at(pos) |= (monsterBit << m); 
                               monsterPosition[m] = pos; break;
            case prisonentry : prisonEntry = pos; at(pos) = empty; break;
            case prisonexit  : prisonExit = pos; at(pos) = empty; break;
            case fruithome   : fruitHome = pos; at(pos) = empty; break;
            case pacmanhome  : pacmanHome = pos; at(pos) = empty; break;
            case monsterhome : monsterHome[m] = pos; at(pos) = empty; 
                               if (m == 0 && prisonExit == OUT)
                                   prisonExit = pos;
                               if (m == 1 && prisonEntry == OUT)
                                   prisonEntry = pos;
                               numMonsters++;
                               break;
            default          : at(pos) = sq;
        }
}

void Board::reset(int pos, Square sq, int m)
{
    bool found = FALSE;
    if (inBounds(pos))
        switch (sq) {
            case out       : at(pos) = empty; break;
            case Point     : at(pos) &= ~ pointBit; numPoints--; break;
            case energizer : at(pos) &= ~ energizerBit;
                             for (int e = 0; e < numEnergizers; e++) {          // delete the position of the eaten
                                 if (found)                                     // energizer in the position array
                                     energizerPosition[e-1] = energizerPosition[e];
                                 if (energizerPosition[e] == pos)               
                                     found = TRUE;
                             }
                             energizerPosition[numEnergizers--] = OUT;
                             break;
            case fruit     : at(pos) &= ~ fruitBit; fruitPosition = OUT; break;
            case pacman    : at(pos) &= ~ pacmanBit; pacmanPosition = OUT; break;
            case monster   : at(pos) &= ~ (monsterBit << m);
                             monsterPosition[m] = OUT; break;
            default        : at(pos) = at(pos) & varBits;
        }
}

int Board::position(Square sq, int m)
{
    switch(sq) {
        case prisonentry  : return prisonEntry;
        case prisonexit   : return prisonExit;
        case fruit        : return fruitPosition;
        case fruithome    : return fruitHome;
        case pacman       : return pacmanPosition;
        case pacmanhome   : return pacmanHome;
        case monster      : return monsterPosition[m];
        case monsterhome  : return monsterHome[m];
        case energizer    : return energizerPosition[m];
        case tunnel       : return tunnelPosition[m];
        default           : return OUT;
    }
}

bool Board::isOut(int pos)
{
    if (inBounds(pos))
        return (at(pos) == OUT ? TRUE : FALSE);
    return TRUE;
}

bool Board::isEmpty(int pos)
{
    if (inBounds(pos))
        return ((at(pos) & fixBits) == empty ? TRUE : FALSE);
    return TRUE;
}

bool Board::isBrick(int pos)
{
    if (inBounds(pos))
        return ((at(pos) & fixBits) == brick ? TRUE : FALSE);
    return FALSE;
}

bool Board::isPrison(int pos)
{
    if (inBounds(pos))
        return ((at(pos) & fixBits) == prison ? TRUE : FALSE);
    return FALSE;
}

bool Board::isGate(int pos)
{
    if (inBounds(pos))
        return ((at(pos) & fixBits) == gate ? TRUE : FALSE);
    return FALSE;
}

bool Board::isTunnel(int pos)
{
    if (inBounds(pos))
        return ((at(pos) & fixBits) == tunnel ? TRUE : FALSE);
    return FALSE;
}

bool Board::isPoint(int pos)
{
    if (inBounds(pos) && at(pos) != OUT)
        return ((at(pos) & pointBit) != 0 ? TRUE : FALSE);
    return FALSE;
}

bool Board::isEnergizer(int pos)
{
    if (inBounds(pos) && at(pos) != OUT)
        return ((at(pos) & energizerBit) != 0 ? TRUE : FALSE);
    return FALSE;
}

bool Board::isFruit(int pos)
{
    if (inBounds(pos) && at(pos) != OUT)
        return ((at(pos) & fruitBit) != 0 ? TRUE : FALSE);
    return FALSE;
}

bool Board::isPacman(int pos)
{
    if (inBounds(pos) && at(pos) != OUT)
        return ((at(pos) & pacmanBit) != 0 ? TRUE : FALSE);
    return FALSE;
}

bool Board::isMonster(int pos)
{
    if (inBounds(pos) && at(pos) != OUT)
        return ((at(pos) & monsterBits) != 0 ? TRUE : FALSE);
    return FALSE;
}

bool Board::isWay(int pos, int dir, Square sq) {
    int p1 = move(pos, dir, 2);
    if (p1 == OUT)
        return (sq == out ? TRUE : FALSE);
    int p2, p3;
    if (dir == N || dir == S) {
        p2 = move(p1, E);
        p3 = move(p1, W);
    } else {
        p2 = move(p1, N);
        p3 = move(p1, S);
    }
    switch (sq) {
        case out    : return isOut(p1) | isOut(p2) | isOut(p3); 
        case empty  : return isEmpty(p1) & isEmpty(p2) & isEmpty(p3);
        case brick  : return isBrick(p1) | isBrick(p2) | isBrick(p3);
        case prison : return isPrison(p1) | isPrison(p2) | isPrison(p3);
        case gate   : return isGate(p1) & isGate(p2) & isGate(p3);
        case tunnel : return isTunnel(p1) &
                             (isTunnel(p2) || isEmpty(p2)) &
                             (isTunnel(p3) || isEmpty(p3));
        default     : return FALSE;
    }
}

bool Board::isJump(int pos, int dir) {
    switch (dir) {
        case NW: return pos < BoardWidth || x(pos) == 0;
        case N:  return pos < BoardWidth;
        case NE: return pos < BoardWidth || x(pos) == BoardWidth-1;
        case W:  return x(pos) == 0;
        case E:  return x(pos) == BoardWidth-1;
        case SW: return pos >= sz-BoardWidth || x(pos) == 0;
        case S:  return pos >= sz-BoardWidth;
        case SE: return pos >= sz-BoardWidth || x(pos) == BoardWidth-1;
    }
    return FALSE;
}

int Board::move(int pos, int dir, int steps)
{
    if (steps < 0) {                    // move backwards
        dir = turn(dir);                // turn around and do your steps
        steps *= -1;
    }

    while (steps-- != 0) {              // until all steps are gone
        switch (dir) {
            case NW: pos = pos >= BoardWidth && x(pos) > 0 ? (pos-BoardWidth)-1 : sz-1;
                     break;
            case N:  pos = pos >= BoardWidth ? pos-BoardWidth : (sz-BoardWidth)+x(pos);
                     break;
            case NE: pos = pos >= BoardWidth && x(pos) < BoardWidth-1 ?
                        (pos-BoardWidth)+1 : sz-BoardWidth;
                     break;
            case W:  pos = x(pos) > 0 ? pos-1 : pos+(BoardWidth-1);
                     break;
            case E:  pos = x(pos) < BoardWidth-1 ? pos+1 : pos-(BoardWidth-1);
                     break;
            case SW: pos = pos < sz-BoardWidth && x(pos) > 0 ? (pos+BoardWidth)-1 : BoardWidth-1;
                     break;
            case S:  pos = pos < sz-BoardWidth ? pos+BoardWidth : x(pos);
                     break;
            case SE: pos = pos < sz-BoardWidth && x(pos) < BoardWidth-1 ? (pos+BoardWidth)+1 : 0;
                     break;
        }
    }
    return pos;                         // here we are
}

int Board::closeup(int pos, int dir, int target)
{
    if (dir == N || dir == S) {
        if (x(target) < x(pos))
            return W;
        if (x(target) > x(pos))
            return E;
    } else {
        if (y(target) < y(pos))
            return N;
        if (y(target) > y(pos))
            return S;
    }
    return dir;
}

int Board::x(int pos)
{
    return pos % BoardWidth;
}

int Board::y(int pos)
{
    return pos/BoardWidth;
}

int Board::turn(int dir)
{
    switch (dir) {
        case N  : return S;
        case NE : return SW;
        case E  : return W;
        case SE : return NW;
        case S  : return N;
        case SW : return NE;
        case W  : return E;
        case NW : return SE;
        default : return dir;
    }
}

int Board::points()
{
    return numPoints;
}

int Board::energizers()
{
    return numEnergizers;
}

int Board::monsters()
{
    return numMonsters;
}

int Board::tunnels()
{
    return numTunnels;
}
