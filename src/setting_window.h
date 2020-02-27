#pragma once


#include "pelcod_protocol.h"
#include <QWidget>


class QByteArray;
class QLineEdit;
class QComboBox;


class SettingWindow : public QWidget {
    Q_OBJECT
private:
    PelcoDTransport *pelcod;

    QLineEdit *firmware_version_le;
    QLineEdit *serial_number_le;
    QLineEdit *protocol_version_le;

    QLineEdit *cu_addr_le;
    QComboBox *cu_speed_list;

    QLineEdit *zoom_le;
    QLineEdit *iris_le;
    QLineEdit *focus_le;
    QLineEdit *comp_zoom_le;

    QWidget *init_version_widget(void);
    QWidget *init_config_uart_widget(void);
    QWidget *init_config_step_widget(void);

public:
    SettingWindow(QWidget *parent = nullptr, PelcoDTransport *p = nullptr);
    ~SettingWindow();
private slots:
    void get_firmware() { emit data_is_send(pelcod->get_fw_version()); }
    void get_serial_numb() { emit data_is_send(pelcod->get_serial_number()); }
    void get_max_step_zoom() { emit data_is_send(pelcod->get_max_step_zoom()); }
    void get_max_step_focus() {
        emit data_is_send(pelcod->get_max_step_focus());
    }
    void get_max_step_iris() { emit data_is_send(pelcod->get_max_step_iris()); }
    void get_compensation_focus() {
        emit data_is_send(pelcod->get_compensation_focus());
    }
    void inialize() { emit data_is_send(pelcod->initialize()); }
    void reset_by_default() { emit data_is_send(pelcod->reset_by_default()); }
    void get_addr_device() { emit data_is_send(pelcod->get_addr_device()); };
    void set_max_step_zoom();
    void set_max_step_focus();
    void set_max_step_iris();
    void set_compensation_focus();
    void set_addr_device();
    void set_baudrate();
public slots:
    void get_read_data(const QByteArray &d);
signals:
    void data_is_send(const QByteArray &);
};
