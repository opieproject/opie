
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
    static QString appName() { return QString::fromLatin1("main-tab"); }
    MainWindow( QWidget* parent, const char* name,  WFlags fl );
    ~MainWindow();

public slots:
    void setDocument( const QString& );
private slots:
    void slotCurrentChanged( QWidget* wid );

private:
    void initUI();
    QAction *m_fire;
    QWidget* m_oldCurrent;
};


/*
 * We will just reuse the two simple widgets for now
 */
class Simple1 : public QWidget {

    Q_OBJECT
public:

    Simple1( QWidget* parent = 0, const char * name = 0,  WFlags fl = 0 );
    ~Simple1();


public slots:
    void slotFire();

private:
    /* my variable */
    QPushButton* m_button;
};

class Simple2 : public QWidget {
    Q_OBJECT
public:

    Simple2( QWidget* parent = 0, const char * name = 0,  WFlags fl = 0 );
    ~Simple2();



public slots:
    void slotFire();

private:
    /* my variable */
    QPushButton* m_button;
};



#endif
