#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QColor>
#include <QWidget>

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

signals:
    void environmentChanged(bool ok);
    void buttonChanged(bool ok);
    void gameEnds(bool ok);

public slots:
    void startGame();
    void stopGame();
    void oneStep();
    void clear();
    int cellNumber();
    void setCellNumber(const int &size);
    void setRule(char* rule);
    void setDefaultRule();
    void setGenerationsNumber(const int &num);
    int interval();
    QColor masterColor();
    void setMasterColor(const QColor &color);
    QString dump();
    void setDump(const int x, const int y);

private slots:
    void paintGrid(QPainter &p);
    void paintUniverse(QPainter &p);
    void newGeneration();

private:
    QColor m_masterColor;
    QTimer* timer;
    int generations;
    bool* universe;
    bool* next;
    char* rule;
    int universeSize;
    int ruleSize;
    bool ruleFlag;
    bool isAlive(int k, int j);
    void resetUniverse();
};

#endif
