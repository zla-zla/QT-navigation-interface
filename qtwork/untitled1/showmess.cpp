#include "showmess.h"
#include "ui_showmess.h"

showmess::showmess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showmess)
{
    ui->setupUi(this);
}

showmess::~showmess()
{
    delete ui;
}

void showmess::senddata(QString s1,QString s2){
    ui->title->setText(s1);
    ui->content->setText(s2);
}
