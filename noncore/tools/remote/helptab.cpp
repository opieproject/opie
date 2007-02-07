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

#include "helptab.h"

HelpTab::HelpTab(QWidget *parent, const char *name):QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QString *string = new QString("<qt><h1>Opie-Remote Usage Instructions</h1><h2>Introduction</h2><p>Opie-Remote allows you to learn the signals from infra-red remotes (for your TV, VCR, DVD player, etc.) and then set up a multi-function remote to control many different devices at once from your handheld.</p><p>Each button on a Remote Layout (as seen on the main <b>Remote</b> tab) can be mapped to any button in a real remote.  This way you can have, for example, a VCR remote layout, in which all the play, pause, etc. buttons are mapped to the buttons on your VCR's remote.  However, most VCRs don't have volume controls, so the volume buttons can be mapped to the volume buttons on your TV.</p><h2>Requirements</h2><p>Opie-Remote requires LIRC, a handheld with supported infra-red hardware, and an appropriate LIRC driver. Your distribution should hopefully have ensured that LIRC and an appropriate driver have been installed along with this program.</p><p>Note that currently Opie-Remote has only been tested on an iPAQ h3800 series handheld.</p><h2>Setup</h2><p>First, you will need to record the signals from a real remote into Opie-Remote. Go to the <b>Learn</b> tab, tap <b>Add</b> and follow the instructions.</p><p>Secondly you need to create a remote layout. Go to the <b>Config</b> tab, and enter a name for your remote layout in the pulldown menu, then tap <b>New</b>. Then, press each button that you want to map, and a dialog should appear.  Select the remote and button that you want to use, and tap <b>OK</b>. Once you are done, go to the <b>Remote</b> tab, and select the new remote from the dropdown menu.</p><h2>About Opie-Remote</h2><p>Please direct all support enquiries to the Opie users mailing list (<a href=mailto:opie@handhelds.org>opie@handhelds.org</a>). Developers can send patches to <a href=mailto:opie-devel@handhelds.org>opie-devel@handhelds.org</a>.</p><p>Maintained by: Paul Eggleton<br>Originally written by: Thomas Stephens</p></qt>");
	QTextView *view = new QTextView((const QString &)*string, 0,  this, "view");
	layout->insertSpacing( -1, 5);
	layout->insertWidget(-1, view);
	layout->insertSpacing(-1, 5);
}
