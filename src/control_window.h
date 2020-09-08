#pragma once

#include "pelcod_protocol.h"
#include <QWidget>


class QByteArray;
class QLineEdit;
class QTimer;
class QCheckBox;


class Control_window : public QWidget {
    Q_OBJECT
private:
    PelcoDTransport *pelcod;
    QTimer *timer = nullptr;

    QLineEdit *speed_zoom_le = nullptr;
    QLineEdit *speed_focus_le = nullptr;
    QLineEdit *presetposition_le = nullptr;
    QLineEdit *zoom_coordinate_le = nullptr;
    QLineEdit *focus_coordinate_le = nullptr;

    QCheckBox *auto_request = nullptr;

    QWidget *setting_speed_widget(void);
    QWidget *manual_ctrl_widget(void);
    QWidget *current_coordinate_widget(void);
    QWidget *ctrl_position_widget(void);

    enum { REQUEST_FOCUS, REQUEST_ZOOM } state_request = REQUEST_FOCUS;

public:
    Control_window(QWidget *parent = nullptr, PelcoDTransport *p = nullptr);
    ~Control_window();
private slots:
    void set_speed_zoom();
    void set_speed_focus();
    void save_presetposition();
    void delete_presetposition();
    void goto_presetposition();
    void zoom_inc() { emit data_is_send(pelcod->zoom_inc()); }
    void zoom_dec() { emit data_is_send(pelcod->zoom_dec()); }
    void focus_inc() { emit data_is_send(pelcod->focus_inc()); }
    void focus_dec() { emit data_is_send(pelcod->focus_dec()); }
    void iris_inc() { emit data_is_send(pelcod->iris_inc()); }
    void iris_dec() { emit data_is_send(pelcod->iris_dec()); }
    void stop_control() { emit data_is_send(pelcod->stop_control()); }
    void get_zoom_coordinate() {
        emit data_is_send(pelcod->get_zoom_coordinate());
    }
    void get_focus_coordinate() {
        emit data_is_send(pelcod->get_focus_coordinate());
    }

    void handle_timer(void);
    void handle_auto_request();
public slots:
    void get_read_data(const QByteArray &d);
signals:
    void data_is_send(const QByteArray &);
};
