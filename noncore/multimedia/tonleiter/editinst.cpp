#include "editinst.h"

Menu::InstEditDialog::InstEditDialog(TonleiterData* data,QWidget* parent,const char* name)
:QDialog(parent,name,true,0),data(data)
{
    setCaption("Tonleiter::"+tr("Instrument"));
    showMaximized();
}
//****************************************************************************
Menu::InstEditDialog::~InstEditDialog()
{
}
//****************************************************************************
//****************************************************************************
