#ifndef PACMAN_H
#define PACMAN_H

#include <stdlib.h>
#include <qpixmap.h>
#include <qwidget.h>

#include "board.h"

enum pacmanState { alive };

class Pacman {
public:
    Pacman(Board *b);
    void init(bool Demo = FALSE);
    void setMaxPixmaps(int max);
    void setAlive(int ticks);
    void setPosition(int pos);
    void setDirection(int dir, bool forced = FALSE);
    void setDemo(bool yes);
    pacmanState state();
    int position();
    int direction();
    bool move();
    int pix();

private:
    Board   *board;

    pacmanState actualState;            // the state of pacman
    bool demo;                          // real life or just demo

    int  pauseDuration;                 // number of ticks before next movement
    int  pause;                         // actual ticks before movement (0=move)

    int  actualDirection;               // actual direction of pacman
    int  nextDirection;                 // where he wants to go
    int  lastPix;                       // last Pixmap-index before no movement
    int  maxPixmaps;                    // Number of Pixmaps (1..)
    int  actualPosition;                // actual position on board
    int  lastPosition;                  // the last position of pacman
    int  mouthPosition;                 // eating

};

#endif // PACMAN_H

