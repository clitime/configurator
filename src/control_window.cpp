#include "control_window.h"

#include "pelcod_protocol.h"
#include <QBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QValidator>


Control_window::Control_window(QWidget *parent, PelcoDTransport *p)
    : QWidget(parent), pelcod(p), timer(new QTimer) {
    connect(timer, &QTimer::timeout, this, &Control_window::handle_timer);

    connect(pelcod, &PelcoDTransport::data_is_ready, this,
            &Control_window::get_read_data);

    QHBoxLayout *main_lay_v = new QHBoxLayout();
    main_lay_v->setSizeConstraint(QLayout::SetFixedSize);

    QVBoxLayout *left_lay = new QVBoxLayout();
    left_lay->setSizeConstraint(QLayout::SetFixedSize);
    left_lay->addWidget(setting_speed_widget());
    left_lay->addWidget(ctrl_position_widget());

    left_lay->addWidget(manual_ctrl_widget());
    left_lay->addWidget(current_coordinate_widget());

    main_lay_v->addLayout(left_lay);

    this->setLayout(main_lay_v);
}


Control_window::~Control_window() {}


void Control_window::handle_timer(void) {
    timer->stop();
    switch (state_request) {
    case REQUEST_ZOOM:
        get_zoom_coordinate();
        state_request = REQUEST_FOCUS;
        break;
    case REQUEST_FOCUS:
        state_request = REQUEST_ZOOM;
        get_focus_coordinate();
        break;
    default:
        break;
    }
    timer->start(500);
}


void Control_window::handle_auto_request() {
    if (auto_request->checkState() == Qt::Checked) {
        state_request = REQUEST_ZOOM;
        get_focus_coordinate();
        timer->start(500);
    } else {
        timer->stop();
    }
}


QWidget *Control_window::setting_speed_widget(void) {
    QGroupBox *setting_speed_gb = new QGroupBox("Установка скорости");
    QHBoxLayout *main_l = new QHBoxLayout();
    setting_speed_gb->setLayout(main_l);

    QVBoxLayout *set_l1 = new QVBoxLayout();
    QVBoxLayout *set_l2 = new QVBoxLayout();
    QVBoxLayout *set_l3 = new QVBoxLayout();

    QLabel *lbl2 = new QLabel("zoom");
    QLabel *lbl3 = new QLabel("focus");
    set_l1->addWidget(lbl2);
    set_l1->addWidget(lbl3);
    main_l->addLayout(set_l1);

    QRegExp pattern("(6553[0-5])|655[0-2][0-9]|65[0-4][0-9]{2}|"
                    "6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|"
                    "[1-9][0-9]{2}|[1-9][0-9]|[1-9]");
    QValidator *uint16val1 = new QRegExpValidator(pattern, this);
    speed_zoom_le = new QLineEdit();
    speed_zoom_le->setText("0");
    speed_zoom_le->setValidator(uint16val1);
    speed_focus_le = new QLineEdit();
    speed_focus_le->setValidator(uint16val1);
    speed_focus_le->setText("0");

    set_l2->addWidget(speed_zoom_le);
    set_l2->addWidget(speed_focus_le);

    main_l->addLayout(set_l2);

    QPushButton *btn3 = new QPushButton("Задать");
    connect(btn3, &QPushButton::clicked, this,
            &Control_window::set_speed_focus);
    QPushButton *btn2 = new QPushButton("Задать");
    connect(btn2, &QPushButton::clicked, this, &Control_window::set_speed_zoom);

    set_l3->addWidget(btn2);
    set_l3->addWidget(btn3);
    main_l->addLayout(set_l3);

    return setting_speed_gb;
}


QWidget *Control_window::manual_ctrl_widget(void) {
    QGroupBox *gbx = new QGroupBox("Ручное управление");
    QVBoxLayout *l1 = new QVBoxLayout();
    QVBoxLayout *l2 = new QVBoxLayout();
    QHBoxLayout *l3 = new QHBoxLayout();
    QVBoxLayout *l4 = new QVBoxLayout();

    QString btn_name[] = {"zoom +", "focus +", "iris +",
                          "zoom -", "focus -", "iris -"};
    QPushButton *btn[6];
    for (quint8 x = 0; x != 6; x++) {
        btn[x] = new QPushButton(btn_name[x]);
    }
    connect(btn[0], &QPushButton::clicked, this, &Control_window::zoom_inc);
    connect(btn[3], &QPushButton::clicked, this, &Control_window::zoom_dec);
    connect(btn[1], &QPushButton::clicked, this, &Control_window::focus_inc);
    connect(btn[4], &QPushButton::clicked, this, &Control_window::focus_dec);
    connect(btn[2], &QPushButton::clicked, this, &Control_window::iris_inc);
    connect(btn[5], &QPushButton::clicked, this, &Control_window::iris_dec);

    for (quint8 x = 0; x != 3; x++) {
        l1->addWidget(btn[x]);
    }
    for (quint8 x = 3; x != 6; x++) {
        l2->addWidget(btn[x]);
    }
    l3->addLayout(l1);
    l3->addLayout(l2);
    l4->addLayout(l3);

    QPushButton *stop = new QPushButton("stop");
    connect(stop, &QPushButton::clicked, this, &Control_window::stop_control);

    l4->addWidget(stop);

    gbx->setLayout(l4);

    return gbx;
}


QWidget *Control_window::current_coordinate_widget(void) {
    QGroupBox *gbx = new QGroupBox("Текущие координаты привода");
    QLabel *lbl1 = new QLabel("zoom, %");
    QLabel *lbl2 = new QLabel("focus, %");

    zoom_coordinate_le = new QLineEdit();
    zoom_coordinate_le->setReadOnly(true);
    focus_coordinate_le = new QLineEdit();
    focus_coordinate_le->setReadOnly(true);

    QPushButton *btn1 = new QPushButton("Запросить");
    connect(btn1, &QPushButton::clicked, this,
            &Control_window::get_zoom_coordinate);
    QPushButton *btn2 = new QPushButton("Запросить");
    connect(btn2, &QPushButton::clicked, this,
            &Control_window::get_focus_coordinate);

    QVBoxLayout *l1 = new QVBoxLayout();
    l1->addWidget(lbl1);
    l1->addWidget(lbl2);

    QVBoxLayout *l2 = new QVBoxLayout();
    l2->addWidget(zoom_coordinate_le);
    l2->addWidget(focus_coordinate_le);

    QVBoxLayout *l3 = new QVBoxLayout();
    l3->addWidget(btn1);
    l3->addWidget(btn2);

    QHBoxLayout *l4 = new QHBoxLayout();
    l4->addLayout(l1);
    l4->addLayout(l2);
    l4->addLayout(l3);
    QVBoxLayout *l5 = new QVBoxLayout();
    l5->addLayout(l4);
    auto_request = new QCheckBox("Авто запрос");
    connect(auto_request, &QCheckBox::stateChanged, this,
            &Control_window::handle_auto_request);
    l5->addWidget(auto_request);

    gbx->setLayout(l5);

    return gbx;
}


QWidget *Control_window::ctrl_position_widget(void) {
    QGroupBox *gbx = new QGroupBox("Управление пресет-позицией");
    QLabel *lbl1 = new QLabel("Номер пресет-позиции");
    presetposition_le = new QLineEdit();
    presetposition_le->setText("0");

    QRegExp uint8val("(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[1-9])");
    presetposition_le->setValidator(new QRegExpValidator(uint8val, this));

    QPushButton *btn1 = new QPushButton("Записать");
    connect(btn1, &QPushButton::clicked, this,
            &Control_window::save_presetposition);
    QPushButton *btn2 = new QPushButton("Удалить");
    connect(btn2, &QPushButton::clicked, this,
            &Control_window::delete_presetposition);
    QPushButton *btn3 = new QPushButton("Перейти");
    connect(btn3, &QPushButton::clicked, this,
            &Control_window::goto_presetposition);

    QHBoxLayout *l1 = new QHBoxLayout();
    l1->addWidget(lbl1);
    l1->addWidget(presetposition_le);
    QHBoxLayout *l2 = new QHBoxLayout();
    l2->addWidget(btn1);
    l2->addWidget(btn2);
    l2->addWidget(btn3);

    QVBoxLayout *l3 = new QVBoxLayout();
    l3->addLayout(l1);
    l3->addLayout(l2);
    gbx->setLayout(l3);
    return gbx;
}


void Control_window::get_read_data(const QByteArray &d) {
    QByteArray data = pelcod->check_receive_pkg(d);
    if (data.isEmpty()) {
        return;
    }
    uint8_t var0 = static_cast<uint8_t>(data[0]);
    uint8_t var1 = static_cast<uint8_t>(data[1]);
    if (var0 == 0x00 && var1 == 0x5D) {
        QString val = QString::number(static_cast<uint8_t>(data[2])) + "." + QString::number(static_cast<uint8_t>(data[3]));
        zoom_coordinate_le->setText(val.toUpper());
        if (auto_request->checkState() == Qt::Checked) {
            handle_timer();
        }
    } else if (var0 == 0x00 && var1 == 0x5F) {
        QString val = QString::number(static_cast<uint8_t>(data[2])) + "." + QString::number(static_cast<uint8_t>(data[3]));
        focus_coordinate_le->setText(val.toUpper());
        if (auto_request->checkState() == Qt::Checked) {
            handle_timer();
        }
    }
}


void Control_window::set_speed_zoom() {
    int p1 = speed_zoom_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->set_speed_zoom(hh, ll));
}
void Control_window::set_speed_focus() {
    int p1 = speed_focus_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->set_speed_focus(hh, ll));
}
void Control_window::save_presetposition() {
    int p1 = presetposition_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->save_presetposition(hh, ll));
}
void Control_window::delete_presetposition() {
    int p1 = presetposition_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->delete_presetposition(hh, ll));
}
void Control_window::goto_presetposition() {
    int p1 = presetposition_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->goto_presetposition(hh, ll));
}
