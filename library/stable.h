 /* Add C includes here */

#if defined __cplusplus

#include <stdlib.h>
#include <iostream>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qimage.h>
#include <qlayout.h>

#define INCLUDE_MENUITEM_DEF
#include <qpopupmenu.h>
#undef INCLUDE_MENUITEM_DEF


#define QTOPIA_INTERNAL_LANGLIST
#define QTOPIA_INTERNAL_INITAPP
#define QTOPIA_INTERNAL_PRELOADACCESS
#include <qtopia/qpeapplication.h>
#include <qtopia/global.h>

#define QTOPIA_INTERNAL_MIMEEXT
#define QTOPIA_INTERNAL_PRELOADACCESS
#define QTOPIA_INTERNAL_APPLNKASSIGN
#include <qtopia/applnk.h>
#undef  QTOPIA_INTERNAL_PRELOADACCESS
#undef  QTOPIA_INTERNAL_MIMEEXT
#undef  QTOPIA_INTERNAL_APPLNKASSIGN

#include "categoryedit_p.h"
#include <qtopia/categorymenu.h>
#include <qtopia/categoryselect.h>
#include <qtopia/categorywidget.h>

#define QTOPIA_INTERNAL_LANGLIST
#include <qtopia/config.h>
#undef  QTOPIA_INTERNAL_LANGLIST

#include <qtopia/datebookdb.h>
#include <qtopia/datebookmonth.h>

#define QTOPIA_INTERNAL_TZSELECT_INC_LOCAL
#include <qtopia/tzselect.h>
#undef  QTOPIA_INTERNAL_TZSELECT_INC_LOCAL

#include <qtopia/resource.h>
#include <qtopia/version.h>
#include <qtopia/storage.h>
#include <qtopia/qpestyle.h>

#include <qtopia/alarmserver.h>
#include <qtopia/calendar.h>
#include <qtopia/fileselector.h>
#include <qtopia/filemanager.h>
#include <qtopia/qcom.h>
#include <qtopia/qcopenvelope_qws.h>
#include <qtopia/qpemessagebox.h>


#undef QTOPIA_INTERAL_PRELOADACCESS
#undef QTOPIA_INTERNAL_INITAPP
#undef QTOPIA_INTERNAL_LANGLIST

#include <qtopia/xmlreader.h>
#include <qtopia/windowdecorationinterface.h>

#define QTOPIA_INTERNAL_TZSELECT_INC_LOCAL
#include <qtopia/tzselect.h>
#undef  QTOPIA_INTERNAL_TZSELECT_INC_LOCAL

#include <qtopia/applicationinterface.h>
#include <qtopia/qpeglobal.h>

#endif
