#ifndef TONLEITER_MENU_WIDGET_H
#define TONLEITER_MENU_WIDGET_H

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qlabel.h>

#include "tonleiterdata.h"

using namespace Data;
namespace Menu
{
    class MenuWidget : public QWidget
    {
        Q_OBJECT
    private:
        TonleiterData* data;
        QComboBox* boxInst;
        QPushButton* editInst;
        QComboBox* boxNote;
        QCheckBox* noteCheck;
        QComboBox* boxScale;
        QPushButton* editScale;
    public:
        MenuWidget(TonleiterData* data,QWidget* parent,const char* name="MenuWidget",WFlags f=0);
        ~MenuWidget();
    private slots:
        void editInstPressed();
        void editScalePressed();
    public slots:
        void updateBoxes();
    };
};

#endif //TONLEITER_MENU_WIDGET_H
