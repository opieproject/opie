#ifndef __TABS_SETTINGS_H__
#define __TABS_SETTINGS_H__

#include <qwidget.h>
#include <qmap.h>

class QListView;


class TabsSettings : public QWidget {
	Q_OBJECT

public:
	TabsSettings ( QWidget *parent = 0, const char *name = 0 );

	void accept ( );

	enum ViewMode {
		Icon,
		List
	};
	enum BackgroundType {
		Ruled,
		SolidColor,
		Image
	};
	struct TabSettings {
		ViewMode       m_view;
		BackgroundType m_bg_type;
		QString        m_bg_image;
		QString        m_bg_color;
		QString        m_text_color;
		QString        m_font_family;
		int            m_font_size;
		bool           m_changed;
	};

protected:
	void init ( );
	void readTabSettings ( );

private:
	QListView *m_list;
//	QString currentTab;
	QStringList m_ids;
	QMap <QString, TabSettings> m_tabs;
};




#endif
