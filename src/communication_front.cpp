#include "communication_front.h"

#include <QBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QStackedWidget>

#include "serial_com.h"
#include "serial_info.h"
#include "tcp_com.h"
#include "udp_com.h"

#include "version.h"


Communication_front::Communication_front(QWidget *parent)
    : QWidget(parent), btn_open_name("Открыть соединение"),
      btn_close_name("Закрыть соединение"), serialInfo(new SerialInfo()),
      type_connection(new QComboBox()), connection(new SerialBack(this)) {

    stack_communication = new QStackedWidget();
    stack_communication->addWidget(create_serial_widget());
    stack_communication->addWidget(create_udp_widget());
    stack_communication->addWidget(create_tcp_widget());

    type_connection->addItems({"Serial port", "UDP" /*, "TCP"*/});
    prev_ix = type_connection->currentIndex();
    connect(type_connection, SIGNAL(currentIndexChanged(int)),
            SLOT(change_port_widgets(int)));

    btn_connection = new QPushButton(btn_open_name);

    connect(btn_connection, &QPushButton::clicked, this,
            &Communication_front::open_connection);

    QHBoxLayout *hdev_addr = new QHBoxLayout();
    hdev_addr->addWidget(new QLabel("Адрес устройства"));
    device_addr = new QLineEdit();
    QRegExp addr_valid("(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[1-9])");
    device_addr->setValidator(new QRegExpValidator(addr_valid, this));
    device_addr->setText("1");
    hdev_addr->addWidget(device_addr);

    QHBoxLayout *version_lay = new QHBoxLayout();
    version_lay->addWidget(new QLabel("Версия ПО: "));
    QLineEdit *version_line = new QLineEdit();
    version_line->setReadOnly(true);
    version_line->setText(full_version);
    version_lay->addWidget(version_line);

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addLayout(hdev_addr);
    lay->addWidget(type_connection);
    lay->addWidget(stack_communication);
    lay->addWidget(btn_connection);
    lay->addLayout(version_lay);
    lay->setSizeConstraint(QLayout::SetFixedSize);

    QGroupBox *v_frame = new QGroupBox("Конфигурация соединения");
    v_frame->setLayout(lay);
    QVBoxLayout *nl = new QVBoxLayout();
    nl->addWidget(v_frame);
    nl->setSizeConstraint(QLayout::SetFixedSize);

    this->setLayout(nl);

    serialInfo->scan_available_port();
}


Communication_front::~Communication_front() {}


void Communication_front::print_available_ports(const QStringList *available) {
    serial_port_list->clear();
    serial_port_list->addItems(*available);
}


QHBoxLayout *Communication_front::create_find_port_layout() {
    QHBoxLayout *available_port_layout = new QHBoxLayout();
    available_port_layout->addWidget(new QLabel("Доступные порты"));
    QPushButton *find_btn = new QPushButton("Найти");
    available_port_layout->addWidget(find_btn);

    serial_port_list = new QComboBox();
    serial_port_list->addItem("/dev/USBtty65");

    available_port_layout->addWidget(serial_port_list);

    connect(find_btn, &QPushButton::clicked, serialInfo,
            &SerialInfo::scan_available_port);

    connect(serialInfo, &SerialInfo::available_port, this,
            &Communication_front::print_available_ports);

    return available_port_layout;
}


QVBoxLayout *Communication_front::create_serial_layout() {
    QHBoxLayout *speed_lay = new QHBoxLayout();
    speed_lay->addWidget(new QLabel("Скорость"));
    const QStringList speeds = {"1200",  "2400",  "4800",  "9600",  "14400",
                                "19200", "38400", "57600", "115200"};
    serial_speed_list = new QComboBox();
    serial_speed_list->addItems(speeds);
    serial_speed_list->setCurrentText(speeds[3]);

    speed_lay->addWidget(serial_speed_list);

    QVBoxLayout *port_lay = new QVBoxLayout();
    port_lay->addLayout(speed_lay);
    port_lay->addStretch(1);
    port_lay->addLayout(speed_lay);

    return port_lay;
}


QWidget *Communication_front::create_serial_widget(void) {
    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addLayout(create_find_port_layout());
    main_layout->addLayout(create_serial_layout());
    QWidget *wgt = new QWidget();
    wgt->setLayout(main_layout);
    return wgt;
}


QVBoxLayout *Communication_front::create_udp_layout() {
    QVBoxLayout *ip_lay = new QVBoxLayout();
    ip_lay->addWidget(new QLabel("IP адрес"));
    ip_lay->addWidget(new QLabel("Порт"));

    QVBoxLayout *port_lay = new QVBoxLayout();

    ip_line_udp = new QLineEdit();

    QRegExp ipRegex(
        "(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[0-9]{2}|[0-9])"
        "(\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[0-9]{2}|[0-9])){3}");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ip_line_udp->setValidator(ipValidator);
    ip_line_udp->setText("127.0.0.1");

    port_lay->addWidget(ip_line_udp);
    port_line_udp = new QLineEdit();

    QRegExp port_val("(6553[0-5])|655[0-2][0-9]|65[0-4][0-9]{2}|"
                     "6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|"
                     "[1-9][0-9]{2}|[1-9][0-9]|[1-9]");
    port_line_udp->setValidator(new QRegExpValidator(port_val, this));
    port_line_udp->setText("5001");
    port_lay->addWidget(port_line_udp);

    QHBoxLayout *hlay = new QHBoxLayout();
    hlay->addLayout(ip_lay);
    hlay->addLayout(port_lay);

    QVBoxLayout *ether_lay = new QVBoxLayout();
    ether_lay->addLayout(hlay);
    return ether_lay;
}


QVBoxLayout *Communication_front::create_tcp_layout() {
    QVBoxLayout *ip_lay = new QVBoxLayout();
    ip_lay->addWidget(new QLabel("IP адрес"));
    ip_lay->addWidget(new QLabel("Порт"));

    QVBoxLayout *port_lay = new QVBoxLayout();

    ip_line_tcp = new QLineEdit();

    QRegExp ipRegex("(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[0-9]{2}|[0-9])(\\.(25["
                    "0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[0-9]{2}|[0-9])){3}");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ip_line_tcp->setValidator(ipValidator);

    port_lay->addWidget(ip_line_tcp);
    port_line_tcp = new QLineEdit();

    QRegExp pattern("(6553[0-5])|655[0-2][0-9]|65[0-4][0-9]{2}|"
                    "6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|"
                    "[1-9][0-9]{2}|[1-9][0-9]|[1-9]");
    QValidator *uint16val1 = new QRegExpValidator(pattern, this);
    port_line_tcp->setValidator(uint16val1);
    port_lay->addWidget(port_line_tcp);

    QHBoxLayout *hlay = new QHBoxLayout();
    hlay->addLayout(ip_lay);
    hlay->addLayout(port_lay);

    QVBoxLayout *ether_lay = new QVBoxLayout();
    ether_lay->addLayout(hlay);
    return ether_lay;
}


QWidget *Communication_front::create_udp_widget() {
    QWidget *wgt = new QWidget();
    wgt->setLayout(create_udp_layout());
    return wgt;
}


QWidget *Communication_front::create_tcp_widget() {
    QWidget *wgt = new QWidget();
    wgt->setLayout(create_tcp_layout());
    return wgt;
}


void Communication_front::open_connection() {
    if (btn_connection->text() == btn_open_name) {
        quint8 addr = static_cast<quint8>(device_addr->text().toInt());
        if (addr == 0) {
            addr = 1;
            device_addr->setText("1");
        }
        emit is_new_connect(addr);
        int ix = type_connection->currentIndex();
        QString name;
        qint32 number;
        switch (ix) {
        case 0:
            name = serial_port_list->currentText();
            number = serial_speed_list->currentText().toInt();
            break;
        case 1: {
            QRegExp ipRegex(
                "(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[0-9]{2}|[0-9])(\\.(25[0-5]"
                "|2[0-4][0-9]|[0-1][0-9]{2}|[0-9]{2}|[0-9])){3}");
            QString ipbuf = ip_line_udp->text();
            if (!ipRegex.exactMatch(ipbuf)) {
                QMessageBox msgBox;
                const QString msg("Неверно задан IP адрес");
                msgBox.setText(msg);
                msgBox.exec();
                return;
            }
            name = ipbuf;
            qint32 port = port_line_udp->text().toInt();
            if (port == 0 || port > 65535) {
                number = 5001;
            } else {
                number = port;
            }
        } break;
        case 2:
            name = ip_line_tcp->text();
            number = port_line_tcp->text().toInt();
            break;
        default:
            return;
        }
        if (!connection->open(name, number)) {
            QMessageBox msgBox;
            const QString msg("Невозможно открыть соединение.\n"
                              "Пожалуйста, выберите другой способ");
            msgBox.setText(msg);
            msgBox.exec();
            return;
        }
        connect(connection, &ICommunication::data_is_ready, this,
                &Communication_front::data_is_ready);
        btn_connection->setText(btn_close_name);
        emit is_open();
    } else {
        connection->close();
        btn_connection->setText(btn_open_name);
        disconnect(connection, &ICommunication::data_is_ready, this,
                   &Communication_front::data_is_ready);
        emit is_close();
    }
}


void Communication_front::change_port_widgets(int idx) {
    if (btn_connection->text() == btn_close_name) {
        type_connection->setCurrentIndex(prev_ix);
        return;
    }
    stack_communication->setCurrentIndex(idx);
    prev_ix = idx;
    delete connection;
    connection = nullptr;
    switch (idx) {
    case 0:
        connection = new SerialBack(this);
        break;
    case 1:
        connection = new UdpBack(this);
        break;
    case 2:
        connection = new TcpBack(this);
        break;
    default:
        break;
    }
}


void Communication_front::write(const QByteArray &data) {
    if (btn_connection->text() == btn_close_name) {
        connection->write(data);
    }
}
