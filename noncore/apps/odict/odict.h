/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qmainwindow.h>

class QLabel;
class QVBox;
class QPopupMenu;
class QMenuBar;
class QHBox;
class QPushButton;
class QLineEdit;
class QAction;
class QVBoxLayout;
class QActionGroup;
class DingWidget;
class QTextBrowser;
class QComboBox;
class DingWidget;

class ODict : public QMainWindow
{
	Q_OBJECT
	
    public:
		ODict(QWidget* parent = 0, const char* name = 0, WFlags fl = 0  );
		QVBox *vbox;
		QTextBrowser *browser_top,
					 *browser_bottom;
		DingWidget *ding;
		static QString appName() { return QString::fromLatin1("odict"); }
	private:
		QPopupMenu *help, *settings, *parameter;
		QMenuBar *menu;
		QHBox *hbox;
		QLineEdit *query_le;
		QComboBox *query_co;
		QPushButton *ok_button;

		QVBoxLayout *vbox_layout;

		QAction *setting_a, *setting_b;
		
		void setupMenus();

		bool casesens, completewords;
		
		void loadConfig();
		void saveConfig();

		QString activated_name;
		QLabel  *bottom_name,
				*top_name;

		QString top_name_content, bottom_name_content;

		void lookupLanguageNames( QString );
	
	private slots:
		void slotStartQuery();
		void slotSettings();
		void slotSetParameter( int );
		void slotMethodChanged( const QString& );
};
