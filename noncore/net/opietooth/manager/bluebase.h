
#ifndef BLUEBASE_H
#define BLUEBASE_H

#include <qvariant.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qsplitter.h>

#include "bluetoothbase.h"

class QVBox;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QPushButton;
class QTabWidget;
class QCheckBox;

class BlueBase : public BluetoothBase {
    Q_OBJECT

public:
    BlueBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~BlueBase();

protected:


        private slots:
    void startScan();


};



#endif
