default:  dynamic

all: default docs

LIBS=library libopie

COMPONENTS=$(LOCALCOMPONENTS) inputmethods/keyboard \
	inputmethods/pickboard \
	inputmethods/handwriting \
	inputmethods/unikeyboard \
	inputmethods/jumpx \
	inputmethods/dvorak \
	taskbar/batteryapplet \
	taskbar/volumeapplet \
	taskbar/clockapplet \
	taskbar/netmonapplet \
	taskbar/screenshotapplet \
	taskbar/vmemo \
	netsetup/dialup \
	netsetup/lan \
	opiemail/ifaces/pop3 opiemail/ifaces/smtp \
	mpegplayer/libmpeg3 \
	mpegplayer/libmad \
	mpegplayer/wavplugin \
	cardmon \
	sdmon 

APPS=$(LOCALAPPS) addressbook calculator clock datebook \
    filebrowser helpbrowser minesweep mpegplayer \
    settings/light-and-power \
    settings/language \
    settings/rotation \
    settings/appearance \
    settings/security \
    settings/sound \
    settings/systemtime \
    solitaire spreadsheet tetrix textedit \
    todo wordgame embeddedkonsole taskbar sysinfo \
    parashoot snake citytime showimg netsetup tableviewer \
    qipkg mindbreaker go qasteroids qcop fifteen keypebble  today opiemail/converter opiemail \
    noncore/tictac noncore/opieftp noncore/drawpad noncore/kcheckers

DOCS = docs/src/opie-policy
single: mpegplayer/libmpeg3

dynamic: $(APPS)

docs: $(DOCS)

$(COMPONENTS): $(LIBS)

$(APPS): $(LIBS) $(COMPONENTS)

$(LIBS) $(COMPONENTS) $(APPS) $(DOCS) single:
	$(MAKE) -C $@ -f Makefile

showcomponents:
	@echo $(LIBS) $(APPS) $(COMPONENTS) single

clean:
	$(MAKE) -C single -f Makefile $@
	for dir in $(APPS) $(LIBS) $(COMPONENTS) $(DOCS); do $(MAKE) -C $$dir -f Makefile $@ || exit 1; done

lupdate:
	for dir in $(APPS) $(LIBS) $(COMPONENTS); do $(MAKE) -C $$dir -f Makefile $@ || exit 1; done

lrelease:
	for dir in $(APPS) $(LIBS) $(COMPONENTS); do $(MAKE) -C $$dir -f Makefile $@ || exit 1; done


.PHONY: default dynamic $(LIBS) $(APPS) $(COMPONENTS) $(DOCS) single showcomponents clean
