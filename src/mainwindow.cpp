#include "mainwindow.h"
#include "communication_front.h"
#include "communication_window.h"
#include "control_window.h"
#include "pelcod_protocol.h"
#include "setting_window.h"
#include <QTabWidget>


MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent), wgt(new QTabWidget(this)) {
    Communication_window *cmw = new Communication_window(this);
    PelcoDTransport *pelcod = new PelcoDTransport(this);
    Communication_front *cmt = new Communication_front(this);
    SettingWindow *setting = new SettingWindow(this, pelcod);
    Control_window *ctrl = new Control_window(this, pelcod);

    connect(cmt, &Communication_front::is_new_connect, pelcod,
            &PelcoDTransport::setAddress);

    connect(cmw, &Communication_window::data_is_send, cmt,
            &Communication_front::write);

    connect(cmt, &Communication_front::data_is_ready, cmw,
            &Communication_window::get_read_data);

    connect(cmt, &Communication_front::data_is_ready, pelcod,
            &PelcoDTransport::receive_msg);

    connect(pelcod, &PelcoDTransport::data_is_ready, setting,
            &SettingWindow::get_read_data);

    connect(setting, &SettingWindow::data_is_send, cmt,
            &Communication_front::write);

    connect(ctrl, &Control_window::data_is_send, cmt,
            &Communication_front::write);

    connect(cmt, &Communication_front::is_open, this,
            &MainWindow::set_active_tab);

    connect(cmt, &Communication_front::is_close, this,
            &MainWindow::set_inactive_tab);

    wgt->addTab(cmt, "Соединение");
    wgt->addTab(cmw, "Терминал");
    wgt->addTab(setting, "Конфигурация");
    wgt->addTab(ctrl, "Управление");
    wgt->setTabEnabled(1, false);
    wgt->setTabEnabled(2, false);
    wgt->setTabEnabled(3, false);
    this->setCentralWidget(wgt);
}


MainWindow::~MainWindow() {}


void MainWindow::set_active_tab(void) {
    wgt->setTabEnabled(1, true);
    wgt->setTabEnabled(2, true);
    wgt->setTabEnabled(3, true);
}
void MainWindow::set_inactive_tab(void) {
    wgt->setTabEnabled(1, false);
    wgt->setTabEnabled(2, false);
    wgt->setTabEnabled(3, false);
}
