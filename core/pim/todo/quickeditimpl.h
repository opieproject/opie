#ifndef OPIE_QUICK_EDIT_IMPL_H
#define OPIE_QUICK_EDIT_IMPL_H

#include <qhbox.h>

#include "quickedit.h"

class QLineEdit;
class QLabel;

class QuickEditImpl : public QHBox, public Todo::QuickEdit {
    Q_OBJECT
public:
    QuickEditImpl( Todo::MainWindow* win ,  QWidget* parent);
    ~QuickEditImpl();
    OTodo todo()const;
    QWidget* widget();
    QSize sizeHint()const;
private slots:
    void slotEnter();
    void slotPrio();
    void slotMore();
private:
    void reinit();
    int m_state;
    QLabel* m_lbl;
    QLineEdit* m_edit;
    QLabel* m_enter;
    QLabel* m_more;
    QPopupMenu* m_menu;
    OTodo m_todo;
};

#endif
