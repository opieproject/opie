/*
 * Copyright (C) 2002 SHARP CORPORATION All rights reserved.
 */
#ifndef _SLMISC_H_INCLUDED
#define _SLMISC_H_INCLUDED

#include <qpe/qpeapplication.h>
#include <qfileinfo.h>
#include <qpe/global.h>
#include <qmessagebox.h>


#define _SLPOWER

class QPopupMenu;
class QListView;
#if !defined(_SLPOWER)
class PowerStatus;
#else
class SlPowerStatus;
#endif
class Config;

typedef ulong SlResolutionScale;

class SlMisc {
public:
	static bool isExecutable(const QString &c) {
		QFileInfo exec(QPEApplication::qpeDir() + "/bin/" + c);
		return exec.isExecutable();
	};
	static ulong availableMemory();
	static bool invalidFileName(QString &fName);
	static QString getValidFileName(const QString &baseName);
	static QString getCanonicalPath(const QString &fName);
	static void enableAutoPowerOff(bool isOn);
	static void enableAutoPowerOff(bool isOn,bool DimAlso,bool ScreenSaverAlso=FALSE);
	static void enableLockApplication(bool);
	static bool isTheSmbShareFileHasWriteEntry(const QString &shareFName);
	static bool isTheSmbShareFileNowCopy(const QString &shareFName);
	static bool checkTheSmbShareFileMode(const QString &shareFName,
						 char modeMask, char checkMode, bool equality);
	static bool isTheFileLockedBySmbd(const QString &fName);
	static bool isSambaForked();
	static void startSamba();
	static void stopSamba();
	enum ListType { Pid, CmdLine, Both };
	static QStringList procList (const QRegExp& re, uint max = 0, enum ListType type = Pid);
	static void dumpMemInfo(bool isShowHeap);
	static bool canCreateTheFile(const QString&);
	static QString getLanguage();
	static QSize getMaximizedWidgetInfo();
	static QSize SlMisc::getScrollbarMetrics();
	static int getDpi();
	static SlResolutionScale getResolutionScale();
	static int getMaximizedWidgetContentsWidth();
	static bool isLandscape();
	static void requestCloseFile(const QString&);
	static void requestOpenFile(const QString&);

	static void insertStandardFontMenu(QPopupMenu *pm);
	static void insertAllFontMenu(QPopupMenu *pm);
	static int getLogicalSizeFromMenuId(int id);
	static void setFontSizeItemChecked(QPopupMenu *pm,int logicalSize);
	static int getFontPointSize(int logicalSize);
	static void toggleFontSize(QPopupMenu *pm,int &logicalSize,bool isLarge=true);
	static void toggleAllFontSize(QPopupMenu *pm,int *fontSize,bool isLarge=true);

	static void SlFontArrange (QWidget* widget);
	static void SlLayoutSpaceArrange (QObject* object);

	static bool checkBatteryError(const QString &module_name, bool force);
	static bool checkBatteryErrorWithoutDialog(const QString &module_name, bool force);

	
	enum InputDeviceType {
		BuiltinKeyboard = 1,
		SoftwareKeyboard = 2
	};
	static InputDeviceType defaultInputDevice();

	
	typedef struct SlListColumnInfo {
		enum Type {
			Fixed = 0,
			StringLen,
			Scale,
			FixedDpiAware
		};
		Type type;
		QString str;
		int val;
	};
	typedef QValueList<SlListColumnInfo> SlListColumnInfos;

	static void setListColumnsWidth(QWidget *w,SlListColumnInfos &infos,
									Config &config);
	static void writeListColumnsWidth(QWidget *w,SlListColumnInfos &infos,
									  Config &config);
	static void writeListColumnsWidth(QWidget *w,SlListColumnInfos &infos,
									  Config &config,bool isLandscape);	
	static void rotateListColumnsWidth(QWidget *w,SlListColumnInfos &infos,
									   Config &config);
	static void setListColumnsWidth(QWidget *w,SlListColumnInfos &infos, Config *config=NULL);
	static void writeListColumnsWidth(QWidget *w,SlListColumnInfos &infos, Config *config=NULL);
	static void writeListColumnsWidth(QWidget *w,SlListColumnInfos &infos, bool isLandscape, Config *config=NULL);
	static void rotateListColumnsWidth(QWidget *w,SlListColumnInfos &infos, Config *config=NULL);
	static void temporaryMessageBox(const QString &caption, const QString &text, int msec=10000, QMessageBox::Icon icon=QMessageBox::NoIcon, int button0=0, int button1=0, int button2=0, QWidget *parent=0, const char *name=0, QWidget::WFlags f=QWidget::WStyle_DialogBorder);
	
	static QString defaultWallPaper();
	typedef enum AppCommand {
		NoneCommand = 0,
		NewCommand,
		ModifyCommand,
		DeleteCommand,
		ZoomInCommand,
		ZoomOutCommand,
		CutCommand = 50,
		CopyCommand,
		PasteCommand
	};
	static AppCommand appCommandFromKey(const QKeyEvent &e);

	typedef enum RequiredSize {
		SmallSize = 0,
		LargeSize
	};
	static const char* getTmpPath(RequiredSize);
	static const char* getTmpPath(ulong);
	static QString getObexTmpPath();
	static ulong availableInternalStorage(void);
	static QString moduleNameFromPath(const QString &path);
};

#endif 
