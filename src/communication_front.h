#pragma once

#include <QWidget>

class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class SerialInfo;
class QStringList;
class QPushButton;
class QStackedWidget;
class ICommunication;
class QLineEdit;

class Communication_front : public QWidget {
    Q_OBJECT

private:
    const QString btn_open_name;
    const QString btn_close_name;
    SerialInfo *serialInfo;
    QComboBox *type_connection;
    ICommunication *connection;

    QComboBox *serial_port_list;
    QComboBox *serial_speed_list;
    QPushButton *btn_connection;
    QStackedWidget *stack_communication;

    QLineEdit *device_addr;
    QLineEdit *ip_line_udp;
    QLineEdit *port_line_udp;
    QLineEdit *ip_line_tcp;
    QLineEdit *port_line_tcp;

    int prev_ix;

    QHBoxLayout *create_find_port_layout();
    QVBoxLayout *create_serial_layout();
    QVBoxLayout *create_udp_layout();
    QVBoxLayout *create_tcp_layout();

    QWidget *create_serial_widget(void);
    QWidget *create_udp_widget(void);
    QWidget *create_tcp_widget(void);

public:
    Communication_front(QWidget *parent = nullptr);
    ~Communication_front();
public slots:
    void write(const QByteArray &data);
private slots:
    void open_connection(void);
    void print_available_ports(const QStringList *available);
    void change_port_widgets(int);
signals:
    void data_is_ready(const QByteArray &);
    void is_new_connect(quint8);
    void is_open(void);
    void is_close(void);
};
