
#ifndef ATCONFIGDIALOG_H
#define ATCONFIGDIALOG_H

#include <qdialog.h>
#include "profile.h"

class QLineEdit;
class QSpinBox;
class QComboBox;

class ATConfigDialog : public QDialog {

    Q_OBJECT

public:
    ATConfigDialog( QWidget* parent = 0,  const char* name = 0, bool modal = FALSE, WFlags fl = 0 );

    ~ATConfigDialog();

    void readConfig( const Profile& );
    void writeConfig( Profile& );

private:
    QWidget *tab0( QWidget* parent);
    QWidget *tab1( QWidget* parent);

private:
    QLineEdit *initStringLine;
    QLineEdit *resetStringLine;
    QLineEdit *dialPref1Line;
    QLineEdit *dialSuf1Line;
    QLineEdit *dialPref2Line;
    QLineEdit *dialSuf2Line;
    QLineEdit *dialPref3Line;
    QLineEdit *dialSuf3Line;
    QLineEdit *connectLine;
    QLineEdit *hangupLine;
    QSpinBox *dialTimeSpin;
    QSpinBox *delayRedialSpin;
    QSpinBox *numberTriesSpin;
    QSpinBox *dtrDropTimeSpin;
    QComboBox *bpsDetectBox;
    QComboBox *dcdLinesBox;
    QComboBox *multiLineUntagBox;

};


#endif
