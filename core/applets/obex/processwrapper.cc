//
m_rec == KProcess
m_count = int // the number of tries to recieve
m_send == KProcess
m_file == QString filename to send
m_outp == the output of the process

//


void receive() {
    m_rec = new KProcess();
    *m_rec << "irobex_palm3";
    // connect to the necessary slots
    connect(m_rec,  SIGNAL(processExited(KProcess*) ),
            this,  SLOT(slotExited(KProcess*) ) );

    connect(m_rec,  SIGNAL(receivedStdout(KProcess*, char*,  int ) ),
            this,  SLOT(slotStdOut(KProcess*, char*, int) ) );

    if(!m_rec->start(KProcess::NotifyOnExit, KProcess::AllOutput) ) {
        qWarning("could not start :(");
        emit done( false );
    }
    emit try(m_count )
}
void send(const QString &fileName) {
    m_count = 0;
    m_file = fileName;
    sendNow();
}
void sendNow() {
    if ( m_count >= 15 ) { // could not send
        emit error(-1 );
    }
    // KProcess inititialisation
    m_send = new KProcess();
    m_send << "irobex_palm3";
    m_send << m_file;

    // connect to slots Exited and and StdOut
    connect(m_send,  SIGNAL(processExited(KProcess*) ),
            this, SLOT(slotExited(KProcess*)) );
    connect(m_send,  SIGNAL(receivedStdout(KProcess*, char*,  int ) )
            this, SLOT(slotStdOut(KProcess*, char*, int) ) );
    // now start it
    if (!m_send->start(/*KProcess::NotifyOnExit,  KProcess::AllOutput*/ ) ) {
        m_count = 15;
        emit error(-1 );
    }
    // end
    m_count++;
    emit try( m_count );
}

void recieved() {
    if (m_rec->normalExit() ) {
        if ( m_rec->exitStatus() == 0 ) { // we got one
            QString filename = parseOut();
            emit recievedFile( filename );
        }
    }else{
        emit error(-1);
    };
    delete m_rec;
}


void slotExited(KProcess* proc) {
    if (proc == m_rec ) { // recieve process
        recieved();
    }else if ( proc == m_send ) {
        send();
    }
}


void send() {
    if (m_send->normalExit() ) {
        if ( m_send->exitStatus() == 0 ) {
            delete m_send;
            m_send=0;
            emit sent();
        }else if (m_send->exitStatus() == 255 ) { // it failed maybe the other side wasn't ready
            // let's try it again
            delete m_send;
            m_send = 0;
            sendNow();
        }
    }else {
        emit error( -1 );
        delete m_send;
        m_send = 0;
    }

}
void parseOut() {
    QStringList list = QStringList::split("\n",  m_outp);
    QStringList::Iterator it;
    for (it = list.begin(); it != list.end(); ++it ) {
        if ( (*it).startsWith("Wrote"  ) ) {
            QStringList pathes = QStringList::split(' ', (*it) );
            QString path = pathes[1];
        }
    }
}

void slotStdOut(KProcess* proc, char* buf, int len) {
    if ( proc == m_rec ) { // only recieve
        QCString cstring( buf,  len );
        m_outp.append( cstring.data() );
    }

}
