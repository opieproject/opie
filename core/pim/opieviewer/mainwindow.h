
#ifndef VIEWER_MAINWINDOW_H
#define VIEWER_MAINWINDOW_H

#include <qbitarray.h>
#include <qmainwindow.h>

class QAction;
class QVBoxLayout;

class Contact;
class ToDoEvent;

namespace Viewer {
    class MainWidget;

    class MainWindow : public QMainWindow {
        Q_OBJECT
        enum Bits { ShowAddress=0, ShowTodo=1, ShowDates=2 };
    public:
        MainWindow( QWidget* parent = 0,  const char* name = 0 );
        ~MainWindow();

    signals:
        void richText( int id, const QString& );
    private:
        void initGUI();
        void doReload();
        QString smallAbText( const Contact& );
        QString smallTodoText( const ToDoEvent& );
        MainWidget* m_main;
        QPopupMenu* m_context;
        QPopupMenu* m_file;
        QAction* m_reload;
        QBitArray m_field; // resize it
        QAction *m_ab;
        QAction *m_todo;
        QAction *m_date;

    private slots:
        void showAddresses( bool show );
        void showTodos( bool show );
        void showDates( bool show );
        void toggleRichText(bool);
        void reload();

        void closeMe();
        void slotShow(const QString& app, int id);
        void slotEdit(const QString& app, int id);
        void slotNew( const QString& app);

    };
}

#endif
