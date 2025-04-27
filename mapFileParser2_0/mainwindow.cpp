#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <QFile>
#include <QSet>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI1();

    {
        QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\linker.ld");
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "do not open";
        }
        QString linkerData = file.readAll();
        ld.InstallDescriptor(&ld_descr);
        ld.clear();
        ld.read(linkerData);
        qDebug().noquote() << ld_descr.log();
        populateData();
        // ld_descr.remove_unnecessary(linkerData);
        // linker.read(ld_descr, linkerData);
        // ld_section.read(ld_descr, linker);
        // ld_subsection.read(ld_descr, ld_section.data().value(".text").body);
        // ld_subsection.append(ld_descr, ".text");
        // ld_mem.read(ld_descr, linker);

        // ld_var.read(ld_descr, linker.data().value(ld_descr.globalName));
        // //ld_var.read(ld_descr, ld_section.data().value("._user_heap_stack").body);


        // for (auto it = linker.data().begin(); it != linker.data().end(); ++it) {
        //     qDebug() << "Blockkk:" << it.key()
        //     << "\nContent:" << it.value()
        //     << "\n";
        // }
        // qDebug() << "Sections ------------------------------------";
        // for (auto it = ld_section.data().begin(); it != ld_section.data().end(); ++it) {
        //     qDebug() << "Section:"<< it.key();
        //     qDebug() << "Attribute:"<< it.value().attribute
        //              << "\nContent:" << it.value().body
        //              <<"\nMemory:" << it.value().mem_region << "\n";
        //     qDebug() << "subsection ------------------------------------";
        //     LinkerSubSection sub;
        //     sub.read(ld_descr, it.value().body);
        //     sub.append(ld_descr, it.key());
        //     for(const auto& var : sub.data()) {
        //         qDebug() << var;
        //     }

        //     qDebug() << "variables ------------------------------------";
        //     LinkerVariable var;
        //     var.read(ld_descr, it.value().body);
        //     for(const auto& vartt : var.data()) {
        //         qDebug() << "lval: " << vartt.lvalue;
        //         qDebug() << "rval: " << vartt.rvalue;
        //         qDebug() << "attr: " << vartt.attribute <<'\n';
        //     }
        //     qDebug() << "\n";
        // }

        // qDebug() << "Memries ------------------------------------";
        // for (auto it = ld_mem.data().begin(); it != ld_mem.data().end(); ++it) {
        //     qDebug() << "Name:" << it->name
        //     << "Attributes:" << it->attributes
        //     << "Begin:" << it->origin//QString("0x%1").arg(it.value().origin, 8, 16, QChar('0'))
        //     << "Length:" << it->length << "Bytes";
        // }
        // qDebug() << "Variables ------------------------------------";
        // for(const auto& var : ld_var.data()) {
        //     qDebug() << "lval: " << var.lvalue;
        //     qDebug() << "rval: " << var.rvalue;
        //     qDebug() << "attr: " << var.attribute <<'\n';
        // }

        // qDebug() << "subsection ------------------------------------";
        // for(const auto& var : ld_subsection.data()) {
        //     qDebug() << var;
        // }
    }

    // {
    //     const auto descr = GnuDescriptor::gnuDefault();
    //     auto mapfile = GnuMapFile(descr);
    //     MapFile general;
    //     //GnuMapFile
    //     // GnuSegmentReader reader;
    //     // GnuSectionReder section;

    //     // open file
    //     QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser\\output2.map");
    //     if (!file.open(QIODevice::ReadOnly)) {
    //         qWarning() << "do not open";
    //     }
    //     QString mapData = file.readAll();
    //     mapfile.read(mapData);
    //     mapfile.parse(general);
    //     //auto sz = reader.read(descr, mapData);
    //     //section.read(descr, reader.data());
    //     qDebug() << "END1111111111111----------";

    // }

    // // Приклад рядка, який потрібно розпарсити
    // QString text = "    PROVIDE_HIDDEN (__init_array_start = .);";

    // // Ініціалізація регулярного виразу з іменованими групами
    // QRegularExpression re(R"((?P<lvalue>[^\(\)\s]*?)\s*=\s*(?P<rvalue>.*?)\s*;)");

    // QRegularExpressionMatch match = re.match(text);
    // if (match.hasMatch()) {
    //     QString lvalue = match.captured("lvalue");
    //     QString rvalue = match.captured("rvalue");

    //     qDebug() << "lvalue:" << lvalue;  // Очікується, наприклад, "_Min_Stack_Size"
    //     qDebug() << "rvalue:" << rvalue;  // Очікується, наприклад, "0x300"
    // } else {
    //     qDebug() << "No match";
    // }

}

void MainWindow::setupUI1()
{
    // Головний контейнер
    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, this);
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, verticalSplitter);

    // Панель секцій
    m_sectionsTree = new QTreeWidget(mainSplitter);
    m_sectionsTree->setHeaderLabels({"Name", "Details"});
    m_sectionsTree->setColumnWidth(0, 250);

    // Панель регіонів
    m_regionsTable = new QTableWidget(mainSplitter);
    m_regionsTable->setColumnCount(6);
    m_regionsTable->setHorizontalHeaderLabels({"Region", "Origin", "Length", "Sections", "attr", "Contains"});

    // Панель глобальних змінних
    m_globalsTable = new QTableWidget(mainSplitter);
    m_globalsTable->setColumnCount(3);
    m_globalsTable->setHorizontalHeaderLabels({"Variable", "Value", "Attributes"});

    mainSplitter->addWidget(m_sectionsTree);
    mainSplitter->addWidget(m_regionsTable);
    mainSplitter->addWidget(m_globalsTable);

    // Налаштування розмірів для mainSplitter
    // m_regionsTable отримує 50% ширини, m_sectionsTree і m_globalsTable по 25%
    int totalWidth = this->width(); // Використовуємо поточну ширину вікна
    mainSplitter->setSizes({static_cast<int>(totalWidth * 0.25), static_cast<int>(totalWidth * 0.50),
                            static_cast<int>(totalWidth * 0.25)});

    m_logText = new QTextEdit(verticalSplitter);
    m_logText->setReadOnly(true);
    verticalSplitter->addWidget(mainSplitter); // Додаємо mainSplitter першим
    verticalSplitter->addWidget(m_logText);

    // Налаштування розмірів для verticalSplitter
    // mainSplitter отримує 80% висоти, m_logText — 20%
    int totalHeight = this->height(); // Використовуємо поточну висоту вікна
    verticalSplitter->setSizes({static_cast<int>(totalHeight * 0.80), static_cast<int>(totalHeight * 0.20)});

    setCentralWidget(verticalSplitter);
    resize(1280, 720);
}
void MainWindow::populateData() {

    for (const auto& section : ld.sections()) {
        QTreeWidgetItem* sectionItem = new QTreeWidgetItem(m_sectionsTree);
        sectionItem->setText(0, section.name);

        // VMA
        QTreeWidgetItem* vmaItem = new QTreeWidgetItem(sectionItem);
        vmaItem->setText(0, "VMA");
        vmaItem->setText(1, section.vma ? section.vma->name : "N/A");

        // LMA
        QTreeWidgetItem* lmaItem = new QTreeWidgetItem(sectionItem);
        lmaItem->setText(0, "LMA");
        lmaItem->setText(1, section.lma ? section.lma->name : "N/A");

        // Subsections
        QTreeWidgetItem* subSectionsItem = new QTreeWidgetItem(sectionItem);
        subSectionsItem->setText(0, "Subsections");
        for (const auto& sub : section.subnames.data()) {
            QTreeWidgetItem* subItem = new QTreeWidgetItem(subSectionsItem);
            subItem->setText(0, sub);
        }

        // Variables
        QTreeWidgetItem* varsItem = new QTreeWidgetItem(sectionItem);
        varsItem->setText(0, "Variables");
        for (const auto& var : section.vars.data()) {
            QTreeWidgetItem* varItem = new QTreeWidgetItem(varsItem);
            varItem->setText(0, var.lvalue);
            varItem->setText(1, var.rvalue);
        }
    }

    // Заповнення регіонів
    int row = 0;
    m_regionsTable->setRowCount(ld.regions().size());
    for (const auto& region : ld.regions()) {
        m_regionsTable->setItem(row, 0, new QTableWidgetItem(region.region.name));
        m_regionsTable->setItem(row, 1, new QTableWidgetItem(region.region.origin));
        m_regionsTable->setItem(row, 2, new QTableWidgetItem(region.region.length));
        m_regionsTable->setItem(row, 3, new QTableWidgetItem(QString::number(region.sections.size())));
        m_regionsTable->setItem(row, 4, new QTableWidgetItem(region.region.attributes));

        QString sections_txt{};
        for (const auto& section : region.sections) {
            sections_txt += section->name;
            sections_txt += '\n';
        }

        m_regionsTable->setItem(row, 5, new QTableWidgetItem(sections_txt));

        for (int i = 0; i < m_regionsTable->columnCount(); ++i) {
            QTableWidgetItem* item = m_regionsTable->item(row, i);

            if (region.region.evaluated) {
                item->setBackground(QBrush(Qt::green));
            } else {
                item->setBackground(QBrush(Qt::red));
            }
        }
        row++;
    }

    // Заповнення глобальних змінних
    row = 0;
    m_globalsTable->setRowCount(ld.globals().data().size());
    for (const auto& var : ld.globals().data()) {
        m_globalsTable->setItem(row, 0, new QTableWidgetItem(var.lvalue));
        m_globalsTable->setItem(row, 1, new QTableWidgetItem(var.rvalue));
        m_globalsTable->setItem(row, 2, new QTableWidgetItem(var.attribute));


        for (int i = 0; i < m_globalsTable->columnCount(); ++i) {
            QTableWidgetItem* item = m_globalsTable->item(row, i);

            if (var.evaluated) {
                item->setBackground(QBrush(Qt::green));
            } else {
                item->setBackground(QBrush(Qt::red));
            }
        }
        row++;
    }

    // Налаштування відображення
    m_regionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_globalsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_regionsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_regionsTable->sortByColumn(1, Qt::AscendingOrder);
    //m_sectionsTree->expandAll();
    m_sectionsTree->sortItems(0, Qt::AscendingOrder);
    m_sectionsTree->verticalScrollMode();

    m_logText->setText(ld_descr.log());
}

MainWindow::~MainWindow()
{
    delete ui;
}
