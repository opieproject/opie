indefault:  dynamic

all: default docs

LIBS=library libopie


INPUTCOMPONENTS= inputmethods/keyboard inputmethods/pickboard \
	inputmethods/handwriting  inputmethods/unikeyboard \
	inputmethods/jumpx inputmethods/kjumpx \
	inputmethods/dvorak  inputmethods/multikey \

APPLETS= core/applets/batteryapplet core/applets/batteryapplet-ipaq \
	core/applets/irdaapplet core/applets/volumeapplet \
	core/applets/clockapplet core/applets/netmonapplet \
	core/applets/screenshotapplet core/applets/clipboardapplet \
	core/applets/cardmon core/applets/obex \
	core/applets/vmemo noncore/net/netsetup/dialup \
	noncore/net/netsetup/lan  \
	noncore/applets/wirelessapplet

MAIL= noncore/mail/libmail noncore/mail/bend 

PLAYER = core/multimedia/opieplayer/libmpeg3 \
	core/multimedia/opieplayer/libmad \
	core/multimedia/opieplayer/wavplugin \
	core/multimedia/opieplayer/modplug

COMPONENTS=$(LOCALCOMPONENTS) $(INPUTCOMPONENTS) $(APPLETS) $(MAIL) $(PLAYER)	


PIMAPPS = core/pim/addressbook core/pim/datebook \
	 core/pim/today core/pim/todo 

COREAPPS = core/apps/filebrowser core/apps/helpbrowser \
	core/apps/textedit core/apps/embeddedkonsole \
	core/launcher  \
	core/opie-login \
	core/apps/oipkg core/apps/qcop

COREMULTIMEDIA = core/multimedia/opieplayer

CORESETTINGS = core/settings/light-and-power core/settings/security \
	core/settings/systemtime core/settings/citytime \

NONCORESETTINGS  =noncore/settings/language noncore/settings/rotation \
	noncore/settings/appearance noncore/settings/sound \
	noncore/settings/mediummount \
	noncore/settings/tabmanager noncore/settings/appskey \
	noncore/settings/netsystemtime \
	noncore/settings/wlansetup \

NONCORETOOLS = noncore/tools/calculator noncore/tools/clock \
	 noncore/tools/remote noncore/tools/opie-sh noncore/apps/advancedfm

NONCORESTYLES = noncore/styles/liquid noncore/styles/theme noncore/styles/metal

GAMES = noncore/games/minesweep noncore/games/solitaire \
	noncore/games/tetrix noncore/games/wordgame \
	noncore/games/parashoot noncore/games/snake \
	noncore/games/mindbreaker \
	noncore/games/go noncore/games/qasteroids \
	noncore/games/fifteen noncore/games/tictac \
	noncore/games/kcheckers noncore/games/kpacman \
	noncore/games/kbill

NONAPPS = noncore/apps/opie-sheet noncore/apps/tableviewer \
	noncore/apps/opie-reader noncore/apps/checkbook \
	noncore/apps/sysinfo \
	
NONNET = noncore/net/netsetup noncore/net/opieftp \
	noncore/mail

NONMULT = noncore/multimedia/showimg noncore/graphics/drawpad \
	noncore/graphics/qpdf noncore/apps/confedit


NONCOMM = noncore/comm/keypebble

APPS=$(LOCALAPPS) $(PIMAPPS)  $(COREAPPS)     \
	$(COREMULTIMEDIA) $(CORESETTINGS) \
	$(NONCORESETTINGS) $(NONCORETOOLS) $(NONCORESTYLES) \
	$(NONAPPS) $(NONNET) $(NONMULT) \
	$(NONCOMM) $(GAMES)
    
    

NONTMAKEAPPS := ipaq/opiealarm 

DOCS = docs/src/opie-policy
single: mpegplayer/libmpeg3

dynamic: $(APPS) $(NONTMAKEAPPS)

docs: $(DOCS)

$(COMPONENTS): $(LIBS)

$(NONTMAKEAPPS) $(APPS): $(LIBS) $(COMPONENTS)

$(LIBS) $(COMPONENTS) $(NONTMAKEAPPS) $(APPS) $(DOCS) single:
	$(MAKE) -C $@ -f Makefile

showcomponents:
	@echo $(LIBS) $(APPS) $(COMPONENTS) single

clean:
	$(MAKE) -C single -f Makefile $@
	for dir in $(NONTMAKEAPPS) $(APPS) $(LIBS) $(COMPONENTS) $(DOCS); do $(MAKE) -C $$dir -f Makefile $@ || exit 1; done

lupdate:
	for dir in $(APPS) $(LIBS) $(COMPONENTS); do $(MAKE) -C $$dir -f Makefile $@ || exit 1; done

lrelease:
	for dir in $(APPS) $(LIBS) $(COMPONENTS); do $(MAKE) -C $$dir -f Makefile $@ || exit 1; done


.PHONY: default dynamic $(NONTMAKEAPPS) $(LIBS) $(APPS) $(COMPONENTS) $(DOCS) single showcomponents clean
