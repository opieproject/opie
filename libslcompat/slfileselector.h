/*
 * Copyright (C) 2002 SHARP CORPORATION All rights reserved.
 */
#ifndef _SLFILESELECTOR_H_INCLUDED
#define _SLFILESELECTOR_H_INCLUDED

#include <opie2/ofileselector.h>
using namespace Opie;
#include <qhbox.h>
#include <qvbox.h>
#include <qlistview.h>
#include <qiconview.h>
#include <qtoolbutton.h>
#include <qfileinfo.h>
#include <qtableview.h>
#include <qwidgetstack.h>
#include <qvaluelist.h>

#include <qpe/applnk.h>
#include <qpe/mimetype.h>
#include <sl/slmisc.h>

class QComboBox;
class SlFileItemType;
class QCopChannel;

struct SlFileListItemPrivate;
class SlFileListItem
{
public:
	SlFileListItem(const QFileInfo&,const MimeType&);
	~SlFileListItem();
	const QFileInfo &file() const;
	const MimeType &mimeType() const;
	const QString getItemIconFilePath() const;
	const QString name() const;
	void setName(const QString&);
	const QString sortKey() const;
	void setSortKey(const QString&);
	QPixmap pixmap() const;
	QPixmap bigPixmap() const;
	const QString dateString() const;
	const QString typeString() const;
	const QString bytesString() const;
	void setDevice();
	bool isDevice() const;
	enum ColumnItem {
		Name = 1,
		Type,
		Date,
		Bytes,
		Custom = 100
	};
private:
	SlFileListItemPrivate *p;
	SlFileListItem(const SlFileListItem&);
};

typedef QValueList<SlFileListItem*> SlFileList;
typedef QValueList<SlFileListItem::ColumnItem> SlFileListColumnConf;

class SlFileListViewItem;

struct SlFileListViewPrivate;
class SlFileListView : public QListView
{
	Q_OBJECT

public:
	SlFileListView(QWidget*, const char*);
	~SlFileListView();
	void createFileList(SlFileList&);
	int fileCount() const;
	void updateItem(SlFileListViewItem*);
	void createItem(SlFileListItem*);
	void deleteItem(const QString&);
	void deleteItem(SlFileListViewItem*);
	SlFileListViewItem* searchItem(const QString&);
	SlFileListColumnConf columnConf();
	void setColumnConf(SlFileListColumnConf&);
	void updateColumn();
	static const QString columnItemName(SlFileListItem::ColumnItem);
	static const SlFileListColumnConf columnItemList();

	//virtual void takeItem( SlFileListViewItem * );

	SlMisc::SlListColumnInfos columnWidthInfos();

protected:
	//void keyPressEvent( QKeyEvent *e );

signals:
	void keyPressed(QKeyEvent *e,bool &isAccepted);

protected slots:

private:
	friend class SlFileSelector;
	SlFileListViewPrivate *p;
	SlFileListView(const SlFileListView&);
};

class SlFileIconViewItem;

struct SlFileIconViewPrivate;
class SlFileIconView : public QIconView
{
	Q_OBJECT

public:
	SlFileIconView(QWidget*, const char*);
	~SlFileIconView();
	void createFileList(SlFileList&);
	int fileCount() const;
	void updateItem(SlFileIconViewItem*);
	void createItem(SlFileListItem*);
	void deleteItem(const QString&);
	void deleteItem(SlFileIconViewItem*);
	SlFileIconViewItem* searchItem(const QString&);
	void setSorting(int column,bool ascending);
	int sortColumn() const;
	void repaintItemsForce();
	SlFileListColumnConf columnConf();
	void setColumnConf(SlFileListColumnConf&);

	//virtual void takeItem( SlFileIconViewItem * );

protected:
	//void keyPressEvent( QKeyEvent *e );

protected slots:

signals:
	void keyPressed(QKeyEvent *e,bool &isAccepted);

private:
	friend class SlFileSelector;
	SlFileIconViewPrivate *p;
	SlFileIconView(const SlFileIconView&);
};

class SlFileSelectorFilter : public QObject
{
public:
	SlFileSelectorFilter(QWidget *parent,const char *name=0);
	~SlFileSelectorFilter();

	virtual bool isAddToList(QFileInfo&) const = 0;
private:
	SlFileSelectorFilter(const SlFileSelectorFilter&);
};

typedef QList<MimeType> SlFileMimeTypeList;

struct SlFileSelectorPrivate;
class SlFileSelector : public OFileSelector /*QVBox*/
{
	Q_OBJECT

public:


	enum SlFileSelectorViewType {

		ListView = 1,

		IconView
	};


	enum SlFileOverwirteMode {

		Overwrite = 1,

		NewName,

		Error
	};
	SlFileSelector(const QString &dirPath,
				   const QString &mimefilter,
				   QWidget *parent,
				   const char *name);
	SlFileSelector(const QString &currentFileName,
				   SlFileSelectorViewType type,
				   const QString &mimeFilter,
				   SlFileListColumnConf &items,
				   int sortColumn,bool sortAscending,
				   SlFileSelectorFilter *filter=NULL,
				   bool isRecursive=FALSE,
				   QWidget *parent=NULL,
				   const char *name=NULL,
				   WFlags f=0);
	SlFileSelector(QWidget *parent=NULL,const char *name=NULL,WFlags f=0);
	virtual ~SlFileSelector();
	void createFileList();
	void createFileList(QDir&);
	int fileCount() const;
	const QFileInfo* selected();
	void modifyItem(QFileInfo*,bool);
	void createItem(QString&);
	void deleteItem(const QString&);
	void deleteItem();
	void updateItem(const QString&);
	SlFileListItem* searchItem(const QString&);
	void setSelected();
	void setSelected(const QString&);
	void ensureItemVisible();
	SlFileListItem* selectedItem();
	bool setSelectedNext(bool);
	bool setSelectedPrev(bool);
	const QString currentDir();
	void setSorting(int,bool);
	int sortColumn() const;
	bool ascending() const;
	SlFileSelectorViewType viewType();
	void setColumnItem(SlFileListColumnConf&);
	void setViewType(SlFileSelectorViewType);
	void setListView();
	void setIconView();
	int columnWidth(int);
	void setColumnWidth(int,int);
	static const QString columnItemName(SlFileListItem::ColumnItem);
	static const SlFileListColumnConf columnItemList();

	int columns() const;

	bool upDir();
	bool isTopDir();

	void setCurrentFile(const QString&);
	const QString currentFile(bool isReal=FALSE);
	static bool copyFile(const QString&,const QString&,
						 SlFileOverwirteMode m=Error,
						 QString *resultName=NULL);

	static bool copyFileCreateDir(const QString &srcName,
								  const QString &dstName,
								  SlFileOverwirteMode m=Error,
								  QString *resultName=NULL);
	static bool moveFile(const QString&,const QString&,
						 SlFileOverwirteMode m=Error,
						 QString *resultName=NULL);
	static QString getCopyFileName(const QString&);

	const QStringList& recent() const;
	void setRecent(const QStringList&);


	void setRecent(const QStringList&,bool isReplace);
	void setRecentEnabled(bool);

	const QStringList& history() const;
	void setHistory(const QStringList&);


	void setHistory(const QStringList&,bool isReplace);
	bool isHistoryBackAvailable() const;
	bool isHistoryForwardAvailable() const;
	bool historyBack();
	bool historyForward();
	QPopupMenu *backHistoryPopupMenu() const;
	QPopupMenu *forwardHistoryPopupMenu() const;
	void setColumnConf(SlFileListColumnConf&);

	void sendFileUpdateMessage(const QString&);
	void updateView();
	void recreateIcon();
	void focusToNext();
	void focusToPrev();
	bool rename();
	bool newfolder();
	bool del();
	bool duplicate();
	bool cut();
	bool copy();
	bool paste();
	void setMimeFilter(const QString &);
	void clearTmpFolder();

	void setRecursive(bool,bool);
	bool recursive() const;
	void setFilter(SlFileSelectorFilter *);
	SlFileSelectorFilter *filter();
	bool isEnablePaste();

	static bool deleteFile(const QString&);
	static bool deleteDir(const QDir&);
	static bool copyDir(const QString &srcName,const QString &dstName,
						SlFileOverwirteMode m=Error);
	static bool moveDir(const QString&,const QString&,
						SlFileOverwirteMode m=Error);
	static QString getCopyDirName(const QString &);
	static bool makeDir(const QString &path);

	static uint size(const QString &fName);
	static uint fileFree(const QString &fName);



	enum DesktopMode {

		HideDesktop = 1,

		ShowAll,

		FileTab
	};
	void setDesktopMode(DesktopMode mode);
	void setEnabledSystemChannel(bool);
	void showUpDirBtn(bool);
	void showWritableFile(bool);


	void setEnableSD(bool);
	void setEnableCF(bool);
	bool getEnableSD();
	bool getEnableCF();




	void sendKeyPressEvent( QKeyEvent *e );

	void setScale(int scale);
	int scale() const;

	SlMisc::SlListColumnInfos columnWidthInfos();



	enum FocusMode {

		Auto = 1,

		ByTab,

		ByCommand
	};
	void setFocusMode(FocusMode mode);

	void adjustContentsSize();

	static void updateFile(const QString &filename);

	bool delFile(const QString &mymsg);

	bool isIncludedFile(const QString &filename);

signals:


	void selectionChanged(const QFileInfo &file,bool isDevice);


	void fileSelected(const QFileInfo &file);


	void dirOpened(const QDir &dir);


	void enabledPaste(bool);


	void columnWidthChanged(int column,int oldSize,int newSize);


	void itemRightPressed(const QFileInfo &file,const QPoint &pos);


	void selectionLost();


	//void keyPressed(QKeyEvent *e,bool &isAccepted);
/*
private slots:
	void fileClicked(QListViewItem *);
	void fileClicked(QIconViewItem *);
	void selectionChanged(QListViewItem *);
	void selectionChanged(QIconViewItem *);
	void dMenuChanged(int);
	void changeDirectory(int,bool isImmediate=TRUE);
	void hMenuSelected(int);
	void cardMessage( const QCString &, const QByteArray &);
	void systemMessage( const QCString &, const QByteArray &);
	void headerClicked(int);
	void asyncHMenuSelected();
	void asyncDMenuChanged();
	void rightPressed(QIconViewItem *,const QPoint&);
	void rightPressed(QListViewItem *,const QPoint&,int);
	void upDirClicked();
	void slotKeyPressed(QKeyEvent *e,bool &isAccepted);
*/
protected:
	void focusInEvent(QFocusEvent*);
	void show();
	void keyPressEvent( QKeyEvent *e );

private:
	void openDirInside(QDir&);
	void createMyPda();
	void init();
	SlFileSelectorPrivate *p;
	SlFileSelector(const SlFileSelector&);
	ulong focusWidgets() const;
	QWidget* nextFocus(QWidget *,bool) const;
	bool focusToTheWidget(QWidget *);
};

#endif





