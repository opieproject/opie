#ifndef NAME_SPACE_HACK_H
#define NAME_SPACE_HACK_H

#include <opie2/otimepicker.h>
#include <opie2/oclickablelabel.h>

class TimePicker : public Opie::Ui::OTimePicker {
public:
    TimePicker( QWidget* p = 0, const char* name = 0, WFlags fl = 0 )
      : Opie::Ui::OTimePicker(p,name,fl){}
    
};
class ClickableLabel : public Opie::Ui::OClickableLabel {
public:
    ClickableLabel(QWidget *p=0,const char* name=0, WFlags fl = 0 )
      : Opie::Ui::OClickableLabel(p,name,fl){}
};


#endif
