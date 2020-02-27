#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>


class Communication_front;

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    QTabWidget *wgt;
    QPushButton *btn;

public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();
private slots:
    void set_active_tab(void);
    void set_inactive_tab(void);
};
#endif // MAINWINDOW_H
