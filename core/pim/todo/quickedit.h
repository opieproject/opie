#ifndef OPIE_QUICK_EDIT_H
#define OPIE_QUICK_EDIT_H

#include <qsignal.h>
#include <qwidget.h>

#include <opie/otodo.h>

namespace Todo{
    class MainWindow;
    struct QuickEditBase {
        virtual OTodo todo()const = 0l;
        virtual QSignal* signal() = 0l;
        virtual QWidget* widget() = 0l;
    };
    /*
     * this is my second try
     * of signal and slots with namespaces
     * and templates
     * I use a different approach now
     * I give a QSignal away
     * and have a protected method called emit
     */
    /**
     * Quick edit is meant to quickly enter
     * OTodos in a fast way
     */
    class QuickEdit : public QuickEditBase{
    public:
        QuickEdit(MainWindow* main );
        virtual ~QuickEdit();
        //OTodo todo()const;
        QSignal* signal();
        //QWidget* widget();
    protected:
        MainWindow* mainWindow();
        void commit();
    private:
        MainWindow* m_main;
        QSignal* m_sig;
    };
};


#endif
