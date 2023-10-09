#ifndef MAP_QUERY_H
#define MAP_QUERY_H

#include <QDialog>
#include"showmess.h"

namespace Ui {
class map_query;
}

class map_query : public QDialog
{
    Q_OBJECT

public:
    explicit map_query(QWidget *parent = nullptr);
    ~map_query();

private slots:
    void paintEvent(QPaintEvent*);              /* 窗口绘制 */
    void mousePressEvent(QMouseEvent* event);   /* 鼠标响应 */
    double Count_distanse(QPoint p_1, QPoint p_2);
    bool is_Click_Suc(QPoint x, QPoint y, int n);
    void Recover();
    void on_back_clicked();

    void on_load_map_clicked();

    void on_shortest_rode_clicked();

    void on_all_rode_clicked();

    void on_mintree_clicked();


    void on_clear_clicked();

private:
    Ui::map_query *ui;
    showmess* newshowmess;
};

#endif // MAP_QUERY_H
