/*
Opie-sh.  convinience app to allow you to use qdialogs in scripts (mainly shell scripts)
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

#include <qstring.h>
#include <qstringlist.h>
#include <qpe/qpeapplication.h>
#include <qmessagebox.h>
#include <qwidget.h>

#include <stdio.h>

#include "mbox.h"
#include "fviewer.h"
#include "inputdialog.h"

int myMessageBox(int wi, int h, QWidget *w, int argc, QStringList args)
{
	int i, type=0;
	QString button0Text, button1Text, button2Text, string, title;
	bool full=true;

	for(i=0; i<argc; i++)
	{
		if(args[i] == "-t")
		{
			title = args[i+1];
		}

		if(args[i] == "-M")
		{
			string =  args[i+1];
		}

		if(args[i] == "-0")
		{
			button0Text = args[i+1];
		}

		if(args[i] == "-1")
		{
			button1Text = args[i+1];
		}

		if(args[i] == "-2")
		{
			button2Text = args[i+1];
		}

		if(args[i] == "-I")
		{
			type=0;
		}

		if(args[i] == "-w")
		{
			type = 1;
		}

		if(args[i] == "-e")
		{
			type = 2;
		}

		if(args[i] == "-g")
		{
			full = false;
		}
	}

	MBox *mbox = new MBox(wi, h, (int)type, title, string, &button0Text, &button1Text, &button2Text, w, (QString)"messagebox");
	if(full)
	{
		w->setCaption(title);
		w->showMaximized();
	}
//	mbox->show();
	switch(mbox->exec() )
	{
	case 0:
		return -1;
	case 1:
		return -1;
	case 2:
		return 0;
	case 3:
		return 1;
	case 4:
		return 2;
	}
}

void printusage()
{
	printf("Usage instructions for Opie-sh\n");
	printf("Usage: opie-sh [dialog type] [type specific options]\n");
	printf("Types:\n");
	printf("      -m                 Message Box\n");
	printf("      -f [filename]      View file [Default = stdin]\n");
	printf("      -i                 Input dialog\n");
	printf("      -h --help          These instructions\n");
	printf("      -t [title]         The window/dialog title\n");
	printf("Message box options:\n");
	printf("      -M [message]       The message to display\n");
	printf("      -I                 Use information icon\n");
	printf("      -w                 Use the warning icon\n");
	printf("      -e                 Use the error icon\n");
	printf("      -0 [text]          First button text [Default = OK]\n");
	printf("      -1 [text]          Second button text\n");
	printf("      -2 [text]          Third button text\n");
	printf("      -g                 Disable fullscreen\n");
	printf("Input Dialog options:\n");
	printf("      -s                 A single line of input (output to console)\n");
	printf("      -l                 List input (newline separated list read in from file)\n");
	printf("      -b                 A list box, enabling multiple selections (input same as -l)\n");
	printf("      -L [label]         The label for the input field\n");
	printf("      -F [filename]      An input file (for when it makes sense) [Default = stdin]\n");
	printf("      -E                 Makes list input editable\n");
	printf("      -g                 Disable fullscreen\n\0");
}

int fileviewer(QPEApplication *a, int argc, QStringList args)
{
	int i;
	QString filename, title, icon;
	bool update=false;

	for(i=0; i < argc; i++)
	{
		if(args[i] == "-f")
		{
			if(args[i+1][0] != '-')
			{
				filename = args[i+1];
			}
		}

		if(args[i] == "-I")
		{
			icon=args[i+1];
		}

		if(args[i] == "-t")
		{
			title = args[i+1];
		}
	}
	FViewer *fview = new FViewer(icon, filename, title, 0, (QString) "fileviewer");
	a->setMainWidget(fview);
	fview->showMaximized();
	return a->exec();
}

int input(int wi, int h, QWidget *w, int argc, QStringList args)
{
	int i, type = 0;
	QString title, label, filename;
	bool edit=false, full=true;

	for(i=0; i < argc; i++)
	{
		if(args[i] == "-s")
		{
			type = 0;
		}

		if(args[i] == "-l")
		{
			type = 1;
		}

		if(args[i] == "-b")
		{
			type = 2;
		}

		if(args[i] == "-t")
		{
			title = args[i+1];
		}

		if(args[i] == "-L")
		{
			label = args[i+1];
		}

		if(args[i] == "-F")
		{
			if(args[i+1][0] != '-')
			{
				filename = args[i+1];
			}
		}

		if(args[i] =="-E")
		{
			edit = true;
		}

		if(args[i] == "-g")
		{
			full = false;
		}
	}
	InputDialog *id = new InputDialog(wi, h, type, label, title, filename, edit, w);
	if(full)
	{
		w->setCaption(title);
		w->showMaximized();
	}
	if( id->exec() == 1)
	{
		printf("%s\n", id->getString().latin1());
		return 0;
	}
	else
	{
		return -1;
	}
}

int main(int argc, char **argv)
{
	int i;
	QStringList args;
	QPEApplication a(argc, argv);
	QWidget w;
	a.setMainWidget(&w);
	QWidget *d = a.desktop();
	int width=d->width();
	int height=d->height();
	
	for(i=0; i < argc; i++)
	{
		args += argv[i];
	}
	
	for(i=0; i < argc; i++)
	{
		if(args[i] == "-m")
		{
			return myMessageBox(width, height, &w, argc, args);
		}

		if(args[i] == "-f")
		{
			return fileviewer(&a, argc, args);
		}

		if(args[i] == "-i")
		{
			return input(width, height, &w, argc, args);
		}

		if(args[i] == "-h" || args[i] =="--help")
		{
			printusage();
			return -1;
		}
	}

	printusage();
	return -1;
}

