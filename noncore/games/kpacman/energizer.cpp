#include "energizer.h"

Energizer::Energizer(Board *b)
{
    board = b;
    setOn();
    actualPosition = OUT;
    maxPixmaps = 0;
}

void Energizer::setMaxPixmaps(int max)
{
    maxPixmaps = max;
}

void Energizer::setOff()
{
    actualState = off;
}

void Energizer::setOn()
{
    actualState = on;
    actualPix = 0;
}

void Energizer::setPosition(int pos)
{
    board->reset(actualPosition, energizer);
    actualPosition = pos;
    board->set(actualPosition, energizer);
}

energizerState Energizer::state()
{
    return actualState;
}

int Energizer::position()
{
    return actualPosition;
}

bool Energizer::move()
{
    if (actualPosition == OUT)
        return FALSE;

    if (++actualPix >= maxPixmaps)
        actualPix = 0;

    return TRUE;
}

int Energizer::pix()
{
    if (actualPosition == OUT || actualState == off)
        return -1;

    return actualPix;
}
