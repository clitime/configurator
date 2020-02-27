#pragma once

#include <QObject>


class QByteArray;
class QString;
class QTimer;


class PelcoDTransport : public QObject {
    Q_OBJECT
private:
    quint8 addr;
    QTimer *timer = nullptr;

    quint8 calc_crc(const QByteArray &d);
    QByteArray parse_transmit_pkg(const QByteArray &d);

    QByteArray make_msg(quint8 cmd1, quint8 cmd2, quint8 p1 = 0,
                        quint8 p2 = 0) {
        QByteArray msg;
        msg.append(static_cast<char>(cmd1));
        msg.append(static_cast<char>(cmd2));
        msg.append(static_cast<char>(p1));
        msg.append(static_cast<char>(p2));
        return msg;
    }
    enum State { wait_start, receive } state = wait_start;
    quint8 receive_b_count = 0;
    QByteArray recv_buffer;

public:
    PelcoDTransport(QObject *obj = nullptr, quint8 a = 0x01);
    ~PelcoDTransport();
    QString getAddr();
    QByteArray check_receive_pkg(const QByteArray &d);
public slots:
    void setAddress(quint8 a);
    void receive_msg(const QByteArray &d);

    QByteArray get_fw_version();
    QByteArray get_serial_number();
    QByteArray get_max_step_zoom();
    QByteArray set_max_step_zoom(quint8 p1, quint8 p2);
    QByteArray get_max_step_focus();
    QByteArray set_max_step_focus(quint8 p1, quint8 p2);
    QByteArray get_max_step_iris();
    QByteArray set_max_step_iris(quint8 p1, quint8 p2);
    QByteArray get_compensation_focus();
    QByteArray set_compensation_focus(quint8 p1, quint8 p2);
    QByteArray initialize();
    QByteArray reset_by_default();
    QByteArray get_addr_device();
    QByteArray set_addr_device(quint8 p1, quint8 p2);
    QByteArray set_baudrate(quint8 p1, quint8 p2);

    QByteArray set_speed_trans(quint8 p1, quint8 p2);
    QByteArray set_speed_zoom(quint8 p1, quint8 p2);
    QByteArray set_speed_focus(quint8 p1, quint8 p2);
    QByteArray save_presetposition(quint8 p1, quint8 p2);
    QByteArray delete_presetposition(quint8 p1, quint8 p2);
    QByteArray goto_presetposition(quint8 p1, quint8 p2);
    QByteArray zoom_inc();
    QByteArray zoom_dec();
    QByteArray focus_inc();
    QByteArray focus_dec();
    QByteArray iris_inc();
    QByteArray iris_dec();
    QByteArray stop_control();
    QByteArray get_zoom_coordinate();
    QByteArray get_focus_coordinate();
private slots:
    void timer_handler(void);
signals:
    void data_is_ready(const QByteArray &d);
};
