#include "themedialog.h"

#include <qdir.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpe/qpeapplication.h>


ThemeDialog::ThemeDialog(QWidget* parent,const char* name,bool modal,WFlags f)
        :QDialog(parent,name,modal,f)
{
    setCaption("Theme Dialog");
    QLabel* labelname=new QLabel("name",this);
    labelname->setGeometry(0,5,40,20);
    lineName=new QLineEdit("user",this);
    lineName->setGeometry(40,5,195,20);

    QLabel* labelboard=new QLabel("board",this);
    labelboard->setGeometry(0,30,40,20);
    boxboard=new QComboBox(this,"");
    boxboard->setGeometry(40,30,195,20);
    fillBox("boards",boxboard);

    QLabel* labelpiecesA=new QLabel("pieces1",this);
    labelpiecesA->setGeometry(0,70,40,20);
    boxpiecesA=new QComboBox(this);
    boxpiecesA->setGeometry(40,70,195,20);
    fillBox("pieces",boxpiecesA);

    QLabel* labelpiecesB=new QLabel("pieces2",this);
    labelpiecesB->setGeometry(0,95,40,20);
    boxpiecesB=new QComboBox(this);
    boxpiecesB->setGeometry(40,95,195,20);
    fillBox("pieces",boxpiecesB);

    QLabel* labeldiceA=new QLabel("dice1",this);
    labeldiceA->setGeometry(0,135,40,20);
    boxdiceA=new QComboBox(this);
    boxdiceA->setGeometry(40,135,195,20);
    fillBox("dice",boxdiceA);

    QLabel* labeldiceB=new QLabel("dice2",this);
    labeldiceB->setGeometry(0,160,40,20);
    boxdiceB=new QComboBox(this);
    boxdiceB->setGeometry(40,160,195,20);
    fillBox("dice",boxdiceB);

    QLabel* labelodds=new QLabel("odds",this);
    labelodds->setGeometry(0,200,40,20);
    boxodds=new QComboBox(this);
    boxodds->setGeometry(40,200,195,20);
    fillBox("odds",boxodds);
    boxodds->setEnabled(false);

    QLabel* labeltable=new QLabel("table",this);
    labeltable->setGeometry(0,225,40,20);
    boxtable=new QComboBox(this);
    boxtable->setGeometry(40,225,195,20);
    fillBox("table",boxtable);

    showMaximized();
}

ThemeDialog::~ThemeDialog()
{}


ImageNames ThemeDialog::getNames()
{
    ImageNames names;
    names.theme=lineName->text();
    names.board=boxboard->currentText();
    names.pieces1=boxpiecesA->currentText();
    names.pieces2=boxpiecesB->currentText();
    names.dice1=boxdiceA->currentText();
    names.dice2=boxdiceB->currentText();
    names.odds=boxodds->currentText();
    names.table=boxtable->currentText();
    return names;
}

void ThemeDialog::setCurrent(const ImageNames& current)
{
    int a=0;
    lineName->setText(current.theme);
    for(a=0;a<boxboard->count();a++)
    {
        if(boxboard->text(a)==current.board)
        {
            boxboard->setCurrentItem(a);
            break;
        }
    }
    for(a=0;a<boxpiecesA->count();a++)
    {
        if(boxpiecesA->text(a)==current.pieces1)
        {
            boxpiecesA->setCurrentItem(a);
            break;
        }
    }
    for(a=0;a<boxpiecesB->count();a++)
    {
        if(boxpiecesB->text(a)==current.pieces2)
        {
            boxpiecesB->setCurrentItem(a);
            break;
        }
    }
    for(a=0;a<boxdiceA->count();a++)
    {
        if(boxdiceA->text(a)==current.dice1)
        {
            boxdiceA->setCurrentItem(a);
            break;
        }
    }
    for(a=0;a<boxdiceB->count();a++)
    {
        if(boxdiceB->text(a)==current.dice2)
        {
            boxdiceB->setCurrentItem(a);
            break;
        }
    }
    for(a=0;a<boxodds->count();a++)
    {
        if(boxodds->text(a)==current.odds)
        {
            boxodds->setCurrentItem(a);
            break;
        }
    }
    for(a=0;a<boxtable->count();a++)
    {
        if(boxtable->text(a)==current.table)
        {
            boxtable->setCurrentItem(a);
            break;
        }
    }
}

void ThemeDialog::fillBox(QString dirname,QComboBox* thebox)
{
    thebox->clear();
    QDir dir(QPEApplication::qpeDir()+"/pics/backgammon/"+dirname);
    dir.setFilter(QDir::Dirs | QDir::Files);
    QFileInfoListIterator it(*(dir.entryInfoList()));
    QFileInfo* fi;

    while((fi=it.current())) // go through all file and subdirs
    {
        QString file=fi->fileName();
        if(file.right(4)==".png")
        {
            thebox->insertItem(file.left(file.find(".png")));
        }
        ++it;
    }
    delete fi;
}
