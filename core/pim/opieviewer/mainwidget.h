#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <qwidget.h>

class QVBoxLayout;

namespace Viewer {
    class RichTextView;
    class MainListView;

    class MainWidget : public QWidget {
    public:
        MainWidget(QWidget* parent, const char* name = 0 );
        ~MainWidget();
        RichTextView *textView()const;
        MainListView* listView()const;

    private:
        RichTextView* m_textView;
        MainListView* m_mainView;
        QVBoxLayout* m_layout;


    };

};

#endif
