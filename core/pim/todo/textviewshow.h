#include <qtextview.h>

#include "todoshow.h"

namespace Todo {

    class TextViewShow : public QTextView, public TodoShow {
    public:
        TextViewShow( QWidget* parent );
        ~TextViewShow();

        QString type()const;
        void slotShow( const ToDoEvent& ev );
        QWidget* widget();

    };
};
