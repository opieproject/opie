#include "editinst.h"

#include <qlabel.h>
#include <qpushbutton.h>

Menu::InstEditDialog::InstEditDialog(TonleiterData* data,QWidget* parent,const char* name)
:QDialog(parent,name,true,0),data(data)
{
    setCaption("Tonleiter::"+tr("Instrument"));
    QBoxLayout* masterlayout=new QBoxLayout(this,QBoxLayout::TopToBottom);
    instid=data->getCurrentInstrumentID();
    stringlist.setAutoDelete(true);

    QBoxLayout* toplayout=new QBoxLayout(masterlayout,QBoxLayout::LeftToRight);

    //Name combo + Add + Delete
    boxInst=new QComboBox(this,"boxInst");
    boxInst->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    connect(boxInst,SIGNAL(activated(int)),this,SLOT(setCurrentInstrumetID(int)));
    for(int i=0;i<data->noOfInstruments();i++)
    {
        Instrument inst=data->getInstrument(i);
        QString name=inst.instName();
        if(name.isNull()) continue;
        boxInst->insertItem(name,i);
    }
    boxInst->setCurrentItem(instid);
    toplayout->addWidget(boxInst);

    QPushButton* addButton=new QPushButton(tr("Add"),this,"addButton");
    connect(addButton,SIGNAL(pressed()),this,SLOT(addInstrument()));
    toplayout->addWidget(addButton);

    QPushButton* delButton=new QPushButton(tr("Delete"),this,"delButton");
    connect(delButton,SIGNAL(pressed()),this,SLOT(deleteInstrument()));
    toplayout->addWidget(delButton);

    QPushButton* addhighButton=new QPushButton(tr("Add High String"),this,"addhighButton");
    masterlayout->addWidget(addhighButton);

    scrollview=new QScrollView(this);
    scrollview->setVScrollBarMode(QScrollView::AlwaysOn);
    scrollview->setHScrollBarMode(QScrollView::AlwaysOff);
    stringwidget=new QVBox(scrollview->viewport());
    stringwidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    stringwidget->setSpacing(10);
    scrollview->addChild(stringwidget);
    scrollview->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    masterlayout->addWidget(scrollview);

    loadInstrument();

    QPushButton* addlowButton=new QPushButton(tr("Add Low String"),this,"addlowButton");
    masterlayout->addWidget(addlowButton);

    //make dialog fit the screen
    showMaximized();
}
//****************************************************************************
Menu::InstEditDialog::~InstEditDialog()
{
}
//****************************************************************************
void Menu::InstEditDialog::setCurrentInstrumetID(int id)
{
    saveInstrument();
    instid=id;
    data->setCurrentInstrumetID(id);
    loadInstrument();
}
//****************************************************************************
void Menu::InstEditDialog::addInstrument()
{
}
//****************************************************************************
void Menu::InstEditDialog::deleteInstrument()
{
}
//****************************************************************************
void Menu::InstEditDialog::addLowString()
{
}
//****************************************************************************
void Menu::InstEditDialog::addHighString()
{
}
//****************************************************************************
void Menu::InstEditDialog::deleteString(int id)
{
}
//****************************************************************************
void Menu::InstEditDialog::saveInstrument()
{
    //auto delete is set to true so widgets _should_ be deleted
    stringlist.clear();
}
//****************************************************************************
void Menu::InstEditDialog::loadInstrument()
{
    stringlist.clear();
    Instrument inst=data->getInstrument(instid);
    qDebug("load Instrument : %d strings",inst.noOfStrings());
    for(uint s=0;s<inst.noOfStrings();s++)
    {
        stringlist.append(new EditStringWidget(inst.string(s),stringwidget));
    }
}
//****************************************************************************
//****************************************************************************
