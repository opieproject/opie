
#ifndef MAIN_RICHTEXTVIEW_H
#define MAIN_RICHTEXTVIEW_H

#include <qtextview.h>

namespace Viewer{
    class RichTextView : public QTextView {
        Q_OBJECT
    public:
        RichTextView( QWidget* parent,  const char* name=0 );
        ~RichTextView();
    public slots:
        void slotShow(const QString& appname, const QString& );
    };
}


#endif
