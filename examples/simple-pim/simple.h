
/*
 * A Simple widget with a button to quit
 *
 */

/*
 * The below sequence is called a guard and guards
 * against multiple inclusion of header files
 * NOTE: you need to use unique names among the header files
 */
#ifndef QUIET_SIMPLE_DEMO_H
#define QUIET_SIMPLE_DEMO_H




#include <qmainwindow.h>  // from this class we will inherit
#include <qlistview.h> // A ListView for our PIM records

#include <opie2/otodoaccess.h>
#include <opie2/odatebookaccess.h>

class QPushButton; // forward declaration to not include the header. This can save time when compiling
class QAction;
class PIMListView;
class QDate;
class QCopChannel;
namespace Opie{
namespace Ui  {
class OWait;
class OTabWidget;
}
}

/*
 * A mainwindow is a special QWidget it helps layouting
 * toolbar, statusbar, menubar. Got dockable areas
 * So in one sentence it is a MainWindow :)
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    static QString appName() { return QString::fromLatin1("simple-pim"); }
    MainWindow( QWidget* parent, const char* name,  WFlags fl );
    ~MainWindow();

public slots:
    void setDocument( const QString& );
private slots:
    void slotDesktopReceive( const QCString&, const QByteArray& );
    void slotLoad();
    void slotLoadForDay(int, int, int );
    void slotLoadForDay(const QDate&);
    void slotShow();
    void slotDate();
    void slotShowRecord( const Opie::OPimRecord& );

private:
    void initUI();
    QAction *m_fire;
    QAction *m_dateAction;
    Opie::Ui::OTabWidget* m_tab;

    Opie::OPimTodoAccess     m_tb;
    Opie::ODateBookAccess m_db;
    PIMListView    *m_todoView;
    PIMListView    *m_dateView;

    int m_synced; // a counter for synced objects..
    QCopChannel    *m_desktopChannel;
    Opie::Ui::OWait          *m_loading;
};

/*
 * Instead of the simple QWidgets we will design
 * a new widget based on a QListView
 * it should show either Todos or EffectiveEvents
 */
class PIMListView :  public QListView {
    Q_OBJECT
public:
    PIMListView( QWidget* parent, const char* name, WFlags fl= 0 );
    ~PIMListView();


    void set( Opie::OPimTodoAccess::List );
    void set( const Opie::OEffectiveEvent::ValueList& );
    void showCurrentRecord();

signals:
    void showRecord( const Opie::OPimRecord& );

private:
    static QString makeString( const Opie::OEffectiveEvent& ev );

};

#endif
