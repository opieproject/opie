TEMPLATE        = subdirs
unix:SUBDIRS    = cornucopia libwellenreiter daemon gui


!contains( platform, x11 ) {
  message( Configuring Wellenreiter for build on Opie )
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  message( Configuring Wellenreiter for build on Qt/X11 )
  system( mkdir -p $OPIEDIR/lib $OPIEDIR/bin $OPIEDIR/share/pics )
}

