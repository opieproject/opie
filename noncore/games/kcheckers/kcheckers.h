
#ifndef KCHECKERS_H
#define KCHECKERS_H

#include <qmainwindow.h>
#include <qlabel.h>

#include "field.h"
#include "checkers.h"

#define WOODEN 1
#define GREEN  2
#define MARBLE 3

#define ENGLISH 11
#define RUSSIAN 12

#define BEGINNER 2
#define NOVICE   4
#define AVERAGE  6
#define GOOD     7
#define EXPERT   8
#define MASTER   9

class QToolButton;


class KCheckers:public QMainWindow
{
  Q_OBJECT
  public:
    KCheckers();

  protected:
    void closeEvent(QCloseEvent*);

  private slots:

    void help();
    void about();
    void aboutQt();
    void newGame();
    void undoMove();
    void click(int);
    void showNumeration();

    void setSkillBeginner() {setSkill(BEGINNER);};
    void setSkillNovice()   {setSkill(NOVICE);};
    void setSkillAverage()  {setSkill(AVERAGE);};
    void setSkillGood()     {setSkill(GOOD);};
    void setSkillExpert()   {setSkill(EXPERT);};
    void setSkillMaster()   {setSkill(MASTER);};

    void setPatternWooden() {setPattern(WOODEN);};
    void setPatternGreen()  {setPattern(GREEN);};
    void setPatternMarble() {setPattern(MARBLE);};

    void setRulesEnglish()  {setRules(ENGLISH);};
    void setRulesRussian()  {setRules(RUSSIAN);};

  private:

    void compGo();
    bool userGo(int);

    void drawBoard(int);
    void drawNumeration();
    void colorChange();
    void unselect();
    void readConfig();

    void setSkill(int);
    void setRules(int);
    void setPattern(int);

    int  from;           // Selected by user
    int  skill;
    int  rules;
    int  pattern;

    int  numID;          // Show Numeration
    int  undoID;         // Undo Move
    int  undoBoard[32];

    bool gameOver;
    bool selected;
    bool userFirst;

    Field*  field[64];   // Fields of board

    QImage* imagePat1;   // Patterns
    QImage* imagePat2;
    QImage* imageFrame;  // Frame of selected field

    QImage* imageWood1;
    QImage* imageWood2;
    QImage* imageWood3;
    QImage* imageGreen1;
    QImage* imageGreen2;
    QImage* imageGreen3;
    QImage* imageMarble1;
    QImage* imageMarble2;
    QImage* imageMarble3;

    QImage* imageMan1;
    QImage* imageMan2;
    QImage* imageKing1;
    QImage* imageKing2;

    Checkers*    game;
    QPopupMenu*  gameMenu;
    QPopupMenu*  skillMenu;
    QPopupMenu*  optionsMenu;
    QToolButton* undoButton;
    QLabel*      statusLabel;

    static QString enNumeration;
    static QString ruNumeration;

    static const int t[32];  // Translate table

};

#endif

