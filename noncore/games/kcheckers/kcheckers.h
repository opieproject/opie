
#ifndef KCHECKERS_H
#define KCHECKERS_H

#include <qmainwindow.h>
#include <qlabel.h>

#include "field.h"
#include "checkers.h"

#define WOOD   1
#define MARBLE 2

#define ENGLISH 11
#define RUSSIAN 12

#define BEGINNER 2
#define NOVICE   4
#define AVERAGE  5
#define GOOD     6
#define EXPERT   7
#define MASTER   8


class KCheckers:public QMainWindow
{
  Q_OBJECT
  public:
    KCheckers();

  private slots:
    void about();
    void aboutQt();
    void newGame();
    void click(int);    // Processes the mouse clics on fields

    void setSkillBeginner() {setSkill(BEGINNER);};
    void setSkillNovice()   {setSkill(NOVICE);};
    void setSkillAverage()  {setSkill(AVERAGE);};
    void setSkillGood()     {setSkill(GOOD);};
    void setSkillExpert()   {setSkill(EXPERT);};
    void setSkillMaster()   {setSkill(MASTER);};

    void setPatternWood()   {setPattern(WOOD);};
    void setPatternMarble() {setPattern(MARBLE);};

    void setRulesEnglish()  {setRules(ENGLISH);};
    void setRulesRussian()  {setRules(RUSSIAN);};

  private:
    void compGo();
    bool userGo(int);

    void drawBoard();
    void colorChange();

    void setSkill(int);
    void setRules(int);
    void setPattern(int);

    int  from;           // Selected by user
    int  skill;
    int  rules;
    int  pattern;
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
    QImage* imageMarble1;
    QImage* imageMarble2;
    QImage* imageMarble3;

    QImage* imageMan1;
    QImage* imageMan2;
    QImage* imageKing1;
    QImage* imageKing2;

    Checkers*   game;
    QPopupMenu* skillMenu;
    QPopupMenu* optionsMenu;
    QLabel*     statusLabel;

    static const int t[32];  // Translate table

};

#endif

