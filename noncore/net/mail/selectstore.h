#ifndef _SELECTSTORE_H
#define _SELECTSTORE_H

#include "selectstoreui.h"

class Selectstore:public selectstoreui
{
    Q_OBJECT
public:
    Selectstore(QWidget* parent = 0, const char* name = 0);
    virtual ~Selectstore();

protected:
    virtual void slotCreateNewFolder();
    virtual void slotMoveMail();
protected slots:

};
#endif
