#include <stdlib.h>

#include "fruit.h"

Fruit::Fruit(Board *b)
{
    board = b;
    maxPixmaps = 0;
    setLevel(0, 0, 0, 0);
}

void Fruit::setEaten(int duration)
{
    actualState = eaten;
    timeLeft = duration;
    actualDirection = X;
}
  
void Fruit::setLevel(int level, int wDuration, int fDuration, int ticks)
{
    actualLevel = level;
    waitDuration = wDuration;
    fruitDuration = fDuration;
    pauseDuration = ticks;
    pause = 0;
    actualState = inactive;
    timeLeft = waitDuration;
    lastPosition = OUT;
    setPosition(OUT);
    setMovement(OUT, OUT, 0);
    actualDirection = X;
    setMaxPixmaps(maxPixmaps);
}

void Fruit::setMaxPixmaps(int max)
{
    maxPixmaps = max;
    if (actualLevel-1 < maxPixmaps)
        actualPix = actualLevel == 0 ? 0 : actualLevel-1;
    else if (maxPixmaps > 0)
        actualPix = rand() % maxPixmaps;
    else
        actualPix = -1;
}

void Fruit::setMovement(int entry, int tunnel, int iq)
{
    homePosition = board->position(fruithome);
    entryPosition = entry;
    tunnelPosition = tunnel;
    IQ = iq;
}

void Fruit::setPosition(int pos)
{
    board->reset(lastPosition, fruit);
    actualPosition = lastPosition = pos;
    board->set(actualPosition, fruit);
}

void Fruit::setDirection(int dir)
{
    actualDirection = dir;
}

fruitState Fruit::state()
{
    return actualState;
}

int Fruit::position()
{
    return actualPosition;
}

int Fruit::direction()
{
    return actualDirection;
}

bool Fruit::move(bool activate)
{
    if (timeLeft > 0) {
        timeLeft--;
    }

    if (actualDirection == X || actualState == inactive) {
        if (timeLeft == 0 || (activate && actualState == inactive)) {
            if (actualState == inactive) {
                if (entryPosition == OUT || tunnelPosition == OUT) {
                    setPosition(board->position(fruithome));
                } else {
                    setPosition(entryPosition);
                    actualDirection = 0;
                    while (!board->isWay(actualPosition, actualDirection, empty) ||
                           board->isJump(actualPosition, actualDirection))
                        actualDirection++;
                }
                timeLeft = fruitDuration;
                setMaxPixmaps(maxPixmaps);
                actualState = active;
            } else {
                actualState = inactive;
                setPosition(OUT);
                timeLeft = waitDuration;
                actualDirection = X;
            }
            return TRUE;
        }
        return FALSE;
    }

    if (pause-- > 0)
        return FALSE;
    else
        pause = pauseDuration;

    if (actualPosition == OUT)
        return FALSE;

    if (actualDirection == X)
        return TRUE;

    int d = actualDirection;

    if (rand() % (int) ((190-IQ)/10) == 0)
        if (timeLeft > 0)               // coming home or leaving again
            d = board->closeup(actualPosition, d, homePosition);
        else                    
            d = board->closeup(actualPosition, d, tunnelPosition);
    else
        do                              // try new direction, but not the opposite
            d = rand() % 4;             // direction, to prevent hectic movement.
        while (d == board->turn(actualDirection));

    while ((!board->isWay(actualPosition, d, empty) &&
            !board->isWay(actualPosition, d, tunnel)) ||
           d == board->turn(actualDirection)) {
        if (d != actualDirection)       // if new direction is not possible,
            d = actualDirection;        // try current direction first.
        else
            d = rand() % 4;
    }

    actualDirection = d;
    actualPosition = board->move(actualPosition, actualDirection);

    if (actualPosition == homePosition) {
        timeLeft = 0;
    }

    if (board->isTunnel(actualPosition)) {
        setPosition(OUT);
        timeLeft = waitDuration;
        actualState = inactive;
        actualDirection = X;
        if (board->tunnels() > 0) {
            entryPosition = board->position(tunnel, rand() % board->tunnels());
            tunnelPosition = board->position(tunnel, rand() % board->tunnels());
        }
    }

    if (actualPosition != lastPosition) {
        setPosition(actualPosition);
    }

    return TRUE;
}

int Fruit::pix()
{
    if (actualPosition == OUT || actualState == inactive)
        return -1;
    else
        return actualPix;
}
