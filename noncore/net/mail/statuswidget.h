#ifndef STATUS_WIDGET_H
#define STATUS_WIDGET_H

#include <qwidget.h>
#include "statuswidgetui.h"

class StatusWidget : public StatusWidgetUI {

Q_OBJECT

public:
    StatusWidget( QWidget* parent = 0, const char* name = 0,WFlags fl = 0 );
    ~StatusWidget();
    

public slots:

    void setText( const QString& text );
    void setProgress( int progress );
};


#endif

