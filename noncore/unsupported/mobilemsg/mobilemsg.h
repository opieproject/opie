#ifndef MOBILEMSG_H
#define MOBILEMSG_H
#include "mobilemsgbase.h"

class MobileMsg : public MobileMsgBase
{ 
    Q_OBJECT

public:
    MobileMsg( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MobileMsg();

private slots:
    void goodBye();
};

#endif // MOBILEMSG_H
