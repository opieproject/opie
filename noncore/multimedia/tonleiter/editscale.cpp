#include "editscale.h"

Menu::ScaleEditDialog::ScaleEditDialog(TonleiterData* data,QWidget* parent,const char* name)
:QDialog(parent,name,true,0),data(data)
{
    setCaption("Tonleiter::"+tr("Scale"));
    showMaximized();
}
//****************************************************************************
Menu::ScaleEditDialog::~ScaleEditDialog()
{
}
//****************************************************************************
//****************************************************************************
