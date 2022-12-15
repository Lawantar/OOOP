#include <QTextStream>
#include <QFileDialog>
#include <QDebug>
#include <QColor>
#include <QColorDialog>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentColor(QColor("#000000")),
    game(new GameWidget(this))
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: #cccccc;");
    QPixmap icon(16, 16);
    icon.fill(currentColor);
    connect(ui->startButton, SIGNAL(clicked()), game,SLOT(startGame()));
    connect(ui->stopButton, SIGNAL(clicked()), game,SLOT(stopGame()));
    connect(ui->stepButton, SIGNAL(clicked()), game,SLOT(oneStep()));
    connect(ui->clearButton, SIGNAL(clicked()), game,SLOT(clear()));
    connect(ui->cellsControl, SIGNAL(valueChanged(int)), game, SLOT(setCellNumber(int)));
    connect(ui->genNum, SIGNAL(valueChanged(int)), game, SLOT(setGenerationsNumber(int)));
    connect(game,SIGNAL(environmentChanged(bool)),ui->cellsControl,SLOT(setDisabled(bool)));
    connect(game,SIGNAL(buttonChanged(bool)),ui->startButton,SLOT(setDisabled(bool)));
    connect(game,SIGNAL(buttonChanged(bool)),ui->clearButton,SLOT(setDisabled(bool)));
    connect(game,SIGNAL(buttonChanged(bool)),ui->stepButton,SLOT(setDisabled(bool)));
    connect(game,SIGNAL(gameEnds(bool)),ui->cellsControl,SLOT(setEnabled(bool)));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveGame()));
    connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(loadGame()));
    ui->mainLayout->setStretchFactor(ui->gameLayout, 8);
    ui->mainLayout->setStretchFactor(ui->setLayout, 2);
    ui->gameLayout->addWidget(game);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveGame()
{
    QString filename = QFileDialog::getSaveFileName(this, tr(""), QDir::homePath(), tr("(*.life)"));
    if(filename.length() < 1)
        return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    QString s = "#S " + QString::number(game->cellNumber()) + "\n";
    file.write(s.toUtf8());
    file.write("#N ");
    file.write(ui->universeName->text().toUtf8());
    file.write("\n");
    file.write(game->dump().toUtf8());
    file.close();
}

void MainWindow::loadGame()
{
    QString filename = QFileDialog::getOpenFileName(this, tr(""), QDir::homePath(), tr("(*.life)"));
    if(filename.length() < 1)
        return;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream in(&file);

    int size, x, y;
    char symb;
    char name[20];
    char newRule[23];
    while (!in.atEnd()) {
        in>>symb;
        if(symb == '#'){
            in>>symb;
            switch(symb){
                case 'S':
                    in >> size;
                    ui->cellsControl->setValue(size);
                    game->setCellNumber(size);
                    break;
                case 'N':
                      in>>name;
                      ui->universeName->setText(name);
                      update();
                      break;
                case 'R':
                     in>>newRule;
                     game->setRule(newRule);
                     break;
                case 'C':
                    while (!in.atEnd()) {
                        in >> x;
                        in >> y;
                        game->setDump(x, y);
                    }
                    break;
                default:
                    QMessageBox msgBox;
                    msgBox.setWindowTitle("Error");
                    msgBox.setText("Something went wrong!\nCheck file!");
                    msgBox.exec();
                    break;
            }
        }
    }
}
