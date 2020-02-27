#include "communication_window.h"

#include <QBoxLayout>
#include <QByteArray>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSplitter>


Communication_window::Communication_window(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *read_lay = new QVBoxLayout();
    QLabel *read_lbl = new QLabel("Полученные данные");
    read_hex_view = new QCheckBox("hex");

    QHBoxLayout *read_title_lay = new QHBoxLayout();
    read_title_lay->addWidget(read_lbl, 1);

    read_title_lay->addWidget(read_hex_view);
    QPushButton *read_clr_btn = new QPushButton("Очистить");

    read_title_lay->addWidget(read_clr_btn);
    read_lay->addLayout(read_title_lay);
    read_txt = new QPlainTextEdit();

    connect(read_clr_btn, &QPushButton::clicked, read_txt,
            &QPlainTextEdit::clear);

    read_txt->setReadOnly(true);
    read_lay->addWidget(read_txt);
    QWidget *read_wgt = new QWidget();
    read_wgt->setLayout(read_lay);

    QVBoxLayout *write_lay = new QVBoxLayout();
    QHBoxLayout *write_title_lay = new QHBoxLayout();
    QLabel *write_lbl = new QLabel("Переданные данные");

    write_title_lay->addWidget(write_lbl, 1);

    QPushButton *write_clear_btn = new QPushButton("Очистить");
    write_title_lay->addWidget(write_clear_btn);
    write_lay->addLayout(write_title_lay);

    write_txt = new QPlainTextEdit();
    write_txt->setReadOnly(true);

    connect(write_clear_btn, &QPushButton::clicked, write_txt,
            &QPlainTextEdit::clear);

    write_lay->addWidget(write_txt);

    QVBoxLayout *send_lay = new QVBoxLayout();
    QLabel *send_lbl = new QLabel("Отправить данные");
    send_lay->addWidget(send_lbl);

    write_line = new QLineEdit();
    QHBoxLayout *send_inter_lay = new QHBoxLayout();
    send_inter_lay->addWidget(write_line);

    write_hex_view = new QCheckBox("hex");
    send_inter_lay->addWidget(write_hex_view);

    connect(write_hex_view, &QCheckBox::stateChanged, this,
            &Communication_window::setHexWriteView);

    QPushButton *write_data = new QPushButton("Отправить");

    connect(write_data, &QPushButton::clicked, this,
            &Communication_window::send);
    send_inter_lay->addWidget(write_data);

    send_lay->addLayout(send_inter_lay);
    send_lay->setSizeConstraint(QLayout::SetFixedSize);
    write_lay->addLayout(send_lay);
    QWidget *write_wgt = new QWidget();
    write_wgt->setLayout(write_lay);

    QSplitter *spltr = new QSplitter(Qt::Vertical, this);
    spltr->addWidget(read_wgt);
    spltr->addWidget(write_wgt);
    QVBoxLayout *main_lay = new QVBoxLayout();
    main_lay->addWidget(spltr);
    this->setLayout(main_lay);
}


Communication_window::~Communication_window() {}


void Communication_window::send(void) {
    QByteArray read_data;

    if (write_hex_view->isChecked()) {
        QString buf(write_line->text());
        write_txt->appendPlainText(buf.toUpper());
        QStringList sl = write_line->text().split(' ');

        bool ok;
        QByteArray *d = new QByteArray();
        for (auto a : sl) {
            d->append(static_cast<char>(a.toUInt(&ok, 16)));
        }
        read_data.append(*d);
    } else {
        write_txt->appendPlainText(write_line->text());
        QByteArray *d = new QByteArray(
            QByteArray::fromStdString(write_line->text().toStdString()));
        read_data.append(*d);
    }
    emit data_is_send(read_data);
}


void Communication_window::get_read_data(const QByteArray &d) {
    if (read_hex_view->isChecked()) {
        QString buf(d.toHex(' '));
        read_txt->appendPlainText(buf.toUpper());
    } else {
        read_txt->appendPlainText(d);
    }
}


void Communication_window::setHexWriteView() {
    const QString write_buf = write_line->text();
    if (write_hex_view->isChecked()) {
        QRegExp ipRegex("([0-9a-fA-F]{2} )+([0-9a-fA-F]{2})?|[0-9a-fA-F]{1,2}");
        if (!ipRegex.exactMatch(write_buf)) {
            write_line->clear();
        }
        QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
        write_line->setValidator(ipValidator);
    } else {
        write_line->setValidator(nullptr);
    }
}
