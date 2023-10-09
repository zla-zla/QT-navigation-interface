#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "map_query.h"
#include "creat_map.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//设计地图
void MainWindow::on_pushButton_clicked()
{
    creat_map *p=new creat_map();
    p->show();
    this->close();
}
//查询地图
void MainWindow::on_pushButton_2_clicked()
{
    map_query *p=new map_query;
    p->show();
    this->close();
}
