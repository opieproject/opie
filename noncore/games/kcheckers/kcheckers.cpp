
#include <qimage.h>
#include <qframe.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpe/config.h>
#include <qwhatsthis.h>
#include <qtoolbutton.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include "kcheckers.h"
#include "echeckers.h"
#include "rcheckers.h"

#include "pics/logo.xpm"
#include "pics/undo.xpm"
#include "pics/exit.xpm"
#include "pics/help.xpm"
#include "pics/wood1.xpm"
#include "pics/wood2.xpm"
#include "pics/wood3.xpm"
#include "pics/green1.xpm"
#include "pics/green2.xpm"
#include "pics/green3.xpm"
#include "pics/marble1.xpm"
#include "pics/marble2.xpm"
#include "pics/marble3.xpm"
#include "pics/biglogo.xpm"
#include "pics/man_black.xpm"
#include "pics/man_white.xpm"
#include "pics/king_black.xpm"
#include "pics/king_white.xpm"
#include "pics/contexthelp.xpm"


QString KCheckers::enNumeration="1 2 3 4 5 6 7 8 9 1011121314151617181920212223242526272829303132";
QString KCheckers::ruNumeration="B8D8F8H8A7C7E7G7B6D6F6H6A5C5E5G5B4D4F4H4A3C3E3G3B2D2F2H2A1C1E1G1";

const int KCheckers::t[]={6,7,8,9,11,12,13,14,17,18,19,20,22,23,
          24,25,28,29,30,31,33,34,35,36,39,40,41,42,44,45,46,47};


KCheckers::KCheckers():QMainWindow(0,0,WStyle_DialogBorder)
{
  setCaption("KCheckers");
  setIcon(QPixmap(biglogo_xpm));

  setToolBarsMovable(false);

  // Make a menubar

  gameMenu=new QPopupMenu;
  CHECK_PTR(gameMenu);

  gameMenu->insertItem(QPixmap(logo_xpm),tr("&New"),this,SLOT(newGame()),CTRL+Key_N);
  gameMenu->insertSeparator();
  undoID=gameMenu->insertItem(QPixmap(undo_xpm),tr("&Undo Move"),this,SLOT(undoMove()),CTRL+Key_Z);
  gameMenu->insertSeparator();
  gameMenu->insertItem(QPixmap(exit_xpm),tr("&Quit"),qApp,SLOT(closeAllWindows()),CTRL+Key_Q);

  skillMenu=new QPopupMenu;
  CHECK_PTR(skillMenu);

  skillMenu->insertItem(tr("&Beginner"),this,SLOT(setSkillBeginner()),CTRL+Key_1,BEGINNER);
  skillMenu->insertItem(tr("&Novice"),  this,SLOT(setSkillNovice()),  CTRL+Key_2,NOVICE);
  skillMenu->insertItem(tr("&Average"), this,SLOT(setSkillAverage()), CTRL+Key_3,AVERAGE);
  skillMenu->insertItem(tr("&Good"),    this,SLOT(setSkillGood()),    CTRL+Key_4,GOOD);
  skillMenu->insertItem(tr("&Expert"),  this,SLOT(setSkillExpert()),  CTRL+Key_5,EXPERT);
  skillMenu->insertItem(tr("&Master"),  this,SLOT(setSkillMaster()),  CTRL+Key_6,MASTER);

  optionsMenu=new QPopupMenu;
  CHECK_PTR(optionsMenu);

  numID=optionsMenu->insertItem(tr("&Show Numeration"),this,SLOT(showNumeration()));
  optionsMenu->insertSeparator();
  optionsMenu->insertItem(tr("&English Rules"),this,SLOT(setRulesEnglish()),0,ENGLISH);
  optionsMenu->insertItem(tr("&Russian Rules"),this,SLOT(setRulesRussian()),0,RUSSIAN);
  optionsMenu->insertSeparator();
  optionsMenu->insertItem(tr("&Green Board"), this,SLOT(setPatternGreen()), 0,GREEN);
  optionsMenu->insertItem(tr("&Marble Board"),this,SLOT(setPatternMarble()),0,MARBLE);
  optionsMenu->insertItem(tr("&Wooden Board"),this,SLOT(setPatternWooden()),0,WOODEN);

  QPopupMenu* helpMenu=new QPopupMenu;
  CHECK_PTR(helpMenu);

  helpMenu->insertItem(QPixmap(contexthelp_xpm),tr("What's This"),this,SLOT(whatsThis()),SHIFT+Key_F1);
  helpMenu->insertItem(QPixmap(help_xpm),tr("&Rules of Play"),this,SLOT(help()),Key_F1);
  helpMenu->insertSeparator();
  helpMenu->insertItem(QPixmap(logo_xpm),tr("&About KCheckers"),this,SLOT(about()));
  helpMenu->insertItem(tr("About &Qt"),this,SLOT(aboutQt()));

  QToolBar* menuToolBar=new QToolBar(this);
  CHECK_PTR(menuToolBar);
  QMenuBar* menuBar=new QMenuBar(menuToolBar);
  CHECK_PTR(menuBar);

  menuBar->insertItem(tr("&Game"),   gameMenu);
  menuBar->insertItem(tr("&Skill"),  skillMenu);
  menuBar->insertItem(tr("&Options"),optionsMenu);
  menuBar->insertItem(tr("&Help"),   helpMenu);

  // Restore a settings

  readConfig();

  skillMenu->setItemChecked(skill,true);
  optionsMenu->setItemChecked(rules,true);
  optionsMenu->setItemChecked(numID,false);

  // Make a toolbar

  QToolBar* emptyToolBar=new QToolBar(this);
  emptyToolBar->setHorizontalStretchable(true);

  QToolBar* toolBar=new QToolBar(this);
  CHECK_PTR(toolBar);

  QSize buttonSize(24,24);

  QToolButton* gameButton=new QToolButton(QPixmap(logo_xpm),tr(" New Game ") ,"",this,SLOT(newGame()),toolBar);
  CHECK_PTR(gameButton);
  gameButton->setMinimumSize(buttonSize);

  undoButton=new QToolButton(QPixmap(undo_xpm),tr(" Undo Move "),"",this,SLOT(undoMove()),toolBar);
  CHECK_PTR(undoButton);
  undoButton->setMinimumSize(buttonSize);

  QToolButton* helpButton=new QToolButton(QPixmap(help_xpm),tr(" Rules of Play "),"",this,SLOT(help()),toolBar);
  CHECK_PTR(helpButton);
  helpButton->setMinimumSize(buttonSize);

  // Make a checkers board

  imageMan1=new QImage(man_black_xpm);   CHECK_PTR(imageMan1);
  imageMan2=new QImage(man_white_xpm);   CHECK_PTR(imageMan2);
  imageKing1=new QImage(king_black_xpm); CHECK_PTR(imageKing1);
  imageKing2=new QImage(king_white_xpm); CHECK_PTR(imageKing2);

  imageWood1=new QImage(wood1_xpm);      CHECK_PTR(imageWood1);
  imageWood2=new QImage(wood2_xpm);      CHECK_PTR(imageWood2);
  imageWood3=new QImage(wood3_xpm);      CHECK_PTR(imageWood3);
  imageGreen1=new QImage(green1_xpm);    CHECK_PTR(imageGreen1);
  imageGreen2=new QImage(green2_xpm);    CHECK_PTR(imageGreen2);
  imageGreen3=new QImage(green3_xpm);    CHECK_PTR(imageGreen3);
  imageMarble1=new QImage(marble1_xpm);  CHECK_PTR(imageMarble1);
  imageMarble2=new QImage(marble2_xpm);  CHECK_PTR(imageMarble2);
  imageMarble3=new QImage(marble3_xpm);  CHECK_PTR(imageMarble3);

  QWidget* centralWidget=new QWidget(this);
  setCentralWidget(centralWidget);

  QFrame* frame=new QFrame(centralWidget);
  CHECK_PTR(frame);
  frame->setFrameStyle(QFrame::Box|QFrame::Plain);
  frame->setFixedSize(SIZE*8+2,SIZE*8+2);

  statusLabel = new QLabel(centralWidget);
  statusLabel->setAlignment(Qt::AlignHCenter);

  QVBoxLayout* vlayout=new QVBoxLayout(centralWidget);

  vlayout->addStretch();
  vlayout->addWidget(frame);
  vlayout->addStretch();
  vlayout->addWidget(statusLabel);
  vlayout->addStretch();

  for(int i=0;i<64;i++)
  {
    field[i]=new Field(frame,i);
    CHECK_PTR(field[i]);
  }

  QGridLayout* grid=new QGridLayout(frame,8,8,1,0);
  CHECK_PTR(grid);

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

  selected=false;

  setPattern(pattern);

  QWhatsThis::add(frame,"A checkers board");

  userFirst=false;

  game=NULL;
  newGame();
}


void KCheckers::readConfig()
{
  Config config("KCheckers");
  config.setGroup("KCheckers");
  QString entry;

  entry=config.readEntry("skill","novice");
  if(entry=="beginner")     skill=BEGINNER;
  else if(entry=="average") skill=AVERAGE;
  else if(entry=="good")    skill=GOOD;
  else if(entry=="expert")  skill=EXPERT;
  else if(entry=="master")  skill=MASTER;
  else skill=NOVICE;

  entry=config.readEntry("rules","english");
  if(entry=="russian") rules=RUSSIAN;
  else rules=ENGLISH;

  entry=config.readEntry("theme","wooden");
  if(entry=="green")       pattern=GREEN;
  else if(entry=="marble") pattern=MARBLE;
  else pattern=WOODEN;
}


void KCheckers::closeEvent(QCloseEvent* event)
{
  Config config("KCheckers");
  config.setGroup("KCheckers");
  QString entry;

  if(skill==BEGINNER) entry="beginner";
  if(skill==NOVICE)   entry="novice";
  if(skill==AVERAGE)  entry="average";
  if(skill==GOOD)     entry="good";
  if(skill==EXPERT)   entry="expert";
  if(skill==MASTER)   entry="master";
  config.writeEntry("skill",entry);

  if(rules==ENGLISH) entry="english";
  if(rules==RUSSIAN) entry="russian";
  config.writeEntry("rules",entry);

  if(pattern==GREEN)  entry="green";
  if(pattern==MARBLE) entry="marble";
  if(pattern==WOODEN) entry="wooden";
  config.writeEntry("theme",entry);

  event->accept();
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
    case GREEN:
      imagePat1=imageGreen1;
      imagePat2=imageGreen2;
      imageFrame=imageGreen3;
      break;
    case MARBLE:
      imagePat1=imageMarble1;
      imagePat2=imageMarble2;
      imageFrame=imageMarble3;
      break;
    case WOODEN:
      imagePat1=imageWood1;
      imagePat2=imageWood2;
      imageFrame=imageWood3;
  }

  for(int i=0; i<32;i++) field[i]->setPattern(imagePat2);
  for(int i=32;i<64;i++) field[i]->setPattern(imagePat1);

  if(selected) field[from]->setFrame(imageFrame);
}


void KCheckers::showNumeration()
{
  if(optionsMenu->isItemChecked(numID))
  {
    optionsMenu->setItemChecked(numID,false);
    for(int i=0;i<32;i++) field[i]->setLabel("");
  }
  else
  {
    optionsMenu->setItemChecked(numID,true);
    drawNumeration();
  }
}


void KCheckers::drawNumeration()
{
  if(rules==ENGLISH)
  {
    if(userFirst)
      for(int i=0;i<32;i++)
        field[i]->setLabel(enNumeration.mid(i*2,2));
    else
      for(int i=0;i<32;i++)
        field[i]->setLabel(enNumeration.mid(62-i*2,2));
  }
  else
  {
    if(userFirst)
      for(int i=0;i<32;i++)
        field[i]->setLabel(ruNumeration.mid(i*2,2));
    else
      for(int i=0;i<32;i++)
        field[i]->setLabel(ruNumeration.mid(62-i*2,2));
  }
}


void KCheckers::drawBoard(int i)
{
  switch(game->board[t[i]])
  {
    case MAN1:
      field[i]->setPicture(imageMan1);
      break;
    case MAN2:
      field[i]->setPicture(imageMan2);
      break;
    case KING1:
      field[i]->setPicture(imageKing1);
      break;
    case KING2:
      field[i]->setPicture(imageKing2);
      break;
    default:
      field[i]->setPicture(NULL);
  }
}


void KCheckers::help()
{
  QMessageBox::information(this,"Rules of Play",
    "In the beginning of game you have\n"
    "12 checkers (men).\n"
    "The men move forward only.\n"
    "The men can capture:\n"
    "- by jumping forward only (english\n"
    "  rules);\n"
    "- by jumping forward or backward\n"
    "  (russian rules).\n"
    "A man which reaches the far side of\n"
    "the board becomes a king.\n"
    "The kings move forward or\n"
    "backward:\n"
    "- to one square only (english rules);\n"
    "- to any number of squares (russian\n"
    "  rules).\n"
    "The kings capture by jumping\n"
    "forward or backward.\n"
    "Whenever a player is able to make a\n"
    "capture he must do so.",
    QMessageBox::Ok|QMessageBox::Default);
}


void KCheckers::about()
{
  QMessageBox::about(this,"About KCheckers",
    "KCheckers, a board game. Ver 0.3\n"
    "(C) 2002, A. Peredri <andi@ukr.net>\n\n"
    "http://kcheckers.tuxfamily.org\n\n"
    "Contributors:\n"
    "S. Rosen <srosen@erols.com>\n\n"
    "Qtopia version: S.Prud'homme\n"
    "<prudhomme@laposte.net>\n\n"
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
  if(game) delete game;

  switch(rules)
  {
    case ENGLISH:
      game=new ECheckers(skill);
      CHECK_PTR(game);
      break;

    case RUSSIAN:
      game=new RCheckers(skill);
      CHECK_PTR(game);
  }

  unselect();
  gameOver=false;

  gameMenu->setItemEnabled(undoID,false);
  undoButton->setEnabled(false);

  colorChange();

  for(int i=0;i<32;i++) drawBoard(i);
  if(optionsMenu->isItemChecked(numID)) drawNumeration();

  if(!userFirst) compGo();

  statusLabel->setText(tr("Go!"));
}


// Undo the last computer and user moves

void KCheckers::undoMove()
{
  for(int i=0;i<32;i++)
  {
    game->board[t[i]]=undoBoard[i];
    drawBoard(i);
  }

  unselect();
  gameOver=false;

  gameMenu->setItemEnabled(undoID,false);
  undoButton->setEnabled(false);

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


void KCheckers::unselect()
{
  if(selected) field[from]->setFrame(NULL);
  selected=false;
}


void KCheckers::click(int fieldNumber)
{
  if(gameOver) return;
  switch(game->board[t[fieldNumber]])
  {
    case MAN1:
    case KING1:
      // User selected
      if(!selected)
      {
        from=fieldNumber;
        selected=true;
        field[fieldNumber]->setFrame(imageFrame);
        return;
      }
      // User reselected
      else
      {
        field[from]->setFrame(NULL);
        from=fieldNumber;
        field[fieldNumber]->setFrame(imageFrame);
        return;
      }
    case FREE:
      if(!selected) return;

      if(!userGo(fieldNumber)) return;

      unselect();

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
  for(int i=0;i<32;i++) save[i]=game->board[t[i]];

  game->go2();

  for(int i=0;i<32;i++)
  {
    if(game->board[t[i]]==save[i]) continue;
    drawBoard(i);
  }
}


bool KCheckers::userGo(int to)
{
  int save[32];
  for(int i=0;i<32;i++) save[i]=game->board[t[i]];

  if(!game->go1(t[from],t[to]))
  {
    statusLabel->setText(tr("Incorrect course."));
    return false;
  }

  for(int i=0;i<32;i++) undoBoard[i]=save[i];
  gameMenu->setItemEnabled(undoID,true);
  undoButton->setEnabled(true);

  for(int i=0;i<32;i++)
  {
    if(game->board[t[i]]==save[i]) continue;
    drawBoard(i);
  }

  return true;
}


