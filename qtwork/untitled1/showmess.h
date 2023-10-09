#ifndef SHOWMESS_H
#define SHOWMESS_H

#include <QDialog>

namespace Ui {
class showmess;
}

class showmess : public QDialog
{
    Q_OBJECT

public:
    explicit showmess(QWidget *parent = nullptr);
    ~showmess();
    void senddata(QString s1,QString s2);

private:
    Ui::showmess *ui;
};

#endif // SHOWMESS_H
