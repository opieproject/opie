#ifndef FRUIT_H
#define FRUIT_H

#include <stdlib.h>

#include "board.h"

enum fruitState { inactive, active, eaten };

class Fruit {
public:
    Fruit(Board *b);
    void setEaten(int duration);
    void setLevel(int level, int wDuration, int fDuration, int ticks = -1);
    void setMaxPixmaps(int max);
    void setMovement(int entry, int tunnel, int iq);
    void setPosition(int pos);
    void setDirection(int dir);
    fruitState state();
    int position();
    int direction();
    bool move(bool activate=FALSE);
    int pix();

private:
    Board   *board;

    int  IQ;                            // Intelligence of movement (0 = dumb..180=smart)

    fruitState actualState;             // the state of fruit

    int  pauseDuration;                 // number of ticks before next movement
    int  pause;                         // actual ticks before movement (0 = move)

    int  timeLeft;                      // Ticks remaining of current state

    int  waitDuration;                  // Time before fruit appears
    int  fruitDuration;                 // Length of active-time in ticks

    int  actualDirection;               // actual direction of the fruit
    int  actualPosition;                // actual position on board
    int  lastPosition;                  // the last position of the fruit
    int  actualLevel;                   // level for kind of fruit and score
    int  actualPix;             
    int  maxPixmaps;                    // Number of Pixmaps (1..)

    int  entryPosition;                 // where to come in
    int  homePosition;                  // where to go, on the way in
    int  tunnelPosition;                // where to exit
};

#endif // FRUIT_H

