#ifndef PROFILE_EDITOR_PLUGINS_H
#define PROFILE_EDITOR_PLUGINS_H

#include "profile.h"

class QWidget;

class ProfileEditorPlugin
{
	public:
		ProfileEditorPlugin(QWidget *parent, Profile p);

		virtual ~ProfileEditorPlugin();

		virtual void save() = 0;

		virtual QWidget *widget() = 0;

	protected:
		QWidget *m_parent, *m_widget;
		Profile m_profile;
};

//#ifdef __cplusplus
//extern "C" {
//#endif

ProfileEditorPlugin *factory_serial(QWidget *parent, const Profile& p);
ProfileEditorPlugin *factory_irda(QWidget *parent, const Profile& p);
ProfileEditorPlugin *factory_modem(QWidget *parent, const Profile& p);

//#ifdef __cplusplus
//}
//#endif

#endif

