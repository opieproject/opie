/****************************************************************************
** Form interface generated from reading ui file 'powerchordbase.ui'
**
** Created: Sun Jan 13 23:04:45 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef POWERCHORDBASE_H
#define POWERCHORDBASE_H

#include <qvariant.h>
#include <qwidget.h>
#include <qpixmap.h>

#include "gs.h"
#include "gt.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class FretBoard;
class QComboBox;
class QFrame;
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;
class QSpinBox;
class QTabWidget;
class VUMeter;

class PowerchordBase : public QWidget
{ 
    Q_OBJECT

public:
    PowerchordBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PowerchordBase();

    QTabWidget* tabs;
    QWidget* tab;
    QComboBox* chordkey;
    QComboBox* chordfret;
    QComboBox* chordnote;
    QLabel* s1_1;
    QLabel* s1_2;
    QLabel* s1_3;
    QLabel* s1_4;
    QLabel* s1_5;
    QLabel* s1_6;
    QLabel* sound_label;
    QFrame* Frame6;
    QLabel* chordshow_label;
    QPushButton* transport_rec;
    QPushButton* play_sound;
    QFrame* Frame6_2;
    QPushButton* transport_play;
    QPushButton* transport_rew;
    QPushButton* transport_fwd;
    QLabel* chordname;
    FretBoard* frets;
    gs* synth;
    gt* tuner;
    QWidget* tab_2;
    QLabel* optlab2;
    QComboBox* span;
    QLabel* TextLabel1;
    QComboBox* tuning;
    QLabel* optlab1;
    QWidget* Tuner;
    QFrame* Frame4;
    VUMeter* vu;
    QComboBox* tuner_note;
    QFrame* Frame8;
    QPushButton* tuner_start;
    QLabel* tuner_lab1;
    QLabel* tuner_lab2;
    QComboBox* tuner_calib_note;
    QSpinBox* tuner_calib_freq;
    QLabel* tuner_lab3;
    QLabel* tuner_pic1;
    QWidget* tab_3;
    QLabel* chordlistlab1;
    QListBox* chordlist;
    QPushButton* list_remove_btn;

public slots:
      virtual void change_handler();

 void transport_rec_cb(); 
 void list_remove_cb();
 void play_chord_cb();
 void tuner_start_cb();
 void tuner_simulation_cb();
 void tuner_cb();
 void audio_cb();

 signals:
 void frequency_change(int);

protected:
 QPixmap image_open;
 QPixmap image6;
 QTimer *simulation_timer;
 QTimer *audio_timer;
 int simulation_iter;
 int simulation_v;
 int simulation_x;
 QHBoxLayout* Layout1;
};

#endif // POWERCHORDBASE_H
