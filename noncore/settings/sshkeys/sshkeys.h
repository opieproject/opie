/* 
 *  ssh-agent key manipulation utility
 *
 *  (C) 2002 David Woodhouse <dwmw2@infradead.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef SSHKEYSAPP_H
#define SSHKEYSAPP_H

#include "sshkeysbase.h"
#include <opie/oprocess.h>

class SSHKeysApp : public SSHKeysBase
{
	Q_OBJECT
		
 public:
	SSHKeysApp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~SSHKeysApp();

 private:
	void log_text(const char *text);
	enum { Noise, KeyName, KeySize, KeyFingerprint } keystate;
	QString incoming_keyname;
	QString incoming_keysize;
	QString incoming_keyfingerprint;
	QString incoming_noise;
	OProcess addprocess;

 private slots:
	void doAddButton();
	void doRefreshListButton();
	void doRemoveAllButton();
	void get_list_keys_output(OProcess *proc, char *buffer, int buflen);
	void log_sshadd_output(OProcess *proc, char *buffer, int buflen);
	void ssh_add_exited(OProcess *proc);
	void add_text_changed(const QString &text);
};
#endif 

