#ifndef TONLEITER_EDIT_STRING_WIDGT_H
#define TONLEITER_EDIT_STRING_WIDGT_H

#include <qframe.h>
#include <qcombobox.h>
#include <qspinbox.h>

#include "tonleiterdata.h"

namespace Menu
{
    class EditStringWidget : public QFrame
    {
        Q_OBJECT
    private:
        QComboBox* boxNote;
    public:
        EditStringWidget(int note,QWidget* parent,const char* name=0,WFlags f=0);
        ~EditStringWidget();
    };
};

#endif //TONLEITER_EDIT_STRING_WIDGT_H

