// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// http://www.viva64.com
#include "pelcod_protocol.h"

#include <QByteArray>
#include <QString>
#include <QTimer>


/**
 *  B0   | B1    | B2       | B3       | B4      | B5      | B6          |
 * ***********************************************************************
 * старт | адрес | команда1 | команда2 | данные1 | данные2 | control sum |
 * 0xff  | 01-ff |          |          |         |         |             |
 *
 * control sum
 *      (B1 + B2 + ... + B6) / 256
 */


PelcoDTransport::PelcoDTransport(QObject *obj, quint8 a)
    : QObject(obj), addr(a), timer(new QTimer()) {
    connect(timer, &QTimer::timeout, this, &PelcoDTransport::timer_handler);
}


PelcoDTransport::~PelcoDTransport() {}


void PelcoDTransport::setAddress(quint8 a) { addr = a; }


quint8 PelcoDTransport::calc_crc(const QByteArray &d) {
    quint32 sum = 0;
    for (auto c : d) {
        sum += static_cast<quint8>(c);
    }
    return static_cast<quint8>(sum % 256);
}


/**
 * пакет без стартового байта и контрольной суммы
 * функция возвращает массив в который добавляет данные байты
 * d - массив из 4-х байт - команда1, команда2, данные 1, данные 2
 * возвращает пакет для отправки 0xff addr d crc
 */
QByteArray PelcoDTransport::parse_transmit_pkg(const QByteArray &d) {
    QByteArray pack = QByteArray();
    pack.append(static_cast<char>(addr));
    pack.append(d);
    pack.append(static_cast<char>(calc_crc(pack)));
    pack.insert(0, static_cast<char>(0xff));
    return pack;
}


void PelcoDTransport::receive_msg(const QByteArray &d) {
    for (auto item : d) {
        if (state == wait_start) {
            if (item == static_cast<char>(0xff)) {
                state = receive;
                receive_b_count = 0;
                timer->start(200);
            }
        } else if (state == receive) {
            timer->start(200);
            recv_buffer.append(item);
            receive_b_count++;
            if (receive_b_count == 6) {
                timer->stop();
                state = wait_start;
                emit data_is_ready(recv_buffer);
                recv_buffer.clear();
            }
        }
    }
}


void PelcoDTransport::timer_handler(void) {
    timer->stop();
    state = wait_start;
}


QByteArray PelcoDTransport::check_receive_pkg(const QByteArray &d) {
    quint8 save_crc = static_cast<quint8>(d[5]);
    QByteArray tmp = d;
    tmp.remove(5, 1);
    quint8 crc = calc_crc(tmp);

    if (save_crc != crc) {
        return QByteArray();
    }

    if (d[0] != addr) {
        return QByteArray();
    }

    tmp.remove(0, 1);

    return tmp;
}

QString PelcoDTransport::getAddr() {
    return QString::number(static_cast<int>(addr));
}


QByteArray PelcoDTransport::get_fw_version() {
    return parse_transmit_pkg(make_msg(0x08, 0x91));
}
QByteArray PelcoDTransport::get_serial_number() {
    return parse_transmit_pkg(make_msg(0x07, 0x91));
}
QByteArray PelcoDTransport::get_max_step_zoom() {
    return parse_transmit_pkg(make_msg(0x40, 0x91));
}
QByteArray PelcoDTransport::set_max_step_zoom(quint8 p1, quint8 p2) {
    return parse_transmit_pkg(make_msg(0x40, 0x95, p1, p2));
}
QByteArray PelcoDTransport::get_max_step_focus() {
    return parse_transmit_pkg(make_msg(0x41, 0x91));
}
QByteArray PelcoDTransport::set_max_step_focus(quint8 p1, quint8 p2) {
    return parse_transmit_pkg(make_msg(0x41, 0x95, p1, p2));
}
QByteArray PelcoDTransport::get_max_step_iris() {
    return parse_transmit_pkg(make_msg(0x42, 0x91));
}
QByteArray PelcoDTransport::set_max_step_iris(quint8 p1, quint8 p2) {
    return parse_transmit_pkg(make_msg(0x42, 0x95, p1, p2));
}
QByteArray PelcoDTransport::get_compensation_focus() {
    return parse_transmit_pkg(make_msg(0x43, 0x91));
}
QByteArray PelcoDTransport::set_compensation_focus(quint8 p1, quint8 p2) {
    return parse_transmit_pkg(make_msg(0x43, 0x95, p1, p2));
}
QByteArray PelcoDTransport::initialize() {
    return parse_transmit_pkg(make_msg(0x00, 0x93));
}
QByteArray PelcoDTransport::reset_by_default() {
    return parse_transmit_pkg(make_msg(0x00, 0x29));
}
QByteArray PelcoDTransport::get_addr_device() {
    QByteArray pack = QByteArray();
    pack.append(static_cast<char>(0xff));
    QByteArray p;
    p.append(static_cast<char>(0xff));
    p.append(make_msg(0x01, 0x91));
    p.append(static_cast<char>(calc_crc(p)));
    pack.append(p);
    return pack;
}
QByteArray PelcoDTransport::set_addr_device(quint8 p1, quint8 p2) {
    return parse_transmit_pkg(make_msg(0x01, 0x95, p1, p2));
}
QByteArray PelcoDTransport::set_baudrate(quint8 p1, quint8 p2) {
    return parse_transmit_pkg(make_msg(0x05, 0x95, p1, p2));
}


/******************************************************************************/

QByteArray PelcoDTransport::set_speed_zoom(quint8 p1, quint8 p2) {
    // ff 01 00 25 00 00 26
    // ff 01 00 25 00 01 27
    return parse_transmit_pkg(make_msg(0x00, 0x25, p1, p2));
}
QByteArray PelcoDTransport::set_speed_focus(quint8 p1, quint8 p2) {
    // ff 01 00 27 00 01 29
    return parse_transmit_pkg(make_msg(0x00, 0x27, p1, p2));
}
QByteArray PelcoDTransport::save_presetposition(quint8 p1, quint8 p2) {
    // ff 01 00 03 00 01 05
    return parse_transmit_pkg(make_msg(0x00, 0x03, p1, p2));
}
QByteArray PelcoDTransport::delete_presetposition(quint8 p1, quint8 p2) {
    // ff 01 00 05 00 01 07
    return parse_transmit_pkg(make_msg(0x00, 0x05, p1, p2));
}
QByteArray PelcoDTransport::goto_presetposition(quint8 p1, quint8 p2) {
    // ff 01 00 07 00 01 09
    return parse_transmit_pkg(make_msg(0x00, 0x07, p1, p2));
}
QByteArray PelcoDTransport::zoom_inc() {
    // ff 01 00 20 00 00 21
    return parse_transmit_pkg(make_msg(0x00, 0x20));
}
QByteArray PelcoDTransport::zoom_dec() {
    // ff 01 00 40 00 00 41
    return parse_transmit_pkg(make_msg(0x00, 0x40));
}
QByteArray PelcoDTransport::focus_inc() {
    // ff 01 00 80 00 00 81
    return parse_transmit_pkg(make_msg(0x00, 0x80));
}
QByteArray PelcoDTransport::focus_dec() {
    // ff 01 01 00 00 00 02
    return parse_transmit_pkg(make_msg(0x01, 0x00));
}
QByteArray PelcoDTransport::iris_inc() {
    // ff 01 02 00 00 00 03
    return parse_transmit_pkg(make_msg(0x02, 0x00));
}
QByteArray PelcoDTransport::iris_dec() {
    // ff 01 04 00 00 00 05
    return parse_transmit_pkg(make_msg(0x04, 0x00));
}
QByteArray PelcoDTransport::stop_control() {
    // ff 01 00 00 00 00 01
    return parse_transmit_pkg(make_msg(0x00, 0x00));
}
QByteArray PelcoDTransport::get_zoom_coordinate() {
    // ff 01 00 55 00 00 56
    return parse_transmit_pkg(make_msg(0x00, 0x55));
}
QByteArray PelcoDTransport::get_focus_coordinate() {
    // ff 01 00 57 00 00 58
    return parse_transmit_pkg(make_msg(0x00, 0x57));
}
