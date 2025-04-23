#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "symbol.h"
#include "QDebug"
#include <QFile>
#include <QSet>

#include "mapparser1.h"
#include "mapfile.h"
#include "gnumapparser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Symbol sym("mySymbol", 0x80000000, 128, 0x1000, 16);

    // // Використання з Qt-контейнерами
    // QSet<Symbol> symbols;
    // symbols.insert(sym);

    // Робота з CSV
    qDebug() << sym.toCsv(); // "mySymbol,0x80000000,128"

    // Порівняння об'єктів
    Symbol sym2("mySymbol", 0x80000000);
    if (sym == sym2) {
        qDebug() << "Symbols are equal 111111111111111111";
    }

    {
        MapDescriptor descr;
        MapFile map;
        map.installDescriptor(descr);

        GnuMapParser gnu;
        gnu.installDescriptor(descr);

        MapFileParser parser;
        QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser\\output2.map");
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "do not open";
        }

        QString mapData = file.readAll();
        map.read(mapData);
        gnu.parse();
        qDebug() << "Total height:";
        ParseResult result = parser.parseMapFile(mapData);



        //qDebug() << "Total height:" << result.itemHeight;

        // Розкоментуйте наступний блок для виведення деталей секцій та елементів:

        for (const Section &section : result.rootSection.subSections) {
            if(section.name.contains(".bss") || section.name.contains(".data")) {
                for (const Item &item : section.items) {
                    qDebug() << QString("  %1: %2 (%3 bytes)")
                    .arg(item.name)
                        .arg(item.address)
                        .arg(item.size);
                }
            }

        }

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
