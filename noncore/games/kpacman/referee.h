#ifndef REFEREE_H
#define REFEREE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#endif

#include <qwidget.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qarray.h>
#include <qbitarry.h>

#include "board.h"
#include "pacman.h"
#include "monster.h"
#include "fruit.h"
#include "energizer.h"
#include "bitfont.h"
#include "painter.h"

enum { Init, Introducing, Playing, Demonstration, Paused, Player, Ready, 
       Scoring, LevelDone, Dying, GameOver, HallOfFame };


class Referee : public QWidget
{
    Q_OBJECT
public:
    Referee (QWidget *parent=0, const char *name=0, int scheme=-1, int mode=-1, Bitfont *font=0);

    void setSkill(int);
    void setRoom(int);

public slots:
    void setScheme(int scheme, int mode, Bitfont *font=0);

    void levelUp();
    void levelUpPlay();

    void pause();
    void ready();
    void intro();
    void introPlay();
    void hallOfFame();
    void demo();
    void play();
    void killed();
    void killedPlay();
    void eaten();
    void toggleHallOfFame();

    void setFocusOutPause(bool focusOutPause);
    void setFocusInContinue(bool focusInContinue);
    void initKeys();

    void repaintFigures();

private slots:
    void start();
    void stop();
    void stopEnergizer();

signals:
    void setScore(int, int);
    void setPoints(int);
    void setLevel(int);
    void setLifes(int);

    void toggleNew();
    void togglePaused();
    void forcedHallOfFame(bool);

protected:
    void timerEvent(QTimerEvent *);
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);

    void focusOutEvent(QFocusEvent *);
    void focusInEvent(QFocusEvent *);

    void fillArray(QArray<int> &, QString, int);
    void fillStrList(QStrList &, QString, int);
    void fillMapName();

    void confScheme();
    void confLevels(bool defGroup=TRUE);
    void confMisc(bool defGroup=TRUE);
    void confTiming(bool defGroup=TRUE);
    void confScoring(bool defGroup=TRUE);

private:
    QBitArray gameState;
    int timerCount;
    int maxLevel;

    int scheme;
    int mode;

    QString pixmapDirectory;
    QString mapDirectory;
    QStrList mapName;

    QArray<int> speed;
    QArray<int> monsterIQ;
    QArray<int> fruitIQ;
    QArray<int> fruitIndex;
    QArray<int> pacmanTicks;
    QArray<int> remTicks;
    QArray<int> dangerousTicks;
    QArray<int> harmlessTicks;
    QArray<int> harmlessDurTicks;
    QArray<int> harmlessWarnTicks;
    QArray<int> arrestTicks;
    QArray<int> arrestDurTicks;
    QArray<int> fruitTicks;
    QArray<int> fruitAppearsTicks;
    QArray<int> fruitDurTicks;
    QArray<int> fruitScoreDurTicks;

    int monsterScoreDurMS;
    int playerDurMS;
    int readyDurMS;
    int gameOverDurMS;
    int afterPauseMS;
    int dyingPreAnimationMS;
    int dyingAnimationMS;
    int dyingPostAnimationMS;
    int introAnimationMS;
    int introPostAnimationMS;
    int levelUpPreAnimationMS;
    int levelUpAnimationMS;
    int energizerAnimationMS;

    int pointScore;
    int energizerScore;
    QArray<int> fruitScore;
    QArray<int> monsterScore;
    QArray<int> extraLifeScore;

    int extraLifeScoreIndex;
    int nextExtraLifeScore;

    int monstersEaten;
    int points;
    int lifes;
    int level;

    bool focusedPause;
    bool focusOutPause;
    bool focusInContinue;

    Board   *board;
    Painter *pix;
    Pacman  *pacman;
    Fruit   *fruit;

    QList<Monster> *monsters;
    QList<QRect> *monsterRect;

    QList<Energizer> *energizers;
    QList<QRect> *energizerRect;

    QRect pacmanRect;
    QRect fruitRect;

    void introMonster(int id);
    void introPaint(int t);

    void initMonsters();
    void initPacman();
    void initFruit(bool fullInitialization=TRUE);
    void initEnergizers();

    void setOnEnergizers();

    int  gameTimer;
    int  energizerTimer;
    void start(int);
    void init(bool);

    void score(int);

    uint UpKey;
    uint DownKey;
    uint RightKey;
    uint LeftKey;
};

#endif // REFEREE_H
