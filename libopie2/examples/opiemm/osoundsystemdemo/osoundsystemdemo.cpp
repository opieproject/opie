#include <opie2/osoundsystem.h>

using namespace Opie::MM;

int main( int argc, char** argv )
{
    qDebug( "OPIE Sound System Demo" );

    OSoundSystem* sound = OSoundSystem::instance();

    OSoundSystem::CardIterator it = sound->iterator();
/*
    while ( it.current() )
    {
        qDebug( "DEMO: OSoundSystem contains Interface '%s'", (const char*) it.current()->name() );
        ++it;
    }

*/
    OSoundCard* card = it.current();

    OMixerInterface* mixer = card->mixer();

    QStringList channels = mixer->allChannels();

    for ( QStringList::Iterator it = channels.begin(); it != channels.end(); ++it )
    {
        qDebug( "OSSDEMO: Mixer has channel %s", (const char*) *it );
        qDebug( "OSSDEMO:              +--- volume %d (left) | %d (right)", mixer->volume( *it ) & 0xff, mixer->volume( *it ) >> 8 );
    }

    return 0;

}
