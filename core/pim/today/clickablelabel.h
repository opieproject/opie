#ifndef CLICKABLELABEL
#define CLICKABLELABEL

#include <qlabel.h> 

class ClickableLabel: public QLabel 
{
  Q_OBJECT
public:
    ClickableLabel(QWidget* parent = 0, const char* name = 0, 
		   WFlags fl = 0);
 protected:
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
 signals:
    void clicked();
};

#endif
