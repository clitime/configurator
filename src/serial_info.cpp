#include "serial_info.h"
#include <QSerialPortInfo>
#include <QStringList>


SerialInfo::SerialInfo(QObject *parent)
    : QObject(parent), serial_source(new QStringList()) {}


SerialInfo::~SerialInfo() {}


QStringList *SerialInfo::get_available(void) {
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    serial_source->clear();
    for (auto sp : serialPortInfos) {
        if (!sp.isBusy()) {
            serial_source->push_back(sp.portName());
        }
    }

    if (serial_source->isEmpty()) {
        serial_source->push_back("Не найден");
    }

    return serial_source;
}
