/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef WORDGAME_H
#define WORDGAME_H

#include "newgamebase.h"
#include "rulesbase.h"

#include <qpe/qdawg.h>
#include <qpe/applnk.h>

#include <qmainwindow.h>
#include <qcanvas.h>
#include <qlabel.h>

class QVBox;
class QLabel;
class QWidgetStack;
class QToolButton;
class Config;

class Tile {
public:
    Tile() {}

    Tile(const Tile& t)
    {
	txt = t.txt;
	val = t.val;
	blank = t.blank;
    }

    Tile(QString text, int value)
    {
	txt = text;
	val = value;
	blank = txt.isEmpty();
    }

    Tile(const QString& key);

    int value() const { return val; }
    bool isBlank() const { return blank; }
    QString text() const { return txt; }
    void setText(const QString& t)
    {
	txt = t;
    }

    int operator==(const Tile& o) const
	{ return o.txt == txt && o.val == val && o.blank == blank; }
    int operator!=(const Tile& o) const
	{ return !operator==(o); }
    Tile& operator=(const Tile& o)
	{ txt=o.txt; val=o.val; blank=o.blank; return *this; }

    QString key() const;

private:
    QString txt;
    int val;
    bool blank;
};

class Bag {
public:
    Bag();

    void readConfig(Config&);
    void writeConfig(Config&);

    void add(const Tile&);
    bool isEmpty() const { return tiles.isEmpty(); }
    Tile takeRandom();
private:
    QList<Tile> tiles;
};

class TileItem : public QCanvasRectangle {
public:
    TileItem(const Tile& tile, bool b, QCanvas* c) :
	QCanvasRectangle(0,0,
	    b?bigWidth():smallWidth(),
	    b?bigHeight():smallHeight(),c),
	t(tile), big(b), s(Firm)
    {
    }

    static int smallWidth();
    static int smallHeight();
    static int bigWidth();
    static int bigHeight();

    enum State { Firm, Floating };
    void setState( State state );
    State state() const { return s; }
    const Tile& tile() const { return t; }
    void setTile(const Tile&);
    void setBig(bool);

protected:
    void drawShape(QPainter&);

private:
    Tile t;
    bool big;
    State s;
};

class Rack : public QCanvasView {
public:
    Rack(int ntiles, QWidget* parent);
    ~Rack();

    void readConfig(Config&);
    void writeConfig(Config&);

    bool isFull() const { return count()==max(); }
    int max() const { return item.count(); }
    int count() const { return n; }
    void addTile(const Tile& t);
    Tile tile(int i) const { return item[i]->tile(); }
    const Tile* tileRef(int i) const { return &item[i]->tile(); }
    void remove(int i);
    void remove(Tile);
    bool arrangeTiles(const Tile** s, int sn);
    void setBlanks(const Tile*);

    void setPlayerName(const QString& name) { nm = name; }
    QString playerName() const { return nm; }
    void setComputerization(int level) { cpu=level; }
    bool computerized() const { return cpu>0; }

    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent*e);
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);
    void contentsMouseReleaseEvent(QMouseEvent*);

private:
    void clear();
    void layoutTiles();
    int n;
    QArray<TileItem*> item;
    int dragging_adj;
    QPoint dragstart;
    QCanvasItem* dragging;
    QString nm;
    int cpu;
};

class Board : public QCanvasView {
    Q_OBJECT
public:
    Board(QPixmap bgshapes, int w, int h, QWidget* parent);
    ~Board();

    void readConfig(Config&);
    void writeConfig(Config&);

    int xTiles() const { return canvas()->tilesHorizontally(); }
    int yTiles() const { return canvas()->tilesVertically(); }

    bool contains(const QPoint& p) const
	{ return p.x() >= 0 && p.y() >= 0
	      && p.x() < canvas()->tilesHorizontally()
	      && p.y() < canvas()->tilesVertically(); }
    const Tile* tile(const QPoint& p) const
	{ TileItem* it=item(p); return it ? &it->tile() : 0; }

    void setRules(const QString& shapes, const int* effects);

    void clear();
    void unsetTile(const QPoint& p);
    void setTile(const QPoint& p, const Tile& t);

    void setTileState(const QPoint& p, TileItem::State s)
    {
	TileItem* it=item(p);
	if (it) it->setState(s);
    }

    void setCurrentRack(Rack*);
    void resetRack();
    void finalizeTurn();
    void showTurn();
    void scoreTurn(const QPoint& at, int n, const QPoint& d);
    bool checkTurn();
    int score(QPoint at, const Tile** tiles, int n,
	const Tile* blankvalue,
	const QPoint& d, bool ignoredict, QStringList* words) const;
    int bonussedValue(const QPoint& at, int base, int& all_mult) const;
    bool isStart(const QPoint& at) const;

    int turnScore() const { return turn_score; }

    QSize sizeHint() const;

signals:
    void temporaryScore(int);

protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);
    void contentsMouseReleaseEvent(QMouseEvent*);

private:
    int idx(const QPoint& p) const
	{ return p.x()+p.y()*canvas()->tilesHorizontally(); }
    TileItem*& item(const QPoint& p) const
	{ return grid[idx(p)]; }
    TileItem **grid;
    QString rule_shape;
    const int* rule_effect;
    int rack_tiles_bonus;
    Rack* current_rack;
    QPoint boardPos(const QPoint&) const;
    QPoint dragstart;
    QPoint shown_at;
    int shown_n;
    QPoint shown_step;
    void unshowTurn();
    int turn_score;
};

class ComputerPlayer
{
    Board* board;
    Rack* rack;

    bool across;
    int dict;
    QPoint current;

    const Tile** best;
    int best_n;
    Tile* best_blankvalues;
    int best_blused;
    int best_score;
    QPoint best_dir;
    QPoint best_start;

public:
    ComputerPlayer(Board* b, Rack* r);
    ~ComputerPlayer();

    bool step();

private:
    void findBest(QPoint at, const QPoint& d, const QDawg::Node* node, ulong used, uchar *nletter, const Tile** tiles, int n, Tile* blankvalues, int blused);
    void noteChoice(const Tile** tiles, int n, const QPoint& d, const Tile* blankvalues, int blused);
};

class ScoreInfo : public QLabel {
    Q_OBJECT
public:
    ScoreInfo( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ScoreInfo();

    void init(const QStringList&);
    void addScore(int player, int change);
    int playerScore(int player) const { return score[player]; }
    void setShowWinner(bool);
    void setBoldOne(int);

    void readConfig(Config&);
    void writeConfig(Config&);

protected:
    QSize sizeHint() const;

public slots:
    void showTemporaryScore(int amount);

private slots:
    void showScores();

private:
    QStringList names;
    int *score;
    QTimer* msgtimer;
    bool showwinner;
    int boldone;
};

class NewGame;

class WordGame : public QMainWindow {
    Q_OBJECT
public:
    WordGame( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~WordGame();

private slots:
    void endTurn();
    void resetTurn();
    void passTurn();
    void think();
    void endGame();
    void startGame();

private:
    void writeConfig();
    void readConfig();

    void startGame(const QStringList& pnames);
    bool mayEndGame();
    void openGameSelector(const QStringList& initnames);
    bool loadRules(const QString& filename);
    void addPlayer(const QString& name);
    void addPlayer(const QString& name, int cpu);
    void nextPlayer();
    bool refillRack(int i);
    void readyRack(int i);
    Rack* rack(int i) const;

    QWidgetStack *racks;
    QToolBar* toolbar;
    QWidget *vbox;
    Board *board;
    Bag *bag;
    ScoreInfo *scoreinfo;
    QToolButton *done;
    QToolButton *reset;
    QTimer* aiheart;
    ComputerPlayer *cpu;
    int player;
    int nplayers;
    QStringList namelist;
    bool gameover;
    QString rules;
    NewGame* newgame;
};

class NewGame : public NewGameBase {
    Q_OBJECT
public:
    NewGame(QWidget* parent);
    QStringList ruleslist;

public slots:
    void updateRuleSets();
};

class Rules : public RulesBase {
    Q_OBJECT

public:
    Rules(QWidget* parent);

signals:
    void rulesChanged();

public slots:
    void editRules();

private:
    void deleteRuleSet();
};

#endif // WORDGAME_H
