SOURCES += \
        core/backends/ocontactaccessbackend_vcard.cpp \
        core/backends/ocontactaccessbackend_xml.cpp \
        core/backends/ocontactaccess.cpp \
        core/backends/odatebookaccessbackend.cpp \
        core/backends/odatebookaccessbackend_xml.cpp \
        core/backends/otodoaccessbackend.cpp \
        core/backends/otodoaccess.cpp \
        core/backends/otodoaccessvcal.cpp \
        core/backends/otodoaccessxml.cpp \
        core/backends/odatebookaccess.cpp
        
HEADERS += \
        core/backends/obackendfactory.h \
        core/backends/ocontactaccessbackend.h \
        core/backends/ocontactaccessbackend_vcard.h \
        core/backends/ocontactaccessbackend_xml.h \
        core/backends/ocontactaccess.h \
        core/backends/odatebookaccessbackend.h \
        core/backends/odatebookaccessbackend_xml.h \
        core/backends/opimaccessbackend.h \
        core/backends/opimaccesstemplate.h \
        core/backends/otodoaccessbackend.h \
        core/backends/otodoaccess.h \
        core/backends/otodoaccessvcal.h \
        core/backends/otodoaccessxml.h \
        core/backends/odatebookaccess.h 

contains( ENABLE_SQL_PIM_BACKEND, y ) {
	message ( Enabling the SQL Backend for libopiepim2 )
	DEFINES += __USE_SQL
	LIBS    += -lopiedb2
	HEADERS += core/backends/otodoaccesssql.h \
                   core/backends/ocontactaccessbackend_sql.h \
                   core/backends/odatebookaccessbackend_sql.h
	SOURCES += core/backends/otodoaccesssql.cpp \
		   core/backends/ocontactaccessbackend_sql.cpp \
		   core/backends/odatebookaccessbackend_sql.cpp
}

!contains( ENABLE_SQL_PIM_BACKEND, y ) {
	message ( No SQL Backend in libopiepim2 )
}

