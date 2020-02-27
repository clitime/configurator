#pragma once

#include <QObject>
class QByteArray;
class QString;


class ICommunication : public QObject {
    Q_OBJECT
public:
    explicit ICommunication(QObject *parent = nullptr);
    virtual ~ICommunication();
    virtual void write(const QByteArray &data) = 0;
signals:
    void data_is_ready(const QByteArray &);
public slots:
    virtual bool open(const QString &arg1, qint32 arg2) = 0;
    virtual void close(void) = 0;
};
