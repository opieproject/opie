#ifndef MOVEENGINE_H
#define MOVEENGINE_H

#include <qobject.h>
#include "definition.h"

class MoveEngine : public QObject
{
    Q_OBJECT
private:
    //normal pieses
    int x_coord[26]; //26 posssible x-positions or piece is not on the board
    int yup_coord[15]; // the 15 posssible y-positions on the upper half of the board
    int ylow_coord[15]; // the 15 posssible y-positions on the lower half of the board
    int z_coord[15]; //the 15 possible z-positionson the board
    //finshed pieces
    int x_fin1[3];
    int x_fin2[3];
    int y_fin[5];
    int z_fin;
    //the board population
    // |12|11|10|09|08|07|06|05|04|03|02|01|00|
    // =======================================
    // |13|14|15|16|17|18|19|20|21|22|23|24|25|
    // endzones 26 player1, 27 player 2
    Population population[28];
    AISettings ai;
    //move information
    int player;
    int otherplayer;
    int dice[4];
    //index of the markers
    int marker_current;
    int marker_next[4];
    //player pieces are all in the end zone
    bool allclear[3];
    //player must bring pieces back into game
    bool pieces_out[3];
    bool move_with_pieces_out;
    //player can rescue pieces with dice bigger than move, even if there are poeces "behind" it
    bool nice_dice;
    int last_piece[3];
    //possible moves
    Possiblilites moves[26];
public:
public:
    MoveEngine();
    ~MoveEngine();
signals:
    void done_dice1();
    void done_dice2();
    void done_dice3();
    void done_dice4();
    void nomove();
    void player_finished(int);
private slots:
    void automove();
public:
    void position(Pieces& pieces,bool non_qte=false);
    void diceroll(const int& player,const int& face1,const int& face2,const int& face3,const int& face4,bool computer);
    void boardpressed(const int& x,const int& y,Marker& marker,bool non_qte=false);
    void reset();
    void loadGame(const LoadSave& load);
    LoadSave saveGame();
    AISettings getAISettings();
    void setAISettings(const AISettings& new_ai);
    void setRules(Rules rules);
private:
    int getPossibleMoves();
    void move(const int& from, int to, const int& dice);
    void checkstate();
    void nomarker(Marker& marker);
    int fieldColor(const int& index) const;
};

#endif //MOVEENGINE_H
