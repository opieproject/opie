#include "pacman.h"
#include "board.h"

Pacman::Pacman(Board *b)
{
    board = b;
    setDemo(FALSE);
    setAlive(0);
    actualPosition = lastPosition = OUT;
    mouthPosition = 0;
    lastPix = 0;
    maxPixmaps = 0;
}

void Pacman::setMaxPixmaps(int max)
{
    if (actualDirection == X && lastPix >= 0) {
        actualDirection = lastPix / (maxPixmaps/4);
        if (max < maxPixmaps)
            mouthPosition = 0;
        else
            mouthPosition = lastPix % (maxPixmaps/4);
        maxPixmaps = max;       

        lastPix = pix();

        actualDirection = X;
    } else
        maxPixmaps = max;
}

void Pacman::setAlive(int ticks)
{
    actualState = alive;
    pauseDuration = ticks;
    pause = 0;
}

void Pacman::setPosition(int pos)
{
    board->reset(lastPosition, pacman);
    actualPosition = lastPosition = pos;
    board->set(actualPosition, pacman);
    mouthPosition = 0;
}

void Pacman::setDirection(int dir, bool forced)
{
    if (forced ||
        board->isWay(actualPosition, dir, empty) ||
        board->isWay(actualPosition, dir, tunnel)) {
        if (dir == X)
            lastPix = pix();
        actualDirection = dir;
        nextDirection = X;
    } else
        nextDirection = dir;
}

void Pacman::setDemo(bool yes)
{
    demo = yes;
}

pacmanState Pacman::state()
{
    return actualState;
}

int Pacman::position()
{
    return actualPosition;
}

int Pacman::direction()
{
    return actualDirection;
}

bool Pacman::move()
{
    if (pause-- > 0) 
        return FALSE;
    else
        pause = pauseDuration;

    if (actualDirection == X || actualPosition == OUT)
        return FALSE;

    lastPosition = actualPosition;

    if (demo) {
        int d = actualDirection;

        do                              // try new direction, but not the opposite
            d = rand() % 4;             // direction, to prevent hectic movement.
        while (d == board->turn(actualDirection));

        while (!board->isWay(actualPosition, d, empty) &&
               !board->isWay(actualPosition, d, tunnel)) {
            if (d != actualDirection)   // if new actualDirection is not possible,
                d = actualDirection;    // try current actualDirection first.
            else
                d = rand() % 4;
        }

        actualDirection = d;
        actualPosition = board->move(actualPosition, actualDirection);

    } else {

        if (nextDirection != X)
            if (board->isWay(actualPosition, nextDirection, empty) ||
                board->isWay(actualPosition, nextDirection, tunnel)) {
                actualDirection = nextDirection;
                nextDirection = X;      
            }

        if (board->isWay(actualPosition, actualDirection, empty) ||
            board->isWay(actualPosition, actualDirection, tunnel))
            actualPosition = board->move(actualPosition, actualDirection);
    }

    if (actualPosition != lastPosition) {
        board->reset(lastPosition, pacman);
        board->set(actualPosition, pacman);

        if (++mouthPosition >= (maxPixmaps/4))
            mouthPosition = 0;
    }
    return TRUE;
}

int Pacman::pix()
{
    if (actualPosition != OUT && maxPixmaps > 0)
        switch (actualDirection) {
            case N : return ((maxPixmaps/4)*0)+mouthPosition;
            case E : return ((maxPixmaps/4)*1)+mouthPosition;
            case S : return ((maxPixmaps/4)*2)+mouthPosition;
            case W : return ((maxPixmaps/4)*3)+mouthPosition;
            case X : return lastPix;
        }

    return -1;
}

