#ifndef CLICKABLELABEL
#define CLICKABLELABEL

#include <qlabel.h> 

class OClickableLabel: public QLabel 
{
  Q_OBJECT
public:
    OClickableLabel(QWidget* parent = 0, const char* name = 0, 
		   WFlags fl = 0);
    void setToggleButton(bool t);
 protected:
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
 public slots:
    void setOn(bool on);
 signals:
    void clicked();
    void toggled(bool on);
 private:
    bool isToggle;
    bool isDown;
    void showState(bool on);
    bool textInverted;
    void setInverted(bool on);
};

#endif
