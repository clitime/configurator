#pragma once


#include <QObject>

class QStringList;

class SerialInfo : public QObject {
    Q_OBJECT
private:
    QStringList *serial_source = nullptr;

public:
    explicit SerialInfo(QObject *parent = nullptr);
    ~SerialInfo();
    QStringList *get_available(void);

signals:
    /**
     * call after scannig available port
     */
    void available_port(const QStringList *);

public slots:
    void scan_available_port(void) { emit available_port(get_available()); };
};
