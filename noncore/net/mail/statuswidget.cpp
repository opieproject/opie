#include <qtimer.h>
#include <qprogressbar.h>
#include <qlabel.h>

#include "statuswidget.h"

// the current problem I see is "locking": used exclusive by one sender


StatusWidget::StatusWidget( QWidget* parent, const char* name,WFlags fl )
    : StatusWidgetUI( parent, name, fl ) {

        setMaximumHeight( 15 );
        // hide for now since nothing reports decent progress data yet.
        statusProgress->hide();
    }

StatusWidget::~StatusWidget() {
}

void StatusWidget::setText( const QString& text ) {
    show();
    statusText->setText(  text );
    QTimer::singleShot( 5000, this, SLOT( hide() ) );
}

void StatusWidget::setProgress( int progress ) {
    show();
    statusProgress->setProgress( progress );
    if ( progress == 100 ) {
        hide();
    }
}
