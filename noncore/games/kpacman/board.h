#ifndef BOARD_H
#define BOARD_H

#include <qarray.h>
#include <qstring.h>
#include <qlist.h>
#include <qrect.h>

#define OUT -1

enum Square {out = OUT, empty, brick, prison, gate, tunnel, prisonentry, prisonexit,
             Point, energizer, fruit, pacman, monster,
             fruithome, pacmanhome, monsterhome};

enum Image { None, Intro, Demo, Level, File };

#define X -1
#define N  0
#define S  1
#define E  2
#define W  3
#define NE 4
#define SE 5
#define NW 6
#define SW 7

#define BoardWidth 59
#define BoardHeight 65

#define fixBits      0x0007
#define varBits      0xFFF8
#define monsterBits  0xFF00

#define pointBit     0x0008
#define energizerBit 0x0010
#define fruitBit     0x0020
#define pacmanBit    0x0040
#define monsterBit   0x0100

class Board : public QArray<int>
{
public:
  Board  (int size);
  ~Board() {};
  void   init(Image image, QString levelName=0);
  void   setup(const uchar *buf);

  void   set(int pos, Square sq, int m = 0);
  void   reset(int pos, Square sq, int m = 0);
  int    position(Square sq, int m = 0);

  bool   isOut(int pos);
  bool   isEmpty(int pos);
  bool   isBrick(int pos);
  bool   isPrison(int pos);
  bool   isGate(int pos);
  bool   isTunnel(int pos);
  bool   isPoint(int pos);
  bool   isEnergizer(int pos);
  bool   isFruit(int pos);
  bool   isPacman(int pos);
  bool   isMonster(int pos);
  bool   isWay(int pos, int dir, Square sq);
  bool   isJump(int pos, int dir);

  int    move(int pos, int dir, int steps = 1);
  int    closeup(int pos, int dir, int target);
  int    x(int pos);
  int    y(int pos);
  int    turn(int dir);

  int    points();
  int    energizers();
  int    monsters();
  int    tunnels();

private:
    bool inBounds(int pos);
    int  sz;                            // size of board

    QString map;
    QString mapName;                    // Filename of the latest loaded map

    int  prisonEntry;                   // position of prisonentry
    int  prisonExit;                    // position of prisonexit
    int  pacmanHome;                    // startposition of pacman
    int  monsterHome[8];                // startposition of monsters
    int  fruitHome;                     // startposition of fruit

    int  pacmanPosition;                // actual position of pacman
    int  monsterPosition[8];            // actual position of monsters
    int  fruitPosition;                 // actual position of fruit
    int  energizerPosition[8];          // actual position of energizers
    int  tunnelPosition[8];             // position of tunnels

    int  numMonsters;                   // number of monsters on the board
    int  numPoints;                     // number of points (left) on the board
    int  numEnergizers;                 // number of energizers (left)
    int  numTunnels;                    // number of tunnels on the board
};

#endif // BOARD_H
