SOURCES += \
        backend/ocontactaccessbackend_vcard.cpp \
        backend/ocontactaccessbackend_xml.cpp   \
        backend/odatebookaccessbackend.cpp      \
        backend/odatebookaccessbackend_xml.cpp  \
        backend/otodoaccessbackend.cpp          \
        backend/otodoaccessvcal.cpp             \
        backend/otodoaccessxml.cpp              
        
HEADERS += \
        backend/obackendfactory.h              \
        backend/ocontactaccessbackend.h        \
        backend/ocontactaccessbackend_vcard.h  \
        backend/ocontactaccessbackend_xml.h    \
        backend/odatebookaccessbackend.h       \
        backend/odatebookaccessbackend_xml.h   \
        backend/opimaccessbackend.h            \        
        backend/otodoaccessbackend.h           \
        backend/otodoaccessvcal.h              \
        backend/otodoaccessxml.h               

contains( ENABLE_SQL_PIM_BACKEND, y ) {
	message ( Enabling the SQL Backend for libopiepim2 )
	DEFINES += __USE_SQL
	LIBS    += -lopiedb2
	HEADERS += backend/otodoaccesssql.h             \
                   backend/ocontactaccessbackend_sql.h  \
                   backend/odatebookaccessbackend_sql.h 
	SOURCES += backend/otodoaccesssql.cpp            \
		   backend/ocontactaccessbackend_sql.cpp \
		   backend/odatebookaccessbackend_sql.cpp
}

!contains( ENABLE_SQL_PIM_BACKEND, y ) {
	message ( No SQL Backend in libopiepim2 )
}

