#include "monster.h"
#include "board.h"

Monster::Monster(Board *b, int mid)
{
    board = b;
    ID = mid;

    setREM(0);
    setHarmless(0, 0, 0);
    setArrested(0, 0);
    setFreedom(board->position(prisonexit));
    if (mid == 0)
        setPrison(board->position(prisonentry));
    else
        setPrison(board->position(monsterhome, mid));
   
    actualPosition = lastPosition = OUT;
    feetPosition = 0;
    IQ = 0;
    maxBodyPixmaps = 0;
    maxEyesPixmaps = 0;
}

void Monster::setMaxPixmaps(int maxBody, int maxEyes)
{
    if (feetPosition >= (maxBody/10))
        feetPosition = 0;
    maxBodyPixmaps = maxBody;
    maxEyesPixmaps = maxEyes;
}

void Monster::setArrested(int ticks, int duration)
{
    actualState = dangerous;
    pauseDuration = ticks;
    pause = 0;
    arrestDuration = arrestLeft = duration;
    arrestPause = ticks;
    harmlessLeft = 0;
}

void Monster::setDangerous(int ticks, int iq)
{
    actualState = dangerous;
    pauseDuration = ticks;
    pause = 0;
    dangerousPause = ticks;
    harmlessLeft = 0;
    IQ = iq;
}

void Monster::setHarmless(int ticks, int hDuration, int wDuration)
{
    actualState = harmless;
    pauseDuration = ticks;
    pause = 0;
    harmlessDuration = harmlessLeft = hDuration;
    warningDuration = wDuration;
}

void Monster::setREM(int ticks)
{
    actualState = rem;
    pauseDuration = ticks;
    pause = 0;
}

void Monster::setPosition(int pos)
{
    board->reset(lastPosition, monster, ID);    // reset old position on the board
    actualPosition = lastPosition = pos;        // set position of monster
    board->set(actualPosition, monster, ID);
    feetPosition = 0;
}

void Monster::setPrison(int pos)
{
    prisonPosition = pos;
}

void Monster::setFreedom(int pos)
{
    freedomPosition = pos;
}

void Monster::setDirection(int dir)
{
    if (dir == X)
        lastDirection = actualDirection;
    actualDirection = dir;
}

monsterState Monster::state()
{
    return actualState;
}

int Monster::position()
{
    return actualPosition;
}

int Monster::direction()
{
    return actualDirection;
}

int Monster::id()
{
    return ID;
}

bool Monster::move()
{
    if (arrestLeft > 1)
        arrestLeft--;

    if (harmlessLeft > 0) {
        harmlessLeft--;
        if (harmlessLeft == 0 && actualState == harmless) {
            actualState = dangerous;
            pauseDuration = dangerousPause; 
        }
    }

    if (pause-- > 0)
        return FALSE;
    else
        pause = pauseDuration;

    if (actualPosition == OUT)
        return FALSE;

    if (actualDirection == X) {
        if (++feetPosition >= (maxBodyPixmaps/10))
            feetPosition = 0;
        return TRUE;
    }

    lastPosition = actualPosition;
    int d = actualDirection;

    if (arrestLeft > 1) {               // during the arrest, only up and down
          if (!board->isWay(actualPosition, d, empty) &&
              !board->isWay(actualPosition, d, tunnel))
            d = board->turn(actualDirection);
    }

    if (arrestLeft == 1) {              // going out of the prison
        if (((d == W || d == E) &&
             board->x(actualPosition) == board->x(freedomPosition)) ||
            ((d == S || d == N) &&
             board->y(actualPosition) == board->y(freedomPosition)) ||
             board->isWay(actualPosition, d, brick) ||
             board->isWay(actualPosition, d, prison)) {
            d = board->closeup(actualPosition, d, freedomPosition);
        }
        while (board->isWay(actualPosition, d, brick) ||
               board->isWay(actualPosition, d, prison)) {
            if (d == actualDirection)
                d = rand() % 4;
            else
                d = actualDirection;
        }
        if (actualState == dangerous)
            pauseDuration = dangerousPause;
        
    } 

    if (arrestLeft == 0)                        
        if (actualState == rem) {       // on the way to prison

            d = board->closeup(actualPosition, d, prisonPosition);

            while (board->isWay(actualPosition, d, brick) ||
                   board->isWay(actualPosition, d, prison)) {
                if (d != actualDirection)       // if new direction is not possible,
                    d = actualDirection;        // try current direction first.
                else
                    d = rand() % 4;
            }

        } else {                                // dangerous or harmless movement
            if (rand() % (int) ((190-IQ)/10) == 0) {
                d = board->closeup(actualPosition, d, board->position(pacman));
                if (actualState == harmless)
                    d = board->turn(d);
            } else
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
        }

    actualDirection = d;
    actualPosition = board->move(actualPosition, actualDirection);
    
    if (arrestLeft == 1 && actualPosition == freedomPosition) 
        arrestLeft = 0;

    if (actualState == rem && actualPosition == prisonPosition) {
        actualState = dangerous;
        pauseDuration = arrestPause;
        arrestLeft = arrestDuration+1;
        actualDirection = S;
    }

    if (actualPosition != lastPosition) {
        board->reset(lastPosition, monster, ID);
        board->set(actualPosition, monster, ID);
    }

    if (++feetPosition >= (maxBodyPixmaps/10))
        feetPosition = 0;

    return TRUE;
}

int Monster::body()
{
    if (actualState == rem || actualPosition == OUT)
        return -1;
    else
        if (actualState == harmless)
            if (harmlessLeft > warningDuration || 
                harmlessLeft % (int) (warningDuration/4.5) > (int) (warningDuration/9))
                return ((maxBodyPixmaps/10)*8)+feetPosition;
            else
                return ((maxBodyPixmaps/10)*9)+feetPosition;
        else
            return ((maxBodyPixmaps/10)*ID)+feetPosition;
}

int Monster::eyes()
{
    if (actualState == harmless || actualPosition == OUT)
        return -1;
    else
        switch (actualDirection) {
            case N  : return 0;
            case E  : return 1;
            case S  : return 2;
            case W  : return 3;
            case X  : switch (lastDirection) {
                          case N  : return 0;
                          case E  : return 1;
                          case S  : return 2;
                          default : return 3;
                      }
            default : return -1;
        }
}

