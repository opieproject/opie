/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#define QTOPIA_INTERNAL_LANGLIST
#include "qpe/network.h"
#include "qpe/networkinterface.h"
#include "qpe/global.h"
#include "qpe/config.h"
#include "qpe/resource.h"
#include "qpe/qpeapplication.h"
#include <qpe/qcopenvelope_qws.h>
#include <qpe/opielibrary.h>

#include <qlistbox.h>
#include <qdir.h>
#include <qlayout.h>
#include <qdict.h>
#include <qtranslator.h>

#include <stdlib.h>

class NetworkEmitter : public QCopChannel {
    Q_OBJECT
public:
    NetworkEmitter() : QCopChannel("QPE/Network",qApp)
    {
    }

    void receive(const QCString &msg, const QByteArray&)
    {
	if ( msg == "choicesChanged()" )
	    emit changed();
    }

signals:
    void changed();
};

/*!
  \internal

  Requests that the service \a choice be started. The \a password is
  the password to use if required.
*/
void Network::start(const QString& choice, const QString& password)
{
    QCopEnvelope e("QPE/Network", "start(QString,QString)");
    e << choice << password;
}

/*!
  \class Network network.h
  \brief The Network class provides network access functionality.
*/

// copy the proxy settings of the active config over to the Proxies.conf file
/*!
  \internal
*/
void Network::writeProxySettings( Config &cfg )
{
    Config proxy( Network::settingsDir() + "/Proxies.conf", Config::File );
    proxy.setGroup("Properties");
    cfg.setGroup("Proxy");
    proxy.writeEntry("type", cfg.readEntry("type") );
    proxy.writeEntry("autoconfig", cfg.readEntry("autoconfig") );
    proxy.writeEntry("httphost", cfg.readEntry("httphost") );
    proxy.writeEntry("httpport", cfg.readEntry("httpport") );
    proxy.writeEntry("ftphost", cfg.readEntry("ftphost") );
    proxy.writeEntry("ftpport", cfg.readEntry("ftpport") );
    proxy.writeEntry("noproxies", cfg.readEntry("noproxies") );
    cfg.setGroup("Properties");
}



/*!
  \internal

  Stops the current network service.
*/
void Network::stop()
{
    QCopEnvelope e("QPE/Network", "stop()");
}

static NetworkEmitter *emitter = 0;

/*!
  \internal
*/
void Network::connectChoiceChange(QObject* receiver, const char* slot)
{
    if ( !emitter )
	emitter = new NetworkEmitter;
    QObject::connect(emitter,SIGNAL(changed()),receiver,slot);
}

/*!
  \internal
*/
QString Network::settingsDir()
{
    return Global::applicationFileName("Network", "modules");
}

/*!
  \internal
*/
QStringList Network::choices(QListBox* lb, const QString& dir)
{
    QStringList list;

    if ( lb )
	lb->clear();

    QString adir = dir.isEmpty() ? settingsDir() : dir;
    QDir settingsdir(adir);
    settingsdir.mkdir(adir);
    
    QStringList files = settingsdir.entryList("*.conf");
    for (QStringList::ConstIterator it=files.begin(); it!=files.end(); ++it ) {
	QString filename = settingsdir.filePath(*it);
	Config cfg(filename, Config::File);
	cfg.setGroup("Info");
	if ( lb )
	    lb->insertItem(Resource::loadPixmap("Network/" + cfg.readEntry("Type")),
		cfg.readEntry("Name"));
	list.append(filename);
    }

    return list;
}

class NetworkServer : public QCopChannel {
    Q_OBJECT
public:
    NetworkServer(QObject* parent) : QCopChannel("QPE/Network",parent)
    {
	up = FALSE;
	examineNetworks( TRUE );
	QCopChannel* card = new QCopChannel("QPE/Card",parent);
	connect(card,SIGNAL(received(const QCString &, const QByteArray&)),
	    this,SLOT(cardMessage(const QCString &, const QByteArray&)));
    }

    ~NetworkServer()
    {
	stop();
    }

    bool networkOnline() const
    {
	return up;
    }

private:
    void receive(const QCString &msg, const QByteArray& data)
    {
	if ( msg == "start(QString,QString)" ) {
	    QDataStream stream(data,IO_ReadOnly);
	    QString file,password;
	    stream >> file >> password;
	    if ( file.isEmpty() ) {
		QStringList l = Network::choices();
		for (QStringList::ConstIterator i=l.begin(); i!=l.end(); ++i) {
		    Config cfg(*i,Config::File);
		    cfg.setGroup("Info");
		    QString type = cfg.readEntry("Type");
		    NetworkInterface* plugin = Network::loadPlugin(type);
		    cfg.setGroup("Properties");
		    if ( plugin && plugin->isAvailable(cfg) ) {
			file = *i;
			break;
		    }
		}
		if ( file.isEmpty() ) {
		    QCopEnvelope("QPE/Network", "failed()");
		    return;
		}
	    }
	    start(file,password);
	} else if ( msg == "stop()" ) {
	    stop();
	} else if ( msg == "choicesChanged()" ) {
	    examineNetworks();
	}
    }

private slots:
    void cardMessage(const QCString &msg, const QByteArray&)
    {
	if ( msg == "stabChanged()" )
	    examineNetworks();
    }

private:
    void examineNetworks( bool firstStart = FALSE )
    {
	QStringList l = Network::choices();
	bool wasup = up; up=FALSE;
	QStringList pavailable = available;
	available.clear();
	for (QStringList::ConstIterator it=l.begin(); it!=l.end(); ++it) {
	    Config cfg(*it,Config::File);
	    cfg.setGroup("Info");
	    QString type = cfg.readEntry("Type");
	    NetworkInterface* plugin = Network::loadPlugin(type);
	    cfg.setGroup("Properties");
	    if ( plugin ) {
		if ( plugin->isActive(cfg) ) {
		    up = TRUE;
		    if ( firstStart )
			plugin->start( cfg );
		}
		if ( plugin->isAvailable(cfg) )
		    available.append(*it);
	    }
	}

	// Try to work around unreproducible bug whereby
	// the netmon applet shows wrong state.
	bool reannounce = wait<0;

	if ( available != pavailable || reannounce ) {
	    QCopEnvelope e("QPE/Network", "available(QStringList)");
	    e << available;
	}
	if ( up != wasup || reannounce ) {
	    QCopEnvelope("QPE/Network", up ? "up()" : "down()");
	}
    }

    void start( const QString& file, const QString& password )
    {
	if ( !current.isEmpty() )
	    stop();
	current = QString::null;
	Config cfg(file, Config::File);
	cfg.setGroup("Info");
	QString type = cfg.readEntry("Type");
	NetworkInterface* plugin = Network::loadPlugin(type);
	bool started = FALSE;
	if ( plugin ) {
	    cfg.setGroup("Properties");
	    if ( plugin->start(cfg,password) ) {
		Network::writeProxySettings( cfg );
		current = file;
		wait=0;
		startTimer(400);
		started = TRUE;
	    }
	}
	if ( !started ) {
	    QCopEnvelope("QPE/Network", "failed()");
	}
    }

    void stop()
    {
	bool stopped = FALSE;
	if ( !current.isEmpty() ) {
	    Config cfg(current, Config::File);
	    cfg.setGroup("Info");
	    QString type = cfg.readEntry("Type");
	    NetworkInterface* plugin = Network::loadPlugin(type);
	    if ( plugin ) {
		cfg.setGroup("Properties");
		if ( plugin->stop(cfg) ) {
		    current = QString::null;
		    wait=0;
		    startTimer(400);
		    stopped = TRUE;
		}
	    }
	}
	if ( !stopped ) {
	    QCopEnvelope("QPE/Network", "failed()");
	}
    }

    void timerEvent(QTimerEvent*)
    {
	examineNetworks();
	if ( wait >= 0 ) {
	    if ( up == !current.isNull() ) {
		// done
		killTimers();
		if ( up ) {
		    startTimer(3000); // monitor link
		    wait = -1;
		}
	    } else {
		wait++;
		if ( wait == 600 ) {
		    killTimers(); // forget about it after 240 s
		    QCopEnvelope("QPE/Network", "failed()");
		    up = !current.isNull();
		}
	    }
	} else if ( !up ) {
	    killTimers();
	}
    }

private:
    QStringList available;
    QString current;
    bool up;
    int wait;
};

static NetworkServer* ns=0;

/*!
  \internal
*/
QString Network::serviceName(const QString& service)
{
    Config cfg(service, Config::File);
    cfg.setGroup("Info");
    return cfg.readEntry("Name");
}

/*!
  \internal
*/
QString Network::serviceType(const QString& service)
{
    Config cfg(service, Config::File);
    cfg.setGroup("Info");
    return cfg.readEntry("Type");
}

/*!
  \internal
*/
bool Network::serviceNeedsPassword(const QString& service)
{
    Config cfg(service,Config::File);
    cfg.setGroup("Info");
    QString type = cfg.readEntry("Type");
    NetworkInterface* plugin = Network::loadPlugin(type);
    cfg.setGroup("Properties");
    return plugin ? plugin->needPassword(cfg) : FALSE;
}

/*!
  \internal
*/
bool Network::networkOnline()
{
    return ns && ns->networkOnline();
}

/*!
  \internal
*/
void Network::createServer(QObject* parent)
{
    ns = new NetworkServer(parent);
}

/*!
  \internal
*/
int Network::addStateWidgets(QWidget* parent)
{
    int n=0;
    QStringList l = Network::choices();
    QVBoxLayout* vb = new QVBoxLayout(parent);
    for (QStringList::ConstIterator it=l.begin(); it!=l.end(); ++it) {
	Config cfg(*it,Config::File);
	cfg.setGroup("Info");
	QString type = cfg.readEntry("Type");
	NetworkInterface* plugin = Network::loadPlugin(type);
	cfg.setGroup("Properties");
	if ( plugin ) {
	    QWidget* w;
	    if ( (w=plugin->addStateWidget(parent,cfg)) ) {
		n++;
		vb->addWidget(w);
	    }
	}
    }
    return n;
}

static QDict<NetworkInterface> *ifaces;

/*!
  \internal
*/
NetworkInterface* Network::loadPlugin(const QString& type)
{
#ifndef QT_NO_COMPONENT
    if ( !ifaces ) ifaces = new QDict<NetworkInterface>;
    NetworkInterface *iface = ifaces->find(type);
    if ( !iface ) {
	QString libfile = QPEApplication::qpeDir() + "/plugins/network/lib" + type + ".so";
	OpieLibrary lib(libfile);
	if ( !lib.queryInterface( IID_Network, (QUnknownInterface**)&iface ) == QS_OK )
	    return 0;
	ifaces->insert(type,iface);
	QStringList langs = Global::languageList();
	for (QStringList::ConstIterator it = langs.begin(); it!=langs.end(); ++it) {
	    QString lang = *it;
	    QTranslator * trans = new QTranslator(qApp);
	    QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/lib"+type+".qm";
	    if ( trans->load( tfn ))
		qApp->installTranslator( trans );
	    else
		delete trans;
	}
    }
    return iface;
#else
    return 0;
#endif
}

#include "network.moc"
