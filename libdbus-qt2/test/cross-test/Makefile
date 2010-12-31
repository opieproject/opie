GENERATED = testsinterface.h testsinterface.cpp testsproxy.h testsproxy.cpp \
            singletestsinterface.h singletestsinterface.cpp \
            singletestsproxy.h singletestsproxy.cpp \
            testclientinterface.h testclientinterface.cpp \
            testclientproxy.h testclientproxy.cpp \
            testsignalsinterface.h testsignalsinterface.cpp \
            testsignalsproxy.h testsignalsproxy.cpp

all: cross-test-compile

cross-test-compile: dbusxml2tq3 crosstest-server crosstest-client

crosstest-server: $(GENERATED) crosstest-server.mak crosstest-server.h crosstest-server.cpp interfaceimpls.h interfaceimpls.cpp
	$(MAKE) -f crosstest-server.mak

crosstest-client: $(GENERATED)  crosstest-client.mak crosstest-client.h crosstest-client.cpp
	$(MAKE) -f crosstest-client.mak

crosstest-server.mak: crosstest-server.pro
	qmake -o crosstest-server.mak crosstest-server.pro

crosstest-client.mak: crosstest-client.pro
	qmake -o crosstest-client.mak crosstest-client.pro

$(GENERATED): test-introspect.xml
	../../tools/dbusxml2qt3/dbusxml2qt3 test-introspect.xml

cross-test-server: crosstest-server
	./crosstest-server 2>>server.err

cross-test-client: crosstest-client
	./crosstest-client 2>>client.err

clean: clean-generate clean-server clean-client

clean-generate:
	rm -f $(GENERATED)

clean-server: crosstest-server.mak
	$(MAKE) -f crosstest-server.mak clean

clean-client: crosstest-client.mak
	$(MAKE) -f crosstest-client.mak clean

distclean: clean-generate distclean-server distclean-client

distclean-server: crosstest-server.mak
	$(MAKE) -f crosstest-server.mak distclean

distclean-client: crosstest-client.mak
	$(MAKE) -f crosstest-client.mak distclean

dbusxml2tq3:
	cd ../../tools/dbusxml2qt3 && qmake dbusxml2qt3.pro
	cd ../../tools/dbusxml2qt3 && $(MAKE)
