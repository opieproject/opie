#ifndef __TABCONFIG_H__
#define __TABCONFIG_H__

struct TabConfig {
	enum ViewMode {	
		Icon,
		List
	};
	enum BackgroundType {
		Ruled,
		SolidColor,
		Image
	};

	ViewMode       m_view;
	BackgroundType m_bg_type;
	QString        m_bg_image;
	QString        m_bg_color;
	QString        m_text_color;
	QString        m_font_family;
	QString        m_font_style;
	int            m_font_size;
	bool           m_changed;
};

#endif
