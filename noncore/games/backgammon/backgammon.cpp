#include "backgammon.h"

#include "aidialog.h"
#include "filedialog.h"
#include "playerdialog.h"
#include "rulesdialog.h"
#include "themedialog.h"

#include <qdatetime.h>
#include <qfile.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qtimer.h>
#include <qmainwindow.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/qpemenubar.h>
#include <qpe/resource.h>

#include <stdlib.h>


BackGammon::BackGammon(QWidget* parent, const char* name, WFlags fl)
        : QMainWindow(parent, name, fl)
{
    if (!name) setName("BackGammon");
    setCaption(tr( "Backgammon") );
    setIcon( Resource::loadPixmap( "backgammon" ) );
    //general counter varaible
    int a=0;
    //the game engine
    move=new MoveEngine();

    //load the default theme
    Config conf("backgammon");
    if(!conf.isValid())
    {
        qDebug("config file does not exist");
        conf.setGroup("general");
        conf.writeEntry("theme","default");
        conf.setGroup("rules");
        conf.writeEntry("move_with_pieces_out",false);
        conf.writeEntry("nice_dice",false);
        conf.setGroup("display");
        conf.writeEntry("small",false);
        conf.writeEntry("warning",true);
        conf.setGroup("ai");
        conf.writeEntry("rescue",6);
        conf.writeEntry("eliminate",4);
        conf.writeEntry("expose",1);
        conf.writeEntry("protect",5);
        conf.writeEntry("safe",3);
        conf.writeEntry("empty",2);

    }
    conf.setGroup("general");
    theme_name=conf.readEntry("theme","default");
    QString theme_file=QPEApplication::qpeDir()+"/backgammon/"+theme_name+".theme";

    //the rules
    conf.setGroup("rules");
    rules.move_with_pieces_out=conf.readBoolEntry("move_with_pieces_out",false);
    rules.generous_dice=conf.readBoolEntry("nice_dice",false);
       
    move->setRules(rules);
    
    //get the AI settings
    AISettings ai;
    conf.setGroup("ai");
    ai.rescue=conf.readNumEntry("rescue",6);
    ai.eliminate=conf.readNumEntry("eliminate",4);
    ai.expose=conf.readNumEntry("expose",1);
    ai.protect=conf.readNumEntry("protect",5);
    ai.safe=conf.readNumEntry("safe",3);
    ai.empty=conf.readNumEntry("empty",2);
    move->setAISettings(ai);


    //get the theme component names
    Config theme(theme_file,Config::File);   
    if(!theme.isValid())
    {
      qDebug("theme file does not exist");
      theme.setGroup("theme");
      theme.writeEntry("board","casino_board_1");
      theme.writeEntry("pieces1","casino_pieces_blue");
      theme.writeEntry("pieces2","casino_pieces_white");
      theme.writeEntry("dice1","casino_dice");
      theme.writeEntry("dice2","casino_dice");
      theme.writeEntry("table","casino_table_green");
      theme.writeEntry("odds","casino_odds");
    }
    theme.setGroup("theme");
    board_name=theme.readEntry("board","casino_board_1");
    piecesA_name=theme.readEntry("pieces1","casino_pieces_blue");
    piecesB_name=theme.readEntry("pieces2","casino_pieces_white");
    diceA_name=theme.readEntry("dice1","casino_dice");
    diceB_name=theme.readEntry("dice2","casino_dice");
    table_name=theme.readEntry("table","casino_table_green");
    odds_name=theme.readEntry("odds","casino_odds");
    

    //the menu
    QPEMenuBar* menuBar = new QPEMenuBar(this);

    QPopupMenu* gamemenu= new QPopupMenu(this);
    gamemenu->insertItem(tr( "New" ),this,SLOT(newgame()));
    gamemenu->insertSeparator();
    gamemenu->insertItem(tr( "Load" ),this,SLOT(loadgame()));
    gamemenu->insertItem(tr( "Save" ),this,SLOT(savegame()));
    gamemenu->insertSeparator();
    gamemenu->insertItem(tr( "Delete" ),this,SLOT(deletegame()));
    menuBar->insertItem(tr( "Game" ),gamemenu);

    QPopupMenu* thememenu= new QPopupMenu(this);
    thememenu->insertItem(tr( "New" ),this,SLOT(newtheme()));
    thememenu->insertSeparator();
    thememenu->insertItem(tr( "Load"),this,SLOT(loadtheme()));
    thememenu->insertItem(tr( "Save" ),this,SLOT(savetheme()));
    thememenu->insertSeparator();
    thememenu->insertItem(tr( "Default"),this,SLOT(themedefault()));
    thememenu->insertItem(tr( "Delete" ),this,SLOT(deletetheme()));
    menuBar->insertItem(tr( "Theme" ),thememenu);

    QPopupMenu* optionmenu=new QPopupMenu(this);
    optionmenu->insertItem(tr( "Player" ),this,SLOT(playerselect()));
    optionmenu->insertSeparator();
    optionmenu->insertItem(tr( "AI" ),this,SLOT(modify_AI()));
    optionmenu->insertItem(tr( "Rules" ),this,SLOT(setrules()));
    menuBar->insertItem(tr( "Options"),optionmenu);

	QWidget* mainarea=new QWidget(this);
	setCentralWidget(mainarea);
    //the main area
	QBoxLayout* layout=new QBoxLayout(mainarea,QBoxLayout::TopToBottom);
    area=new QCanvas(235,235);
    boardview=new BackGammonView(area,mainarea);
    boardview->setMaximumHeight(240);
	layout->addWidget(boardview);
    connect(boardview,SIGNAL(mouse(int,int)),this,SLOT(mouse(int,int)));
    //status bar
    message=new QLabel("<b>Backgammon</b>",mainarea);
    message->setAlignment(AlignHCenter);
    layout->addWidget(message);

    //the marker
    marker_current=new QCanvasRectangle(area);
    marker_current->setBrush(QColor(0,0,255));
    marker_current->setSize(15,5);
    marker_current->setZ(1);

    for(a=0;a<4;a++)
    {
        marker_next[a]=new QCanvasRectangle(area);
        marker_next[a]->setBrush(QColor(0,255,0));
        marker_next[a]->setSize(15,5);
        marker_next[a]->setZ(1);
    }

    connect(move,SIGNAL(done_dice1()),this,SLOT(done_dice1()));
    connect(move,SIGNAL(done_dice2()),this,SLOT(done_dice2()));
    connect(move,SIGNAL(done_dice3()),this,SLOT(done_dice3()));
    connect(move,SIGNAL(done_dice4()),this,SLOT(done_dice4()));
    connect(move,SIGNAL(nomove()),this,SLOT(nomove()));
    connect(move,SIGNAL(player_finished(int)),this,SLOT(finished(int)));

    //the pieces
    p1=new CanvasImageItem*[15];
    p1_side=new CanvasImageItem*[15];
    QImage piece_1_all(Resource::loadImage("backgammon/pieces/"+piecesA_name));
    QImage piece_1_front=piece_1_all.copy(0,0,15,15);
    QImage piece_1_side=piece_1_all.copy(0,15,15,5);

    p2=new CanvasImageItem*[15];
    p2_side=new CanvasImageItem*[15];
    QImage piece_2_all(Resource::loadImage("backgammon/pieces/"+piecesB_name));
    QImage piece_2_front=piece_2_all.copy(0,0,15,15);
    QImage piece_2_side=piece_2_all.copy(0,15,15,5);


    for(a=0;a<15;a++)
    {
        p1[a]=new CanvasImageItem(piece_1_front,area);
        p1[a]->setSize(15,15);
        p1_side[a]=new CanvasImageItem(piece_1_side,area);
        p1_side[a]->setSize(15,5);

        p2[a]=new CanvasImageItem(piece_2_front,area);
        p2[a]->setSize(15,15);
        p2_side[a]=new CanvasImageItem(piece_2_side,area);
        p2_side[a]->setSize(15,5);
    }
    draw();

    //the dice
    QImage dicebgA_all(Resource::loadImage("backgammon/dice/"+diceA_name));
    diceA1=new CanvasImageItem*[7];
    diceA2=new CanvasImageItem*[7];
    QImage dicebgB_all(Resource::loadImage("backgammon/dice/"+diceB_name));
    diceB1=new CanvasImageItem*[7];
    diceB2=new CanvasImageItem*[7];
    QImage oddsbg_all=(Resource::loadImage("backgammon/odds/"+odds_name));
    //oddsDice=new CanvasImageItem*[6];


    for(a=0;a<7;a++)
    {
        QImage dicebgA=dicebgA_all.copy(a*25,0,25,25);
        diceA1[a]=new CanvasImageItem(dicebgA,area);
        diceA1[a]->setX(5);
        diceA1[a]->setY(205-2);
        diceA1[a]->setZ(1);
        diceA1[a]->setSize(25,25);
        diceA2[a]=new CanvasImageItem(dicebgA,area);
        diceA2[a]->setX(35);
        diceA2[a]->setY(205-2);
        diceA2[a]->setZ(1);
        diceA2[a]->setSize(25,25);

        QImage dicebgB=dicebgB_all.copy(a*25,0,25,25);
        diceB1[a]=new CanvasImageItem(dicebgB,area);
        diceB1[a]->setX(175);
        diceB1[a]->setY(205-2);
        diceB1[a]->setZ(1);
        diceB1[a]->setSize(25,25);
        diceB2[a]=new CanvasImageItem(dicebgB,area);
        diceB2[a]->setX(205);
        diceB2[a]->setY(205-2);
        diceB2[a]->setZ(1);
        diceB2[a]->setSize(25,25);

        /*
        if(a<6)
        {
            QImage oddsbg=oddsbg_all.copy(a*15,0,15,15);
            oddsDice[a]=new CanvasImageItem(oddsbg,area);
            oddsDice[a]->setX(110);
            oddsDice[a]->setY(210-2);
            oddsDice[a]->setZ(1);
            oddsDice[a]->setSize(15,15);
            oddsDice[a]->hide();
        }
        */
    }
    //oddsDice[0]->show();

    //set the board
    QImage boardbg(Resource::loadImage("backgammon/boards/"+board_name));
    board=new CanvasImageItem(boardbg,area);
    board->setX(0);
    board->setY(0);
    board->setZ(0);
    board->setSize(235-2,200-2);
    board->show();

    //the table
    QImage tablebg(Resource::loadImage("backgammon/table/"+table_name));
    table=new CanvasImageItem(tablebg,area);
    table->setX(0);
    table->setY(200-2);
    table->setZ(0);
    table->setSize(235-2,20);
    table->show();

    //the no move marker
    QImage nomovebg(Resource::loadImage("backgammon/no_move"));
    nomove_marker=new CanvasImageItem(nomovebg,area);
    nomove_marker->setX(0);
    nomove_marker->setY(200);
    nomove_marker->setZ(2);
    nomove_marker->hide();

    //default human against computer
    player1_auto=false;
    player2_auto=true;
    //start new game
    newgame();
}

BackGammon::~BackGammon()
{
  //DESTRUCTOR
}

void BackGammon::newgame()
{
    gameFinished=false;
    QDateTime now=QDateTime::currentDateTime();
    game_name=now.date().toString()+"_"+now.time().toString();
    move->reset();
    draw();
    diceA1_value=7;
    diceA2_value=7;
    diceA3_value=7;
    diceA4_value=7;
    diceB1_value=7;
    diceB2_value=7;
    diceB3_value=7;
    diceB4_value=7;
    showdice();
    player=2;
    dice1_played=true;
    dice2_played=true;
    dice3_played=true;
    dice4_played=true;
    dice_rolled=false;
    setplayer();
    area->update();
}

void BackGammon::playerselect()
{
    PlayerDialog* playerdialog=new PlayerDialog(this);
    playerdialog->setAuto1(player1_auto);
    playerdialog->setAuto2(player2_auto);
    if(!playerdialog->exec())
        return;
    player1_auto=playerdialog->getAuto1();
    player2_auto=playerdialog->getAuto2();
}

void BackGammon::loadgame()
{
    FileDialog* file=new FileDialog(this,"Load Game",".game");
    if(!file->exec())
        return;

    game_name=file->filename();
    QString game_file=QPEApplication::qpeDir()+"/backgammon/"+game_name+".game";

    Config game(game_file,Config::File);
    game.setGroup("dice");
    diceA1_value=game.readNumEntry("diceA1_value");
    diceA2_value=game.readNumEntry("diceA2_value");
    diceA3_value=game.readNumEntry("diceA3_value");
    diceA4_value=game.readNumEntry("diceA4_value");
    diceB1_value=game.readNumEntry("diceB1_value");
    diceB2_value=game.readNumEntry("diceB2_value");
    diceB3_value=game.readNumEntry("diceB3_value");
    diceB4_value=game.readNumEntry("diceB4_value");
    player=game.readNumEntry("player");
    dice1_played=game.readBoolEntry("dice1_played");
    dice2_played=game.readBoolEntry("dice2_played");
    dice3_played=game.readBoolEntry("dice3_played");
    dice4_played=game.readBoolEntry("dice4_played");
    dice_rolled=game.readBoolEntry("dice_rolled");
    player1_auto=game.readBoolEntry("player1_auto");
    player2_auto=game.readBoolEntry("player2_auto");

    game.setGroup("pieces");
    QString label;
    LoadSave load;
    for(int a=0;a<28;a++)
    {
        label.setNum(a);
        load.pop[a].total = game.readNumEntry(label,0);
    }

    move->loadGame(load);
    setplayer();
    showdice();
    draw();
    area->update();
}

void BackGammon::savegame()
{
    QString game_file=QPEApplication::qpeDir()+"/backgammon/"+game_name+".game";

    Config game(game_file,Config::File);
    game.setGroup("dice");
    game.writeEntry("diceA1_value",diceA1_value);
    game.writeEntry("diceA2_value",diceA2_value);
    game.writeEntry("diceA3_value",diceA3_value);
    game.writeEntry("diceA4_value",diceA4_value);
    game.writeEntry("diceB1_value",diceB1_value);
    game.writeEntry("diceB2_value",diceB3_value);
    game.writeEntry("diceB3_value",diceB4_value);
    game.writeEntry("diceB4_value",diceB4_value);
    game.writeEntry("player",player);
    game.writeEntry("dice1_played",dice1_played);
    game.writeEntry("dice2_played",dice2_played);
    game.writeEntry("dice3_played",dice3_played);
    game.writeEntry("dice4_played",dice4_played);
    game.writeEntry("dice_rolled",dice_rolled);
    game.writeEntry("player1_auto",player1_auto);
    game.writeEntry("player2_auto",player2_auto);

    game.setGroup("pieces");
    QString label;
    LoadSave save=move->saveGame();
    for(int a=0;a<28;a++)
    {
        label.setNum(a);
        game.writeEntry(label,save.pop[a].total);
    }
    QMessageBox::information(this,"Backgammon","Game saved","OK");
}

void BackGammon::deletegame()
{
    FileDialog* file=new FileDialog(this,"Delete Game",".game");
    if(!file->exec())
        return;

    game_name=file->filename();
    QString game_file=QPEApplication::qpeDir()+"/backgammon/"+game_name+".game";

    if(!QMessageBox::warning(this,"Backgammon","deleted game\n"+game_name+" ?","OK","Cancel"))
    {
      QFile(game_file).remove();
    }
}


void BackGammon::newtheme()
{
    ThemeDialog* theme=new ThemeDialog(this);

    ImageNames names;
    names.theme=theme_name;
    names.board=board_name;
    names.pieces1=piecesA_name;
    names.pieces2=piecesB_name;
    names.dice1=diceA_name;
    names.dice2=diceB_name;
    names.odds=odds_name;
    names.table=table_name;

    theme->setCurrent(names);
    if(!theme->exec())
        return;

    names=theme->getNames();
    theme_name=names.theme;
    board_name=names.board;
    piecesA_name=names.pieces1;
    piecesB_name=names.pieces2;
    diceA_name=names.dice1;
    diceB_name=names.dice2;
    odds_name=names.odds;
    table_name=names.table;

    applytheme();
}

void BackGammon::loadtheme()
{
    FileDialog* file=new FileDialog(this,"Load Theme",".theme");
    if(!file->exec())
        return;

    theme_name=file->filename();
    QString theme_file=QPEApplication::qpeDir()+"/backgammon/"+theme_name+".theme";

    Config theme(theme_file,Config::File);
    theme.setGroup("theme");
    board_name=theme.readEntry("board","board_1");
    piecesA_name=theme.readEntry("pieces1","pieces_1");
    piecesB_name=theme.readEntry("pieces2","pieces_2");
    diceA_name=theme.readEntry("dice1","dice_1");
    diceB_name=theme.readEntry("dice2","dice_2");
    table_name=theme.readEntry("table","table_1");
    odds_name=theme.readEntry("odds","odds_1");

    applytheme();

}

void BackGammon::savetheme()
{
    if(theme_name=="default")
    {
        QMessageBox::information(this,"Backgammon","Sorry\nCannot overwrite default.theme","OK");
        return;
    }
    QString theme_file=QPEApplication::qpeDir()+"/backgammon/"+theme_name+".theme";
    if(QMessageBox::information(this,"Backgammon","Save Theme\n"+theme_name,"Yes","No"))
      return;
      
    Config theme(theme_file,Config::File);
    theme.setGroup("theme");
    theme.writeEntry("board",board_name);
    theme.writeEntry("pieces1",piecesA_name);
    theme.writeEntry("pieces2",piecesB_name);
    theme.writeEntry("dice1",diceA_name);
    theme.writeEntry("dice2",diceB_name);
    theme.writeEntry("table",table_name);
    theme.writeEntry("odds",odds_name);
    
}

void BackGammon::themedefault()
{
    if(QMessageBox::information(this,"Backgammon","Make Theme\n"+theme_name+"\nthe default theme","Yes","No"))
      return;

    Config conf("backgammon");
    conf.setGroup("general");
    conf.writeEntry("theme",theme_name);    
}

void BackGammon::deletetheme()
{
    FileDialog* file=new FileDialog(this,"Delete Theme",".theme");
    if(!file->exec())
        return;

    theme_name=file->filename();
    QString theme_file=QPEApplication::qpeDir()+"/backgammon/"+theme_name+".theme";

    if(!QMessageBox::warning(this,tr( "Backgammon" ),tr( "deleted theme %1?").arg(theme_name),tr( "OK" ),tr( "Cancel" )))
    {
      QFile(theme_file).remove();
    }
}

void BackGammon::modify_AI()
{
    AI_Dialog* ai_mod=new AI_Dialog(this,tr( "Load Theme" ),".theme");
    ai_mod->setAISettings(move->getAISettings());
    if(!ai_mod->exec())
        return;
    
    //get the AI settings
    AISettings ai=ai_mod->getAISettings();
    move->setAISettings(ai);
    //write new settings to conf file
    Config conf("backgammon");
    conf.setGroup("ai");
    conf.writeEntry("rescue",ai.rescue);
    conf.writeEntry("eliminate",ai.eliminate);
    conf.writeEntry("expose",ai.expose);
    conf.writeEntry("protect",ai.protect);
    conf.writeEntry("safe",ai.safe);
    conf.writeEntry("empty",ai.empty);
}

void BackGammon::setrules()
{
    RulesDialog* rulesdialog=new RulesDialog(this,tr( "Load Theme" ),".theme");
    rulesdialog->setRules(rules);
    if(!rulesdialog->exec())
        return;
    rules=rulesdialog->getRules();
    Config conf("backgammon");
    conf.setGroup("rules");
    conf.writeEntry("move_with_pieces_out",rules.move_with_pieces_out);
    conf.writeEntry("nice_dice",rules.generous_dice);
    move->setRules(rules);
}


void BackGammon::draw()
{
    Pieces pieces;
    move->position(pieces);
    for(int a=0;a<15;a++)
    {
        if(!pieces.player1[a].side)
        {
            p1[a]->setX(pieces.player1[a].x);
            p1[a]->setY(pieces.player1[a].y);
            p1[a]->setZ(pieces.player1[a].z);
            p1[a]->show();
            p1_side[a]->hide();
        }
        else
        {
            p1_side[a]->setX(pieces.player1[a].x);
            p1_side[a]->setY(pieces.player1[a].y);
            p1_side[a]->setZ(pieces.player1[a].z);
            p1_side[a]->show();
            p1[a]->hide();
        }

        if(!pieces.player2[a].side)
        {
            p2[a]->setX(pieces.player2[a].x);
            p2[a]->setY(pieces.player2[a].y);
            p2[a]->setZ(pieces.player2[a].z);
            p2[a]->show();
            p2_side[a]->hide();
        }
        else
        {
            p2_side[a]->setX(pieces.player2[a].x);
            p2_side[a]->setY(pieces.player2[a].y);
            p2_side[a]->setZ(pieces.player2[a].z);
            p2_side[a]->show();
            p2[a]->hide();
        }
    }
}

void BackGammon::mouse(int x,int y)
{
    if(gameFinished)
    {
      newgame();
      return;
    }
    if(y<=200) //move pieces
    {
        if((player==1 && player1_auto) || (player==2 && player2_auto))
            return;

        Marker marker;

        move->boardpressed(x,y,marker);
        if(marker.visible_current)
        {
            marker_current->setX(marker.x_current);
            marker_current->setY(marker.y_current);
            marker_current->show();
        }
        else
        {
            marker_current->hide();
        }

        for(int a=0;a<4;a++)
        {
            if(marker.visible_next[a])
            {
                marker_next[a]->setX(marker.x_next[a]);
                marker_next[a]->setY(marker.y_next[a]);
                marker_next[a]->show();
            }
            else
            {
                marker_next[a]->hide();
            }
        }
        area->update();
    }
    else //roll dice
    {
        if(x>=10 && x<=65 && player==1 && !dice_rolled)
        {
            dice1_played=false;
            dice2_played=false;
            dice3_played=false;
            dice4_played=false;
            dice_rolled=true;
            srand(QTime::currentTime().msec());
            diceA1_value=1+(int) (6.0*rand()/(RAND_MAX+1.0));
            diceA2_value=1+(int) (6.0*rand()/(RAND_MAX+1.0));
            if(diceA1_value==diceA2_value)
            {
                diceA3_value=diceA1_value;
                diceA4_value=diceA1_value;
            }
            else
            {
                diceA3_value=7;
                dice3_played=true;
                diceA4_value=7;
                dice4_played=true;
            }
            showdice();
            area->update();
            move->diceroll(1,diceA1_value,diceA2_value,diceA3_value,diceA4_value,player1_auto);

        }
        else if(x>=160 && x<=225 && player==2 && !dice_rolled)
        {
            dice1_played=false;
            dice2_played=false;
            dice3_played=false;
            dice4_played=false;
            dice_rolled=true;
            srand(QTime::currentTime().msec());
            diceB1_value=1+(int) (6.0*rand()/(RAND_MAX+1.0));
            diceB2_value=1+(int) (6.0*rand()/(RAND_MAX+1.0));
            if(diceB1_value==diceB2_value)
            {
                diceB3_value=diceB1_value;
                diceB4_value=diceB1_value;
            }
            else
            {
                diceB3_value=7;
                dice3_played=true;
                diceB4_value=7;
                dice4_played=true;
            }
            showdice();
            area->update();
            move->diceroll(2,diceB1_value,diceB2_value,diceB3_value,diceB4_value,player2_auto);
        }
    }
}

void BackGammon::done_dice1()
{
    dice1_played=true;
    if(player==1)
        diceA1_value=7;
    else
        diceB1_value=7;
    setplayer();
    showdice();
    draw();
    area->update();
    if(!dice2_played || !dice3_played || !dice4_played)
    {
        if(player==1)
        {
            move->diceroll(1,diceA1_value,diceA2_value,diceA3_value,diceA4_value,player1_auto);
        }
        else
        {
            move->diceroll(2,diceB1_value,diceB2_value,diceB3_value,diceB4_value,player2_auto);
        }
    }
}

void BackGammon::done_dice2()
{
    dice2_played=true;
    if(player==1)
        diceA2_value=7;
    else
        diceB2_value=7;
    setplayer();
    showdice();
    draw();
    area->update();
    if(!dice1_played || !dice3_played || !dice4_played)
    {
        if(player==1)
        {
            move->diceroll(1,diceA1_value,diceA2_value,diceA3_value,diceA4_value,player1_auto);
        }
        else
        {
            move->diceroll(2,diceB1_value,diceB2_value,diceB3_value,diceB4_value,player2_auto);
        }
    }
}


void BackGammon::done_dice3()
{
    dice3_played=true;
    if(player==1)
        diceA3_value=7;
    else
        diceB3_value=7;
    setplayer();
    showdice();
    draw();
    area->update();
    if(!dice1_played || !dice2_played || !dice4_played)
    {
        if(player==1)
        {
            move->diceroll(1,diceA1_value,diceA2_value,diceA3_value,diceA4_value,player1_auto);
        }
        else
        {
            move->diceroll(2,diceB1_value,diceB2_value,diceB3_value,diceB4_value,player2_auto);
        }
    }
}


void BackGammon::done_dice4()
{
    dice4_played=true;
    if(player==1)
        diceA4_value=7;
    else
        diceB4_value=7;
    setplayer();
    showdice();
    draw();
    area->update();
    if(!dice1_played || !dice2_played || !dice3_played)
    {
        if(player==1)
        {
            move->diceroll(1,diceA1_value,diceA2_value,diceA3_value,diceA4_value,player1_auto);
        }
        else
        {
            move->diceroll(2,diceB1_value,diceB2_value,diceB3_value,diceB4_value,player2_auto);
        }
    }
}


void BackGammon::nomove()
{
    if(player==1)
        nomove_marker->setX(0);
    else
        nomove_marker->setX(170);
    nomove_marker->show();
    message->setText(tr( "<b>no move</b>" ));
    dice1_played=true;
    dice2_played=true;
    dice3_played=true;
    dice4_played=true;
    if(player==1)
    {
        diceA1_value=7;
        diceA2_value=7;
        diceA3_value=7;
        diceA4_value=7;
    }
    else
    {
        diceB1_value=7;
        diceB2_value=7;
        diceB3_value=7;
        diceB4_value=7;
    }
    area->update();
    QTimer::singleShot(2000,this,SLOT(nomove2()));
}

void BackGammon::nomove2()
{
    nomove_marker->hide();
    setplayer();
    showdice();
    draw();
    area->update();
}

void BackGammon::finished(int theplayer)
{
    nomove_marker->hide();
    if(theplayer==1)
        message->setText(tr( "<b>Player 1 wins. Click on board for new game.</b>" ));
    else
        message->setText(tr( "<b>Player 2 wins. Click on board for new game.</b>" ));
    diceA1_value=7;
    diceA2_value=7;
    diceB1_value=7;
    diceB2_value=7;
    player=0;
    showdice();
    draw();
    area->update();
    gameFinished=true;
}

void BackGammon::showdice()
{
    int value_diceA1=diceA1_value-1;
    if(diceA1_value==7 && diceA3_value!=7)
        value_diceA1=diceA3_value-1;

    int value_diceA2=diceA2_value-1;
    if(diceA2_value==7 && diceA4_value!=7)
        value_diceA2=diceA4_value-1;

    int value_diceB1=diceB1_value-1;
    if(diceB1_value==7 && diceB3_value!=7)
        value_diceB1=diceB3_value-1;

    int value_diceB2=diceB2_value-1;
    if(diceB2_value==7 && diceB4_value!=7)
        value_diceB2=diceB4_value-1;

    for(int index=0;index<7;index++)
    {
        if(value_diceA1==index)
            diceA1[index]->show();
        else
            diceA1[index]->hide();

        if(value_diceA2==index)
            diceA2[index]->show();
        else
            diceA2[index]->hide();

        if(value_diceB1==index)
            diceB1[index]->show();
        else
            diceB1[index]->hide();

        if(value_diceB2==index)
            diceB2[index]->show();
        else
            diceB2[index]->hide();
    }
}

void BackGammon::setplayer()
{
    if(dice1_played && dice2_played && dice3_played && dice4_played && player==1)
    {
        message->setText(tr( "<b>P2 turn</b>", "P means player" ));
        dice_rolled=false;
        player=2;
        if(player2_auto)
            QTimer::singleShot(2000,this,SLOT(autoroll_dice2()));
    }
    else if(dice1_played && dice2_played && dice3_played && dice4_played && player==2)
    {
        message->setText(tr( "<b>P1 turn</b>", "P means player" ));
        dice_rolled=false;
        player=1;
        if(player1_auto)
            QTimer::singleShot(2000,this,SLOT(autoroll_dice1()));
    }
}

void BackGammon::autoroll_dice1()
{
    mouse(20,210);
}

void BackGammon::autoroll_dice2()
{
    mouse(170,210);
}

void BackGammon::applytheme()
{
    QImage boardbg(Resource::loadImage("backgammon/boards/"+board_name));
    board->setImage(boardbg);

    QImage tablebg(Resource::loadImage("backgammon/table/"+table_name));
    table->setImage(tablebg);

    QImage piece_1_all(Resource::loadImage("backgammon/pieces/"+piecesA_name));
    QImage piece_1_front=piece_1_all.copy(0,0,15,15);
    QImage piece_1_side=piece_1_all.copy(0,15,15,5);

    QImage piece_2_all(Resource::loadImage("backgammon/pieces/"+piecesB_name));
    QImage piece_2_front=piece_2_all.copy(0,0,15,15);
    QImage piece_2_side=piece_2_all.copy(0,15,15,5);

    int a=0;
    for(a=0;a<15;a++)
    {
        p1[a]->setImage(piece_1_front);
        p1_side[a]->setImage(piece_1_side);

        p2[a]->setImage(piece_2_front);
        p2_side[a]->setImage(piece_2_side);
    }
    draw();

    QImage dicebgA_all(Resource::loadImage("backgammon/dice/"+diceA_name));
    QImage dicebgB_all(Resource::loadImage("backgammon/dice/"+diceB_name));
    QImage oddsbg_all=(Resource::loadImage("backgammon/odds/"+odds_name));

    for(a=0;a<7;a++)
    {
        QImage dicebgA=dicebgA_all.copy(a*25,0,25,25);
        diceA1[a]->setImage(dicebgA);
        diceA2[a]->setImage(dicebgA);

        QImage dicebgB=dicebgB_all.copy(a*25,0,25,25);
        diceB1[a]->setImage(dicebgB);
        diceB2[a]->setImage(dicebgB);
        /*
        if(a<6)
        {
            QImage oddsbg=oddsbg_all.copy(a*15,0,15,15);
            oddsDice[a]->setImage(oddsbg);
        }
        */
    }
}


