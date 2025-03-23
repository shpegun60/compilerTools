#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "symbol.h"
#include "QDebug"
#include <QSet>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Symbol sym("mySymbol", 0x80000000, 128, 0x1000, 16);

    // Використання з Qt-контейнерами
    QSet<Symbol> symbols;
    symbols.insert(sym);

    // Робота з CSV
    qDebug() << sym.toCsv(); // "mySymbol,0x80000000,128"

    // Порівняння об'єктів
    Symbol sym2("mySymbol", 0x80000000);
    if (sym == sym2) {
        qDebug() << "Symbols are equal";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
