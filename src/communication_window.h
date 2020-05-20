#pragma once


#include <QWidget>


class QLineEdit;
class QPlainTextEdit;
class QCheckBox;


class Communication_window : public QWidget {
    Q_OBJECT

private:
    QPlainTextEdit *read_txt;
    QPlainTextEdit *write_txt;
    QLineEdit *write_line;

    QCheckBox *read_hex_view;
    QCheckBox *write_hex_view;

public:
    Communication_window(QWidget *parent = nullptr);
    ~Communication_window();

signals:
    void data_is_send(const QByteArray &);
public slots:
    void get_read_data(const QByteArray &d);
private slots:
    void send(void);
    void setHexWriteView();
};
