#ifndef __OPIEAPPINTERFACE_H
#define __OPIEAPPINTERFACE_H

#include <qlist.h>
#include <qpe/qcom.h>

class QWidget;

#ifndef QT_NO_COMPONENT
// {16556BFC-891D-46A9-BE59-DBa158C3A266}
#ifndef IID_OAppInterface
#define IID_OAppInterface QUuid( 0x16556bfc, 0x891d, 0x46a9, 0xbe, 0x59, 0xdb, 0xa1, 0x58, 0xc3, 0xa2, 0x66)
#endif
#endif

enum OAppPos { leftPos, midPos, rightPos };
 
struct OAppInterface : public QUnknownInterface
{
    virtual QList<QWidget> widgets() = 0;
    virtual OAppPos position() const = 0;
};

#endif // __OPIEAPPINTERFACE_H
