#include <qpe/qlibrary.h>

#include "osqldriver.h"

OSQLDriver::OSQLDriver( QLibrary* lib )
: QObject(), m_lib(lib) {
}
OSQLDriver::~OSQLDriver() {
    delete m_lib;
}
bool OSQLDriver::sync() {
    return true;
}
