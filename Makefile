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
	core/applets/homeapplet core/applets/suspendapplet \
	core/applets/logoutapplet \
	core/applets/screenshotapplet core/applets/clipboardapplet \
	core/applets/cardmon core/applets/obex \
	core/applets/vmemo \
	noncore/net/networksetup/wlan \
	noncore/applets/wirelessapplet noncore/applets/notesapplet

MAIL= noncore/mail/libmail noncore/mail/bend 

PLAYER = core/multimedia/opieplayer/libmpeg3 \
	core/multimedia/opieplayer/libmad \
	core/multimedia/opieplayer/wavplugin \
	core/multimedia/opieplayer/modplug

COMPONENTS=$(LOCALCOMPONENTS) $(INPUTCOMPONENTS) $(APPLETS) $(MAIL) $(PLAYER)	


PIMAPPS = core/pim/addressbook core/pim/datebook \
	 core/pim/today core/pim/todo 

TODAYPLUGINS = core/pim/today/plugins/datebook \
		core/pim/today/plugins/todolist \
		core/pim/today/plugins/mail \


COREAPPS = core/apps/filebrowser core/apps/helpbrowser \
	core/apps/textedit core/apps/embeddedkonsole \
	core/launcher  \
	core/opie-login \
	core/apps/oipkg core/apps/qcop

COREMULTIMEDIA = core/multimedia/opieplayer

CORESETTINGS = core/settings/light-and-power core/settings/security \
	core/settings/systemtime core/settings/citytime \
	core/settings/launcher

NONCORESETTINGS  =noncore/settings/language noncore/settings/rotation \
	noncore/settings/appearance2 noncore/settings/sound \
	noncore/settings/mediummount \
	noncore/settings/tabmanager noncore/settings/appskey \
	noncore/settings/netsystemtime noncore/settings/wlansetup \
	noncore/settings/sshkeys noncore/settings/usermanager \
	noncore/apps/backup

NONCORETOOLS = noncore/tools/calculator noncore/tools/clock \
	 noncore/tools/remote noncore/tools/opie-sh noncore/apps/advancedfm

NONCORESTYLES = noncore/styles/liquid noncore/styles/theme noncore/styles/metal \
	noncore/styles/flat noncore/styles/fresh

NONCOREDECOS = noncore/decorations/liquid noncore/decorations/flat \
	noncore/decorations/polished

GAMES = noncore/games/minesweep noncore/games/solitaire \
	noncore/games/tetrix noncore/games/wordgame \
	noncore/games/parashoot noncore/games/snake \
	noncore/games/mindbreaker \
	noncore/games/go noncore/games/qasteroids \
	noncore/games/fifteen noncore/games/tictac \
	noncore/games/kcheckers noncore/games/kpacman \
	noncore/games/kbill noncore/games/buzzword \
	noncore/games/bounce 

NONAPPS = noncore/apps/opie-sheet noncore/apps/tableviewer \
	noncore/apps/opie-reader noncore/apps/checkbook noncore/apps/oxygen \
 	noncore/apps/sysinfo noncore/net/opieirc noncore/apps/aqpkg \
 	noncore/apps/opie-console 
	
NONNET = noncore/net/networksetup \
	noncore/net/opieftp \
	noncore/mail

NONMULT = noncore/multimedia/showimg noncore/graphics/drawpad \
	noncore/graphics/qpdf noncore/apps/confedit


NONCOMM = noncore/comm/keypebble

APPS=$(LOCALAPPS) $(PIMAPPS)  $(COREAPPS)     \
	$(COREMULTIMEDIA) $(CORESETTINGS) \
	$(NONCORESETTINGS) $(NONCORETOOLS) $(NONCORESTYLES) \
	$(NONCOREDECOS) $(NONAPPS) $(NONNET) $(NONMULT) \
	$(NONCOMM) $(GAMES) $(TODAYPLUGINS)
    
    

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
