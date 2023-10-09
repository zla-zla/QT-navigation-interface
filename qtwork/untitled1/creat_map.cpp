#include "creat_map.h"
#include "ui_creat_map.h"
#include "mainwindow.h"

#include <QtWidgets>
#include <vector>
#include <fstream>
#include <algorithm>

typedef struct line_struct	// 边的结构体
{
    int ind;                // 每条边的编号
    int node_1, node_2;     // 边的起点和终点
    double dis;             // 边的长度
}Line;

static const int show_window_x = 20;            // 窗口的左上角x
static const int show_window_y = 60;            // 窗口的左上角y
static const int show_window_width = 1200;      // 窗口的宽
static const int show_window_height = 540;      // 窗口的高

static const int Node_MAX_NUM = 100;            // 设置最大结点数
static const int Side_MAX_NUM = 300;            // 设置最大边数
static const int RADIUS = 5;                    // 点击半径
static int function_num = 0;                    // 功能号(针对不同的操作会产生不同的效果)
static int node_num;                            // 结点个数
static int side_num;                            // 边的条数
static int info_ind;                            // 结点标签的数组下标

static QPoint point[Node_MAX_NUM];              // 结点数组
static Line line[Side_MAX_NUM];                 // 边数组
static double dis_matrix[Node_MAX_NUM][Node_MAX_NUM];      // 距离矩阵
static QString point_name[Node_MAX_NUM];        // 结点的文字标签数组
static QString point_info[Node_MAX_NUM];        // 结点的信息标签数组
static QString pic_path;                        // 图片路径
static bool show_len_flag = false;              // 是否显示当前路径长度

//构造函数
creat_map::creat_map(QWidget *parent) :QDialog(parent),ui(new Ui::creat_map)
{
    ui->setupUi(this);
    Recover();
}
//析构函数
creat_map::~creat_map()
{
    delete ui;
}

// 按钮信息重置
void creat_map::Recover()
{
    // ui->Message_1->setTextElideMode(Qt::ElideNone); // 自动换行
    ui->Message_1->clear();
    ui->add_node->setText("添加点");
    ui->add_edge->setText("添加边");
    ui->del_node->setText("删除点");
    ui->del_edge->setText("删除边");
    ui->mod_node->setText("修改点");
    ui->mod_edge->setText("修改边");
    ui->back->setText("返回");
    ui->clear_bg->setText("清除屏幕");
    ui->load_bg->setText("加载背景");
    ui->load_map->setText("加载地图");
    ui->save_map->setText("保存地图");
    ui->showlen->setText("显示路径长度");

    ui->add_node->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->add_edge->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->del_node->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->del_edge->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->mod_node->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->mod_edge->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->back->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->clear_bg->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->load_bg->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->load_map->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->save_map->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->showlen->setStyleSheet("border-image: url(../image/btn_1.png);");
}

// 计算两点间距离的函数
double creat_map::Count_distanse(QPoint p_1, QPoint p_2)
{
    double delta_x, delta_y;
    delta_x = p_1.x() - p_2.x();
    delta_y = p_1.y() - p_2.y();
    return sqrt( delta_x*delta_x + delta_y*delta_y );
}

// 判断目标是否选中的函数
bool creat_map::is_Click_Suc(QPoint x, QPoint y, int n)   // 判断目标是否选中的函数
{
    if (Count_distanse(x, y) <= n)
        return true;
    else
        return false;
}

//绘制函数
void creat_map::paintEvent(QPaintEvent*){
    QPainter painter(this);                     // 画笔，this为当前窗口
    QFont font1("ZYSong 18030", 8);             // 新建 1 号字体
    QFont font2("ZYSong 18030", 10);            // 新建 2 号字体
    this->setMaximumSize(1500,900);             // 固定大小
    this->setMinimumSize(1500,900);
    if( pic_path == "" )            // 如果无背景图片
        painter.setBrush(Qt::white);            // 白底填充
    else
    {
        painter.drawPixmap(show_window_x, show_window_y, show_window_width, show_window_height, QPixmap(pic_path));   // 载入背景图片并显示出来
    }
    painter.drawRect(show_window_x, show_window_y, show_window_width, show_window_height);   // 绘制一个矩形框
    painter.setFont(font2);
    painter.setRenderHint(QPainter::Antialiasing, true);    // 设置:绘图光滑
    //画线
    for( int i = 1; i <= side_num; i++ )       // 先画线，更好看！不然线有覆盖“景点”的情况出现
    {
        painter.setPen(QPen(Qt::blue,2));           // 设置画笔颜色为蓝色
        painter.drawLine(point[line[i].node_1], point[line[i].node_2]);   //两点连线
    }
    //标边长
    painter.setFont(font1);
    if(show_len_flag)        //如果显示边的标签
    {
        painter.setPen(Qt::darkMagenta);            // 洋红色
        for( int i = 1; i < node_num; i++ )
        {
            for( int j = i + 1; j <= node_num; j++ )
            {
                if( dis_matrix[i][j] )             //若路径存在
                {
                    int zuo_x, zuo_y;
                    QString str = QString::number(dis_matrix[i][j]);    // 标签
                    zuo_x = (point[i].x() + point[j].x()) / 2 - 10;
                    zuo_y = (point[i].y() + point[j].y()) / 2 + 5;
                    painter.drawText( QPoint(zuo_x, zuo_y), str );      // 写在该“边”的中间偏左下的地方
                }
            }
        }
    }
    //标点
    painter.setFont(font2);
    for(int i = 1; i <= node_num; i++)
    {
        painter.setPen(Qt::black);      // 黑色
        painter.setBrush(Qt::yellow);     // 内部填充(黄色)
        painter.drawEllipse(point[i], 4, 4);        // 把点画出来
        painter.drawText(QPoint(point[i].x() + 5, point[i].y() + 5), point_name[i]);    // 画出点的标签，point_name[i]为标签
    }
}



Line temp_Line;                     // 临时边
QPoint temp_Point_1, temp_Point_2;  // 两个临时的点
QString tmp1,tmp2;
//鼠标响应
void creat_map::mousePressEvent(QMouseEvent* e){
    if (e->button() == Qt::LeftButton)              // 按左键
    {
        QPoint cur_click_pos = e->pos();            // e->pos(): 获取当前点击位置
        switch(function_num)
        {
        case 1: // 添加点
            if(node_num < Node_MAX_NUM && cur_click_pos.x() >= show_window_x &&
                    cur_click_pos.x() <= show_window_x+show_window_width && cur_click_pos.y() >= show_window_y &&
                    cur_click_pos.y() <= show_window_y+show_window_height) // 判断所加的点是否在窗口范围内
            {
                int save_node_num = node_num;
                node_num++;
                for(int i = 1; i < node_num; i++)
                {
                    if(is_Click_Suc(cur_click_pos, point[i], RADIUS+10))   // 判断鼠标所点击位置和图上所有已添加的结点位置,是否靠的太近
                    {
                        node_num--;
                        QMessageBox::warning(this, "警告", "两个点靠太近！");
                    }
                }
                if(save_node_num == node_num)
                    break;
                point[node_num] = e->pos();                             // 当前位置赋给最新的结点
                point_name[node_num] = QString::number(++info_ind);     // 创建默认标签
                point_info[node_num]="无信息";
                update();
            }
            else if(node_num >= Node_MAX_NUM) QMessageBox::warning(this, "警告", "目前结点数已达上限，无法再继续添加！");
            else QMessageBox::warning(this, "警告", "新加结点已超出边界！");
            ui->Message_1->clear();
            ui->Message_1->addItem("目前有结点个数：" + QString::number(node_num));
            ui->Message_1->addItem("目前有边的条数：" + QString::number(side_num));
            ui->Message_1->addItem("如果还要继续添加, 请选择下一个点的位置。");
            function_num = 1;           // 功能号 1 保持不变(便于重复添加点)
            break;
        case 2:         // 新增边(选择边的起点)
            if(side_num >= Side_MAX_NUM) QMessageBox::warning(this, "警告", "路径数已达上限！");
            else
            {
                for( int i = 1; i <= node_num; i++ )
                {
                    if( is_Click_Suc(cur_click_pos, point[i], RADIUS) )   // 判断是否选中
                    {
                        function_num = 3;           // 找到了新增边的起点后, 还需找到其终点. 故把控制权交给功能号3
                        temp_Point_1 = point[i];
                        line[side_num + 1].node_1 = i;
                        ui->Message_1->clear();
                        ui->Message_1->addItem("请选择边的终点位置");
                        break;
                    }
                }
            }
            update();
            break;

        case 3:     // 新增边(选择边的终点)
            for( int i = 1; i <= node_num; i++ )
            {
                if(point[i] != temp_Point_1 && is_Click_Suc(cur_click_pos, point[i], RADIUS)) // 若选中了与第一个点不同的点
                {
                    function_num = 2;                       // 重新把控制权交给功能号2(便于重复添加“边”)
                    int save_side_num = side_num++;         // 线数量 + 1
                    temp_Point_2 = point[i];
                    line[side_num].node_2 = i;
                    if(line[side_num].node_1 > line[side_num].node_2)        // 确保边的起点下标比终点的小, 不然做交换
                    {
                        int temp = line[side_num].node_1;
                        line[side_num].node_1 = line[side_num].node_2;
                        line[side_num].node_2 = temp;
                    }

                    for( int j = 1; j < side_num; j++ )          // 判断是否路线已经存在
                    {
                        if(line[side_num].node_1 == line[j].node_1 && line[side_num].node_2 == line[j].node_2)
                        {
                            line[side_num] = line[0];
                            side_num--;
                            QMessageBox::warning(this, "警告", "该路径已添加！");
                            break;
                        }
                    }

                    if(save_side_num != side_num)      // 如果路该线之前在图中不存在, 则该表达式成立
                    {
                        int ind_1 = line[side_num].node_1;
                        int ind_2 = line[side_num].node_2;
                        dis_matrix[ind_1][ind_2] = dis_matrix[ind_2][ind_1] = Count_distanse(point[ind_1], point[ind_2]);   // 距离矩阵赋值
                        line[side_num].ind = side_num;      // 边的“编号”
                        line[side_num].dis = Count_distanse(point[ind_1], point[ind_2]);    // 边的长度
                    }
                    ui->Message_1->clear();
                    ui->Message_1->addItem("目前有结点个数：" + QString::number(node_num));
                    ui->Message_1->addItem("目前有边的条数：" + QString::number(side_num));
                    ui->Message_1->addItem("如果还要继续添加边, 请选择下一条边的起点");
                    break;
                }
            }
            update();
            break;

        case 4:                     // 删除结点
            if(node_num == 0)
            {
                QMessageBox::warning(this, "警告", "没有结点可删除！");
            }
            else
            {
                for( int i = 1; i <= node_num; i++ )
                {
                    if(is_Click_Suc(cur_click_pos, point[i], RADIUS))
                    {
                        for( int j = i; j < node_num; j++ )
                        {
                            point[j] = point[j + 1];                        // 将删除的结点后的点前移
                            point_name[j] = point_name[j + 1];              // 结点的标签做同样处理
                            point_info[j] = point_info[j + 1];
                        }
                        // 删除该结点所在的行
                        for( int j = i; j < node_num; j++ )
                            for ( int k = 1; k <= node_num; k++ )
                                dis_matrix[k][j] = dis_matrix[k][j + 1];
                        // 删除该结点所在的列
                        for( int j = i; j < node_num; j++ )
                            for( int k = 1; k <= node_num; k++ )
                                dis_matrix[j][k] = dis_matrix[j + 1][k];
                        //删除最后一行一列
                        for (int j = 1; j <= node_num; j++)
                            dis_matrix[j][node_num] = dis_matrix[node_num][j] = 0;

                        point[node_num] = point[0];                 // 最后一个点消失
                        point_name[node_num] = point_name[0];       // 最后一个点标签消失(注意:其他点的标签保持不变)
                        point_info[node_num] = point_info[0];
                        node_num--;

                        for( int j = 1; j <= side_num; j++ )        // 对每一边都进行相应的判断
                        {
                            if(i == line[j].node_1 || i == line[j].node_2)   // 只要边的起点和终点有一个是“即将被删除的结点”,则进行后续处理
                            {
                                for( int k = j; k < side_num; k++ )     // 将“边数组”也一并前移【除ind】
                                {
                                    line[k].dis = line[k + 1].dis;
                                    line[k].node_1 = line[k + 1].node_1;
                                    line[k].node_2 = line[k + 1].node_2;
                                }
                                line[side_num] = line[0];           // 将最后一条边去除
                                side_num--;
                                j--;                // 连续判断所需(因为边数组已前移)
                            }
                            else                    // 在“边数组”里面, 也要更新每一条边(大于删除结点编)的起点和终点
                            {
                                if(line[j].node_1 > i)
                                    line[j].node_1--;
                                if(line[j].node_2 > i)
                                    line[j].node_2--;
                            }
                        }
                        update();
                        break;
                    }
                }
            }
            ui->Message_1->clear();
            ui->Message_1->addItem("目前有结点个数：" + QString::number(node_num));
            ui->Message_1->addItem("目前有边的条数：" + QString::number(side_num));
            ui->Message_1->addItem("如果还要继续删除结点, 请选择下一个点的位置。");
            function_num = 4;           // 功能号 4 保持不变(便于重复进行删除操作)
            break;

        case 5:         // 删除边(选择边的起点)
            if(side_num == 0) QMessageBox::warning(this, "警告", "图中已无边可删除！");
            else
            {
                for( int i = 1; i <= node_num; i++ )
                {
                    if(is_Click_Suc(cur_click_pos, point[i], RADIUS))
                    {
                        function_num = 6;           // 找到了删除边的起点后, 还需找到其终点. 故把控制权交给功能号6
                        temp_Point_1 = point[i];
                        temp_Line.node_1 = i;
                        ui->Message_1->addItem("请选择要需删除边的终点");
                        break;
                    }
                }
            }
            break;
        case 6:         // 删除边(选择边的终点)
            for(int i = 1; i <= node_num; i++)
            {
                if(point[i] != temp_Point_1 && is_Click_Suc(cur_click_pos, point[i], RADIUS)) // 选中
                {
                    function_num = 5;                          // 重新把控制权交给功能号5(便于重复删除“边”)
                    temp_Point_2 = point[i];
                    temp_Line.node_2 = i;
                    if(temp_Line.node_1 > temp_Line.node_2)    // 保证线的起点的下标小于终点的下标
                    {
                        int temp = temp_Line.node_1;
                        temp_Line.node_1 = temp_Line.node_2;
                        temp_Line.node_2 = temp;
                    }
                    int side_save_num = side_num;
                    for( int j = 1; j <= side_num; j++ )
                    {
                        if(temp_Line.node_1 == line[j].node_1 && temp_Line.node_2 == line[j].node_2)   // 判断图中是否有该边
                        {
                            int start = line[j].node_1;
                            int end = line[j].node_2;
                            dis_matrix[start][end] = dis_matrix[end][start] = 0;        // 将距离矩阵中的该边清零
                            for (int k = j; k < side_num; k++)      // 把(编号大于删除边的)下标前移【除ind】
                            {
                                line[k].dis = line[k + 1].dis;
                                line[k].node_1 = line[k + 1].node_1;
                                line[k].node_2 = line[k + 1].node_2;
                            }
                            line[side_num] = line[0];
                            side_num--;
                            break;
                        }
                    }
                    if(side_save_num == side_num)QMessageBox::warning(this, "警告", "图中无此边！");
                    else
                    {
                        ui->Message_1->clear();
                        ui->Message_1->addItem("目前有结点个数：" + QString::number(node_num));
                        ui->Message_1->addItem("目前有边的条数：" + QString::number(side_num));
                        ui->Message_1->addItem("如果还要继续删除边, 请选择下一条边的起点");
                        break;
                    }
                }
            }
            update();
            break;

        case 7:             // 编辑结点的标签
            if(node_num == 0)
            {
                QMessageBox::warning(this, "警告", "没有结点可编辑！");
            }
            else
            {
                for(int i = 1; i <= node_num; i++)
                {
                    if(is_Click_Suc(cur_click_pos, point[i], RADIUS))
                    {
                        QString str = QInputDialog::getText(this, "编辑框", "输入文本(最多15个字)");
                        if (str != "")
                            point_name[i] = str.left(15);   // 左对齐
                        break;
                    }
                }
            }
            ui->Message_1->clear();
            ui->Message_1->addItem("如果还要继续编辑结点信息, 请选择下一个结点");
            break;

        case 8:     // 修改边(选择边的起点)
            if (side_num == 0) QMessageBox::warning(this, "警告", "无任何路径");
            else
            {
                for (int i = 1; i <= node_num; i++)
                {
                    if (is_Click_Suc(cur_click_pos, point[i], RADIUS))
                    {
                        function_num = 9;               // 找到了修改边的起点后, 还需找到其终点. 故把控制权交给功能号9
                        temp_Point_1 = point[i];
                        temp_Line.node_1 = i;
                        ui->Message_1->addItem("请选择需修改边的终点");
                        break;
                    }
                }
            }
            break;

        case 9:    // 修改边(选择边的终点)
            for (int i = 1; i <= node_num; i++)
            {
                if (point[i] != temp_Point_1 && is_Click_Suc(cur_click_pos, point[i], RADIUS))  //不能和起点重合
                {
                    function_num = 8;                   // 重新把控制权交给功能号8(便于重复修改“边”)
                    temp_Point_2 = point[i];
                    temp_Line.node_2 = i;
                    if (temp_Line.node_1 > temp_Line.node_2)    // 确保修改边起点下标小于终点的下标
                    {
                        int temp = temp_Line.node_1;
                        temp_Line.node_1 = temp_Line.node_2;
                        temp_Line.node_2 = temp;
                    }
                    bool temp_flag = false;             // 临时标记(看是否已作修改)
                    for (int j = 1; j <= side_num; j++)
                    {
                        if (temp_Line.node_1 == line[j].node_1 && temp_Line.node_2 == line[j].node_2)   // 判断图中有无此边
                        {
                            double dis = dis_matrix[temp_Line.node_1][temp_Line.node_2];
                            double number = QInputDialog::getDouble(this, "修改边的长度", "输入浮点数(0.0001~999999)", dis, 0.0001, 999999, 4);
                            if(number)
                                dis_matrix[temp_Line.node_1][temp_Line.node_2] = dis_matrix[temp_Line.node_2][temp_Line.node_1] = number;   // 若输入了数，则边进行修改
                            temp_flag = true;
                            break;
                        }
                    }
                    if (!temp_flag) QMessageBox::warning(this, "警告", "图中无此边！");
                    else ui->Message_1->addItem("如果还要继续修改边, 请选择下一条边的起点");
                    update();
                    break;
                }
            }
            break;
        }
    }
}


//编辑按键
void creat_map::on_add_node_clicked()
{
    Recover();
    if(function_num != 1)
    {
        function_num = 1;
        ui->add_node->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->add_node->setText("停止该操作");
        ui->Message_1->clear();
        ui->Message_1->addItem("请选择一个位置添加新结点");
    }
    else
    {
        function_num = 0;
        ui->add_node->setText("添加点");
        ui->Message_1->clear();
    }
}

void creat_map::on_add_edge_clicked()
{
    Recover();
    if(function_num != 2)
    {
        function_num = 2;
        ui->add_edge->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->add_edge->setText("停止该操作");
        ui->Message_1->clear();
        ui->Message_1->addItem("请选择新增边的起点");
    }
    else
    {
        function_num = 0;
        ui->add_edge->setText("添加边");
        ui->Message_1->clear();
    }
}

void creat_map::on_del_node_clicked()
{
    Recover();
    if(function_num != 4)
    {
        function_num = 4;
        ui->del_node->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->del_node->setText("停止该操作");
        ui->Message_1->clear();
        ui->Message_1->addItem("请选择删除结点的位置");
    }
    else
    {
        function_num = 0;
        ui->del_node->setText("删除点");
        ui->Message_1->clear();
    }
}

void creat_map::on_del_edge_clicked()
{
    Recover();
    if(function_num != 5)
    {
        function_num = 5;
        ui->del_edge->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->del_edge->setText("停止该操作");
        ui->Message_1->clear();
        ui->Message_1->addItem("请选择需删除边的起点");
    }
    else
    {
        function_num = 0;
        ui->del_edge->setText("删除边");
        ui->Message_1->clear();
    }

}

void creat_map::on_mod_node_clicked()
{
    Recover();
    if(function_num != 7)
    {
        function_num = 7;
        ui->mod_node->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->mod_node->setText("停止该操作");
        ui->Message_1->addItem("请选择需编辑的结点");
    }
    else
    {
        function_num = 0;
        ui->mod_node->setText("修改点");
        ui->Message_1->clear();
    }

}

void creat_map::on_mod_edge_clicked()
{
    Recover();
    if(function_num != 8)
    {

        function_num = 8;
        ui->mod_edge->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->mod_edge->setText("停止该操作");
        ui->Message_1->addItem("请选择需要修改的边的起点");
    }
    else
    {
        function_num = 0;
        ui->mod_edge->setText("修改边");
        ui->Message_1->clear();
    }
}

//返回按键
void creat_map::on_back_clicked()
{
    MainWindow *p=new MainWindow();
    p->show();
    this->close();
}
//展示边长
void creat_map::on_showlen_clicked()
{
    if(!show_len_flag )
    {
        show_len_flag = true;
        ui->showlen->setText("隐藏所有边的长");
        ui->showlen->setStyleSheet("border-image: url(../image/btn_2.png);");
    }
    else
    {
        show_len_flag = false;
        ui->showlen->setText("显示所有边的长");  // 每点一下切换一下状态
        ui->showlen->setStyleSheet("border-image: url(../image/btn_1.png);");
    }
    update();
}
//加载地图
void creat_map::on_load_map_clicked()
{
    Recover();
    ui->load_map->setStyleSheet("border-image: url(../image/btn_2.png);");
    using namespace std;
    QString filename = QFileDialog::getOpenFileName(this, "选择文件", "", "地图文件(*.My_map)");
    if(filename != "")      // 判断文件地址是否输入成功
    {
        int x, y;           // 用于读取征信数据
        string str;         // 用于读取字符串数据
        ifstream in(filename.toStdString());
        if(in.is_open())
        {
            in >> side_num >> node_num;
            for( int i = 1; i <= node_num; i++ )
            {
                in >> x >> y;       // 读取每一个点的横纵坐标
                point[i].setX(x);
                point[i].setY(y);
                in >> str;
                point_name[i] = QString::fromStdString(str);
                in >> str;
                point_info[i] = QString::fromStdString(str);
            }
            for( int i = 1; i <= node_num; i++ )
                for (int j = 1; j <= node_num; j++)
                    in >> dis_matrix[i][j];     // 读取距离矩阵

            for( int i = 1; i <= side_num; i++ )
                in >> line[i].ind >> line[i].node_1 >> line[i].node_2 >> line[i].dis;   // 读取每条边的数据

            in >> str;          // 背景图片读取路径
            pic_path = QString::fromStdString(str);
            QMessageBox::information(this, "提示", "读取地图成功");
            in.close();         // 关闭文件
        }
        else
        {
            QMessageBox::information(this, "提示", "读取地图失败");
        }
    }
    ui->load_map->setStyleSheet("border-image: url(../image/btn_1.png);");

}
//保存地图
void creat_map::on_save_map_clicked()
{
    using namespace std;
    Recover();
    ui->save_map->setStyleSheet("border-image: url(../image/btn_2.png);");
    QString filename = QFileDialog::getSaveFileName(this, "保存文件", "", "地图文件(*.My_map)");  // 文件扩展名为.My_map
    if(filename != "")         // 判断文件地址是否输入成功
    {
        ofstream out(filename.toStdString());               // 转换为标准字符串并进行保存
        if(out.is_open())
        {
            out << side_num << ' ' << node_num << endl;     // 先输入边的条数和结点个数
            for( int i = 1; i <= node_num; i++ )
                out << point[i].x() << ' ' << point[i].y() << ' ' << point_name[i].toStdString()<< ' ' << point_info[i].toStdString() << endl;

            for( int i = 1; i <= node_num; i++ )
            {
                for( int j = 1; j <= node_num; j++ )
                    out << dis_matrix[i][j] << ' ';
                out << endl;
            }

            for( int i = 1; i <= side_num; i++ )
                out << line[i].ind << ' ' << line[i].node_1 << ' ' << line[i].node_2 << ' ' << line[i].dis << endl;
            out << pic_path.toStdString() << endl;

            QMessageBox::information(this, "提示", "保存地图成功");
            out.close();        // 关闭文档
        }
        else
            QMessageBox::information(this, "提示", "保存地图失败");
    }
    ui->save_map->setStyleSheet("border-image: url(../image/btn_1.png);");
}
//加载背景图
void creat_map::on_load_bg_clicked()
{
    Recover();
    ui->load_bg->setStyleSheet("border-image: url(../image/btn_2.png);");
    QString temp = QFileDialog::getOpenFileName(this, "选择文件", "", "图像文件(*.jpg *.jpeg *.bmg *.png *.gif)");   // 设定只支持三种图片格式
    if(temp != "")
        pic_path = temp;     // 传入图片存储路径
    update();                // 重绘(背景图片就会被画上去)
    ui->load_bg->setStyleSheet("border-image: url(../image/btn_1.png);");
}
//清空背景
void creat_map::on_clear_bg_clicked()
{
    QMessageBox::StandardButton answer;
    Recover();
    ui->clear_bg->setStyleSheet("border-image: url(../image/btn_2.png);");
    answer = QMessageBox::question(this, "提示", "确认要清除屏幕吗？", QMessageBox::Yes | QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
        pic_path = "";              // 图片路径清零
        for ( int i = 1; i <= node_num; i++ )
        {
            point[i] = point[0];    // 各个点的坐标位置清零
            point_name[i] = point_name[0];          // 各个点的标签清零
            for ( int j = 1; j <= node_num; j++ )
                dis_matrix[i][j] = 0;               // 距离矩阵清零
        }
        for (int i = 1; i <= node_num; i++)
            line[i] = line[0];                      // 各条边清零
        function_num = node_num = side_num = info_ind = 0;      // 功能号、结点数、边数、结点标签的数组下标都清零
        ui->Message_1->clear();
    }
    ui->clear_bg->setStyleSheet("border-image: url(../image/btn_1.png);");

}
