#ifndef TONLEITER_EDIT_INSTRUMENT_DIALOG_H
#define TONLEITER_EDIT_INSTRUMENT_DIALOG_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qscrollview.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qlist.h>
#include <qvbox.h>


#include "editstringwidget.h"

using namespace Data;


namespace Menu
{
    class InstEditDialog : public QDialog
    {
        Q_OBJECT
    private:
        TonleiterData* data;
        int instid;

        QComboBox* boxInst;
        QSpinBox* stringBox;

        QScrollView* scrollview;
        QVBox* stringwidget;

        //this is a QPtrList !!
        typedef QList<EditStringWidget> StringWidgetList;
        StringWidgetList stringlist;
    public:
        InstEditDialog(TonleiterData* data,QWidget* parent,const char* name="InstEditDialog");
        ~InstEditDialog();
    private slots:
        void setCurrentInstrumetID(int id);
        void addInstrument();
        void deleteInstrument();

        void addLowString();
        void addHighString();
        void deleteString(int id);
    private:
        void saveInstrument();
        void loadInstrument();
    };
};

#endif //TONLEITER_EDIT_INSTRUMENT_DIALOG_H
