#include <qtextview.h>

#include "todoshow.h"

namespace Todo {

    class TextViewShow : public QTextView, public TodoShow {
    public:
        TextViewShow( QWidget* parent, MainWindow* );
        ~TextViewShow();

        QString type()const;
        void slotShow( const OTodo& ev );
        QWidget* widget();

    protected:
        void keyPressEvent( QKeyEvent* );

    };
};
