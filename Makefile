default:  dynamic

all: default docs

LIBS=library libopie

COMPONENTS=$(LOCALCOMPONENTS) inputmethods/keyboard \
	inputmethods/pickboard \
	inputmethods/handwriting \
	inputmethods/unikeyboard \
	inputmethods/jumpx \
	inputmethods/dvorak \
	launcher/batteryapplet \
	launcher/irdaapplet \
	launcher/volumeapplet \
	launcher/clockapplet \
	launcher/netmonapplet \
	launcher/screenshotapplet \
	launcher/vmemo \
	netsetup/dialup \
	netsetup/lan \
	opiemail/ifaces/pop3 opiemail/ifaces/smtp \
	core/opieplayer/libmpeg3 \
	core/opieplayer/libmad \
	core/opieplayer/wavplugin \
	cardmon \
	sdmon 

APPS=$(LOCALAPPS) core/addressbook calculator clock datebook \
    filebrowser helpbrowser minesweep core/opieplayer \
    settings/light-and-power \
    settings/language \
    settings/rotation \
    settings/appearance \
    settings/security \
    settings/sound \
    settings/systemtime \
    settings/mediummount \
    settings/tabmanager \
    solitaire spreadsheet tetrix textedit \
    todo wordgame embeddedkonsole launcher sysinfo \
    parashoot snake citytime showimg netsetup core/wlansetup tableviewer \
    core/oipkg mindbreaker go qasteroids qcop fifteen keypebble  today opiemail/converter opiemail \
    noncore/tictac noncore/opieftp noncore/drawpad noncore/kcheckers noncore/appskey noncore/qpdf \
    noncore/kpacman noncore/advancedfm noncore/kbill

NONTMAKEAPPS := noncore/nethack

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
AKEAPPS) $(LIBS) $(APPS) $(COMPONENTS) $(DOCS) single showcomponents clean
