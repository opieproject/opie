#ifndef TONLEITER_EDIT_SCALE_DIALOG_H
#define TONLEITER_EDIT_SCALE_DIALOG_H

#include <qdialog.h>
#include <qcombobox.h>

#include "tonleiterdata.h"
#include "pianoscale.h"

using namespace Data;

namespace Menu
{
    class ScaleEditDialog : public QDialog
    {
        Q_OBJECT
    private:
        TonleiterData* data;
        QComboBox* boxScale;
        PianoScale* pianoscale;
    public:
        ScaleEditDialog(TonleiterData* data,QWidget* parent,const char* name="ScaleEditDialog");
        ~ScaleEditDialog();
    };
};

#endif //TONLEITER_EDIT_SCALE_DIALOG_H
