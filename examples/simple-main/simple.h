
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


class QPushButton; // forward declaration to not include the header. This can save time when compiling
class QAction;

/*
 * A mainwindow is a special QWidget it helps layouting
 * toolbar, statusbar, menubar. Got dockable areas
 * So in one sentence it is a MainWindow :)
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    static QString appName() { return QString::fromLatin1("simple-main"); }
    MainWindow( QWidget* parent, const char* name,  WFlags fl );
    ~MainWindow();

public slots:
    void setDocument( const QString& );

private:
    void initUI();
    QAction *m_fire;
};


/*
 * Simple inherits from QWidget
 */
class Simple : public QWidget {
    /*
     * Q_OBJECT must always be the first thing you include
     * This is a macro and is responsible for the concepts of
     * dynamic signal and slots and other MetaObjects as
     * superClass(), inherits(), isA()...
     * If you use multiple inheritance include the class derived
     * from QObject first
     */
    Q_OBJECT
public:
    /*
     * C'tor for the Simple
     * make sure to always have these three when you use
     * the quicklaunch factory ( explained in the implementation )
     */
    Simple( QWidget* parent = 0, const char * name = 0,  WFlags fl = 0 );
    ~Simple();

    /*
     * We now make it public because our mainwindow wants to call it
     */
public slots:
    void slotFire();

private:
    /* my variable */
    QPushButton* m_button;
};




#endif
