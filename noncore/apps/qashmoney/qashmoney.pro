TEMPLATE = app
CONFIG = qt warn_on
HEADERS =   qashmoney.h \
            accountdisplay.h \
                        account.h \
                        transaction.h \
                        transactiondisplay.h \
                        newtransaction.h \
                        transfer.h \
                        transferdialog.h \
                        preferences.h \
                        preferencedialogs.h \
                        memory.h \
                        memorydialog.h \
                        newaccount.h \
                        calculator.h \
                        datepicker.h \
                        budget.h \
                        budgetdisplay.h \
                        currency.h
SOURCES =   qashmoney.cpp \
            accountdisplay.cpp \
                        account.cpp \
                        transaction.cpp \
                        transactiondisplay.cpp \
                        newtransaction.cpp \
                        transfer.cpp \
                        transferdialog.cpp \
                        preferences.cpp \
                        preferencedialogs.cpp \
                        memory.cpp \
                        memorydialog.cpp \
                        newaccount.cpp \
                        calculator.cpp \
                        datepicker.cpp \
            main.cpp \
                        budget.cpp \
                        budgetdisplay.cpp \
                        currency.cpp
INCLUDEPATH = $(OPIEDIR)/include
DEPENDPATH = $(OPIEDIR)/include
TARGET = qashmoney
DESTDIR = $(OPIEDIR)/bin

unix:LIBS += -lm
LIBS += -lqpe -lopiecore2 -lsqlite

include ( $(OPIEDIR)/include.pro )

!isEmpty( LIBSQLITE_INC_DIR ) {
    INCLUDEPATH = $$LIBSQLITE_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBSQLITE_LIB_DIR ) {
    LIBS = -L$$LIBSQLITE_LIB_DIR $$LIBS
}
