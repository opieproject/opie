#ifndef DCCTRANSFERTAB_H
#define DCCTRANSFERTAB_H

#include "dcctransfer.h"
#include "irctab.h"

template <class T> class QList;

class DCCProgress;
class IRCSession;
class QString;
class QHBox;
class MainWindow;

class DCCTransferTab: public IRCTab {
    Q_OBJECT
public:

    DCCTransferTab(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    ~DCCTransferTab();
    virtual QString title();
    virtual IRCSession* session(){return 0;};
    virtual void appendText(QString){};
    virtual void remove();
    virtual void settingsChanged() {};
    void addTransfer(DCCTransfer::Type type, Q_UINT32 ip4Addr,
            Q_UINT16 port, const QString &filename,
            const QString &nickname, unsigned int size);
    static bool confirm(QWidget *parent = 0,
            const QString &nickname = QString::null,
            const QString &filename = QString::null,
            unsigned int size = 0);
private:
    QHBox *m_hbox;
    QList <DCCProgress> m_progressList;
    MainWindow *m_parent;

};

#endif

