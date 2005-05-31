#include <opie2/opcmciasystem.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

int main( int argc, char** argv )
{
    odebug << "start" << oendl;
    OPcmciaSystem* sys = OPcmciaSystem::instance();

    odebug << "number of detected sockets is = " << sys->count() << oendl;
    odebug << "number of populated sockets is = " << sys->cardCount() << oendl;

    OPcmciaSystem::CardIterator it = sys->iterator();
    OPcmciaSocket* sock = 0;
    while ( sock = it.current() )
    {
        odebug << "card in socket # " << sock->number() << " is '" << sock->identity() << "'" << oendl;
        odebug << "card status is " << sock->status() << oendl;
        odebug << "card function is " << sock->function() << oendl;
        ++it;
    }

    odebug << "end" << oendl;
    return 0;
}

