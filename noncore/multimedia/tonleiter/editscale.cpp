#include "editscale.h"

#include <qlayout.h>
#include <qpushbutton.h>

#include <qpe/qpeapplication.h>

Menu::ScaleEditDialog::ScaleEditDialog(TonleiterData* data,QWidget* parent,const char* name)
:QDialog(parent,name,true,0),data(data)
{
    setCaption("Tonleiter::"+tr("Scale"));
    QBoxLayout* masterlayout=new QBoxLayout(this,QBoxLayout::TopToBottom);

    QBoxLayout* toplayout=new QBoxLayout(masterlayout,QBoxLayout::LeftToRight);

    boxScale=new QComboBox(this,"boxScale");
    for(int s=0;s<data->noOfScales();s++)
    {
        Scale scale=data->getScale(s);
        QString name=scale.scaleName();
        if(name.isNull()) continue;
        //boxScale->insertItem(name,s);
    }
    boxScale->setCurrentItem(data->getCurrentScaleID());
    boxScale->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    connect(boxScale,SIGNAL(activated(int)),data,SLOT(setCurrentScaleID(int)));
    toplayout->addWidget(boxScale);

    QPushButton* addButton=new QPushButton(tr("Add"),this,"addButton");
    connect(addButton,SIGNAL(pressed()),this,SLOT(addInstrument()));
    toplayout->addWidget(addButton);

    QPushButton* delButton=new QPushButton(tr("Delete"),this,"delButton");
    connect(delButton,SIGNAL(pressed()),this,SLOT(deleteInstrument()));
    toplayout->addWidget(delButton);

    pianoscale=new PianoScale(this);
    masterlayout->addWidget(pianoscale);
    
    //make dialog fit the screen
    QPEApplication::showDialog( this );
}
//****************************************************************************
Menu::ScaleEditDialog::~ScaleEditDialog()
{
}
//****************************************************************************
//****************************************************************************
