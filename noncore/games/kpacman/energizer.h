#ifndef ENERGIZER_H
#define ENERGIZER_H

#include "board.h"

enum energizerState { on, off };

class Energizer {
public:
    Energizer(Board *b);
    void setMaxPixmaps(int max);
    void setOff();
    void setOn();
    void setPosition(int pos);
    energizerState state();
    int position();
    bool move();
    int pix();

private:
    Board   *board;

    energizerState actualState;         // the state of energizer

    int  actualPix;                     // last Pixmap-index
    int  maxPixmaps;                    // Number of Pixmaps (1..)
    int  actualPosition;                // actual position on board
};

#endif // ENERGIZER_H
