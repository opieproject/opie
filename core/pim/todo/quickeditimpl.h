#ifndef OPIE_QUICK_EDIT_IMPL_H
#define OPIE_QUICK_EDIT_IMPL_H

#include <qpe/qpetoolbar.h>

#include "quickedit.h"

class QLineEdit;
class QLabel;

class QuickEditImpl : public QPEToolBar, public Todo::QuickEdit {
    Q_OBJECT
public:
    QuickEditImpl( QWidget* parent,  bool visible);
    ~QuickEditImpl();
    OTodo todo()const;
    QWidget* widget();
private slots:
    void slotEnter();
    void slotPrio();
    void slotMore();
    void slotCancel();
private:
    void reinit();
    int m_state;
    QLabel* m_lbl;
    QLineEdit* m_edit;
    QLabel* m_enter;
    QLabel* m_more;
    QPopupMenu* m_menu;
    OTodo m_todo;
    bool m_visible;
};

#endif
