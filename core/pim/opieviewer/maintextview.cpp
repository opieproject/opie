
#include "maintextview.h"

using namespace Viewer;

RichTextView::RichTextView( QWidget* parent, const char* name )
    : QTextView( parent, name ) {

    setMinimumHeight( 100 );
    setMaximumHeight( 100 );
}
RichTextView::~RichTextView() {

}
void RichTextView::slotShow( const QString& app, const QString& showit ) {
    setText( showit );
}
