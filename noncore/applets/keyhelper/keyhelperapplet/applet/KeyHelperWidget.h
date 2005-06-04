#ifndef _KEY_HELPER_WIDGET_H_
#define _KEY_HELPER_WIDGET_H_

#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qaccel.h>
#include <opie2/otaskbarapplet.h>
#include <opie2/okeyfilter.h>
#include <opie2/oresource.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/global.h>
#include "KeyHelper.h"
#include "AppLnkManager.h"
#include "ConfigEx.h"

#include <syslog.h>

Q_EXPORT void MsgHandler(QtMsgType type, const char* msg);

class KeyHelperWidget : public QLabel
{
    Q_OBJECT
public:
    KeyHelperWidget(QWidget* parent = 0, const char* name=0);
    ~KeyHelperWidget();
    static int position();

    virtual bool eventFilter(QObject* o, QEvent* e);

public slots:
    //void windowEvent(QWSWindow* w, QWSServer::WindowEvent e);
protected:
    QCopChannel* m_pChannel;
    QCopChannel* m_pSysChannel;
    QPixmap disabled;

protected slots:
    void receiveMessage(const QCString& msg, const QByteArray& data);
    void sysMessage(const QCString& msg, const QByteArray& data);
    void reload();
    void set();
    void unset();
    void init();
    void mouseReleaseEvent(QMouseEvent*);

private:
    void loadUseFilterApps();
    void enable();
    void disable();
    void pause();
    void restart();
    void version();
    void setDebugLevel(int level);
    void initDebugLevel();

    bool m_reset;
    bool m_useFilter;
    bool m_status;
    QString m_xmlfile;
    msg_handler m_defHandler;
    bool m_enable;
    bool m_saved;
    QStringList m_apps;
    KeyHelper* m_pHelper;
private slots:
    void doReload(bool showstatus=true);
    void doEvent(int,int,int,int,int);
    void doEvent(const QString&, int);
};

#endif /* _KEY_HELPER_WIDGET_H_ */
