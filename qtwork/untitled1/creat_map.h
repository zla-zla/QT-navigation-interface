#ifndef CREAT_MAP_H
#define CREAT_MAP_H

#include <QDialog>

namespace Ui {
class creat_map;
}

class creat_map : public QDialog
{
    Q_OBJECT

public:
    explicit creat_map(QWidget *parent = nullptr);
    ~creat_map();

private slots:
    void paintEvent(QPaintEvent*);              /* 窗口绘制 */
    void mousePressEvent(QMouseEvent* event);   /* 鼠标响应 */
    void Recover();
    double Count_distanse(QPoint p_1, QPoint p_2);
    bool is_Click_Suc(QPoint x, QPoint y, int n);


    void on_back_clicked();

    void on_add_node_clicked();

    void on_add_edge_clicked();

    void on_del_node_clicked();

    void on_del_edge_clicked();

    void on_mod_node_clicked();

    void on_mod_edge_clicked();

    void on_showlen_clicked();

    void on_load_map_clicked();

    void on_save_map_clicked();

    void on_load_bg_clicked();

    void on_clear_bg_clicked();

private:
    Ui::creat_map *ui;
};

#endif // CREAT_MAP_H
