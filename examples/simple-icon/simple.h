
/*
 * A Simple widget with a button to quit
 * and Pixmaps and Sound
 */

/*
 * The below sequence is called a guard and guards
 * against multiple inclusion of header files
 */
#ifndef QUIET_SIMPLE_DEMO_H
#define QUIET_SIMPLE_DEMO_H




#include <qwidget.h>  // from this class we will inherit

class QPushButton; // forward declaration to not include the header. This can save time when compiling


/*
 * Simple inherits from QWidget
 * Remeber each example is based on the other
 */
class Simple : public QWidget {
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
    static QString appName() { return QString::fromLatin1("simple-icon"); }

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
