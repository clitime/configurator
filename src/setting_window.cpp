#include "setting_window.h"


#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>


SettingWindow::SettingWindow(QWidget *parent, PelcoDTransport *p)
    : QWidget(parent), pelcod(p) {
    connect(pelcod, &PelcoDTransport::data_is_ready, this,
            &SettingWindow::get_read_data);

    QHBoxLayout *main_lay_v = new QHBoxLayout();
    main_lay_v->setSizeConstraint(QLayout::SetFixedSize);

    QVBoxLayout *vl2 = new QVBoxLayout();

    vl2->addWidget(init_version_widget());
    vl2->addWidget(init_config_uart_widget());

    vl2->setSizeConstraint(QLayout::SetFixedSize);
    vl2->addWidget(init_config_step_widget());
    vl2->addStretch(1);

    main_lay_v->addLayout(vl2);

    QPushButton *rst_def_btn = new QPushButton("Сброс параметров");
    connect(rst_def_btn, &QPushButton::clicked, this,
            &SettingWindow::reset_by_default);

    QPushButton *init_btn = new QPushButton("Инициализация");
    connect(init_btn, &QPushButton::clicked, this, &SettingWindow::inialize);

    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(init_btn);
    hl->addWidget(rst_def_btn);
    vl2->addLayout(hl);

    this->setLayout(main_lay_v);
}


SettingWindow::~SettingWindow() {}


QWidget *SettingWindow::init_version_widget(void) {
    QVBoxLayout *vl1 = new QVBoxLayout();
    QPushButton *vfw_btn = new QPushButton("Версия FW");

    connect(vfw_btn, &QPushButton::clicked, this, &SettingWindow::get_firmware);

    QPushButton *serial_n_btn = new QPushButton("Серийный номер");
    connect(serial_n_btn, &QPushButton::clicked, this,
            &SettingWindow::get_serial_numb);

    vl1->addWidget(vfw_btn);
    vl1->addWidget(serial_n_btn);

    QHBoxLayout *hl1 = new QHBoxLayout();
    firmware_version_le = new QLineEdit();
    firmware_version_le->setReadOnly(true);
    serial_number_le = new QLineEdit();
    serial_number_le->setReadOnly(true);
    protocol_version_le = new QLineEdit();
    protocol_version_le->setReadOnly(true);

    hl1->addWidget(firmware_version_le);
    hl1->addWidget(protocol_version_le);

    QHBoxLayout *hl2 = new QHBoxLayout();
    hl2->addWidget(serial_number_le);
    hl2->addStretch(1);

    QVBoxLayout *vl2 = new QVBoxLayout();
    vl2->addLayout(hl1);
    vl2->addLayout(hl2);

    QHBoxLayout *info_lay = new QHBoxLayout();
    info_lay->addLayout(vl1);
    info_lay->addLayout(vl2);
    QGroupBox *ginfo = new QGroupBox("Информация об устройстве");
    ginfo->setLayout(info_lay);
    return ginfo;
}


QWidget *SettingWindow::init_config_uart_widget(void) {
    QGroupBox *gb_config_uart = new QGroupBox("Конфигурация RS-485");
    QLabel *cu_addr_lbl = new QLabel("Адрес");
    cu_addr_le = new QLineEdit();

    QRegExp addr_valid("(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[1-9])");
    cu_addr_le->setValidator(new QRegExpValidator(addr_valid, this));

    cu_addr_le->setText(pelcod->getAddr());
    QPushButton *cu_get_addr_btn = new QPushButton("Получить");
    connect(cu_get_addr_btn, &QPushButton::clicked, this,
            &SettingWindow::get_addr_device);

    QPushButton *cu_set_addr_btn = new QPushButton("Установить");
    connect(cu_set_addr_btn, &QPushButton::clicked, this,
            &SettingWindow::set_addr_device);

    QHBoxLayout *cu_addr_lh = new QHBoxLayout();
    cu_addr_lh->addWidget(cu_addr_lbl);
    cu_addr_lh->addWidget(cu_addr_le);
    cu_addr_lh->addWidget(cu_get_addr_btn);
    cu_addr_lh->addWidget(cu_set_addr_btn);

    QLabel *cu_speed_lbl = new QLabel("Скорость");

    cu_speed_list = new QComboBox();
    cu_speed_list->addItems({"2400", "4800", "9600"});
    cu_speed_list->setCurrentText("9600");
    QPushButton *cu_set_speed_btn = new QPushButton("Установить");
    connect(cu_set_speed_btn, &QPushButton::clicked, this,
            &SettingWindow::set_baudrate);

    QHBoxLayout *cu_speed_lh = new QHBoxLayout();
    cu_speed_lh->addWidget(cu_speed_lbl);
    cu_speed_lh->addWidget(cu_speed_list);
    cu_speed_lh->addWidget(cu_set_speed_btn);

    QVBoxLayout *config_uart_vl = new QVBoxLayout();
    config_uart_vl->addLayout(cu_addr_lh);
    config_uart_vl->addLayout(cu_speed_lh);
    gb_config_uart->setLayout(config_uart_vl);
    return gb_config_uart;
}


QWidget *SettingWindow::init_config_step_widget(void) {
    QHBoxLayout *main_lay = new QHBoxLayout();

    QLabel *zoom_lbl = new QLabel("zoom");
    QLabel *iris_lbl = new QLabel("iris");
    QLabel *focus_lbl = new QLabel("focus");
    QLabel *comp_zoom_lbl = new QLabel("Компенсация люфта focus");
    QVBoxLayout *lbl_vlay = new QVBoxLayout();
    lbl_vlay->addWidget(zoom_lbl);
    lbl_vlay->addWidget(iris_lbl);
    lbl_vlay->addWidget(focus_lbl);
    lbl_vlay->addWidget(comp_zoom_lbl);

    zoom_le = new QLineEdit();
    QRegExp pattern("(6553[0-5])|655[0-2][0-9]|65[0-4][0-9]{2}|"
                    "6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|"
                    "[1-9][0-9]{2}|[1-9][0-9]|[1-9]");
    QValidator *uint16val1 = new QRegExpValidator(pattern, this);
    zoom_le->setValidator(uint16val1);
    zoom_le->setText("0");
    iris_le = new QLineEdit();
    iris_le->setText("0");
    iris_le->setValidator(uint16val1);
    focus_le = new QLineEdit();
    focus_le->setText("0");
    focus_le->setValidator(uint16val1);
    comp_zoom_le = new QLineEdit();
    comp_zoom_le->setText("0");
    comp_zoom_le->setValidator(uint16val1);
    QVBoxLayout *le_vlay = new QVBoxLayout();
    le_vlay->addWidget(zoom_le);
    le_vlay->addWidget(iris_le);
    le_vlay->addWidget(focus_le);
    le_vlay->addWidget(comp_zoom_le);

    QPushButton *set_zoom_btn = new QPushButton("Задать");
    connect(set_zoom_btn, &QPushButton::clicked, this,
            &SettingWindow::set_max_step_zoom);

    QPushButton *set_iris_btn = new QPushButton("Задать");
    connect(set_iris_btn, &QPushButton::clicked, this,
            &SettingWindow::set_max_step_iris);

    QPushButton *set_focus_btn = new QPushButton("Задать");
    connect(set_focus_btn, &QPushButton::clicked, this,
            &SettingWindow::set_max_step_focus);

    QPushButton *set_comp_zoom_btn = new QPushButton("Задать");
    connect(set_comp_zoom_btn, &QPushButton::clicked, this,
            &SettingWindow::set_compensation_focus);

    QVBoxLayout *sbtn_lay = new QVBoxLayout();
    sbtn_lay->addWidget(set_zoom_btn);
    sbtn_lay->addWidget(set_iris_btn);
    sbtn_lay->addWidget(set_focus_btn);
    sbtn_lay->addWidget(set_comp_zoom_btn);

    QPushButton *get_zoom_btn = new QPushButton("Получить");
    connect(get_zoom_btn, &QPushButton::clicked, this,
            &SettingWindow::get_max_step_zoom);

    QPushButton *get_iris_btn = new QPushButton("Получить");
    connect(get_iris_btn, &QPushButton::clicked, this,
            &SettingWindow::get_max_step_iris);

    QPushButton *get_focus_btn = new QPushButton("Получить");
    connect(get_focus_btn, &QPushButton::clicked, this,
            &SettingWindow::get_max_step_focus);

    QPushButton *get_comp_zoom_btn = new QPushButton("Получить");
    connect(get_comp_zoom_btn, &QPushButton::clicked, this,
            &SettingWindow::get_compensation_focus);

    QVBoxLayout *gbtn_lay = new QVBoxLayout();
    gbtn_lay->addWidget(get_zoom_btn);
    gbtn_lay->addWidget(get_iris_btn);
    gbtn_lay->addWidget(get_focus_btn);
    gbtn_lay->addWidget(get_comp_zoom_btn);

    main_lay->addLayout(lbl_vlay);
    main_lay->addLayout(le_vlay);
    main_lay->addLayout(sbtn_lay);
    main_lay->addLayout(gbtn_lay);

    QGroupBox *gbox = new QGroupBox("Максимальное к-во шагов");
    gbox->setLayout(main_lay);
    return gbox;
}


void SettingWindow::get_read_data(const QByteArray &d) {
    QByteArray data = pelcod->check_receive_pkg(d);
    if (data.isEmpty()) {
        return;
    }

    switch (data[0]) {
    case 0x00:
        break;
    case 0x01:
        if (data[1] != static_cast<char>(0xA1)) {
            break;
        }
        cu_addr_le->setText(QString::number(static_cast<int>(data[3])));
        pelcod->setAddress(static_cast<quint8>(data[3]));
        break;
    case 0x05:
        if (data[1] != static_cast<char>(0xA1)) {
            break;
        }
        break;
    case 0x07: {
        if (data[1] != static_cast<char>(0xA1)) {
            break;
        }
        int val = (static_cast<int>(data[2]) << 8) | static_cast<int>(data[3]);
        serial_number_le->setText(QString::number(val));
    } break;
    case 0x08:
        if (data[1] != static_cast<char>(0xA1)) {
            break;
        }
        firmware_version_le->setText(
            QString::number(static_cast<int>(data[2])));
        protocol_version_le->setText(
            QString::number(static_cast<int>(data[3])));
        break;
    case 0x40: {
        if (data[1] != static_cast<char>(0xA1)) {
            break;
        }
        int val = (static_cast<int>(data[2]) << 8) | static_cast<int>(data[3]);
        zoom_le->setText(QString::number(val));
    } break;
    case 0x41: {
        if (data[1] != static_cast<char>(0xA1)) {
            break;
        }
        int val = (static_cast<int>(data[2]) << 8) | static_cast<int>(data[3]);
        focus_le->setText(QString::number(val));
    } break;
    case 0x42: {
        if (data[1] != static_cast<char>(0xA1)) {
            break;
        }
        int val = (static_cast<int>(data[2]) << 8) | static_cast<int>(data[3]);
        iris_le->setText(QString::number(val));
    } break;
    case 0x43: {
        if (data[1] != static_cast<char>(0xA1)) {
            break;
        }
        int val = (static_cast<int>(data[2]) << 8) | static_cast<int>(data[3]);
        comp_zoom_le->setText(QString::number(val));
    } break;
    default:
        break;
    }
}


void SettingWindow::set_max_step_zoom() {
    int p1 = zoom_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->set_max_step_zoom(hh, ll));
}
void SettingWindow::set_max_step_focus() {
    int p1 = focus_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->set_max_step_focus(hh, ll));
}
void SettingWindow::set_max_step_iris() {
    int p1 = iris_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->set_max_step_iris(hh, ll));
}
void SettingWindow::set_compensation_focus() {
    int p1 = comp_zoom_le->text().toInt();
    quint8 hh = static_cast<quint8>((p1 & 0xff00) >> 8);
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->set_compensation_focus(hh, ll));
}
void SettingWindow::set_addr_device() {
    int p1 = cu_addr_le->text().toInt();
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->set_addr_device(0x00, ll));
}
void SettingWindow::set_baudrate() {
    int p1 = cu_speed_list->currentIndex();
    if (p1 > 2)
        p1 = 2;
    quint8 ll = static_cast<quint8>(p1 & 0xff);
    emit data_is_send(pelcod->set_baudrate(0x00, ll));
}
