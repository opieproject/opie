#include "menuwidget.h"


#include "editinst.h"
#include "editscale.h"

Menu::MenuWidget::MenuWidget(TonleiterData* data,QWidget* parent,const char* name,WFlags f)
:QWidget(parent,name,f),data(data)
{
    QGridLayout* layout=new QGridLayout(this,3,3,10,-1,"menulayout");

    //Instrument
    QLabel* instLabel=new QLabel(tr("Instr."),this,"instLabel");
    instLabel->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    layout->addWidget(instLabel,0,0);

    boxInst=new QComboBox(this,"boxInst");
    boxInst->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    connect(boxInst,SIGNAL(activated(int)),data,SLOT(setCurrentInstrumetID(int)));
    layout->addWidget(boxInst,0,1);

    editInst=new QPushButton(tr("Edit"),this,"editInst");
    editInst->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    connect(editInst,SIGNAL(pressed()),this,SLOT(editInstPressed()));
    layout->addWidget(editInst,0,2);

    //Note
    QLabel* noteLabel=new QLabel(tr("Note"),this,"noteLabel");
    noteLabel->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    layout->addWidget(noteLabel,1,0);

    boxNote=new QComboBox(this,"boxNote");
    boxNote->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    connect(boxNote,SIGNAL(activated(int)),data,SLOT(setCurrentNote(int)));
    layout->addWidget(boxNote,1,1);
    for(int a=0;a<12;a++)
        boxNote->insertItem(Note::notenames[a],a);
    boxNote->setCurrentItem(data->getCurrentBaseNote());

    noteCheck=new QCheckBox(tr("show"),this,"noteCheck");
    noteCheck->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    noteCheck->setChecked(data->isDrawNames());
    connect(noteCheck,SIGNAL(toggled(bool)),data,SLOT(setDrawNames(bool)));
    layout->addWidget(noteCheck,1,2);

    //Scale
    QLabel* scaleLabel=new QLabel(tr("Scale"),this,"scaleLabel");
    scaleLabel->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    layout->addWidget(scaleLabel,2,0);

    boxScale=new QComboBox(this,"boxScale");
    boxScale->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    connect(boxScale,SIGNAL(activated(int)),data,SLOT(setCurrentScaleID(int)));
    layout->addWidget(boxScale,2,1);

    editScale=new QPushButton(tr("Edit"),this,"editScale");
    editScale->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    connect(editScale,SIGNAL(pressed()),this,SLOT(editScalePressed()));
    layout->addWidget(editScale,2,2);

    updateBoxes();
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
}
//****************************************************************************
Menu::MenuWidget::~MenuWidget()
{
}
//****************************************************************************
void Menu::MenuWidget::editInstPressed()
{
    InstEditDialog* instdialog=new InstEditDialog(data,this);
    instdialog->exec();
}
//****************************************************************************
void Menu::MenuWidget::editScalePressed()
{
    ScaleEditDialog* scaledialog=new ScaleEditDialog(data,this);
    scaledialog->exec();
}
//****************************************************************************
void Menu::MenuWidget::updateBoxes()
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
