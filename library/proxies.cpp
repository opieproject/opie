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

#include "proxies.h"
#include "proxiesbase_p.h"
#include "config.h"

#include <qcombobox.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qvalidator.h>
#include <qspinbox.h>
#include <qradiobutton.h>

Proxies::Proxies( QWidget* parent ) :
    QVBox(parent)
{
    d = new ProxiesBase( this );
    connect(d->type,SIGNAL(activated(int)),this,SLOT(typeChanged(int)));
}

void Proxies::typeChanged(int t)
{
    switch (t) {
     case 0: d->autopanel->hide(); d->setpanel->hide(); break;
     case 1: d->setpanel->hide(); d->autopanel->show(); break;
     case 2: d->autopanel->hide(); d->setpanel->show(); break;
    }
}

class ProxyValidator : public QValidator {
public:
    ProxyValidator( QWidget * parent, const char *name = 0 ) :
	QValidator(parent,name)
    {
    }
 
    State validate( QString &s, int &pos ) const
    {
	int i;
	for (i=0; i<(int)s.length(); i++) {
	    if ( s[i] == ',' || s[i] == ';' || s[i] == '\n' || s[i] == '\r' )
		s[i] = ' ';
	}
	for (i=0; i<(int)s.length()-1; ) {
	    if ( s[i] == ' ' && s[i+1] == ' ' ) {
		if (pos>i) pos--;
		s = s.left(i)+s.mid(i+2);
	    } else
		i++;
	}
	return Valid;
    }
};

void Proxies::readConfig(Config& cfg)
{
    int t = cfg.readNumEntry("type",0);
    d->type->setCurrentItem(t);
    typeChanged(t);

    QString s;

    s = cfg.readEntry("autoconfig");
    if ( !s.isEmpty() )
	d->autoconfig->insertItem(s);


    s = cfg.readEntry("httphost");
    if ( !s.isEmpty() )
	d->httphost->setText(s);

    int i;
    i = cfg.readNumEntry("httpport");
    if ( i>0 )
	d->httpport->setValue(i);

    s = cfg.readEntry("ftphost");
    if ( !s.isEmpty() )
	d->ftphost->setText(s);

    i = cfg.readNumEntry("ftpport");
    if ( i>0 )
	d->ftpport->setValue(i);

    s = cfg.readEntry("noproxies");
    d->noproxies->setValidator(new ProxyValidator(this));
    d->noproxies->setText(s);
}

void Proxies::writeConfig( Config &cfg )
{
    cfg.writeEntry("type",d->type->currentItem());
    cfg.writeEntry("autoconfig", d->autoconfig->currentText());
    cfg.writeEntry("httphost", d->httphost->text());
    cfg.writeEntry("httpport", d->httpport->text());
    cfg.writeEntry("ftphost", d->ftphost->text());
    cfg.writeEntry("ftpport", d->ftpport->text());
    cfg.writeEntry("noproxies", d->noproxies->text());
}

