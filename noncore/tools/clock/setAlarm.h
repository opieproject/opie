 /***************************************************************************
//                            setAlarm.h  -  description
//                               -------------------
//       Created: Wed Mar 13 19:47:24 2002
//      copyright            : (C) 2002 by ljp
//      email                : ljp@llornkcor.com
// 
***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SET_ALARM_H
#define SET_ALARM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLCDNumber;
class QLabel;
class QRadioButton;
class QSlider;
class QButtonGroup;
class QCheckBox;
class Set_Alarm : public QDialog
{ 
    Q_OBJECT

public:
    Set_Alarm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Set_Alarm();

    QLabel *TextLabel1, *TextLabel2, *TextLabel3;
    QSlider *Hour_Slider, *Minute_Slider, *SnoozeSlider;
    QLCDNumber *Hour_LCDNumber, *Minute_LCDNumber, *Snooze_LCDNumber;

    QRadioButton* Am_RadioButton;

    QRadioButton* Pm_RadioButton;
    QButtonGroup *ButtonGroup1;
    QCheckBox *useMp3Check;
protected slots:
        void slotChangemp3CkeckBox(bool);   
    void slotChangeHour(int);
    void slotChangeMinute(int);
    void slotChangeSnooze(int);
    void amButtonToggled(bool);
    void pmButtonToggled(bool);
    void cleanUp();    
protected:
    QGridLayout* Set_AlarmLayout;
};

#endif // SET_ALARM_H
