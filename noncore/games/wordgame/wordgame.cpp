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


#include "wordgame.h"

#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/filemanager.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdir.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qpe/qpetoolbar.h>
#include <qtoolbutton.h>
#include <qvbox.h>
#include <qwidgetstack.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qregexp.h>

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

enum RuleEffects {
    Multiplier=15,
    MultiplyAll=64,
    Start=128
};

static int tile_smallw = 16;
static int tile_smallh = 16;
static int tile_bigw = 22;
static int tile_bigh = 22;
static int tile_stweak = -2;
static int tile_btweak = -1;

static const int rack_tiles=7;

const char* sampleWGR=
	"wordgame_shapes\n"
	"15 15\n"
	"400001040100004\n"
	"030000000000030\n"
	"002002000200200\n"
	"000300020003000\n"
	"000020000020000\n"
	"102001000100201\n"
	"000000202000000\n"
	"400200050002004\n"
	"000000202000000\n"
	"102001000100201\n"
	"000020000020000\n"
	"000300020003000\n"
	"002002000200200\n"
	"030000000000030\n"
	"400001040100004\n"
	"1 2 3 66 67 194 100 0\n"
	"1 j 8\n"
	"1 q 7\n"
	"1 x 6\n"
	"1 z 6\n"
	"1 w 4\n"
	"1 k 4\n"
	"1 v 3\n"
	"1 f 3\n"
	"2 y 3\n"
	"2 h 2\n"
	"2 b 2\n"
	"2 m 2\n"
	"3 p 2\n"
	"3 g 2\n"
	"3 u 2\n"
	"4 d 2\n"
	"4 c 2\n"
	"5 l 1\n"
	"5 o 1\n"
	"7 t 1\n"
	"7 n 1\n"
	"7 a 1\n"
	"7 r 1\n"
	"8 s 1\n"
	"8 i 1\n"
	"11 e 1\n"
	"0\n";

WordGame::WordGame( QWidget* parent, const char* name, WFlags fl ) :
    QMainWindow(parent, name, fl)
{
    if ( qApp->desktop()->width() < 240 ) {
	tile_smallw = 10;
	tile_smallh = 10;
	tile_bigw = 16;
	tile_bigh = 16;
	tile_stweak = 0;
	tile_btweak = 0;
    }

    setIcon( Resource::loadPixmap( "wordgame/WordGame.png" ) );
    setCaption( tr("Word Game") );

    setToolBarsMovable( FALSE );
    vbox = new QVBox(this);

    setCentralWidget(vbox);
    toolbar = new QPEToolBar(this);
    addToolBar(toolbar, Bottom);
    reset = new QToolButton(Resource::loadPixmap("back"), tr("Back"), "", this, SLOT(resetTurn()), toolbar);
    done = new QToolButton(Resource::loadPixmap("done"), tr("Done"), "", this, SLOT(endTurn()), toolbar);
    scoreinfo = new ScoreInfo(toolbar);
    scoreinfo->setFont(QFont("Helvetica",10));
    new QToolButton(Resource::loadPixmap("finish"), tr("Close"), "", this, SLOT(endGame()), toolbar);
    toolbar->setStretchableWidget(scoreinfo);

    cpu = 0;
    board = 0;
    bag = 0;
    racks = 0;

    aiheart = new QTimer(this);
    connect(aiheart, SIGNAL(timeout()), this, SLOT(think()));

    readConfig();
}

WordGame::~WordGame()
{
    writeConfig();
}

void WordGame::writeConfig()
{
    Config cfg("WordGame");
    cfg.setGroup("Game");
    cfg.writeEntry("NameList",namelist,';');
    cfg.writeEntry("CurrentPlayer",gameover ? 0 : player+1);
    if ( !gameover ) {
	cfg.writeEntry("Rules",rules);
	bag->writeConfig(cfg);
	board->writeConfig(cfg);
	scoreinfo->writeConfig(cfg);
    }
    for (int p=0; p<nplayers; p++) {
	cfg.setGroup("Player"+QString::number(p+1));
	if ( gameover ) cfg.clearGroup(); else rack(p)->writeConfig(cfg);
    }
}

void WordGame::readConfig()
{
    Config cfg("WordGame");
    cfg.setGroup("Game");
    int currentplayer = cfg.readNumEntry("CurrentPlayer",0);
    QStringList pnames = cfg.readListEntry("NameList",';');
    if ( currentplayer ) {
	gameover = FALSE;
	rules = cfg.readEntry("Rules");
	if ( rules.find("x-wordgamerules") >= 0 ) {
	    // rules files moved
	    rules = "Sample.rules";
	}
	if ( loadRules(rules) ) {
	    startGame(pnames);
	    bag->readConfig(cfg);
	    board->readConfig(cfg);
	    scoreinfo->readConfig(cfg);
	    for (int p=0; p<nplayers; p++) {
		cfg.setGroup("Player"+QString::number(p+1));
		rack(p)->readConfig(cfg);
	    }
	    player=currentplayer-1;
	    readyRack(player);
	    return;
	}
    }
    // fall-back
    openGameSelector(pnames);
}

void WordGame::openGameSelector(const QStringList& initnames)
{
    toolbar->hide();
    gameover = FALSE;

    delete board;
    board = 0;
    delete racks;
    racks = 0;

    delete cpu;
    cpu = 0;

    newgame = new NewGame(vbox);

    //Rules rules(this);
    //connect(game.editrules, SIGNAL(clicked()), &rules, SLOT(editRules()));
    //connect(&rules, SIGNAL(rulesChanged()), &game, SLOT(updateRuleSets()));
    struct passwd* n = getpwuid(getuid());
    QString playername = n ? n->pw_name : "";
    if ( playername.isEmpty() ) {
	playername = "Player";
    }
    newgame->player0->changeItem(playername,0);
    newgame->player1->setCurrentItem(1);
    newgame->updateRuleSets();
    newgame->show();

    connect(newgame->buttonOk, SIGNAL(clicked()), this, SLOT(startGame()));
}

void WordGame::startGame()
{
    rules = newgame->ruleslist[newgame->rules->currentItem()];
    if ( loadRules(rules) ) {
	QStringList names;
	names.append(newgame->player0->currentText());
	names.append(newgame->player1->currentText());
	names.append(newgame->player2->currentText());
	names.append(newgame->player3->currentText());
	names.append(newgame->player4->currentText());
	names.append(newgame->player5->currentText());
	delete newgame;
	startGame(names);
    } else {
	// error...
	delete newgame;
	close();
    }
}

void WordGame::startGame(const QStringList& playerlist)
{
    toolbar->show();
    racks = new QWidgetStack(vbox);
    racks->setFixedHeight(TileItem::bigHeight()+2);
    namelist.clear();
    nplayers=0;
    for (QStringList::ConstIterator it=playerlist.begin(); it!=playerlist.end(); ++it)
	addPlayer(*it);
    scoreinfo->init(namelist);

    if ( nplayers ) {
	player=0;
	readyRack(player);
    }

    board->show();
    racks->show();
}

bool WordGame::loadRules(const QString &name)
{
    QString filename = Global::applicationFileName( "wordgame", name );
    QFile file( filename );
    if ( !file.open( IO_ReadOnly ) )
	return FALSE;

    QTextStream ts( &file );

    QString title = name;
    title.truncate( title.length() - 6 );
    setCaption( title );

    QString shapepixmap;
    ts >> shapepixmap;
    int htiles,vtiles;
    ts >> htiles >> vtiles;

    if ( htiles < 3 || vtiles < 3 )
	return FALSE;

    QString rule_shapes;
    for (int i=0; i<vtiles; i++) {
	QString line;
	ts >> line;
	rule_shapes += line;
    }
    static int rule_effects[12];
    int re=0,e;
    ts >> e;
    while ( e && re < 10 ) {
	rule_effects[re] = e;
	if ( re++ < 10 ) ts >> e;
    }

    QImage shim = Resource::loadImage("wordgame/wordgame_shapes.xpm");
    shim = shim.smoothScale((re-1)*TileItem::smallWidth(),TileItem::smallHeight());
    QPixmap bgshapes;
    bgshapes.convertFromImage(shim);

    rule_effects[re++] = 100; // default bonus
    board = new Board(bgshapes, htiles, vtiles, vbox);
    board->setRules(rule_shapes, rule_effects);
    connect(board, SIGNAL(temporaryScore(int)), scoreinfo, SLOT(showTemporaryScore(int)));

    bag = new Bag;

    int count;
    ts >> count;
    while ( count ) {
	QString text;
	int value;
	ts >> text >> value;
	if ( text == "_" )
	    text = "";

	Tile t(text, value);
	for (int n=count; n--; )
	    bag->add(t);

	ts >> count;
    }

    return TRUE;
}


NewGame::NewGame(QWidget* parent) :
    NewGameBase(parent)
{
}

void NewGame::updateRuleSets()
{
    rules->clear();

    QString rulesDir = Global::applicationFileName( "wordgame", "" );
    QDir dir( rulesDir, "*.rules" );
    ruleslist = dir.entryList();
    if ( ruleslist.isEmpty() ) {
	// Provide a sample
	QFile file( rulesDir + "Sample.rules" );
	if ( file.open( IO_WriteOnly ) ) {
	    file.writeBlock( sampleWGR, strlen(sampleWGR) );
	    file.close();
	    updateRuleSets();
	}
	return;
    }
    int newest=0;
    int newest_age=INT_MAX;
    QDateTime now = QDateTime::currentDateTime();
    QStringList::Iterator it;
    for ( it = ruleslist.begin(); it != ruleslist.end(); ++it ) {
	QFileInfo fi((*it));
	int age = fi.lastModified().secsTo(now);
	QString name = *it;
	name.truncate( name.length()-6 ); // remove extension
	rules->insertItem( name );
	if ( age < newest_age ) {
	    newest_age = age;
	    newest = rules->count()-1;
	}
    }
    rules->setCurrentItem(newest);
}

Rules::Rules(QWidget* parent) :
    RulesBase(parent,0,TRUE)
{
}

void Rules::editRules()
{
    if ( exec() ) {
	// ### create a new set of rules
	emit rulesChanged();
    }
}

void Rules::deleteRuleSet()
{
    // ### delete existing rule set
    emit rulesChanged();
}

void WordGame::addPlayer(const QString& name)
{
    if ( !name.isEmpty() ) {
	int colon = name.find(':');
	int cpu = (colon >=0 && name.left(2) == "AI") ? name.mid(2,1).toInt() : 0;
	addPlayer(name,cpu);
    }
}

void WordGame::addPlayer(const QString& name, int cpu)
{
    Rack* r = new Rack(rack_tiles,racks);
    r->setPlayerName(name);
    r->setComputerization(cpu);
    racks->addWidget(r, nplayers);
    refillRack(nplayers);
    namelist.append(name);

    ++nplayers;
}

void WordGame::nextPlayer()
{
    if ( !refillRack(player) ) {
	endGame();
    } else {
	player = (player+1)%nplayers;
	scoreinfo->setBoldOne(player);
	readyRack(player);
    }
}

bool WordGame::mayEndGame()
{
    int out=-1;
    int i;
    for (i=0; i<nplayers; i++)
	if ( !rack(i)->count() )
	    out = i;
    if ( out<0 ) {
	if ( QMessageBox::warning(this,tr("End game"),
				  tr("Do you want to end the game early?"), 
				  tr("Yes"), tr("No") )!=0 )
	{
	    return FALSE;
	}
    }
    return TRUE;
}

void WordGame::endGame()
{
    if ( gameover ) {
	close();
	return;
    }

    if ( !mayEndGame() )
	return;
    int out=-1;
    int totalleft=0;
    int i;
    for (i=0; i<nplayers; i++) {
	Rack* r = rack(i);
	int c = r->count();
	if ( c ) {
	    int lose=0;
	    for ( int j=0; j<c; j++ )
		lose += r->tileRef(j)->value();
	    totalleft += lose;
	    scoreinfo->addScore(i,-lose);
	} else {
	    out = i;
	}
    }
    int highest=0;
    int winner=0;
    for (i=0; i<nplayers; i++) {
	int s = scoreinfo->playerScore(i);
	if ( s > highest ) {
	    highest = s;
	    winner = i;
	}
    }
    if ( out >= 0 )
	scoreinfo->addScore(out,totalleft);
    scoreinfo->setBoldOne(winner);
    gameover = TRUE;
    done->setEnabled(TRUE);
    reset->setEnabled(FALSE);
}

void WordGame::endTurn()
{
    if ( gameover ) {
	openGameSelector(namelist);
    } else {
	if ( board->checkTurn() ) {
	    if ( board->turnScore() >= 0 ) {
		scoreinfo->addScore(player,board->turnScore());
		board->finalizeTurn();
	    } else {
		QApplication::beep();
	    }
	    nextPlayer();
	}
    }
}

void WordGame::resetTurn()
{
    board->resetRack();
}

void WordGame::passTurn()
{
    // ######## trade?
    nextPlayer();
}

bool WordGame::refillRack(int i)
{
    Rack* r = rack(i);
    while ( !bag->isEmpty() && !r->isFull() ) {
	r->addTile(bag->takeRandom());
    }
    return r->count() != 0;
}

void WordGame::readyRack(int i)
{
    Rack* r = rack(i);
    racks->raiseWidget(i);
    board->setCurrentRack(r);

    done->setEnabled( !r->computerized() );
    reset->setEnabled( !r->computerized() );

    if ( r->computerized() ) {
	cpu = new ComputerPlayer(board, r);
	aiheart->start(0);
    }
}

Rack* WordGame::rack(int i) const
{
    return (Rack*)racks->widget(i);
}

void WordGame::think()
{
    if ( !cpu->step() ) {
	delete cpu;
	cpu = 0;
	aiheart->stop();
	if ( board->turnScore() < 0 )
	    passTurn();
	else
	    endTurn();
    }
}

ComputerPlayer::ComputerPlayer(Board* b, Rack* r) :
    board(b), rack(r), best(new const Tile*[rack_tiles]),
	best_blankvalues(new Tile[rack_tiles])
{
    best_score = -1;
    across=FALSE;
    dict=0;
}

ComputerPlayer::~ComputerPlayer()
{
    delete [] best;
    delete [] best_blankvalues;
}

bool ComputerPlayer::step()
{
    const QDawg::Node* root = dict ? Global::dawg("WordGame").root()
				: Global::fixedDawg().root();
    QPoint d = across ? QPoint(1,0) : QPoint(0,1);
    const Tile* tiles[99]; // ### max board size
    uchar nletter[4095]; // QDawg only handles 0..4095
    memset(nletter,0,4096);
    for (int i=0; i<rack->count(); i++) {
	const Tile* r = rack->tileRef(i);
	if ( r->isBlank() )
	    nletter[0]++;
	else
	    nletter[r->text()[0].unicode()]++;
    }
    Tile blankvalues[99]; // ### max blanks
    findBest(current, d, root, 0, nletter, tiles, 0, blankvalues, 0);
    if ( ++current.rx() == board->xTiles() ) {
	current.rx() = 0;
	if ( ++current.ry() == board->yTiles() ) {
	    if ( across ) {
		if ( dict == 1 ) {
		    if ( best_score >= 0 ) {
			rack->arrangeTiles(best,best_n);
			rack->setBlanks(best_blankvalues);
			board->scoreTurn(best_start, best_n, best_dir);
			board->showTurn();
		    }
		    return FALSE;
		}
		dict++;
		across = FALSE;
		current = QPoint(0,0);
	    } else {
		across = TRUE;
		current = QPoint(0,0);
	    }
	}
    }
    return TRUE;
}

void ComputerPlayer::findBest(QPoint at, const QPoint& d, const QDawg::Node* node, ulong used, uchar* nletter, const Tile** tiles, int n, Tile* blankvalues, int blused)
{
    if ( !node )
	return;
    QChar l = node->letter();
    const Tile* cur = board->tile(at);
    if ( cur ) {
	if ( cur->text()[0] == l ) {
	    bool nextok =  board->contains(at+d);
	    if ( node->isWord() && n && (!nextok || !board->tile(at+d)) )
		noteChoice(tiles,n,d,blankvalues,blused);
	    if ( nextok )
		findBest(at+d, d, node->jump(), used, nletter, tiles, n, blankvalues, blused);
	    // #### text()[1]...
	}
    } else {
	if ( nletter[l.unicode()] || nletter[0] ) {
	    int rc = rack->count();
	    ulong msk = 1;
	    for ( int x=0; x<rc; x++ ) {
		if ( !(used&msk) ) {
		    const Tile* t = rack->tileRef(x);
		    if ( t->isBlank() || t->text() == l ) { // #### multi-char value()s
			bool nextok =  board->contains(at+d);
			tiles[n++] = t;
			if ( t->isBlank() )
			    blankvalues[blused++] = Tile(l,0);
			if ( node->isWord() && (!nextok || !board->tile(at+d)) )
			    noteChoice(tiles,n,d,blankvalues,blused);
			used |= msk; // mark
			nletter[t->text()[0].unicode()]--;
			if ( nextok )
			    findBest(at+d, d, node->jump(), used, nletter, tiles, n, blankvalues, blused);
			n--;
			nletter[t->text()[0].unicode()]++;
			if ( t->isBlank() ) {
			    // keep looking
			    blused--;
			    used &= ~msk; // unmark
			} else {
			    break;
			}
		    }
		}
		msk <<= 1;
	    }
	}
	// #### text()[1]...
    }
    findBest(at, d, node->next(), used, nletter, tiles, n, blankvalues, blused);
}

void ComputerPlayer::noteChoice(const Tile** tiles, int n, const QPoint& d, const Tile* blankvalues, int blused)
{
    int s = board->score(current, tiles, n, blankvalues, d, TRUE, 0);
/*
if (s>0 || current==QPoint(5,1)){
QString st;
for ( int i=0; i<n; i++ )
    st += tiles[i]->text();
qDebug("%d,%d: %s (%d) for %d",current.x(),current.y(),st.latin1(),n,s);
}
*/
    if ( s > best_score ) {
	int i;
	for ( i=0; i<n; i++ )
	    best[i] = tiles[i];
	for ( i=0; i<blused; i++ )
	    best_blankvalues[i] = blankvalues[i];
	best_n = n;
	best_blused = blused;
	best_score = s;
	best_dir = d;
	best_start = current;
    }
}

int TileItem::smallWidth()
{
    return tile_smallw;
}

int TileItem::smallHeight()
{
    return tile_smallh;
}

int TileItem::bigWidth()
{
    return tile_bigw;
}

int TileItem::bigHeight()
{
    return tile_bigh;
}

void TileItem::setState( State state )
{
    hide();
    s = state;
    show(); // ### use update() in Qt 3.0
}

void TileItem::setTile(const Tile& tile)
{
    hide();
    t = tile;
    show(); // ### use update() in Qt 3.0
}

void TileItem::setBig(bool b)
{
    big = b;
}

void TileItem::drawShape(QPainter& p)
{
    static QFont *value_font=0;
    static QFont *big_font=0;
    static QFont *small_font=0;
    if ( !value_font ) {
	value_font = new QFont("helvetica",8);
	if ( TileItem::bigWidth() < 20 ) {
	    big_font = new QFont("helvetica",12);
	    small_font = new QFont("helvetica",8);
	} else {
	    big_font = new QFont("smoothtimes",17);
	    small_font = new QFont("smoothtimes",10);
	}
    }

    QRect area(x(),y(),width(),height());
    p.setBrush(s == Floating ? yellow/*lightGray*/ : white);
    p.drawRect(area);
    if ( big ) {
	p.setFont(*value_font);
	QString n = QString::number(t.value());
	int w = p.fontMetrics().width('1');
	int h = p.fontMetrics().height();
	w *= n.length();
	QRect valuearea(x()+width()-w-1,y()+height()-h,w,h);
	p.drawText(valuearea,AlignCenter,n);
	p.setFont(*big_font);
	area = QRect(x(),y()+tile_btweak,width()-4,height()-1);
    } else {
	p.setFont(*small_font);
	area = QRect(x()+1+tile_stweak,y()+1,width(),height()-3);
    }
    if ( t.value() == 0 )
	p.setPen(darkGray);
    p.drawText(area,AlignCenter,t.text().upper());
}

Board::Board(QPixmap bgshapes, int w, int h, QWidget* parent) :
    QCanvasView(new QCanvas(bgshapes,w,h, TileItem::smallWidth(), TileItem::smallHeight()),
		parent)
{
    setFixedSize(w*TileItem::smallWidth(),h*TileItem::smallHeight());
    grid = new TileItem*[w*h];
    memset(grid,0,w*h*sizeof(TileItem*));
    setFrameStyle(0);
    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(AlwaysOff);
    current_rack = 0;
    shown_n = 0;
}

Board::~Board()
{
    delete canvas();
}

QSize Board::sizeHint() const
{
    return QSize(canvas()->width(),canvas()->height());
}

void Board::writeConfig(Config& cfg)
{
    QStringList t;
    int n=canvas()->tilesHorizontally()*canvas()->tilesVertically();
    for (int i=0; i<n; i++)
	t.append( grid[i] ? grid[i]->tile().key() : QString(".") );
    cfg.writeEntry("Board",t,';');
}

void Board::readConfig(Config& cfg)
{
    clear();
    QStringList t = cfg.readListEntry("Board",';');
    int i=0;
    int h=canvas()->tilesHorizontally();
    for (QStringList::ConstIterator it=t.begin(); it!=t.end(); ++it) {
	if ( *it != "." ) {
	    QPoint p(i%h,i/h);
	    setTile(p,Tile(*it));
	}
	i++;
    }
    canvas()->update();
}

void Board::clear()
{
    int n=canvas()->tilesHorizontally()*canvas()->tilesVertically();
    for (int i=0; i<n; i++) {
	delete grid[i];
	grid[i]=0;
    }
}


void Board::setCurrentRack(Rack* r)
{
    turn_score = -1;
    current_rack = r;
}

void Board::resetRack()
{
    unshowTurn();
    canvas()->update();
}

void Board::contentsMousePressEvent(QMouseEvent* e)
{
    dragstart = e->pos();
}

void Board::contentsMouseMoveEvent(QMouseEvent* e)
{
    if ( current_rack && !current_rack->computerized() ) {
	QPoint d = e->pos() - dragstart;
	if ( d.x() <= 0 && d.y() <= 0 ) {
	    // None
	    resetRack();
	} else {
	    int n;
	    QPoint start=boardPos(dragstart);
	    QPoint end=boardPos(e->pos());
	    QPoint diff=end-start;
	    QPoint dir;
	    if ( d.x() > d.y() ) {
		n = diff.x()+1;
		dir = QPoint(1,0);
	    } else {
		n = diff.y()+1;
		dir = QPoint(0,1);
	    }

	    unshowTurn();

	    // Subtract existing tiles from n
	    QPoint t = start;
	    for ( int i=n; i--; ) {
		if ( contains(t) && tile(t) )
		    n--;
		t += dir;
	    }

	    // Move start back to real start
	    while (contains(start-dir) && tile(start-dir))
		start -= dir;

	    scoreTurn(start, n, dir);
	    showTurn();
	}
    }
}

void Board::finalizeTurn()
{
    int i=0;
    QPoint at = shown_at;
    while ( i<shown_n && contains(at) ) {
	if ( item(at) && item(at)->state() == TileItem::Floating ) {
	    current_rack->remove(item(at)->tile());
	    setTileState(at,TileItem::Firm);
	    i++;
	}
	at += shown_step;
    }
    canvas()->update();
}

void Board::unshowTurn()
{
    int i=0;
    QPoint at = shown_at;
    while ( i<shown_n && i<current_rack->count() && contains(at) ) {
	if ( item(at) && item(at)->state() == TileItem::Floating ) {
	    unsetTile(at);
	    i++;
	}
	at += shown_step;
    }
}

void Board::showTurn()
{
    unshowTurn();
    QPoint at = shown_at;
    int i=0;
    while ( i<shown_n && i<current_rack->count() && contains(at) ) {
	if ( !tile(at) ) {
	    Tile t = current_rack->tile(i);
	    setTile(at,t);
	    setTileState(at,TileItem::Floating);
	    i++;
	}
	at += shown_step;
    }
    canvas()->update();
}

int Board::bonussedValue(const QPoint& at, int base, int& all_mult) const
{
    int rule = rule_shape[idx(at)]-'0';
    int effect = rule_effect[rule];
    int mult = effect&Multiplier;
    if ( effect & MultiplyAll ) {
	all_mult *= mult;
	return base;
    } else {
	return base * mult;
    }
}

bool Board::isStart(const QPoint& at) const
{
    int rule = rule_shape[idx(at)]-'0';
    int effect = rule_effect[rule];
    return effect&Start;
}

bool Board::checkTurn()
{
    if ( current_rack->computerized() )
	return TRUE; // computer doesn't cheat, and has already set blanks.

    QPoint at = shown_at;
    int n = shown_n;
    QPoint d = shown_step;
    const Tile* tiles[99];
    Tile blankvalues[99];
    if ( n > current_rack->count() )
	n = current_rack->count();

    QDialog check(this,0,TRUE);
    (new QVBoxLayout(&check))->setAutoAdd(TRUE);

    QHBox mw(&check);
    new QLabel(tr("Blanks: "),&mw);

    int bl=0;
    QLineEdit* le[99];
    for (int i=0; i<n; i++) {
	tiles[i] = current_rack->tileRef(i);
	if ( tiles[i]->isBlank() ) {
	    QLineEdit *l = new QLineEdit(&mw);
	    le[bl++] = l;
	    l->setMaxLength(1);
	    l->setFixedSize(l->minimumSizeHint());
	}
    }

    QHBox btns(&check);
    connect(new QPushButton(tr("OK"),&btns), SIGNAL(clicked()), &check, SLOT(accept()));
    connect(new QPushButton(tr("Cancel"),&btns), SIGNAL(clicked()), &check, SLOT(reject()));

    if ( bl ) {
retry:
	if ( !check.exec() ) {
	    unshowTurn();
	    canvas()->update();
	    return FALSE;
	}

	for (int b=0; b<bl; b++) {
	    QString v = le[b]->text();
	    blankvalues[b]=Tile(v,0);
	    if ( v.length() != 1 )
		goto retry;
	}
    }

    QStringList words;
    unshowTurn();
    turn_score = score(at,tiles,n,blankvalues,d,FALSE,&words);
    showTurn();
    QStringList to_add;
    for (QStringList::Iterator it=words.begin(); it!=words.end(); ++it) {
	if ( !Global::fixedDawg().contains(*it)
		&& !Global::dawg("WordGame").contains(*it) ) {
	    switch (QMessageBox::warning(this, tr("Unknown word"),
		tr("<p>The word \"%1\" is not in the dictionary.").arg(*it),
		tr("Add"), tr("Ignore"), tr("Cancel")))
	    {
	    case 0:
		// ####### add to wordgame dictionary
		to_add.append(*it);
		break;
	    case 1:
		break;
	    case 2:
		unshowTurn();
		canvas()->update();
		return FALSE;
	    }
	}
    }
    if ( to_add.count() )
	Global::addWords("WordGame",to_add);
    return TRUE;
}

void Board::scoreTurn(const QPoint& at, int n, const QPoint& d)
{
    unshowTurn();
    shown_at = at;
    shown_n = n;
    shown_step = d;
    const Tile* tiles[99];
    if ( n > current_rack->count() )
	n = current_rack->count();
    for (int i=0; i<n; i++)
	tiles[i] = current_rack->tileRef(i);
    turn_score = score(at,tiles,n,0,d,FALSE,0);
    emit temporaryScore(turn_score);
}

int Board::score(QPoint at, const Tile** tiles, int n, const Tile* blankvalue, const QPoint& d, bool checkdict, QStringList* words) const
{
    int total=0;
    int totalsidetotal=0;

    // words gets filled with words made

    // mainword==0 ->
    // Checks side words, but not main word

    // -1 means words not in dict, or illegally positioned (eg. not connected)

    // text is assumed to fit on board.

    if ( words ) *words=QStringList();

    QPoint otherd(d.y(), d.x());

    int all_mult = 1;
    int bl=0;

    bool connected = FALSE;

    QString mainword="";

    if ( contains(at-d) && tile(at-d) ) {
	return -1; // preceeding tiles
    }

    const Tile* t;
    for (int i=0; contains(at) && ((t=tile(at)) || i<n); ) {
	if ( t ) {
	    if ( checkdict || words ) mainword += t->text();
	    total += t->value();
	    connected = TRUE;
	} else {
	    QString sideword;
	    QString tt;
	    if ( tiles[i]->isBlank() ) {
		if ( blankvalue )
		    tt = blankvalue[bl++].text();
	    } else {
		tt = tiles[i]->text();
	    }
	    sideword=tt;
	    if ( checkdict || words ) mainword += tt;
	    int side_mult = 1;
	    int tilevalue = bonussedValue(at,tiles[i]->value(),side_mult);
	    all_mult *= side_mult;
	    if ( !connected && isStart(at) )
		connected = TRUE;
	    total += tilevalue;
	    int sidetotal = tilevalue;
	    {
		QPoint side = at-otherd;
		
		while ( contains(side) && (t=tile(side)) ) {
		    sidetotal += t->value();
		    sideword.prepend(t->text());
		    side -= otherd;
		}
	    }
	    {
		QPoint side = at+otherd;
		while ( contains(side) && (t=tile(side)) ) {
		    sidetotal += t->value();
		    sideword.append(t->text());
		    side += otherd;
		}
	    }
	    if ( sideword.length() > 1 ) {
		if ( words )
		    words->append(sideword);
		if ( checkdict && !Global::fixedDawg().contains(sideword)
		               && !Global::dawg("WordGame").contains(sideword) )
		    return -1;
		totalsidetotal += sidetotal * side_mult;
		connected = TRUE;
	    }
	    i++;
	}
	at += d;
    }

    if ( words )
	words->append(mainword);
    if ( checkdict && !Global::fixedDawg().contains(mainword)
		   && !Global::dawg("WordGame").contains(mainword) )
	return -1;

    if ( n == rack_tiles )
	totalsidetotal += rack_tiles_bonus;

    return connected ? totalsidetotal + total * all_mult : -1;
}

QPoint Board::boardPos(const QPoint& p) const
{
    return QPoint(p.x()/canvas()->tileWidth(), p.y()/canvas()->tileHeight());
}

void Board::contentsMouseReleaseEvent(QMouseEvent*)
{
    if ( current_rack ) {
    }
}


void Board::setRules(const QString& shapes, const int* effects)
{
    rule_shape=shapes; rule_effect=effects;
    int i=0;
    int maxre=0;
    for (int y=0; y<yTiles(); y++) {
	for (int x=0; x<xTiles(); x++) {
	    int re = shapes[i++]-'0';
	    if ( re > maxre ) maxre = re;
	    canvas()->setTile(x,y,re);
	}
    }
    rack_tiles_bonus=effects[maxre+1];
}

void Board::unsetTile(const QPoint& p)
{
    delete item(p);
    grid[idx(p)] = 0;
}

void Board::setTile(const QPoint& p, const Tile& t)
{
    TileItem* it=item(p);
    if ( !it ) {
	it = grid[idx(p)] = new TileItem(t,FALSE,canvas());
	it->move(p.x()*canvas()->tileWidth(), p.y()*canvas()->tileHeight());
	it->show();
    } else {
	it->setTile(t);
    }
}

Rack::Rack(int ntiles, QWidget* parent) : QCanvasView(
	new QCanvas(ntiles*TileItem::bigWidth(),TileItem::bigHeight()),
	    parent),
	item(ntiles)
{
    setLineWidth(1);
    setFixedHeight(sizeHint().height());
    n = 0;
    for (int i=0; i<ntiles; i++)
	item[i]=0;
    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(AlwaysOff);
    canvas()->setBackgroundColor(gray);
    dragging = 0;
}

Rack::~Rack()
{
    clear();
    delete canvas();
}

QSize Rack::sizeHint() const
{
    return QSize(-1,TileItem::bigHeight()+2);
}

void Rack::clear()
{
    for (int i=0; i<n; i++)
	delete item[i];
    n=0;
}

void Rack::writeConfig(Config& cfg)
{
    QStringList l;
    for (int i=0; i<n; i++)
	l.append(tile(i).key());
    cfg.writeEntry("Tiles",l,';');
}

void Rack::readConfig(Config& cfg)
{
    clear();
    int x=0;
    QStringList l = cfg.readListEntry("Tiles",';');
    for (QStringList::ConstIterator it=l.begin(); it!=l.end(); ++it) {
	TileItem *i = new TileItem(Tile(*it),TRUE,canvas());
	i->move(x++,0);
	i->show();
	item[n++] = i;
    }
    layoutTiles();
}

static int cmp_tileitem(const void *a, const void *b)
{
    const TileItem* ia = *(TileItem**)a;
    const TileItem* ib = *(TileItem**)b;
    return int(ia->x() - ib->x());
}

void Rack::layoutTiles()
{
    int w = TileItem::bigWidth()+2;

    if ( dragging ) dragging->moveBy(dragging_adj,0);
    qsort(item.data(), n, sizeof(TileItem*), cmp_tileitem);
    if ( dragging ) dragging->moveBy(-dragging_adj,0);

    for (int i=0; i<n ;i++)
	if ( item[i] == dragging ) {
	    item[i]->setZ(1);
	} else {
	    item[i]->move(i*w, 0);
	    item[i]->setZ(0);
	}
    canvas()->update();
}

void Rack::setBlanks(const Tile* bv)
{
    for (int j=0; j<n; j++) {
	Tile tt = item[j]->tile();
	if ( tt.isBlank() ) {
	    tt.setText(bv->text());
	    item[j]->setTile(tt);
	    bv++;
	}
    }
}

bool Rack::arrangeTiles(const Tile** s, int sn)
{
    bool could = TRUE;
    for (int j=0; j<n; j++) {
	Tile tt = item[j]->tile();
	int f=-1;
	for (int i=0; i<sn && f<0; i++) {
	    if (s[i] && *s[i] == tt ) {
		s[i]=0;
		f=i;
	    }
	}
	if ( f >= 0 ) {
	    item[j]->move(f-999,0);
	} else {
	    could = FALSE;
	}
    }
    layoutTiles();
    return could;
}

void Rack::addTile(const Tile& t)
{
    TileItem *i = new TileItem(t,TRUE,canvas());
    i->show();
    item[n++] = i;
    layoutTiles();
}

void Rack::remove(Tile t)
{
    for (int i=0; i<n ;i++)
	if ( item[i]->tile() == t ) {
	    remove(i);
	    return;
	}
}

void Rack::remove(int i)
{
    delete item[i];
    n--;
    for (;i<n;i++)
	item[i]=item[i+1];
    layoutTiles();
}

void Rack::resizeEvent(QResizeEvent* e)
{
    canvas()->resize(width()-frameWidth()*2,height()-frameWidth()*2);
    QCanvasView::resizeEvent(e);
}

void Rack::contentsMousePressEvent(QMouseEvent* e)
{
    if ( computerized() )
	return;
    QCanvasItemList list = canvas()->collisions(e->pos());
    if (list.count()) {
	dragging = list.first();
	dragstart = e->pos()-QPoint(int(dragging->x()),int(dragging->y()));
    } else {
	dragging = 0;
    }
}

void Rack::contentsMouseMoveEvent(QMouseEvent* e)
{
    if ( computerized() )
	return;
    //int w = TileItem::bigWidth()+2;
    if ( dragging ) {
	dragging_adj = TileItem::bigWidth()/2;
	if ( dragging->x() > e->x()-dragstart.x() )
	    dragging_adj = -dragging_adj;
	dragging->move(e->x()-dragstart.x(),0);
	layoutTiles();
    }
}

void Rack::contentsMouseReleaseEvent(QMouseEvent* e)
{
    if ( computerized() )
	return;
    if ( dragging ) {
	dragging=0;
	layoutTiles();
    }
}

Tile::Tile(const QString& key)
{
    int a=key.find('@');
    txt = key.left(a);
    val = key.mid(a+1).toInt();
    blank = txt.isEmpty();
}

QString Tile::key() const
{
    return txt+"@"+QString::number(val);
}

Bag::Bag()
{
    tiles.setAutoDelete(TRUE);
}

void Bag::writeConfig(Config& cfg)
{
    QStringList t;
    for (QListIterator<Tile> it(tiles); it; ++it)
	t.append((*it)->key());
    cfg.writeEntry("Tiles",t,';');
}

void Bag::readConfig(Config& cfg)
{
    tiles.clear();
    QStringList t = cfg.readListEntry("Tiles",';');
    for (QStringList::ConstIterator it=t.begin(); it!=t.end(); ++it )
	add(Tile(*it));
}

void Bag::add(const Tile& t)
{
    tiles.append(new Tile(t));
}

Tile Bag::takeRandom()
{
    Tile* rp = tiles.take(random()%tiles.count());
    Tile r=*rp;
    return r;
}

ScoreInfo::ScoreInfo( QWidget* parent, const char* name, WFlags fl ) :
    QLabel("<P>",parent,name,fl)
{
    score=0;
    msgtimer = new QTimer(this);
    connect(msgtimer, SIGNAL(timeout()), this, SLOT(showScores()));
    setBackgroundMode( PaletteButton );
}

ScoreInfo::~ScoreInfo()
{
    if ( score ) delete [] score;
}

void ScoreInfo::writeConfig(Config& cfg)
{
    QStringList l;
    for (int i=0; i<(int)names.count(); i++)
	l.append(QString::number(score[i]));
    cfg.writeEntry("Scores",l,';');
}

void ScoreInfo::readConfig(Config& cfg)
{
    QStringList l = cfg.readListEntry("Scores",';');
    int i=0;
    for (QStringList::ConstIterator it=l.begin(); it!=l.end(); ++it )
	score[i++]=(*it).toInt();
    showScores();
}


QSize ScoreInfo::sizeHint() const
{
    return QSize(QLabel::sizeHint().width(),fontMetrics().height());
}

void ScoreInfo::init(const QStringList& namelist)
{
    names = namelist;
    if ( score ) delete [] score;
    score = new int[names.count()];
    memset(score,0,sizeof(int)*names.count());
    boldone = -1;
    showScores();
}

void ScoreInfo::addScore(int player, int change)
{
    score[player] += change;
    showScores();
}

void ScoreInfo::setBoldOne(int b)
{
    boldone=b;
    showScores();
}

void ScoreInfo::showScores()
{
    QString r="<p>";
    int i=0;
    //int spl=(names.count()+1)/2; // 2 lines
    for (QStringList::ConstIterator it=names.begin(); it!=names.end(); ) {
	if ( i==boldone ) r += "<b>";
	QString n = *it;
	n.replace(QRegExp(":.*"),"");
	r += n;
	r += ":";
	r += QString::number(score[i]);
	if ( i==boldone ) r += "</b>";

	++i;
	++it;
	if ( it != names.end() )
	    r += " ";
    }
    setText(r);
}

void ScoreInfo::showTemporaryScore(int amount)
{
    if ( amount < 0 )
	setText(tr("<P>Invalid move"));
    else
	setText(tr("<P>Score: ")+QString::number(amount));
    msgtimer->start(3000,TRUE);
}

