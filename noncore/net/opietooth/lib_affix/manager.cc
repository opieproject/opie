#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <opie/oprocess.h>


#include "manager.h"
#include "../include/remotedevice.h"

#include <affix/bluetooth.h>
#include <affix/btcore.h>
#include <affix/sdpclt.h>

using namespace OpieTooth;

Manager::Manager( const QString& mac )
{
	qWarning("created");
	m_device = mac;
	m_hcitool = 0;
	m_sdp = 0;
}
Manager::Manager( Device* /*dev*/ )
{
	m_hcitool = 0;
	m_sdp = 0;
}
Manager::Manager()
{
	m_hcitool = 0;
	m_sdp = 0;
}
Manager::~Manager(){
	delete m_hcitool;
	delete m_sdp;
}
void Manager::setDevice( const QString& mac ){
	m_device = mac;
}
void Manager::setDevice( Device* /*dev*/ ){
	
}
void Manager::isAvailable( const QString& mac )
{
	// This function will try to send a ping to the given mac.
	// If it return this ping it should be available..
	// It if it returns nothing ( as MacOS X does ), we 
	// are in trouble.. =:S 
	// Maybe we should just open a connection, and returning true
	// if this works..
	
	qWarning( "Manager::isAvailable: Try to ping to: %s", mac.latin1() );
	
	struct sockaddr_affix sa;
	int fd, err;
	
	fd = socket( PF_AFFIX, SOCK_SEQPACKET, BTPROTO_L2CAP );
	if ( fd < 0 ){
		qWarning( "Manager::isAvailable: Socket not available !" );
		emit available( mac, false );
		return;
	}
	
	// Hmmm. There must be a better solution.. :SX
	char* buffer = (char*) malloc( strlen( mac.latin1() ) + 1 );
	strncpy( buffer, mac.latin1(), strlen( mac.latin1() ) + 1 );
	
	sa.family = PF_AFFIX;
	str2bda( &sa.bda, buffer );
	sa.port = 0;
	sa.local = BDADDR_ANY;
	
	free( buffer );
	
	err = ::connect( fd, ( struct sockaddr* ) &sa, sizeof( sa ) );
	if ( err ) {
		qWarning( "Manager::isAvailable: Error connecting mac: %s", mac.latin1() );
		emit available( mac, false );
		close( fd );
		return;
	}
	char data[8];
	strncpy ( data, "abcdxx\0", 4 );
	if ( l2cap_singleping( fd, data , 4 ) != 4 ){
		qWarning( "Manager::isAvailable: Error sending ping to: %s", mac.latin1() );
		emit available( mac, false );
		close( fd );
		return;
	}
	
	close( fd );
	
	qWarning( "Manager::isAvailable: YES--Ping returned: %s", mac.latin1() );
	emit available( mac, true );
	
}

void Manager::isAvailable( Device* /*dev*/ ){
	
	// isAvailable( dev->mac() );
	
}

void Manager::searchDevices( const QString& device ){
	qWarning("search devices");
	
	__u32		length = 8;
	int		        err, fd;
	INQUIRY_ITEM	devs[20];
	char		namebuf[248];
	__u8		num;
	
	fd = hci_open( device.isEmpty() ? (char*)"bt0" : (char*)device.latin1() );
	if (fd < 0) {
		qWarning( "Unable to open device %s: %s\n", device.isEmpty() ? "bt0" : device.latin1() , 
			  strerror(errno));
		return;
	}
	printf("Searching %d sec ...\n", length);
	err = HCI_Inquiry(fd, length, 20, devs, &num);
	if (err) {
		qWarning( "%s", hcierror(err));
		exit(1);
	}
	
	RemoteDevice::ValueList list;
	//     RemoteDevice r_device( ); QString( *bda2str(&devs->bda)
	if (num == 0) {
		qWarning("done.\nNo devices found.\n");
	} else {
		qWarning("Searching done. Resolving names ...\n");
		for (int i = 0; i < num; i++) {
			RemoteDevice* r_device = new RemoteDevice( QString( bda2str(&(devs[i].bda)) ), 
								   QString( "UNKNOWN" ) );
			// 		    r_device.setMac( QString( *bda2str(&devs->bda) ) );
			err = HCI_RemoteNameRequest(fd, &devs[i], namebuf);
			if (!err)
				r_device->setName( QString( namebuf ) );
			list.append( *r_device );
			
			qWarning( "Found bda: %s with name %s", r_device->mac().latin1(), 
				  r_device->name().latin1() );
			delete r_device;
		}
		qWarning("done.\n");
	}
	
	emit foundDevices( device, list );
}

void Manager::searchDevices(Device* /*d*/ ){
	qWarning( "This call is not defined ! " );
	
	
}
void Manager::addService(const QString& name ){
	
	qWarning("Manager::addService( %s ) not implemented !", name.latin1() );
	
	//    OProcess proc;
	//    proc << "sdptool" << "add" << name;
	//     bool bo = true;
	//     if (!proc.start(OProcess::DontCare ) )
	//         bo = false;
	emit addedService( name, false );
}
void Manager::addServices(const QStringList& list){
	QStringList::ConstIterator it;
	for (it = list.begin(); it != list.end(); ++it )
		addService( (*it) );
}

void Manager::removeService( const QString& name ){
	
	qWarning("Manager::removeService( %s ) not implemented !", name.latin1() );
	
	//     OProcess prc;
	//     prc << "sdptool" << "del" << name;
	//     bool bo = true;
	//     if (!prc.start(OProcess::DontCare ) )
	//         bo = false;
	emit removedService( name,  false );
}

void Manager::removeServices( const QStringList& list){
	QStringList::ConstIterator it;
	for (it = list.begin(); it != list.end(); ++it )
		removeService( (*it) );
}

// This function was extracted out of the AFFIX-Utilies
int Manager::extractGroups(UUID *groupToExtract, SDPServerHandle srvHandle, 
			   Services::ValueList& listOfServices )
{
	int status = -1, i;
	uint16_t count = 0;
	slist_t *searchList = NULL;
	slist_t *attrIdList = NULL;
	slist_t *svcRecList = NULL;
	uint32_t range = 0x0000ffff;
	slist_t *pslist_t = NULL;
	AccessProtocols *pAccessProtocols = NULL;
	
	s_list_append(&searchList, groupToExtract);
	s_list_append(&attrIdList, &range);
	status = SDPServiceSearchAttributeRequest( srvHandle, searchList, 
						   RangeOfAttributes, attrIdList, 0xFFFF, 
						   &svcRecList, &count);
	//printf("Status : %d Count : %d\n", status, count);
	
	if (status || count == 0)
		return status;
	
	for (i = 0; i < (int)s_list_length(svcRecList); i++) {
		
		Services* foundService = new Services();
		
		ServiceRecord *svcRec;
		svcRec = (ServiceRecord *)s_list_nth_data(svcRecList, i);
		
		// RecHandle
		foundService->setRecHandle( svcRec->serviceRecordHandle );
		qWarning( "SvcRecHdl: 0x%x\n", foundService->recHandle() );
		
		ServiceAttributeIdentifier attrId = AttrServiceName_Primary;
		SDPData *pSDPData = getSDPData(svcRec, attrId);   
		if (pSDPData != NULL) {
			foundService->setServiceName( pSDPData->sdpDataValue.stringPtr );
			qDebug( "Service Name: %s", foundService->serviceName().latin1() );
		}
		
		
		if (getServiceClassIDList(svcRec, &pslist_t) == E_OK) {
			printf("Service Class ID List: \n");
			
			slist_t	*entry;
			
			for (entry = pslist_t; entry; entry = entry->next){
				
				UUID *pUUID = NULL;
				char ServiceClassUUID_str[MAX_LEN_SERVICECLASS_UUID_STR];
				
				pUUID = (UUID *)entry->data;
				ServiceClassUUID2strn(pUUID, ServiceClassUUID_str, 
						      MAX_LEN_SERVICECLASS_UUID_STR);
				int uuid = 0;
				switch ( pUUID->uuidType ){
				case UUID_16:
					uuid = pUUID->value.uuid16Bit;
					break;
				case UUID_32:
					uuid = pUUID->value.uuid32Bit;
					break;
				case UUID_128:
					// uuid = pUUID->value.uuid128Bit;
					qWarning( "128 Bit UUID not supported !" );
					exit (-1);
					break;
				default:
					qWarning("Error undefined uuidType !!");
					exit (-1);
				}
				qWarning( "Found ServiceClass  \"%s\" (0x%x)\n", 
					  ServiceClassUUID_str, uuid );
				foundService->insertClassId( uuid, ServiceClassUUID_str );
			}
			
		}
		
		if (getAccessProtocols(svcRec, &pAccessProtocols) == E_OK) {
			printf("Protocol Descriptor List: \n");
			
			slist_t	*entry;
			
			for (entry = pAccessProtocols->pSequence; entry; entry = entry->next){
				
				slist_t *protDescSeq = NULL;
				
				protDescSeq = (slist_t *)entry->data;
				
				slist_t	*entry2;
				for (entry2 = protDescSeq; entry2; entry2 = entry2->next){
					
					Services::ProtocolDescriptor* foundDescriptor = 
						new Services::ProtocolDescriptor();
					
					ProtocolDescriptor *pPD = NULL;
					char ProtocolUUID_str[MAX_LEN_PROTOCOL_UUID_STR];
					
					pPD = (ProtocolDescriptor *)entry2->data;
					ProtocolUUID2strn(&(pPD->protocolId), ProtocolUUID_str, 
							  MAX_LEN_PROTOCOL_UUID_STR);
					
					printf("Protocol  \"%s\"\n", ProtocolUUID_str);
					foundDescriptor->setName( ProtocolUUID_str );
					
					if (pPD->portNumber) {
						//printf("    PortSize: %d\n", pPD->portNumberSize);
						printf("    Port/Channel: %d\n", pPD->portNumber);
						foundDescriptor->setPort( pPD->portNumber );
					}
					if (pPD->versionNumber) {
						printf("    Version: %#.4x\n", pPD->versionNumber);
					}
					
					foundService->insertProtocolDescriptor( *foundDescriptor );
					delete foundDescriptor;
				}
			}
		}
		
		
		if (getProfileDescriptor(svcRec, &pslist_t) == E_OK) {
			printf("Profile Descriptor List: \n");
			
			slist_t	*entry;
			
			for (entry = pslist_t; entry; entry = entry->next){
				
				Services::ProfileDescriptor* profDescriptor = 
					new Services::ProfileDescriptor();
				
				ProfileDescriptor *profDesc = NULL;
				char ProfileDescriptorUUID_str[MAX_LEN_PROFILEDESCRIPTOR_UUID_STR];
				
				profDesc = (ProfileDescriptor *)entry->data;
				ProfileDescriptorUUID2strn(&(profDesc->profileUUID), 
							   ProfileDescriptorUUID_str, 
							   MAX_LEN_PROFILEDESCRIPTOR_UUID_STR);
				int uuid = 0;
				switch ( profDesc->profileUUID.uuidType ){
				case UUID_16:
					uuid = profDesc->profileUUID.value.uuid16Bit;
					break;
				case UUID_32:
					uuid = profDesc->profileUUID.value.uuid32Bit;
					break;
				case UUID_128:
					// uuid = pUUID->value.uuid128Bit;
					qWarning( "128 Bit UUID not supported !" );
					exit (-1);
					break;
				default:
					qWarning("Error undefined uuidType !!");
					exit (-1);
				}
				printf("  \"%s\" (0x%x)\n", ProfileDescriptorUUID_str, uuid );
				profDescriptor->setId( ProfileDescriptorUUID_str );
				profDescriptor->setId( uuid );
				if (profDesc->vnum) {
					printf("    Version: %#.4x\n", profDesc->vnum);
					profDescriptor->setVersion( profDesc->vnum );
				}
				
				foundService->insertProfileDescriptor( *profDescriptor );
				delete profDescriptor;
			}
			
		}
		
		listOfServices.append( *foundService );
		delete foundService;
	}
	
	return status;
}



void Manager::searchServices( const QString& remDevice ){
	qWarning ("Manager::searchServices( %s ) !", remDevice.latin1() );
	
	Services::ValueList listOfServices;
	
	int 			status;
	struct sockaddr_affix	sa;
	
	status = SDPInit(0);
	if (status < 0){
		qWarning( "Manager::searchServices: SDPInit failed!" );
		emit foundServices( remDevice, listOfServices );
		return;
	}
	
	// Hmmm. There must be a better solution.. :SX
	char* buffer = (char*) malloc( strlen( remDevice.latin1() ) + 1 );
	strncpy( buffer, remDevice.latin1(), strlen( remDevice.latin1() ) + 1 );
	
	sa.family = PF_AFFIX;
	sa.local = BDADDR_ANY;
	str2bda( &sa.bda, buffer );
	
	free( buffer );
	
	printf("Connecting to host %s ...\n", bda2str(&sa.bda));
	SDPServerHandle srvHandle = SDPOpenServerConnection( &sa );
       	if (srvHandle < 0){
		qWarning( "Manager::searchServices: SDPOpenServerConnection failed!" );
		emit foundServices( remDevice, listOfServices );
		return;
	}
	
	UUID uuid;
	makeUUIDFrom16Bits(&uuid, PublicBrowseGroup); // for PublicBrowseGroup see: sdp.h
	status = extractGroups( &uuid, srvHandle, listOfServices );
	if ( status != 0 ){
		qWarning( "Manager::searchServices: ServiceScan failed!" );
		// Return a 0 list due to misparsed services..
		SDPCloseServerConnection(srvHandle);
		Services::ValueList nullList;
		emit foundServices( remDevice, nullList );
		return;
	}
	
	SDPCloseServerConnection(srvHandle);
	emit foundServices( remDevice, listOfServices );
}

void Manager::searchServices( const RemoteDevice& dev){
	searchServices( dev.mac() );
}

QString Manager::toDevice( const QString& /*mac*/ ){
	return QString::null;
}

QString Manager::toMac( const QString &/*device*/ ){
	return QString::null;
}


////// hcitool cc and hcitool con

/**
 * Create it on the stack as don't care
 * so we don't need to care for it
 * cause hcitool gets reparented
 */
void Manager::connectTo( const QString& mac) {
	//     OProcess proc;
	//     proc << "hcitool";
	//     proc << "cc";
	//     proc << mac;
	//     proc.start(OProcess::DontCare); // the lib does not care at this point
	
	qWarning( "Manager::connectTo( %s ) not implemented !", mac.latin1() );
}


void Manager::searchConnections() {
	//     qWarning("searching connections?");
	//     OProcess* proc = new OProcess();
	//     m_hcitoolCon = QString::null;
	
	//     connect(proc, SIGNAL(processExited(OProcess*) ),
	//             this, SLOT(slotConnectionExited( OProcess*) ) );
	//     connect(proc, SIGNAL(receivedStdout(OProcess*, char*, int) ),
	//             this, SLOT(slotConnectionOutput(OProcess*, char*, int) ) );
	//     *proc << "hcitool";
	//     *proc << "con";
	
	//     if (!proc->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
	//         ConnectionState::ValueList list;
	//         emit connections( list  );
	//         delete proc;
	//     }
	
	qWarning( "Manager::searchConnections() not implemented !" );
	ConnectionState::ValueList list;
	emit connections( list  );
	
}


void Manager::signalStrength( const QString &mac ) {	
	qWarning( "Manager::signalStrength( %s ) ", mac.latin1() );
	
	int fd, err;
	int handle;
	__u8  quality;
	BD_ADDR bda;
	
	fd = hci_open(btdev);
	if (fd < 0) {
		qWarning("Manager::signalStrength:Unable to open device %s: %s\n", btdev, strerror(errno));
		emit signalStrength( mac, "-1" );
		return;
	}
	
	// Hmmm. There must be a better solution.. :SX
	char* buffer = (char*) malloc( strlen( mac.latin1() ) + 1 );
	strncpy( buffer, mac.latin1(), strlen( mac.latin1() ) + 1 );
	
	str2bda( &bda, buffer );
	
	handle = hci_get_conn( fd, &bda );
	if (handle < 0) {
		qWarning("Manager::signalStrength:Connection not found\n");
		emit signalStrength( mac, "-1" );
		return;
	}
	
	
	// 	fd = hci_open( device.isEmpty() ? "bt0" : device.latin1() );
 	err = HCI_GetLinkQuality( fd, handle, &quality );
	if ( err ){
		qWarning("Manager::signalStrength:Problem executing GetLinkQuality \n");
		emit signalStrength( mac, "-1" );
		return;
	}
	
	qWarning("Manager::signalStrength:Link-Quality is as int = %d and as string = %s ", 
		 quality, QString().setNum( quality ).latin1() );
	
	emit signalStrength( mac, "-1" );
	
	close( fd );
}

// This macro defines a globally known method which is our
// "hook" to instantiate the plugin class...
Q_EXPORT_INTERFACE(){
	// Instantiates the class "PluginInterface", calls the method
	// "queryInterface" inside of it and returns the received this-pointer
	// to it..
	Q_CREATE_INSTANCE( PluginInterface );
}
