#include "map_query.h"
#include "ui_map_query.h"
#include"mainwindow.h"


#include <QtWidgets>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <QApplication>
#include <QLabel>

typedef struct line_struct	// 边的结构体
{
    int ind;                // 每条边的编号
    int node_1, node_2;     // 边的起点和终点
    double dis;             // 边的长度
    bool flag;              // 是否是当前标记为最短路径
}Line;

static const int show_window_x = 20;            // 窗口的左上角x
static const int show_window_y = 40;            // 窗口的左上角y
static const int show_window_width = 1200;      // 窗口的宽
static const int show_window_height = 540;      // 窗口的高

static const int Node_MAX_NUM = 100;            // 设置最大结点数
static const int Side_MAX_NUM = 300;            // 设置最大边数
static const int RADIUS = 5;               // 点击半径
static int function_num = 0;               // 功能号(针对不同的操作会产生不同的效果)
static int node_num;           // 结点个数
static int side_num;           // 边的条数
static int info_ind;           // 结点标签的数组下标

static QPoint point[Node_MAX_NUM];         // 结点数组
static Line line[Side_MAX_NUM];            // 边数组
static QString point_name[Node_MAX_NUM];   // 结点的编号标签数组
static QString point_info[Node_MAX_NUM];   // 结点的信息标签数组
static double dis_matrix[Node_MAX_NUM][Node_MAX_NUM];      // 距离矩阵
static QVector<int> shortest_path;          //最短路径
static QVector<QVector<int>> res;           //所有路径

static QString pic_path;                   // 图片路径
static bool shortest_flag;                     // 两点之间的最短路径的绘制标志

//构造函数
map_query::map_query(QWidget *parent) :QDialog(parent),ui(new Ui::map_query)
{
    ui->setupUi(this);
    Recover();
}
//析构函数
map_query::~map_query()
{
    delete ui;
}
//重置按键
void map_query::Recover()
{
    ui->Message_1->clear();
    ui->shortest_rode->setText("求两点间最短路径");
    ui->all_rode->setText("求两点间所有路径");
    ui->load_map->setText("加载地图");
    ui->clear->setText("清除地图");
    ui->mintree->setText("最小生成树");
    ui->back->setText("返回");

    ui->shortest_rode->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->all_rode->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->load_map->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->clear->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->mintree->setStyleSheet("border-image: url(../image/btn_1.png);");
    ui->back->setStyleSheet("border-image: url(../image/btn_1.png);");

}
// 计算两点间距离的函数
double map_query::Count_distanse(QPoint p_1, QPoint p_2)
{
    double delta_x, delta_y;
    delta_x = p_1.x() - p_2.x();
    delta_y = p_1.y() - p_2.y();
    return sqrt( delta_x*delta_x + delta_y*delta_y );
}
// 判断目标是否选中的函数
bool map_query::is_Click_Suc(QPoint x, QPoint y, int n)
{
    if (Count_distanse(x, y) <= n)
        return true;
    else
        return false;
}
// 画带箭头的线
void drawArrow(QPoint startPoint, QPoint endPoint, QPainter &p)
{
    double par = 15.0;                  // 箭头部分三角形的腰长
    double slopy = atan2((endPoint.y() - startPoint.y()), (endPoint.x() - startPoint.x()));
    double cosy = cos(slopy);
    double siny = sin(slopy);
    p.setBrush(Qt::red);                // 内部填充(白色)
    QPoint point1 = QPoint(endPoint.x() + int(-par*cosy - (par / 2.0*siny)), endPoint.y() + int(-par*siny + (par / 2.0*cosy)));
    QPoint point2 = QPoint(endPoint.x() + int(-par*cosy + (par / 2.0*siny)), endPoint.y() - int(par / 2.0*cosy + par*siny));
    QPoint points[3] = { endPoint, point1, point2 };
    p.setRenderHint(QPainter::Antialiasing, true);  // 消锯齿
    QPen drawTrianglePen;                           // 创建画笔
    drawTrianglePen.setColor(Qt::red);
    drawTrianglePen.setWidth(1);
    p.setPen(drawTrianglePen);
    p.drawPolygon(points, 3);           // 绘制箭头部分
    int offsetX = int(par*siny / 3);
    int offsetY = int(par*cosy / 3);
    QPoint point3, point4;
    point3 = QPoint(endPoint.x() + int(-par*cosy - (par / 2.0*siny)) + offsetX, endPoint.y() + int(-par*siny + (par / 2.0*cosy)) - offsetY);
    point4 = QPoint(endPoint.x() + int(-par*cosy + (par / 2.0*siny) - offsetX), endPoint.y() - int(par / 2.0*cosy + par*siny) + offsetY);
    QPoint arrowBodyPoints[3] = { startPoint, point3, point4 };
    p.drawPolygon(arrowBodyPoints, 3);  // 绘制箭身部分
}


//绘图
void map_query::paintEvent(QPaintEvent*){
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
    //划线
    for( int i = 1; i <= side_num; i++ )
    {
        if(!line[i].flag)
        {
            painter.setPen(QPen(Qt::blue,2));           // 设置画笔颜色为蓝色
            painter.drawLine(point[line[i].node_1], point[line[i].node_2]);   //两点连线
        }
    }
    for( int i = 1; i <= side_num; i++ )
    {
        if(line[i].flag)
        {
            painter.setPen(QPen(Qt::red,4));            // 设置画笔颜色为红色(最小生成树)
            painter.drawLine(point[line[i].node_1], point[line[i].node_2]);   //两点连线
        }
    }
    if(shortest_flag)
    {
        for( int i = shortest_path.size()-1 ; i >0; i-- )           // 画最短路径
            drawArrow(point[shortest_path[i]], point[shortest_path[i-1]], painter);
    }
    //标点
    painter.setFont(font2);
    for(int i = 1; i <= node_num; i++)
    {
        painter.setPen(Qt::black);      // 黑色
        painter.setBrush(Qt::yellow);     // 内部填充(黄色)
        painter.drawEllipse(point[i], 4, 4);        // 把点画出来
        painter.drawText(QPoint(point[i].x() + 5, point[i].y() + 5), point_name[i]);
    }
}
//鼠标点击事件
void map_query::mousePressEvent(QMouseEvent* e){
    if (e->button() == Qt::LeftButton)              // 按左键
    {
        QPoint cur_click_pos = e->pos();
        if(cur_click_pos.x() >= show_window_x &&cur_click_pos.x() <= show_window_x+show_window_width
           && cur_click_pos.y() >= show_window_y &&cur_click_pos.y() <= show_window_y+show_window_height)
        {
            for(int i = 1; i < node_num; i++)
            {
                if(is_Click_Suc(cur_click_pos, point[i], RADIUS+10))   // 判断鼠标所点击位置和图上所有已添加的结点位置
                {
                    newshowmess=new showmess;
                    newshowmess->senddata(point_name[i],point_info[i]);
                    newshowmess->show();
                }
            }
        }
    }
}

//最短路径
bool dijkstra(int a, int b)
{
    double g[Node_MAX_NUM][Node_MAX_NUM], dist[Node_MAX_NUM];
    bool st[Node_MAX_NUM] = { 0 };
    int prev[Node_MAX_NUM];
    memset(prev, -1, sizeof prev);
    for (int i = 1; i <= node_num; i++) {
        dist[i] = 1000000;
        for (int j = 1; j <= node_num; j++) {
            if (i == j) g[i][j] = 0;
            else if (dis_matrix[i][j] == 0) g[i][j] = 1000000;
            else g[i][j] = dis_matrix[i][j];
        }
    }
    dist[a] = 0;
    for (int i = 1; i <= node_num; i++)
    {
        // 寻找距离最小的点
        int t = -1;
        for (int j = 1; j <= node_num; j++) {
            if (!st[j] && (t == -1 || dist[t] > dist[j])) t = j;
        }
        // 更新
        st[t] = true;
        for (int j = 1; j <= node_num; j++)
            if (dist[t] + g[t][j] < dist[j]) {
                dist[j] = dist[t] + g[t][j];
                prev[j] = t;
            }
    }
    if (dist[b] == 1000000) return false;
    shortest_path.clear();
    for (; b != -1; b = prev[b]) shortest_path.push_back(b);
    return true;
}
//所有路径
void dfs(QVector<QVector<int>> &res, QVector<bool>& flag, QVector<int>& temp, int cur, int end) {
    if (cur == end) {
        res.push_back(temp);
        return;
    }
    for (int i = 1; i <= node_num; i++) {
        if (dis_matrix[cur][i] == 0) continue;
        if (!flag[i]) {
            flag[i] = true;
            temp.push_back(i);
            dfs(res, flag, temp, i, end);
            flag[i] = false;
            temp.pop_back();
        }
    }
}
void allPathsSourceTarget(int a, int b) {
    res.clear();
    QVector<bool> flag(Node_MAX_NUM);
    flag[a] = true;
    QVector<int> temp = { a };
    dfs(res, flag, temp, a, b);
}
//prim最小生成树
QVector<QPair<int,int>> prim() {
    double gra[Node_MAX_NUM][Node_MAX_NUM], dis[Node_MAX_NUM], sum = 0;
    bool flag[Node_MAX_NUM] = { 0 };
    int prev[Node_MAX_NUM];
    memset(prev, -1, sizeof prev);
    QVector<QPair<int,int>> res;
    for (int i = 1; i <= node_num; i++) {
        dis[i] = 1000000;
        for (int j = 1; j <= node_num; j++) {
            if (i == j) gra[i][j] = 0;
            else if (dis_matrix[i][j] == 0) gra[i][j] = 1000000;
            else gra[i][j] = dis_matrix[i][j];
        }
    }
    for (int i = 1; i <= node_num; i++) {
        int t = -1;
        for (int j = 1; j <= node_num; j++) {
            if (!flag[j] && (t == -1 || dis[t] > dis[j])) t = j;
        }
        if (i != 1) sum += dis[t];
        flag[t] = 1;
        for (int j = 1; j <= node_num; j++) {
            if (!flag[j] && dis[j] > gra[t][j]) {
                dis[j] = gra[t][j];
                prev[j] = t;
            }
        }
    }
    for (int i = 1; i <= node_num; i++) {
        if (prev[i] != -1) res.push_back({ i,prev[i] });
    }
    return res;

}


                                         //按键
//返回
void map_query::on_back_clicked()
{
    MainWindow *p=new MainWindow;
    p->show();
    this->close();
}
//加载地图
void map_query::on_load_map_clicked()
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
            //填写选择框
            ui->start->clear();
            ui->end->clear();
            for( int i = 1; i <= node_num; i++ ){
                ui->start->addItem(point_name[i]);
                ui->end->addItem(point_name[i]);
            }
        }
        else
        {
            QMessageBox::information(this, "提示", "读取地图失败");
        }
    }
    ui->load_map->setStyleSheet("border-image: url(../image/btn_1.png);");
}
//清除地图
void map_query::on_clear_clicked()
{
    QMessageBox::StandardButton answer;
    Recover();
    ui->clear->setStyleSheet("border-image: url(../image/btn_2.png);");
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
    ui->clear->setStyleSheet("border-image: url(../image/btn_1.png);");
}
//最短路径
void map_query::on_shortest_rode_clicked()
{
    if(function_num != 1)
    {
        Recover();
        function_num = 1;
        ui->shortest_rode->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->shortest_rode->setText("停止该操作");
        QString s1=ui->start->currentText();
        QString s2=ui->end->currentText();
        int a,b;
        for(int i=1;i<node_num;i++){
            if(point_name[i]==s1)a=i;
            if(point_name[i]==s2)b=i;
        }
        if(dijkstra(a,b)){
            shortest_flag=true;
            update();
         }
         else {
            QMessageBox::warning(this, "警告", "这两点间没有通路");
            function_num = 0;
            Recover();
         }
    }
    else
    {
        Recover();
        function_num = 0;
        shortest_flag=false;
        ui->Message_1->clear();
        update();
    }
}
//所有路径
void map_query::on_all_rode_clicked()
{
    Recover();
    if(function_num != 2)
    {
        function_num = 2;
        ui->all_rode->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->all_rode->setText("停止该操作");
        ui->Message_1->clear();
        QString s1=ui->start->currentText();
        QString s2=ui->end->currentText();
        int a,b;
        for(int i=1;i<node_num;i++){
            if(point_name[i]==s1)a=i;
            if(point_name[i]==s2)b=i;
        }
        allPathsSourceTarget(a,b);
        QString tmp;
        for(int i=0;i<res.size();i++){
            for(int j=0;j<res[i].size();j++){
                tmp+=point_name[res[i][j]];
                if(j!=res[i].size()-1)tmp+="->";
            }
            ui->Message_1->addItem(tmp);
            tmp.clear();
        }
    }
    else
    {
        function_num = 0;
        ui->all_rode->setText("求两点间所有路径");
        ui->Message_1->clear();
    }
}
//最小生成树
void map_query::on_mintree_clicked()
{
    Recover();
    if(function_num != 1)
    {
        function_num = 1;
        ui->mintree->setStyleSheet("border-image: url(../image/btn_2.png);");
        ui->mintree->setText("停止该操作");
        ui->Message_1->clear();
        QVector<QPair<int,int>>ans=prim();
        for(int i=0;i<ans.size();i++){
            for(int j=1;j<=side_num;j++){
                if((line[j].node_1==ans[i].first&&line[j].node_2==ans[i].second)||
                   (line[j].node_2==ans[i].first&&line[j].node_1==ans[i].second)){
                    line[j].flag=true;
                    break;
                }
            }
        }
        update();
    }
    else
    {
        function_num = 0;
        ui->mintree->setText("求最小生成树");
        ui->Message_1->clear();
        for(int j=1;j<=side_num;j++)line[j].flag=false;
        update();
    }
}

