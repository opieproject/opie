#ifndef _KEY_HELPER_APPLET_H_
#define _KEY_HELPER_APPLET_H_

#if 0
#include <qpe/taskbarappletinterface.h>
#include "KeyHelperWidget.h"

class KeyHelperApplet : public TaskbarAppletInterface
{
public:
    KeyHelperApplet();
    virtual ~KeyHelperApplet();

    QRESULT queryInterface(const QUuid&, QUnknownInterface**);
    Q_REFCOUNT

    virtual QWidget* applet(QWidget* parent);
    virtual int position() const;
private:
    KeyHelperWidget* widget;
    ulong ref;
};

#endif

#endif /* _KEY_HELPER_APPLET_H_ */
