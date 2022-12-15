#include <QMessageBox>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>
#include <QRectF>
#include <QPainter>
#include <qmath.h>
#include "gamewidget.h"
#include "mainwindow.h"

GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    timer(new QTimer(this)),
    generations(-1),
    universeSize(10),
    ruleFlag(false)
{
    timer->setInterval(50);
    m_masterColor = "yellow";
    universe = new bool[(universeSize + 2) * (universeSize + 2)];
    next = new bool[(universeSize + 2) * (universeSize + 2)];
    connect(timer, SIGNAL(timeout()), this, SLOT(newGeneration()));
    memset(universe, false, sizeof(bool)*(universeSize + 2) * (universeSize + 2));
    memset(next, false, sizeof(bool)*(universeSize + 2) * (universeSize + 2));
}

GameWidget::~GameWidget()
{
    delete [] universe;
    delete [] next;
}

void GameWidget::setRule(char *newRule){
    ruleSize = strlen(newRule);
    rule = new char[ruleSize];
    strcpy(rule, newRule);
    ruleFlag = true;
}

void GameWidget::setDefaultRule(){
    ruleSize = strlen("B3/S23");
    rule = new char[ruleSize];
    strcpy(rule, "B3/S23");
    ruleFlag = true;
}

void GameWidget::startGame()
{
    if(ruleFlag == false){
        GameWidget::setDefaultRule();
    }
    buttonChanged(true);
    timer->start();
}

void GameWidget::stopGame()
{
    buttonChanged(false);
    timer->stop();
}

void GameWidget::oneStep()
{
    if(ruleFlag == false){
        GameWidget::setDefaultRule();
    }
    generations = 1;
    timer->start();
}

void GameWidget::clear()
{
    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
            universe[k*universeSize + j] = false;
        }
    }
    gameEnds(true);
    update();
}

int GameWidget::cellNumber()
{
    return universeSize;
}

void GameWidget::setCellNumber(const int &size)
{
    universeSize = size;
    resetUniverse();
    update();
}

void GameWidget::setGenerationsNumber(const int &num)
{
    generations = num;
}

void GameWidget::resetUniverse()
{
    delete [] universe;
    delete [] next;
    universe = new bool[(universeSize + 2) * (universeSize + 2)];
    next = new bool[(universeSize + 2) * (universeSize + 2)];
    memset(universe, false, sizeof(bool)*(universeSize + 2) * (universeSize + 2));
    memset(next, false, sizeof(bool)*(universeSize + 2) * (universeSize + 2));
}

QString GameWidget::dump()
{
    char temp[10];
    QString master = "";
    master.append("#R ");
    master.append(rule);
    master.append("\n#C\n");
    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
            if(universe[k*universeSize + j] == true) {
                itoa(k, temp, 10);
                master.append(temp);
                master.append(" ");
                itoa(j, temp, 10);
                master.append(temp);
                master.append("\n");
            }
        }
    }
    return master;
}

void GameWidget::setDump(const int x, const int y)
{
    universe[x*universeSize + y] = true;
    update();
}

int GameWidget::interval()
{
    return timer->interval();
}

bool GameWidget::isAlive(int k, int j)
{
    int alive = 0;
    int ruleNum, i;
    alive += universe[((k+1)*universeSize + j)];
    alive += universe[((k-1)*universeSize + j)];
    alive += universe[(k*universeSize + (j+1))];
    alive += universe[(k*universeSize + (j-1))];
    alive += universe[((k+1)*universeSize + (j-1))];
    alive += universe[((k-1)*universeSize + (j+1))];
    alive += universe[((k-1)*universeSize + (j-1))];
    alive += universe[((k+1)*universeSize + (j+1))];
    if (universe[k*universeSize + j] == true){
        i = 0;
        while(rule[i] != 'S'){
            i++;
        }
        i++;
        while(i < ruleSize){
            ruleNum = rule[i] - '0';
            if(alive == ruleNum){
                return true;
            }
            i++;
        }
    } else{
        i = 1;
        while(rule[i] != '/'){
            ruleNum = int(rule[i] - '0');
            if(alive == ruleNum){
               return true;
            }
            i++;
        }
    }
    return false;
}

void GameWidget::newGeneration()
{
    if(generations < 0){
        generations++;
    }
    for(int k=1; k <= universeSize; k++) {
        for(int j=1; j <= universeSize; j++) {
            next[k*universeSize + j] = isAlive(k, j);
        }
    }
    for(int k=1; k <= universeSize; k++) {
        for(int j=1; j <= universeSize; j++) {
            universe[k*universeSize + j] = next[k*universeSize + j];
        }
    }
    update();
    generations--;
    if(generations == 0) {
        stopGame();
        gameEnds(true);
    }
}
void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    paintGrid(p);
    paintUniverse(p);
}

void GameWidget::mousePressEvent(QMouseEvent *e)
{
    environmentChanged(true);
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    int k = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;
    universe[k*universeSize + j] = !universe[k*universeSize + j];
    update();
}

void GameWidget::mouseMoveEvent(QMouseEvent *e)
{
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    int k = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;
    int currentLocation = k*universeSize + j;
    if(!universe[currentLocation]){
        universe [currentLocation]= !universe[currentLocation];
        update();
    }
}

void GameWidget::paintGrid(QPainter &p)
{
    QRect borders(0, 0, width()-1, height()-1);
    QColor gridColor = "#000000";
    gridColor.setAlpha(50);
    p.setPen(gridColor);
    double cellWidth = (double)width()/universeSize;
    for(double k = cellWidth; k <= width(); k += cellWidth)
        p.drawLine(k, 0, k, height());
    double cellHeight = (double)height()/universeSize;
    for(double k = cellHeight; k <= height(); k += cellHeight)
        p.drawLine(0, k, width(), k);
    p.drawRect(borders);
}

void GameWidget::paintUniverse(QPainter &p)
{
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    for(int k=1; k <= universeSize; k++) {
        for(int j=1; j <= universeSize; j++) {
            if(universe[k*universeSize + j] == true) {
                qreal left = (qreal)(cellWidth*j-cellWidth);
                qreal top  = (qreal)(cellHeight*k-cellHeight);
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
                p.fillRect(r, QBrush(m_masterColor));
            }
        }
    }
}

QColor GameWidget::masterColor()
{
    return m_masterColor;
}

void GameWidget::setMasterColor(const QColor &color)
{
    m_masterColor = color;
    update();
}
