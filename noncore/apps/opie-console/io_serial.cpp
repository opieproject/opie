#include "io_serial.h"

IOSerial::IOSerial(const Config &config) : IOLayer(config) {
}


void IOSerial::error(int number, const QString &error) {
}

void IOSerial::received(const QByteArray &array) {
}

