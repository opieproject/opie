#ifndef _OPIETOOTH_PLUGIN_INTERFACE_H_
#define _OPIETOOTH_PLUGIN_INTERFACE_H_

#include "manager_base.h"

#include <qpe/qcom.h>
#include <qpe/qlibrary.h>
#include <qpe/qpeapplication.h>


namespace OpieTooth {

	/** Abstract base-class must be implemented by the plugin
	 */
	struct Manager_PluginInterface: public QUnknownInterface {
		/**
		 * return the Manager implementation
		 */
		virtual Manager_Base* manager() = 0;
	};


	/** This class contains all Informations about the loaded Plugin
	 */
	class OpieToothPlugin{
	public:
		OpieToothPlugin(): 
			m_library( 0l ), m_iface( 0 ){}
		OpieToothPlugin( QLibrary* library, QInterfacePtr<Manager_PluginInterface>& iface ): 
			m_library( library ), m_iface( iface ){}
		OpieToothPlugin( const OpieToothPlugin& plugin ){
			m_library = plugin.m_library;
			m_iface = plugin.m_iface;
		}
		void setPluginInterfacePtr( QInterfacePtr<Manager_PluginInterface>& iface ){
			m_iface = iface;
		}
		void setLibrary( QLibrary* libr ){
			m_library = libr;
		}
		QInterfacePtr<Manager_PluginInterface> pluginInterfacePtr(){
			return m_iface;
		}
		QLibrary* library(){
			return m_library;
		}
	private:
		QLibrary* m_library;
		QInterfacePtr<Manager_PluginInterface> m_iface;		
	};

	/** This factory returns the bluetooth library
	 * It loads the plugin if needed.. 
	 */
	class LibOpieTooth_Factory{
	public:

		/** Returns the Pointer to the bluetooth-ressource manager
		 */
		static OpieToothPlugin library( ){

			OpieToothPlugin lib;
			QInterfacePtr<Manager_PluginInterface> pluginPtr;

			QString path = QPEApplication::qpeDir() + "/plugins/opietooth";
			QDir dir( path, "lib*.so" );	
			QStringList list = dir.entryList();
			
			// Take the first found plugin ..
			// FIXME: It should be possible to select the plugin (se)
			QStringList::Iterator it;
			for ( it = list.begin(); it != list.end(); ++it ) {
				qDebug( "Accessing: %s", QString( path + "/" + *it ).latin1() );

				QLibrary* pluginLib = new QLibrary( path + "/" + *it );
				if ( pluginLib ){
					// "&pluginPtr" returns a pointer to the pointer to the member class Manager_Base 
					// (see operator overloading of "&" in QInterfacePtr), which is the child of 
					// "QUnknownInterface"..
					if ( pluginLib->queryInterface( IID_QUnknown, (QUnknownInterface**)&pluginPtr ) == QS_OK ) {
						qDebug( "accepted: %s", QString( path + "/" + *it ).latin1() );
						lib.setLibrary( pluginLib );
						lib.setPluginInterfacePtr( pluginPtr );
						break;
					}
				} else {
					qWarning( "Was unable to load Library !!");
				}
			}


			return lib;
		}
	};
	
       
}

#endif
