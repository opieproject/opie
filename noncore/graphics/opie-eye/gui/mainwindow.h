/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_MAIN_WINDOW_H
#define PHUNK_MAIN_WINDOW_H

#include <qmainwindow.h>

#include <qpe/config.h>

class PIconView;
class PMainWindow : public QMainWindow {
    Q_OBJECT
public:
    static QString appName() { return QString::fromLatin1("opie-eye" ); }
    PMainWindow(QWidget*, const char*, WFlags );
    ~PMainWindow();

private:
    Config m_cfg;
    PIconView* m_view;

private slots:
    void slotConfig();
};

#endif
