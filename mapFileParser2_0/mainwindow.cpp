#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <QFile>
#include <QSet>

#include "linkerfilefactory.h"


#include "mapfilereader.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI1();

    // linker reader
    {
        ld = LinkerFileFactory::create("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\linker.ld");
        populateData();
    }

    // map reader
    {
        gnu::MapFileReader reader;


        QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\output.map");
        //QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\test\\cleo.map");
        //QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\test\\risc5.map");
        //QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\test\\hello.map");
        //QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\test\\output.map");
        //QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\test\\output1.map");
        //QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\test\\maps\\w0_000.map");
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "do not open";
        }

        qDebug() << "Start read 1111111111111----------";
        QString mapData = QString::fromUtf8(file.readAll());
        reader.read(mapfile, mapData);
        qDebug() << "END1111111111111----------";
        file.close();


        qDebug() << "Window build----------";
        mapView = new MapFileViewer(mapfile, ui->tabWidget);
        ui->tabWidget->addTab(mapView, "MAP");
        qDebug() << "END Window build----------";
    }

    {

        // Create a HashIndex mapping QString to int
        HashIndex<QString, int> idx;

        // Insert some values
        auto iA = idx.insert("apple", 10);
        auto iB = idx.insert("banana", 20);
        auto iC = idx.insert("cherry", 30);
        qDebug() << "Inserted indices:" << iA << iB << iC;

        // Lookup by key
        auto opt = idx.indexOf("banana");
        auto aaaa = (opt > 0) ? (*idx.at(opt)) : -1;
        qDebug() << "Value for 'banana' at index" << opt << ":" <<  aaaa;

        // operator[] usage
        idx["date"] = idx["cherry"] = 40;
        qDebug() << "date ->" << idx["date"];

        // Iterate over all active elements
        qDebug() << "All key-value pairs:";
        for (auto it = idx.begin(); it != idx.end(); ++it) {
            int index = it.getIndex(); // direct access to index
            qDebug() << "  [" << index << "]" << idx.keyAt(index) << ":" << (*it).value;
        }

        qDebug() << "range based";
        for (auto i : idx) {
            qDebug() << i.key << ":" << i.value;
        }

        // Remove an element
        idx.remove("banana");
        qDebug() << "After removing 'banana': size=" << idx.size();
        qDebug() << "range based";
        for (auto i : idx) {
            qDebug() << i.key << ":" << i.value;
        }


        // Compact storage
        idx.compact();
        qDebug() << "After compact: size=" << idx.size();

        // Validate holes are gone
        for (auto it = idx.begin(); it != idx.end(); ++it) {
            int index = it.getIndex();
            qDebug() << "  [" << index << "]" << idx.keyAt(index) << ":" << (*it).value;
        }
        // 1. Створення контейнера з ключами QString та значеннями int
        // HashIndex<QString, int> container;

        // // 2. Вставка елементів
        // qDebug() << "Inserting elements:";
        // int appleIdx = container.insert("apple", 10);
        // int bananaIdx = container.insert("banana", 20);
        // qDebug() << "  apple index:" << appleIdx << ", banana index:" << bananaIdx;

        // // 3. Спроба вставити дубльований ключ
        // int invalidIdx = container.insert("apple", 30);
        // qDebug() << "  Duplicate 'apple' insert result:" << invalidIdx << "(INVALID)";

        // // 4. Доступ за індексом
        // qDebug() << "\nAccess by index:";
        // qDebug() << "  apple value at index" << appleIdx << ":" << *container.at(appleIdx);
        // qDebug() << "  key at index" << bananaIdx << ":" << container.keyAt(bananaIdx);

        // // 5. Доступ за ключем через operator[]
        // qDebug() << "\nAccess by key:";
        // qDebug() << "  container['apple'] =" << container["apple"];
        // container["cherry"] = 50; // Автоматично додасть "cherry" зі значенням 50
        // qDebug() << "  container['cherry'] after auto-insert:" << container["cherry"];

        // // 6. Ітерація по активним елементам
        // qDebug() << "\nIterating elements:";
        // for (const auto& value : container) {
        //     qDebug() << "  Value:" << value;
        // }

        // // 7. Видалення елемента
        // qDebug() << "\nRemoving 'banana':";
        // bool removed = container.remove("banana");
        // qDebug() << "  Removal success:" << removed;
        // qDebug() << "  Size after removal:" << container.size();

        // // 8. Вставка нового елемента після видалення (має перевикористати індекс)
        // //int orangeIdx = container.insert("orange", 100);
        // //qDebug() << "  'orange' inserted at recycled index:" << orangeIdx;

        // // 9. Виклик компактного переформатування
        // qDebug() << "\nBefore compacting:";
        // qDebug() << "  Elements size:" << container.getElements().size();
        // container.compact();
        // qDebug() << "After compacting:";
        // qDebug() << "  Elements size:" << container.getElements().size();
        // qDebug() << "  container['orange'] new index:" << container.indexOf("orange");
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
    m_sectionsTree->setHeaderLabels({"Name", "Details", "Attr"});
    m_sectionsTree->setColumnWidth(0, 150);

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

    ui->tabWidget->addTab(verticalSplitter, "Linker");
    //ui->tab.s(verticalSplitter);
    //setCentralWidget(verticalSplitter);
    resize(1280, 720);
}
void MainWindow::populateData() {

    for (const auto& section : ld->sections()) {
        QTreeWidgetItem* sectionItem = new QTreeWidgetItem(m_sectionsTree);
        sectionItem->setText(0, section.name);

        // VMA
        QTreeWidgetItem* vmaItem = new QTreeWidgetItem(sectionItem);
        vmaItem->setText(0, "VMA");
        if(section.vma.size() == 0) {
            vmaItem->setText(1, "N/A");
        } else if(section.vma.size() == 1) {
            vmaItem->setText(1, section.vma[0]->name);
        } else {
            for (const auto& vma_it : section.vma) {
                QTreeWidgetItem* subItem = new QTreeWidgetItem(vmaItem);
                subItem->setText(0, vma_it->name);
            }
        }

        //vmaItem->setText(1, section.vma ? section.vma->name : "N/A");

        // LMA
        QTreeWidgetItem* lmaItem = new QTreeWidgetItem(sectionItem);
        lmaItem->setText(0, "LMA");

        if(section.lma.size() == 0) {
            lmaItem->setText(1, "N/A");
        } else if(section.lma.size() == 1) {
            lmaItem->setText(1, section.lma[0]->name);
        } else {
            for (const auto& lma_it : section.lma) {
                QTreeWidgetItem* subItem = new QTreeWidgetItem(lmaItem);
                subItem->setText(0, lma_it->name);
            }
        }

        // Subsections
        QTreeWidgetItem* subSectionsItem = new QTreeWidgetItem(sectionItem);
        subSectionsItem->setText(0, "Subsections");
        if(section.subnames.size() == 0) {
            subSectionsItem->setText(1, "N/A");
        } else {
            for (const auto& sub : section.subnames) {
                QTreeWidgetItem* subItem = new QTreeWidgetItem(subSectionsItem);
                subItem->setText(0, sub);
            }
        }


        // Variables
        QTreeWidgetItem* varsItem = new QTreeWidgetItem(sectionItem);
        varsItem->setText(0, "Variables");
        if(section.vars.size() == 0) {
            varsItem->setText(1, "N/A");
        } else {
            for (const auto& var : section.vars) {
                QTreeWidgetItem* varItem = new QTreeWidgetItem(varsItem);
                varItem->setText(0, var.lvalue);
                varItem->setText(1, var.rvalue);
                varItem->setText(2, var.attribute);
            }
        }
    }

    // Заповнення регіонів
    int row = 0;
    m_regionsTable->setRowCount(ld->regions().size());
    for (const auto& region : ld->regions()) {
        m_regionsTable->setItem(row, 0, new QTableWidgetItem(region.region.name));
        m_regionsTable->setItem(row, 1, new QTableWidgetItem(region.region.origin));
        m_regionsTable->setItem(row, 2, new QTableWidgetItem(region.region.length));
        m_regionsTable->setItem(row, 3, new QTableWidgetItem(QString::number(region.sections.size())));
        m_regionsTable->setItem(row, 4, new QTableWidgetItem(region.region.attributes));

        QString sections_txt{};
        for (const auto& section : region.sections) {
            sections_txt += section.second->name;
            if(section.first != ILinkerFile::None) {
                sections_txt += " (";
                sections_txt += section.first == ILinkerFile::LMA ? "LMA" : "VMA";
                sections_txt += ")";
            }
            sections_txt += '\n';

        }

        m_regionsTable->setItem(row, 5, new QTableWidgetItem(sections_txt));

        for (int i = 0; i < m_regionsTable->columnCount(); ++i) {
            QTableWidgetItem* item = m_regionsTable->item(row, i);

            if (region.region.isEvaluated) {
                item->setBackground(QBrush(Qt::green));
            } else {
                item->setBackground(QBrush(Qt::red));
            }
        }
        row++;
    }

    // Заповнення глобальних змінних
    row = 0;
    m_globalsTable->setRowCount(ld->globals().size());
    for (const auto& var : ld->globals()) {
        m_globalsTable->setItem(row, 0, new QTableWidgetItem(var.lvalue));
        m_globalsTable->setItem(row, 1, new QTableWidgetItem(var.rvalue));
        m_globalsTable->setItem(row, 2, new QTableWidgetItem(var.attribute));


        for (int i = 0; i < m_globalsTable->columnCount(); ++i) {
            QTableWidgetItem* item = m_globalsTable->item(row, i);

            if (var.isEvaluated) {
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

    m_logText->setText(ld->getlog());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete ld;
    LinkerFileFactory::clearCache();
}
