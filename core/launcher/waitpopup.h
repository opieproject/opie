#ifndef WAITPOPUP_H
#define WAITPOPUP_H

#include <qdialog.h>

class WaitPopup : public QDialog {
    Q_OBJECT

public:
    WaitPopup(QWidget *parent=0,const char* msg=0,bool dispIcon=TRUE);
    ~WaitPopup();
};

#endif
