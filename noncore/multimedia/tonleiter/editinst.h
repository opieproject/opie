#ifndef TONLEITER_EDIT_INSTRUMENT_DIALOG_H
#define TONLEITER_EDIT_INSTRUMENT_DIALOG_H

#include <qdialog.h>

#include "tonleiterdata.h"

using namespace Data;

namespace Menu
{
    class InstEditDialog : public QDialog
    {
        Q_OBJECT
    private:
        TonleiterData* data;
    public:
        InstEditDialog(TonleiterData* data,QWidget* parent,const char* name="InstEditDialog");
        ~InstEditDialog();
    };
};

#endif //TONLEITER_EDIT_INSTRUMENT_DIALOG_H
