#include "editstringwidget.h"

#include <qlabel.h>
#include <qlayout.h>

using namespace Data;

Menu::EditStringWidget::EditStringWidget(int note,QWidget* parent,const char* name,WFlags f)
:QFrame(parent,name,f)
{
    QBoxLayout* layout=new QBoxLayout(this,QBoxLayout::LeftToRight,10);

    boxNote=new QComboBox(this,"boxNote");
    boxNote->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    //connect(boxNote,SIGNAL(activated(int)),data,SLOT(setCurrentNote(int)));
    layout->addWidget(boxNote,1,1);
    for(int a=0;a<12;a++)
        boxNote->insertItem(Note::notenames[a],a);
    boxNote->setCurrentItem(note-12*Note::getOctaveOfNote(note));
    layout->addWidget(boxNote);

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);

}
//****************************************************************************
Menu::EditStringWidget::~EditStringWidget()
{
}
//****************************************************************************
//****************************************************************************
