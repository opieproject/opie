#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <qdialog.h>
#include <qlayout.h>
#include <qlineedit.h>

class Calculator : public QDialog
{
  Q_OBJECT

  public:

    Calculator ( QWidget* parent );

    QLineEdit* display;

  private slots:

    void displayOne ();
    void displayTwo ();
    void displayThree ();
    void displayFour ();
    void displayFive ();
    void displaySix ();
    void displaySeven ();
    void displayEight ();
    void displayNine ();
    void displayZero ();
    void displayPoint ();
    void back ();

  private:

    QGridLayout *layout;

};

#endif











