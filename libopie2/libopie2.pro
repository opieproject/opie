TEMPLATE        = subdirs
unix:SUBDIRS    = opiecore opiedb opiepim opieui opienet opiemm examples

!contains( platform, x11 ) {
  message( Configuring libopie2 for build on Opie )
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  message( Configuring libopie2 for build on Qt/X11 )
}
