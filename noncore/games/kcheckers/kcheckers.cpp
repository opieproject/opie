
#include <qimage.h>
#include <qframe.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include "kcheckers.h"
#include "echeckers.h"
#include "rcheckers.h"

#include "pics/exit.xpm"
#include "pics/logo.xpm"
#include "pics/wood1.xpm"
#include "pics/wood2.xpm"
#include "pics/wood3.xpm"
#include "pics/marble1.xpm"
#include "pics/marble2.xpm"
#include "pics/marble3.xpm"
#include "pics/biglogo.xpm"
#include "pics/man_black.xpm"
#include "pics/man_white.xpm"
#include "pics/king_black.xpm"
#include "pics/king_white.xpm"
#include "pics/contexthelp.xpm"


const int KCheckers::t[]={6,7,8,9,11,12,13,14,17,18,19,20,22,23,
          24,25,28,29,30,31,33,34,35,36,39,40,41,42,44,45,46,47};

KCheckers::KCheckers():QMainWindow(0,0,WStyle_DialogBorder)
{
  setCaption("KCheckers");
  setIcon(QPixmap(biglogo));
  showMaximized();

  QPopupMenu* gameMenu=new QPopupMenu;
  gameMenu->insertItem(QPixmap(logo),tr("New"),this,SLOT(newGame()));
  gameMenu->insertSeparator();
  gameMenu->insertItem(QPixmap(exitx),tr("Quit"),qApp,SLOT(quit()));

  skillMenu=new QPopupMenu;

  skillMenu->insertItem(tr("Beginner"),this,SLOT(setSkillBeginner()),0,BEGINNER);
  skillMenu->insertItem(tr("Novice"),  this,SLOT(setSkillNovice()),  0,NOVICE);
  skillMenu->insertItem(tr("Average"), this,SLOT(setSkillAverage()), 0,AVERAGE);
  skillMenu->insertItem(tr("Good"),    this,SLOT(setSkillGood()),    0,GOOD);
  skillMenu->insertItem(tr("Expert"),  this,SLOT(setSkillExpert()),  0,EXPERT);
  skillMenu->insertItem(tr("Master"),  this,SLOT(setSkillMaster()),  0,MASTER);

  optionsMenu=new QPopupMenu;

  optionsMenu->insertItem(tr("English Rules"),this,SLOT(setRulesEnglish()),0,ENGLISH);
  optionsMenu->insertItem(tr("Russian Rules"),this,SLOT(setRulesRussian()),0,RUSSIAN);
  optionsMenu->insertSeparator();
  optionsMenu->insertItem(tr("Marble Board"),this,SLOT(setPatternMarble()),0,MARBLE);
  optionsMenu->insertItem(tr("Wooden Board"),this,SLOT(setPatternWood()),  0,WOOD);

  QPopupMenu* helpMenu=new QPopupMenu;

  helpMenu->insertItem(QPixmap(contexthelp),tr("What's This"),this,SLOT(whatsThis()));
  helpMenu->insertSeparator();
  helpMenu->insertItem(QPixmap(logo),tr("About KCheckers"),this,SLOT(about()));
  helpMenu->insertItem(tr("About &Qt"),this,SLOT(aboutQt()));

  menuBar()->insertItem(tr("Game"),   gameMenu);
  menuBar()->insertItem(tr("Skill"),  skillMenu);
  menuBar()->insertItem(tr("Options"),optionsMenu);
  menuBar()->insertItem(tr("Help"),   helpMenu);

  skill=NOVICE;
  skillMenu->setItemChecked(skill,true);

  rules=ENGLISH;
  optionsMenu->setItemChecked(rules,true);

  pattern=WOOD;
  optionsMenu->setItemChecked(pattern,true);

  imageMan1=new QImage(man_black);
  imageMan2=new QImage(man_white);
  imageKing1=new QImage(king_black);
  imageKing2=new QImage(king_white);

  imageWood1=new QImage(wood1);
  imageWood2=new QImage(wood2);
  imageWood3=new QImage(wood3);
  imageMarble1=new QImage(marble1);
  imageMarble2=new QImage(marble2);
  imageMarble3=new QImage(marble3);

  imagePat1=imageWood1;
  imagePat2=imageWood2;
  imageFrame=imageWood3;

  QWidget* centralWidget=new QWidget(this);
  setCentralWidget(centralWidget);

  QFrame* frame=new QFrame(centralWidget);
  frame->setFrameStyle(QFrame::Box|QFrame::Plain);
  frame->setFixedSize(SIZE*8+2,SIZE*8+2);

  QVBoxLayout* vlayout=new QVBoxLayout(centralWidget);
  vlayout->addWidget(frame);

  statusLabel = new QLabel(centralWidget);
  vlayout->addWidget(statusLabel);

  for(int i=0;i<64;i++) field[i]=new Field(frame,i);

  QGridLayout* grid=new QGridLayout(frame,8,8,1,0);

  for(int i=0;i<4;i++)
  {
    for(int k=0;k<4;k++)
    {
      grid->addWidget(field[i*8+k+32],i*2,  k*2  );
      grid->addWidget(field[i*8+k   ],i*2,  k*2+1);
      grid->addWidget(field[i*8+k+4 ],i*2+1,k*2  );
      grid->addWidget(field[i*8+k+36],i*2+1,k*2+1);
    }
  }

  for(int i=0;i<32;i++)
    connect(field[i],SIGNAL(click(int)),this,SLOT(click(int)));

  userFirst=false;

  game=NULL;
  newGame();
}


void KCheckers::setSkill(int set)
{
  skillMenu->setItemChecked(skill,false);

  skill=set;
  skillMenu->setItemChecked(skill,true);

  game->setLevel(skill);
}


void KCheckers::setRules(int set)
{
  optionsMenu->setItemChecked(rules,false);

  rules=set;
  optionsMenu->setItemChecked(rules,true);

  colorChange();
  newGame();
}


void KCheckers::setPattern(int set)
{
  optionsMenu->setItemChecked(pattern,false);

  pattern=set;
  optionsMenu->setItemChecked(pattern,true);

  switch(pattern)
  {
    case MARBLE:
      imagePat1=imageMarble1;
      imagePat2=imageMarble2;
      imageFrame=imageMarble3;
      break;
    case WOOD:
      imagePat1=imageWood1;
      imagePat2=imageWood2;
      imageFrame=imageWood3;
  }

  drawBoard();

  if(selected) field[from]->draw(imageFrame);
}


void KCheckers::about()
{
  QMessageBox::about(this, "About",
  "KCheckers, a board game. Ver 0.2\n"
  "(C) 2002, A. Peredri <andi@ukr.net>\n\n"
  "http://kcheckers.tuxfamily.org\n\n"
  "Opie version by S.Prud'homme\n"
  "<prudhomme@laposte.net>\n\n"
  "http://opie.handhelds.org\n\n"
  "This program is distributed under the\n"
  "terms of the GNU General Public\n"
  "License.");
}


void KCheckers::aboutQt()
{
  QMessageBox::aboutQt(this);
}


void KCheckers::newGame()
{
  if(game)
  {
    delete game;
    QWhatsThis::remove(this);
  }

  switch(rules)
  {
    case ENGLISH:
      game=new ECheckers(skill);
      QWhatsThis::add(this,
      "<b>Rules of English Checkers</b>"
      "<ul>"
      "<li>In the beginning of game you have<br>12 checkers (men)."
      "<li>Men move forward only."
      "<li>Men capture by jumping forward only."
      "<li>A man which reaches the far side of<br>the board becomes a king."
      "<li>Kings move forward or backward to<br>one square."
      "<li>Kings capture by jumping forward or<br>backward."
      "<li>Whenever a player is able to make<br> a capture he must do so."
      "</ul>");
      break;
    case RUSSIAN:
      game=new RCheckers(skill);
      QWhatsThis::add(this,
      "<b>Rules of Russian Checkers</b>"
      "<ul>"
      "<li>In the beginning of game you have<br> 12 checkers (men)."
      "<li>Men move forward only."
      "<li>Men capture by jumping forward or<br>backward."
      "<li>A man which reaches the far side of<br> the board becomes a king."
      "<li>Kings move forward or backward to<br>any number of squares."
      "<li>Kings capture by jumping forward or<br>backward."
      "<li>Whenever a player is able to make<br> a capture he must do so."
      "</ul>");
  }

  selected=false;
  gameOver=false;

  colorChange();
  drawBoard();

  if(!userFirst) compGo();

  statusLabel->setText(tr("Go!"));
}


void KCheckers::colorChange()
{
  userFirst=!userFirst;

  QImage* image;
  image=imageMan1;
  imageMan1=imageMan2;
  imageMan2=image;
  image=imageKing1;
  imageKing1=imageKing2;
  imageKing2=image;
}


void KCheckers::drawBoard()
{
  // Drawing of start position
  for(int i=0;i<32;i++)
  {
    field[i]->draw(imagePat2);
    if(game->getBoard(t[i])==MAN1) field[i]->draw(imageMan1);
    else if(game->getBoard(t[i])==MAN2)  field[i]->draw(imageMan2);
    else if(game->getBoard(t[i])==KING1) field[i]->draw(imageKing1);
    else if(game->getBoard(t[i])==KING2) field[i]->draw(imageKing2);
  }

  for(int i=32;i<64;i++) field[i]->draw(imagePat1);
}


void KCheckers::click(int fieldNumber)
{
  if(gameOver) return;
  switch(game->getBoard(t[fieldNumber]))
  {
    case MAN1:
    case KING1:
      // User selected
      if(!selected)
      {
        from=fieldNumber;
        selected=true;
        field[fieldNumber]->draw(imageFrame);
        return;
      }
      // User reselected
      else
      {
        field[from]->draw(imagePat2);
        if(game->getBoard(t[from])==MAN1) field[from]->draw(imageMan1);
        else field[from]->draw(imageKing1);
        from=fieldNumber;
        field[fieldNumber]->draw(imageFrame);
        return;
      }
    case FREE:
      if(!selected) return;

      if(!userGo(fieldNumber)) return;
      selected=false;

      if(!(game->checkMove2() || game->checkCapture2()))
      {
        gameOver=true;
        statusLabel->setText(tr("Congratulation! You have won!"));
        return;
      }

      statusLabel->setText(tr("Please wait..."));
      qApp->processEvents();

      compGo();
      if(!(game->checkMove1() || game->checkCapture1()))
      {
        gameOver=true;
        statusLabel->setText(tr("You have lost. Game over."));
        return;
      }
      statusLabel->setText(tr("Go!"));
  }
}


void KCheckers::compGo()
{
  int save[32];
  for(int i=0;i<32;i++) save[i]=game->getBoard(t[i]);

  game->go2();

  for(int i=0;i<32;i++)
  {
    if(game->getBoard(t[i])==save[i]) continue;
    field[i]->draw(imagePat2);
    if(game->getBoard(t[i])==MAN1) field[i]->draw(imageMan1);
    else if(game->getBoard(t[i])==MAN2)  field[i]->draw(imageMan2);
    else if(game->getBoard(t[i])==KING1) field[i]->draw(imageKing1);
    else if(game->getBoard(t[i])==KING2) field[i]->draw(imageKing2);
  }
}


bool KCheckers::userGo(int to)
{
  int save[32];
  for(int i=0;i<32;i++) save[i]=game->getBoard(t[i]);

  if(!game->go1(t[from],t[to]))
  {
    statusLabel->setText(tr("Incorrect course."));
    return false;
  }

  for(int i=0;i<32;i++)
  {
    if(game->getBoard(t[i])==save[i]) continue;
    field[i]->draw(imagePat2);
    if(game->getBoard(t[i])==MAN1) field[i]->draw(imageMan1);
    else if(game->getBoard(t[i])==MAN2)  field[i]->draw(imageMan2);
    else if(game->getBoard(t[i])==KING1) field[i]->draw(imageKing1);
    else if(game->getBoard(t[i])==KING2) field[i]->draw(imageKing2);
  }
  return true;
}


