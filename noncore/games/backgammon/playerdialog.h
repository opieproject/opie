#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include <qradiobutton.h>
#include <qdialog.h>

class PlayerDialog : public QDialog
{
    Q_OBJECT
private:
    bool auto1;
    bool auto2;
	QRadioButton* manual_button1;
	QRadioButton* auto_button1;
	QRadioButton* manual_button2;
	QRadioButton* auto_button2;
public:
    PlayerDialog(QWidget* parent=0,const char* name=0,bool modal=TRUE,WFlags f=0);
	~PlayerDialog();
private slots:
    void button_manual1();
	void button_auto1();
	void button_manual2();
	void button_auto2();
private:
    void button1_state(bool computer);
    void button2_state(bool computer);
public:
    void setAuto1(bool newstate);
	bool getAuto1();
	void setAuto2(bool newstate);
	bool getAuto2();
};

#endif //PLAYERDIALOG_H
