/*
 * This file contains hacks or workarounds, that make it possible to use a normal
 * libopie arm build (iPAQ or OZ) directly on the Sharp retail ROM.
 * This way, we only need one 'official' libopie binary for all platforms.
 */


// 1) Opie's libqpe.so has an additional function in Sound, which is utilized
//    in ODevice:

// ok this is really evil ;), but Sound::isFinished is only needed in the
// iPAQ part of ODevice, which is never called on Z's
// we add a "weak" symbol here. This will be used, if ld.so does not find
// a normal ("hard") symbol of the same name - hence only on the retail Z

struct Sound { bool Sound::isFinished ( ) const __attribute__(( weak )); };
bool Sound::isFinished ( ) const { return true; }

