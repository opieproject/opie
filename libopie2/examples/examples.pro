TEMPLATE        = subdirs
unix:SUBDIRS    = opieui opienet opiecore opiemm opiedb


!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}

