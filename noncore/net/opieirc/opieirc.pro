TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= ircchannel.h ircconnection.h \
              ircmessage.h \
              ircmessageparser.h ircoutput.h \
              ircperson.h ircserver.h ircsession.h \
              mainwindow.h irctab.h ircservertab.h \
              ircchanneltab.h ircchannellist.h \
              ircserverlist.h ircservereditor.h \
              ircquerytab.h
SOURCES		= ircchannel.cpp ircconnection.cpp \
              ircmessage.cpp \
              ircmessageparser.cpp ircoutput.cpp \
              ircperson.cpp ircserver.cpp \
              ircsession.cpp main.cpp mainwindow.cpp \
              irctab.cpp ircservertab.cpp \
              ircchanneltab.cpp ircchannellist.cpp \
              ircserverlist.cpp ircservereditor.cpp \
              ircquerytab.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe

