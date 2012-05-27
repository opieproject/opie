#ifndef __ADDDLG_H__
#define __ADDDLG_H__

#include <qwidget.h>
#include <qtimer.h>
#include <qlabel.h>

class AddDlg : public QWidget {
    Q_OBJECT

public:
    AddDlg( QWidget* parent = 0, const char* name = 0 );
    ~AddDlg();

signals:
    void closed( int keycode );

protected:
    virtual void keyPressEvent ( QKeyEvent *e );
    virtual void keyReleaseEvent ( QKeyEvent *e );
    virtual void closeEvent ( QCloseEvent *e );

private slots:
    void keyTimeout();

private:
    QTimer *m_timer;
    QLabel *m_keyLabel;
    QLabel *m_instrLabel;
    int m_last_key;
    bool m_closing;

};

#endif
