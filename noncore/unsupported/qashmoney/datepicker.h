#ifndef DATEPICKER_H
#define DATEPICKER_H

#include <qdialog.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <qcombobox.h>
#include <qlayout.h>

class DatePicker : public QDialog

  {
    Q_OBJECT

    public:
      QLabel *daylabel;
      QComboBox *daybox;

      QLabel *monthlabel;
      QComboBox *monthbox;

      DatePicker ( QDate entrydate );
      int getDay ();
      int getMonth ();
      int getYear ();

    private slots:
      void setDay ( int );
      void setMonth ( int );
      void setYear ( int );

    private:
      void displayDays ( QComboBox *daybox );
      void displayMonths ( QComboBox *monthbox );
      void displayYears ( QComboBox *yearbox );

      QDate date;

      QLabel *yearlabel;
      QComboBox *yearbox;

      QGridLayout *layout;

      int day, month, year;

  };

#endif












