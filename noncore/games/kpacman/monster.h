#ifndef MONSTER_H
#define MONSTER_H

#include <stdlib.h>
#include <qpixmap.h>
#include <qwidget.h>

#include "board.h"

enum monsterState { dangerous, harmless, rem, arrested };

class Monster {
public:
    Monster(Board *b, int mid = 0);
    void setMaxPixmaps(int maxBody, int maxEyes);
    void setArrested(int ticks, int duration);
    void setDangerous(int ticks, int IQ);
    void setHarmless(int ticks, int hDuration, int wDuration);
    void setREM(int ticks);
    void setPosition(int pos);
    void setPrison(int pos);
    void setFreedom(int pos);
    void setDirection(int dir);
    monsterState state();
    int position();
    int direction();
    int id();
    bool move();
    int body();
    int eyes();
private:
    Board   *board;
    int  ID;                            // ID of monster (0 = 1st, 1 = 2nd ... 7 = last)
    int  IQ;                            // Intelligence of movement (0 = dumb..180 = smart)

    monsterState actualState;           // The state of the monster

    int  pauseDuration;                 // Number of ticks before movement
    int  pause;                         // actual ticks before moevment (0 = move)
    int  dangerousPause;                // pause in dangerous-state

    int  harmlessDuration;              // Length of harmless-time in ticks
    int  harmlessLeft;                  // rest of the time in harmless-state
    int  warningDuration;               // warningtime before monster get dangerous again

    int  arrestDuration;                // Length of arrest in ticks
    int  arrestLeft;                    // time left of arrest
    int  arrestPause;                   // pause in arrest-state

    int  actualDirection;               // actual direction of monster
    int  lastDirection;                 // last direction, before no movement (X)
    int  actualPosition;                // actual position on board
    int  lastPosition;                  // the last position of the monster
    int  feetPosition;                  // left, right, left, right, ...
    int  maxBodyPixmaps;                
    int  maxEyesPixmaps;
    int  prisonPosition;                // where to go, if arrested
    int  freedomPosition;               // where to go, if released from prison
};

#endif // MONSTER_H

