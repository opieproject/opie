#include <opie2/multiauthpassword.h>

#include <opie2/oapplication.h>

/// Run an authentication sequence using the global opie-security settings
int main( int argc, char ** argv )
{
    Opie::Core::OApplication app(argc, argv, "Multi-authentication demo");

    // Run the authentication process until it succeeds
    Opie::Security::MultiauthPassword::authenticate(Opie::Security::LockNow);
}
