#include "menuwidget.h"

#include <qlabel.h>
#include <qlayout.h>

MenuWidget::MenuWidget(TonleiterData* data,QWidget* parent,const char* name,WFlags f)
:QWidget(parent,name,f),data(data)
{
    QGridLayout* layout=new QGridLayout(this,3,3,10,-1,"menulayout");

    QLabel* instLabel=new QLabel(tr("Instrument"),this,"instLabel");
    layout->addWidget(instLabel,0,0);
    boxInst=new QComboBox(this,"boxInst");
    connect(boxInst,SIGNAL(activated(int)),data,SLOT(setCurrentInstrumetID(int)));
    layout->addWidget(boxInst,0,1);
    editInst=new QPushButton(tr("Edit"),this,"editInst");
    layout->addWidget(editInst,0,2);

    QLabel* noteLabel=new QLabel(tr("Note"),this,"noteLabel");
    layout->addWidget(noteLabel,1,0);
    boxNote=new QComboBox(this,"boxNote");
    connect(boxNote,SIGNAL(activated(int)),data,SLOT(setCurrentNote(int)));
    layout->addWidget(boxNote,1,1);
    for(int a=0;a<12;a++)
        boxNote->insertItem(Note::notenames[a],a);
    boxNote->setCurrentItem(data->getCurrentBaseNote());
    noteCheck=new QCheckBox(tr("show notes"),this,"noteCheck");
    noteCheck->setChecked(data->isDrawNames());
    connect(noteCheck,SIGNAL(toggled(bool)),data,SLOT(setDrawNames(bool)));
    layout->addWidget(noteCheck,1,2);

    QLabel* scaleLabel=new QLabel(tr("Scale"),this,"scaleLabel");
    layout->addWidget(scaleLabel,2,0);
    boxScale=new QComboBox(this,"boxScale");
    connect(boxScale,SIGNAL(activated(int)),data,SLOT(setCurrentScaleID(int)));
    layout->addWidget(boxScale,2,1);
    editScale=new QPushButton(tr("Edit"),this,"editScale");
    layout->addWidget(editScale,2,2);

    updateBoxes();

}
//****************************************************************************
MenuWidget::~MenuWidget()
{
}
//****************************************************************************
void MenuWidget::updateBoxes()
{
    boxInst->clear();

    for(int i=0;i<data->noOfInstruments();i++)
    {
        Instrument inst=data->getInstrument(i);
        QString name=inst.instName();
        if(name.isNull()) continue;
        boxInst->insertItem(name,i);
    }
    boxInst->setCurrentItem(data->getCurrentInstrumentID());

    boxScale->clear();
    for(int s=0;s<data->noOfScales();s++)
    {
        Scale scale=data->getScale(s);
        QString name=scale.scaleName();
        if(name.isNull()) continue;
        boxScale->insertItem(name,s);
    }
    boxScale->setCurrentItem(data->getCurrentScaleID());
}
//****************************************************************************
//****************************************************************************
