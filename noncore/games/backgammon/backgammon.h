#ifndef BACKGAMMON_H
#define BACKGAMMON_H

#include "canvasimageitem.h"
#include "definition.h"
//#include "rulesdialog.h"
#include "moveengine.h"

#include <qcanvas.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwidget.h>


class BackGammonView : public QCanvasView
{
    Q_OBJECT
public:
    BackGammonView(QCanvas* canvas,QWidget* parent);
    ~BackGammonView();
signals:
    void mouse(int,int);
private:
    void contentsMousePressEvent(QMouseEvent* e);
};

class BackGammon : public QWidget
{
    Q_OBJECT
private:
    //GUI
    //is the style not qte ?
    bool non_qte;
    int offset;
    //the "status" bar
    QLineEdit* inputfield;
    //the main drawing area
    QCanvas* area;
    BackGammonView* boardview;
    CanvasImageItem* board;
    CanvasImageItem* table;
    CanvasImageItem** p1;
    CanvasImageItem** p2;
    CanvasImageItem** p1_side;
    CanvasImageItem** p2_side;

    CanvasImageItem** diceA1;
    CanvasImageItem** diceA2;
    CanvasImageItem** diceB1;
    CanvasImageItem** diceB2;
    //CanvasImageItem** oddsDice;
    CanvasImageItem* nomove_marker;
   
    QCanvasRectangle* marker_current;
    QCanvasRectangle* marker_next[4];
    QLabel* message;
    //ENGINE
    MoveEngine* move;
    //the dice values
    int diceA1_value;
    int diceA2_value;
    int diceA3_value;
    int diceA4_value;
    int diceB1_value;
    int diceB2_value;
    int diceB3_value;
    int diceB4_value;

    int player;
    bool dice1_played;
    bool dice2_played;
    bool dice3_played;
    bool dice4_played;
    bool dice_rolled;
    //computer opponent
    bool player1_auto;
    bool player2_auto;

    //the images;
    QString theme_name;
    QString board_name;
    QString piecesA_name;
    QString piecesB_name;
    QString diceA_name;
    QString diceB_name;
    QString odds_name;
    QString table_name;

    //save game
    QString game_name;

    //the rules
    Rules rules;

    //display settings
    Display display;
    //is the game finished ?
    bool gameFinished;

public:
    BackGammon( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~BackGammon();
private slots:
    void newgame();
    void playerselect();
    void loadgame();
    void savegame();
    void deletegame();
    void newtheme();
    void loadtheme();
    void savetheme();
    void themedefault();
    void deletetheme();
    void modify_AI();
    void setrules();
    void displaySettings();
    void mouse(int x,int y);
    void done_dice1();
    void done_dice2();
    void done_dice3();
    void done_dice4();
    void nomove();
    void nomove2();
    void finished(int theplayer);
    void autoroll_dice1();
    void autoroll_dice2();
private:
    void draw();
    void showdice();
    void setplayer();
    void applytheme();
};

#endif //BACKGAMMON_H
