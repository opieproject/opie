#ifndef PROFILE_EDITOR_PLUGINS_H
#define PROFILE_EDITOR_PLUGINS_H

#include "profile.h"

#include "qobject.h"

class QWidget;

class ProfileEditorPlugin : public QObject
{
	Q_OBJECT
	public:
		ProfileEditorPlugin(QWidget *parent, Profile p);

		virtual ~ProfileEditorPlugin();

		virtual void save() = 0;

		virtual QWidget *widget() = 0;

		QWidget *connection_widget();
		QWidget *terminal_widget();

	public slots:
		void slotConnFlow(int id);
		void slotConnParity(int id);
		void slotConnSpeed(int id);
		void slotTermTerm(int id);
		void slotTermColour(int id);
		void slotTermFont(int id);
		void slotTermEcho(bool on);
		void slotTermWrap(bool on);
		void slotTermInbound(bool on);
		void slotTermOutbound(bool on);

	protected:
		QWidget *m_parent, *m_widget;
		Profile m_profile;

	private:
		enum ParityIds
		{
			id_parity_odd,
			id_parity_even
		};

		enum FlowIds
		{
			id_flow_hw,
			id_flow_sw
		};

		enum SpeedIds
		{
			id_baud_115200,
			id_baud_57600,
			id_baud_38400,
			id_baud_19200,
			id_baud_9600
		};

		enum TermIds
		{
			id_term_vt100,
			id_term_vt220,
			id_term_ansi
		};

		enum ColourIds
		{
			id_term_black,
			id_term_white
		};

		enum FontIds
		{
			id_size_small,
			id_size_medium,
			id_size_large
		};
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

