#ifndef DEFINITION_H
#define DEFINITION_H


struct Coord
{
    int x;
    int y;
    int z;
    bool side;
};

struct Pieces
{
    Coord player1[15];
    Coord player2[15];
};

struct Population
{
    //positive = player 1
    //negative = player 2
    //zero = no pieces
    int total;
};

struct Marker
{
    int x_current;
    int y_current;
    bool visible_current;

    int x_next[4];
    int y_next[4];
    bool visible_next[4];
};

struct LoadSave
{
    Population pop[28];
};

struct Possiblilites
{
    int weight[4];
    int to[4];
};

struct AISettings
{
    int rescue;
    int eliminate;
    int expose;
    int protect;
    int  safe;
    int empty;
};

struct Rules
{
    bool move_with_pieces_out;
    bool generous_dice;
};


struct Display
{
  bool small;
  bool warning;
};



#endif //DEFINITION_H
