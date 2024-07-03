#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QDebug>
#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QWidget#centralwidget { border-image: url(:/images/aircraft-jet-landing-cloud-46148.jpeg) 0 0 0 0 stretch stretch; }");
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);

    QPushButton *addItemButton = new QPushButton("Add Item", this);
    QPushButton *removeItemButton = new QPushButton("Remove Item", this);
    QPushButton *sortListButton = new QPushButton("Sort List", this);
    QPushButton *countSortButton = new QPushButton("Count Sort", this);

    ui->verticalLayout->addWidget(addItemButton);
    ui->verticalLayout->addWidget(removeItemButton);
    ui->verticalLayout->addWidget(sortListButton);
    ui->verticalLayout->addWidget(countSortButton);

    connect(addItemButton, &QPushButton::clicked, this, &MainWindow::onAddItemButtonClicked);
    connect(removeItemButton, &QPushButton::clicked, this, &MainWindow::onRemoveItemButtonClicked);
    connect(sortListButton, &QPushButton::clicked, this, &MainWindow::onSortListButtonClicked);
    connect(countSortButton, &QPushButton::clicked, this, &MainWindow::onCountSortButtonClicked);

    // Создание синего виджета
    blueWidget = new QWidget(this);
    blueWidget->setStyleSheet("background-color: blue;");
    blueWidget->setFixedSize(200, 200); // Установка размера виджета
    ui->verticalLayout->addWidget(blueWidget); // Добавление виджета в макет

    // Обработка событий мыши для синего виджета
    blueWidget->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoadButtonClicked()
{
    loadAirplanesData("airplanes.txt");
}

void MainWindow::loadAirplanesData(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file: " + fileName);
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    int flightCount = line.toInt();

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(flightCount);
    ui->tableWidget->setColumnCount(3);
    QStringList headers = {"Flight Number", "Flight Name", "Flight Date"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    int row = 0;
    while (!in.atEnd() && row < flightCount) {
        line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() == 3) {
            qint16 flightNumber = fields[0].toShort();
            QString flightName = fields[1];
            QString flightDate = fields[2];

            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(flightNumber)));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(flightName));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(flightDate));
        }
        ++row;
    }

    file.close();
}

void MainWindow::onAddItemButtonClicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->listWidget->addItem("New Item");
}

void MainWindow::onRemoveItemButtonClicked()
{
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    delete ui->listWidget->currentItem();
}

void MainWindow::onSortListButtonClicked()
{
    ui->tableWidget->sortItems(0, Qt::AscendingOrder);
}

void MainWindow::onCountSortButtonClicked()
{
    countSortTable();
}

void MainWindow::countSortTable()
{
    int rowCount = ui->tableWidget->rowCount();
    if (rowCount == 0)
        return;

    qint16 maxFlightNumber = 0;
    for (int i = 0; i < rowCount; ++i) {
        qint16 flightNumber = ui->tableWidget->item(i, 0)->text().toShort();
        if (flightNumber > maxFlightNumber)
            maxFlightNumber = flightNumber;
    }

    QVector<QVector<QTableWidgetItem*>> count(maxFlightNumber + 1);
    for (int i = 0; i < rowCount; ++i) {
        qint16 flightNumber = ui->tableWidget->item(i, 0)->text().toShort();
        count[flightNumber].append(new QTableWidgetItem(*ui->tableWidget->item(i, 0)));
        count[flightNumber].append(new QTableWidgetItem(*ui->tableWidget->item(i, 1)));
        count[flightNumber].append(new QTableWidgetItem(*ui->tableWidget->item(i, 2)));
    }

    int row = 0;
    for (qint16 i = 0; i <= maxFlightNumber; ++i) {
        for (int j = 0; j < count[i].size(); j += 3) {
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(*count[i][j]));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(*count[i][j + 1]));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(*count[i][j + 2]));
            ++row;
        }
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == blueWidget && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            blueWidget->hide();
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    qDebug() << event;
    QMainWindow::keyPressEvent(event);
}
