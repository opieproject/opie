#ifndef MEMORYDIALOG_H
#define MEMORYDIALOG_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qstring.h>
#include <qhbox.h>

class MemoryDialog : public QDialog
  {
    Q_OBJECT

    public:
      MemoryDialog();
      ~MemoryDialog();

    private slots:
     void addItem();
     void editItem();
     void deleteItem();

    private:

      QHBox *secondline;

      QListBox* listbox;

      QLineEdit* lineedit;

      QPushButton* newbutton;
      QPushButton* editbutton;
      QPushButton* deletebutton;

      QBoxLayout *layout;
};

#endif
