/*
 * May be used, copied and modified wihtout any limitation 
 */

#ifndef OSPlitter_EXAMPLE_H
#define OSPlitter_EXAMPLE_H

#include <qvbox.h>
#include <opie/ofileselector.h>
 
class OSplitterExample : public QWidget {
    Q_OBJECT
public:
    static QString appName() { return QString::fromLatin1("osplitter_example"); }
    OSplitterExample( QWidget *parent, const char* name, WFlags fl );

};


#endif
