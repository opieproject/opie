/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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
#include "readmail.h"
#include <qimage.h>
#include <qmime.h>
#include <qaction.h>
#include <qpe/resource.h>

ReadMail::ReadMail( QWidget* parent,  const char* name, WFlags fl )
	: QMainWindow(parent, name, fl)
{
	plainTxt = FALSE;
	
	init();
	viewAtt = new ViewAtt(0, "View Attatchments");
}

ReadMail::~ReadMail()
{
	delete emailView->mimeSourceFactory();
	delete viewAtt;
}

void ReadMail::init()
{
	setToolBarsMovable(FALSE);
	
	bar = new QToolBar(this);
	bar->setHorizontalStretchable( TRUE );

	menu = new QMenuBar( bar );

	viewMenu = new QPopupMenu(menu);
	menu->insertItem( tr( "&View" ), viewMenu);

	mailMenu = new QPopupMenu(menu);
	menu->insertItem( tr( "&Mail" ), mailMenu);

	bar = new QToolBar(this);
	
    	//reply dependant on viewing inbox
	replyButton = new QAction( tr( "Reply" ), Resource::loadPixmap( "mailit/reply" ),
		QString::null, 0, this, 0 );
	connect(replyButton, SIGNAL(activated()), this, SLOT(reply()) );
  	replyButton->setWhatsThis(tr("Click here to reply to the selected mail"));
		
	forwardButton = new QAction( tr( "Forward" ), Resource::loadPixmap( "mailit/forward" ),
	QString::null, 0, this, 0 );
	connect(forwardButton, SIGNAL(activated()), this, SLOT(forward()) );
	forwardButton->setWhatsThis(tr("Click here to forward the selected mail"));
	
	previousButton = new QAction( tr( "Previous" ), Resource::loadPixmap( "back" ), QString::null, 0, this, 0 );
	connect( previousButton, SIGNAL( activated() ), this, SLOT( previous() ) );
	previousButton->addTo(bar);
	previousButton->addTo(viewMenu);
	previousButton->setWhatsThis(tr("Read the previous mail in the list"));

	nextButton = new QAction( tr( "Next" ), Resource::loadPixmap( "forward" ), QString::null, 0, this, 0 );
	connect( nextButton, SIGNAL( activated() ), this, SLOT( next() ) );
	nextButton->addTo(bar);
	nextButton->addTo(viewMenu);
	previousButton->setWhatsThis(tr("Read the next mail in the list"));

	attachmentButton = new QAction( tr( "Attatchments" ), Resource::loadPixmap( "mailit/attach" ), QString::null, 0, this, 0 );
	connect( attachmentButton, SIGNAL( activated() ), this,
		SLOT( viewAttachments() ) );
	attachmentButton->addTo(bar);
	attachmentButton->addTo(viewMenu);
	attachmentButton->setWhatsThis(tr("Click here to add attachments to your mail"));
	
	plainTextButton = new QAction( tr( "Text Format" ), Resource::loadPixmap( "DocsIcon" ), QString::null, 0, this, 0, TRUE);
	connect( plainTextButton, SIGNAL( activated() ), this, SLOT( shiftText() ) );
	plainTextButton->addTo(bar);
	plainTextButton->addTo(viewMenu);
	plainTextButton->setWhatsThis(tr("The mail view has 2 modes:\n"
	"<LI><B>RichText</B> shows the mail as HTML with reach features (no standard line breaks)</LI>"
	"<LI><B>Plain</B> shows the mail as standard plain text</LI>"
	"Click here to switch between those view modes" ));

	deleteButton = new QAction( tr( "Delete" ), Resource::loadPixmap( "trash" ), QString::null, 0, this, 0 );
	connect( deleteButton, SIGNAL( activated() ), this, SLOT( deleteItem() ) );
	deleteButton->addTo(bar);
	deleteButton->addTo(mailMenu);
	deleteButton->setWhatsThis(tr("Click here to remove the selected mail"));

	viewMenu->insertItem(Resource::loadPixmap("close"), "Close", this, SLOT(close()));
		
	emailView = new QTextView( this, "emailView" );
	
	setCentralWidget(emailView);

	mime = new QMimeSourceFactory();
	emailView->setMimeSourceFactory(mime);
}

void ReadMail::updateView()
{
	Enclosure *ePtr;
	QString mailStringSize;
	QString text, temp;
	
	mail->read = TRUE;			//mark as read
	inbox = mail->received;
	
	replyButton->removeFrom(mailMenu);
	replyButton->removeFrom(bar);
	forwardButton->removeFrom(mailMenu);
	forwardButton->removeFrom(bar);
	
	if (inbox == TRUE) {
		replyButton->addTo(bar);
		replyButton->addTo(mailMenu);
		forwardButton->addTo(bar);
		forwardButton->addTo(mailMenu);

				
		if (!mail->downloaded) {
			//report currently viewed mail so that it will be
			//placed first in the queue of new mails to download
			emit viewingMail(mail);
			
			double mailSize = (double) mail->size;
			if (mailSize < 1024) {
				mailStringSize.setNum(mailSize);
				mailStringSize += " Bytes";
			} else if (mailSize < 1024*1024) {
				mailStringSize.setNum( (mailSize / 1024), 'g', 2 );
				mailStringSize += " Kb";
			} else {
				mailStringSize.setNum( (mailSize / (1024*1024)), 'g', 3);
				mailStringSize += " Mb";
			}
		}
	}

	QMimeSourceFactory *mime = emailView->mimeSourceFactory();
	
	if (! plainTxt) {						//use RichText, inline pics etc.
		emailView->setTextFormat(QTextView::RichText);
		text = "<b><big><center><font color=\"blue\">" + mail->subject
			+"</font></center></big></b><br>";
		text += "<b>From: </b>" + mail->from + " <i>" +
			mail->fromMail + "</i><br>";
	
		text +="<b>To: </b>";
	        for (QStringList::Iterator it = mail->recipients.begin();
	        	it != mail->recipients.end(); ++it ) {
			text += *it + " ";
		}

		text +="<br><b>CC: </b>";
	        for (QStringList::Iterator it = mail->carbonCopies.begin();
	        	it != mail->carbonCopies.end(); ++it ) {
			text += *it + " ";
		}
		
		text += "<br>"  + mail->date;
	
		if (mail->files.count() > 0) {
			text += "<br><b>Attatchments: </b>";
			
			for ( ePtr=mail->files.first(); ePtr != 0; ePtr=mail->files.next() ) {
				text += ePtr->originalName + " ";
			}
			text += "<hr><br>" + mail->body;
		
			if (inbox) {
				for ( ePtr=mail->files.first(); ePtr != 0; ePtr=mail->files.next() ) {

					text += "<br><hr><b>Attatchment: </b>" +
						ePtr->originalName + "<hr>";
				
					if (ePtr->contentType == "TEXT") {
						QFile f(ePtr->path + ePtr->name);
					
						if (f.open(IO_ReadOnly) ) {
							QTextStream t(&f);
							temp = t.read();
							text += temp + "<br>";
							f.close();
						} else {
							text += "<b>Could not locate file</b><br>";
						}
					
					}
					if (ePtr->contentType == "IMAGE") {
//						temp.setNum(emailView->width());	//get display width
//						text += "<img width=" + temp +" src =""" +
//							ePtr->originalName + """> </img>";
						text += "<img src =""" +
							ePtr->originalName + """> </img>";
						mime->setPixmap(ePtr->originalName, QPixmap( (ePtr->path + ePtr->name) ));
					}
				}
			}
		} else {
			if (mail->downloaded || !inbox) {
				text += "<hr><br>" + mail->body;
			} else {
				text += "<hr><br><b> Awaiting download </b><br>";
				text += "Size of mail: " + mailStringSize;
			}
		}
		emailView->setText(text);
	} else {										// show plain txt mail
		emailView->setTextFormat(QTextView::PlainText);
		text = "Subject: " + mail->subject + "\n";
		text += "From: " + mail->from + " " + mail->fromMail + "\n";
		text += "To: ";
	        for (QStringList::Iterator it = mail->recipients.begin();
	        	it != mail->recipients.end(); ++it ) {
			text += *it + " ";
		}
		
		text += "\nCC: ";
	        for (QStringList::Iterator it = mail->carbonCopies.begin();
	        	it != mail->carbonCopies.end(); ++it ) {
			text += *it + " ";
		}

				
		text += "\nDate: " + mail->date + "\n";
		if (mail->files.count() > 0) {
			text += "Attatchments: ";
			for ( ePtr=mail->files.first(); ePtr != 0; ePtr=mail->files.next() ) {
				text += ePtr->originalName + " ";
			}
			text += "\n\n";
		} else text += "\n";
		
		if (!inbox) {
			text += mail->body;
		} else if (mail->downloaded) {
			text += mail->bodyPlain;
		} else {
			text += "\nAwaiting download\n";
			text += "Size of mail: " + mailStringSize;
		}
		
		emailView->setText(text);
	}
	
	if (mail->files.count() == 0)
		attachmentButton->setEnabled(FALSE);
	else attachmentButton->setEnabled(TRUE);
	
	setCaption("Examining mail: " + mail->subject);
}

//update view with current EmailListItem (item)
void ReadMail::update(QListView *thisView, Email *mailIn)
{
	view = thisView;
	item = (EmailListItem *) view->selectedItem();
	mail = mailIn;
	updateView();
	updateButtons();	
}

void ReadMail::mailUpdated(Email *mailIn)
{
	if (mailIn == mail) {
		updateView();
	} else {
		updateButtons();
	}
}

void ReadMail::close()
{
	emit cancelView();
}

//gets next item in listview, exits if there is no next
void ReadMail::next()
{
	item = (EmailListItem *) item->nextSibling();
	if (item != NULL) {
		mail = item->getMail();
		updateView();
	}
	updateButtons();
}

//gets previous item in listview, exits if there is no previous
void ReadMail::previous()
{
	item = (EmailListItem *) item->itemAbove();
	if (item != NULL) {
		mail = item->getMail();
		updateView();
	}
	updateButtons();
}

//deletes item, tries bringing up next or previous, exits if unsucessful
void ReadMail::deleteItem()
{
	EmailListItem *temp = item;
	temp = (EmailListItem *) item->nextSibling();	//trybelow
	if (temp == NULL)
		temp = (EmailListItem *) item->itemAbove(); //try above
	
	emit removeItem(item, inbox);
	
	item = temp;
	if (item != NULL) {								//more items in list
		mail = item->getMail();
		updateView();
		updateButtons();
	} else close();								//no more items to see
}

void ReadMail::updateButtons()
{
	EmailListItem *temp;
	
	temp = item;
	if ((EmailListItem *) temp->nextSibling() == NULL)
		nextButton->setEnabled(FALSE);
	else nextButton->setEnabled(TRUE);
	
	temp = item;
	if ((EmailListItem *) temp->itemAbove() == NULL)
		previousButton->setEnabled(FALSE);
	else previousButton->setEnabled(TRUE);
}

void ReadMail::shiftText()
{
	plainTxt = ! plainTxt;
	updateView();
}

void ReadMail::viewAttachments()
{
	viewAtt->update(mail, inbox);
	viewAtt->showMaximized();
}

void ReadMail::reply()
{
	emit replyRequested(*mail, (bool&)TRUE);
}

void ReadMail::forward()
{
	emit forwardRequested(*mail);
}

