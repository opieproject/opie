#ifndef QT3NAMESPACE_H
#define QT3NAMESPACE_H

#include <qnamespace.h>

#define Q_ASSERT ASSERT
#define Q_WS_QWS

#define QMemArray QArray
#define QPtrList QList
#define QPtrListIterator QListIterator
#define QPtrVector QVector

namespace Qt3 {

enum NewAlignmentFlags {
    AlignAuto = 0x0000,
    AlignJustify = 0x0080,
    AlignHorizontal_Mask = Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter | AlignJustify
};

enum NewWidgetFlags {
    WStaticContents = Qt::WNorthWestGravity
};

}

#endif // QT3NAMESPACE_H
