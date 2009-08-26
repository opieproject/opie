SOURCES += \
        backend/ocontactaccessbackend.cpp       \
        backend/ocontactaccessbackend_vcard.cpp \
        backend/ocontactaccessbackend_xml.cpp   \
        backend/odatebookaccessbackend.cpp      \
        backend/odatebookaccessbackend_xml.cpp  \
        backend/odatebookaccessbackend_vcal.cpp \
        backend/opimbackendoccurrence.cpp       \
        backend/otodoaccessbackend.cpp          \
        backend/otodoaccessvcal.cpp             \
        backend/otodoaccessxml.cpp              \
        backend/omemoaccessbackend.cpp          \
        backend/omemoaccessbackend_text.cpp     \
        backend/opimio.cpp
        
HEADERS += \
        backend/obackendfactory.h              \
        backend/ocontactaccessbackend.h        \
        backend/ocontactaccessbackend_vcard.h  \
        backend/ocontactaccessbackend_xml.h    \
        backend/odatebookaccessbackend.h       \
        backend/odatebookaccessbackend_xml.h   \
        backend/odatebookaccessbackend_vcal.h  \
        backend/opimaccessbackend.h            \ 
        backend/opimbackendoccurrence.h        \       
        backend/otodoaccessbackend.h           \
        backend/otodoaccessvcal.h              \
        backend/otodoaccessxml.h               \
        backend/omemoaccessbackend.h           \
        backend/omemoaccessbackend_text.h      \
        backend/opimio.h

contains( ENABLE_SQL_PIM_BACKEND, y ) {
	message ( Enabling the SQL Backend for libopiepim2 )
	DEFINES += __USE_SQL
	LIBS    += -lopiedb2
	HEADERS += backend/otodoaccesssql.h             \
                   backend/ocontactaccessbackend_sql.h  \
                   backend/odatebookaccessbackend_sql.h 
	SOURCES += backend/otodoaccesssql.cpp            \
		   backend/ocontactaccessbackend_sql.cpp \
		   backend/odatebookaccessbackend_sql.cpp \
           backend/omemoaccessbackend_sql.cpp
}

!contains( ENABLE_SQL_PIM_BACKEND, y ) {
	message ( No SQL Backend in libopiepim2 )
}

