indefault:  dynamic

all: default docs

LIBS=library libopie

COMPONENTS=$(LOCALCOMPONENTS) inputmethods/keyboard \
	inputmethods/pickboard \
	inputmethods/handwriting \
	inputmethods/unikeyboard \
	inputmethods/jumpx \
	inputmethods/kjumpx \
	inputmethods/dvorak \
	core/applets/batteryapplet \
	core/applets/batteryapplet-ipaq \
	core/applets/irdaapplet \
	core/applets/volumeapplet \
	core/applets/clockapplet \
	core/applets/netmonapplet \
	core/applets/screenshotapplet \
	core/applets/clipboardapplet \
	core/applets/cardmon \
	core/applets/obex \
	core/applets/vmemo \
	noncore/net/netsetup/dialup \
	noncore/net/netsetup/lan \
	core/multimedia/opieplayer/libmpeg3 \
	core/multimedia/opieplayer/libmad \
	core/multimedia/opieplayer/wavplugin \
	noncore/mail/libmail \
	noncore/mail/bend \

APPS=$(LOCALAPPS) core/pim/addressbook noncore/tools/calculator \
    noncore/tools/clock core/pim/datebook core/apps/filebrowser \
    core/apps/helpbrowser noncore/games/minesweep \
    core/multimedia/opieplayer core/settings/light-and-power \
    noncore/settings/language \
    noncore/settings/rotation \
    noncore/settings/appearance \
    core/settings/security \
    noncore/settings/sound \
    core/settings/systemtime \
    noncore/settings/mediummount \
    noncore/settings/tabmanager \
    noncore/games/solitaire noncore/apps/spreadsheet \
    noncore/games/tetrix  core/apps/textedit  \
    core/pim/todo noncore/games/wordgame core/apps/embeddedkonsole \
    core/launcher core/apps/sysinfo \
    noncore/games/parashoot noncore/games/snake core/settings/citytime  \
    noncore/multimedia/showimg noncore/net/netsetup core/settings/wlansetup \
    noncore/apps/tableviewer   core/apps/oipkg noncore/games/mindbreaker \
    noncore/games/go noncore/games/qasteroids core/apps/qcop \
    noncore/games/fifteen noncore/comm/keypebble  \
    core/pim/today  noncore/games/tictac noncore/net/opieftp  \
    noncore/graphics/drawpad noncore/games/kcheckers noncore/settings/appskey \
    noncore/graphics/qpdf   noncore/games/kpacman noncore/apps/advancedfm  \
    noncore/games/kbill noncore/tools/remote  \
    noncore/tools/opie-sh \
    noncore/mail

NONTMAKEAPPS := ipaq/opiealarm noncore/nethack

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
