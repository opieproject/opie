/*
 * original code from global1.cpp Copyright by trolltech
 * GPLv2 only zecke@handhelds.org
 *
 */
#define private protected
#include <qtopia/global.h>
#undef private

namespace Opie {
    struct Global : public ::Global{
        static bool isAppLnkFileName( const QString& str );
        static QString tempDir();
        static Command* builtinCommands();
        static QGuardedPtr<QWidget>* builtinRunning();
        static QString uuid();
        static QByteArray encodeBase64(const QByteArray& data );
    };
}
