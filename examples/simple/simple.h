
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




#include <qwidget.h>  // from this class we will inherit

class QPushButton; // forward declaration to not include the header. This can save time when compiling


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
     * appName is used by the Application factory.
     * make sure the name matches the one of the executable
     */
    static QString appName() { return QString::fromLatin1("simple"); }

    /*
     * use private slots: to mark your slots as such
     * A slot can also be called as a normal method
     */
private slots:
    void slotQuit();

private:
    /* my variable */
    QPushButton* m_button;
};




#endif
