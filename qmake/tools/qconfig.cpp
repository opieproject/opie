/* Install paths from configure */

static const char QT_INSTALL_PREFIX [256] = "/usr/src/coding/projects/userspace/qt-embedded-free-3.1.0-b2";
static const char QT_INSTALL_BINS   [256] = "/usr/src/coding/projects/userspace/qt-embedded-free-3.1.0-b2/bin";
static const char QT_INSTALL_DOCS   [256] = "/usr/src/coding/projects/userspace/qt-embedded-free-3.1.0-b2/doc";
static const char QT_INSTALL_HEADERS[256] = "/usr/src/coding/projects/userspace/qt-embedded-free-3.1.0-b2/include";
static const char QT_INSTALL_LIBS   [256] = "/usr/src/coding/projects/userspace/qt-embedded-free-3.1.0-b2/lib";
static const char QT_INSTALL_PLUGINS[256] = "/usr/src/coding/projects/userspace/qt-embedded-free-3.1.0-b2/plugins";
static const char QT_INSTALL_DATA   [256] = "/usr/src/coding/projects/userspace/qt-embedded-free-3.1.0-b2";

const char *qInstallPath()        { return QT_INSTALL_PREFIX;  }
const char *qInstallPathDocs()    { return QT_INSTALL_DOCS;    }
const char *qInstallPathHeaders() { return QT_INSTALL_HEADERS; }
const char *qInstallPathLibs()    { return QT_INSTALL_LIBS;    }
const char *qInstallPathBins()    { return QT_INSTALL_BINS;    }
const char *qInstallPathPlugins() { return QT_INSTALL_PLUGINS; }
const char *qInstallPathData()    { return QT_INSTALL_DATA;    }
