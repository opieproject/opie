TEMPLATE        = subdirs

!contains( platform, x11 ) {
  message( Configuring Wellenreiter for build on Opie )
  SUBDIRS = gui
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  message( Configuring Wellenreiter for build on Qt/X11 )
  SUBDIRS = lib gui
  system( mkdir -p $OPIEDIR/lib $OPIEDIR/bin $OPIEDIR/share/pics )
}
