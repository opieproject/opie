/*
Opie-Remote.  emulates remote controlls on an iPaq (and maybe a Zaurus) in Opie.
Copyright (C) 2002 Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <qpe/config.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
//#include <qscrollview.h>
//#include <qcolor.h>
//#include <qvbox.h>

#include "buttondialog.h"
#include "channelgroupconf.h"
#include "dvdgroupconf.h"
#include "topgroupconf.h"
#include "vcrgroupconf.h"

class ConfigTab : public QWidget
{
	Q_OBJECT
public:
	ConfigTab(QWidget *parent=0, const char *name=0);
	void setConfig(Config *newCfg);
public slots:
	void newPressed();
	void remoteSelected(const QString &string);
	void buttonPressed();
signals:
	void remotesChanged();
private:
	Config *cfg;
	TopGroupConf *topGroupConf;
	DVDGroupConf *dvdGroupConf;
	VCRGroupConf *vcrGroupConf;
	ChannelGroupConf *channelGroupConf;
};
