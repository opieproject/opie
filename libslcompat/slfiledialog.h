/*
 * Copyright (C) 2002 SHARP CORPORATION All rights reserved.
 */
#ifndef __SLFILEDIALOG_H__
#define __SLFILEDIALOG_H__

#include <qdialog.h>
#include <qfileinfo.h>
#include <qvalidator.h>
#include <sl/slmisc.h>

class SlFileNameValidator : public QValidator
{
  Q_OBJECT
public:
	SlFileNameValidator( QWidget * parent, const char * name = 0 ) : QValidator(parent,name) {};
	virtual State validate(QString&,int&) const;
	virtual void fixup(QString&) const;
};

class SlFileDialogPrivate;
class SlFileDialog : public QDialog
{
	Q_OBJECT
public:
	
	
	SlFileDialog(bool bSaveAs = FALSE, QWidget * parent=0, const char * name=0, bool modal=TRUE, WFlags f=0 );

	
	virtual ~SlFileDialog();

	
	void setNewDirEnabled(bool);

	
	void setDefaultFile( QString path );
	void setDefaultName( QString name );

	
	void setMimeType( QString mime );

	
	void setComplementExt( QString ext );
  
	
	void setIconViewType(bool isIcon);

	
	QString &getFilePath();
	QString &getFileName();

	int		exec();

protected slots:
	virtual void accept();

	void fileSelected(const QFileInfo &fInfo);
	void slotNewDir();
	void rotateChanged();
	void keyPressed(QKeyEvent*,bool&);
  
protected:
	void getDefaultColumnWidth(SlMisc::SlListColumnInfos &infos);

	SlFileDialogPrivate *m_pD;

};

#endif 
