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

TRANSLATIONS  = ../../../i18n/de/wellenreiter.ts \
                ../../../i18n/nl/wellenreiter.ts \
                ../../../i18n/da/wellenreiter.ts \
                ../../../i18n/xx/wellenreiter.ts \
                ../../../i18n/en/wellenreiter.ts \
                ../../../i18n/es/wellenreiter.ts \
                ../../../i18n/fr/wellenreiter.ts \
                ../../../i18n/hu/wellenreiter.ts \
                ../../../i18n/ja/wellenreiter.ts \
                ../../../i18n/ko/wellenreiter.ts \
                ../../../i18n/no/wellenreiter.ts \
                ../../../i18n/pl/wellenreiter.ts \
                ../../../i18n/pt/wellenreiter.ts \
                ../../../i18n/pt_BR/wellenreiter.ts \
                ../../../i18n/sl/wellenreiter.ts \
                ../../../i18n/zh_CN/wellenreiter.ts \
                ../../../i18n/zh_TW/wellenreiter.ts

