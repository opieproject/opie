
#ifndef ATCONFIGDIALOG_H
#define ATCONFIGDIALOG_H

#include <qdialog.h>
#include "profile.h"

#define MODEM_DEFAULT_INIT_STRING  "~^M~ATZ^M~"
#define MODEM_DEFAULT_RESET_STRING "~^M~ATZ^M~"
#define MODEM_DEFAULT_DIAL_PREFIX1 "ATDT"
#define MODEM_DEFAULT_DIAL_SUFFIX1 "^M"
#define MODEM_DEFAULT_DIAL_PREFIX2 "ATDP"
#define MODEM_DEFAULT_DIAL_SUFFIX2 "^M"
#define MODEM_DEFAULT_DIAL_PREFIX3 "ATX1DT"
#define MODEM_DEFAULT_DIAL_SUFFIX3 ";X4D^M"
#define MODEM_DEFAULT_CONNECT_STRING "CONNECT"
#define MODEM_DEFAULT_HANGUP_STRING "~~+++~~ATH^M"
#define MODEM_DEFAULT_CANCEL_STRING "^M"
#define MODEM_DEFAULT_DIAL_TIME     45
#define MODEM_DEFAULT_DELAY_REDIAL  2
#define MODEM_DEFAULT_NUMBER_TRIES  10
#define MODEM_DEFAULT_DTR_DROP_TIME 1
#define MODEM_DEFAULT_BPS_DETECT    0   // bool
#define MODEM_DEFAULT_DCD_LINES     1   //bool
#define MODEM_DEFAULT_MULTI_LINE_UNTAG 0 // bool

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
