#include "mapfileviewer.h"
#include <QComboBox>
#include <QVBoxLayout>

MapFileViewer::MapFileViewer(const compiler_tools::IMapFile& mapFile, QWidget* parent)
    : QWidget(parent), m_mapFile(mapFile)
{
    // Створення сплітера для поділу на 3 області
    m_splitter = new QSplitter(Qt::Horizontal, this);

    // Створення таблиць для кожної області
    m_symbolsTable = new QTableWidget(this);
    m_filesTable = new QTableWidget(this);
    m_sectionsTable = new QTableWidget(this);

    // Додавання таблиць до сплітера
    m_splitter->addWidget(m_symbolsTable);
    m_splitter->addWidget(m_filesTable);
    m_splitter->addWidget(m_sectionsTable);

    // Налаштування таблиць
    setupSymbolsTable();
    setupFilesTable();
    setupSectionsTable();

    m_symbolsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_filesTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_sectionsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Розтягування сплітера на весь віджет
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Без відступів
    layout->addWidget(m_splitter);
    setLayout(layout); // Встановлення макету для MapFileViewer
}

MapFileViewer::~MapFileViewer()
{
}

void MapFileViewer::setupSymbolsTable()
{
    m_symbolsTable->setColumnCount(7);
    QStringList headers = {"Name", "VRAM", "VROM", "Size", "ID", "Section", "File"};
    m_symbolsTable->setHorizontalHeaderLabels(headers);

    const auto& symbols = m_mapFile.symbols();
    m_symbolsTable->setRowCount(symbols.size());

    int row = 0;
    for (const auto& sym : symbols) {
        const auto& symbol = sym.value;
        m_symbolsTable->setItem(row, 0, new QTableWidgetItem(symbol.name));
        m_symbolsTable->setItem(row, 1, new QTableWidgetItem(QString::number(symbol.vram, 16)));
        m_symbolsTable->setItem(row, 2, new QTableWidgetItem(symbol.vrom ? QString::number(*symbol.vrom, 16) : ""));
        m_symbolsTable->setItem(row, 3, new QTableWidgetItem(symbol.size ? QString::number(*symbol.size) : ""));
        m_symbolsTable->setItem(row, 4, new QTableWidgetItem(QString::number(symbol.id)));

        QComboBox* sectionCombo = new QComboBox();
        for (const auto& sec : m_mapFile.sections()) {
            sectionCombo->addItem(sec.key, sec.value.id);
        }
        sectionCombo->setCurrentIndex(sectionCombo->findData(symbol.idSection));
        m_symbolsTable->setCellWidget(row, 5, sectionCombo);

        QComboBox* fileCombo = new QComboBox();
        for (const auto& f : m_mapFile.files()) {
            fileCombo->addItem(f.value.filename, f.value.id);
        }
        fileCombo->setCurrentIndex(fileCombo->findData(symbol.idFile));
        m_symbolsTable->setCellWidget(row, 6, fileCombo);

        row++;
    }

    //m_symbolsTable->resizeColumnsToContents();
    m_symbolsTable->resize(100, 10);
}

void MapFileViewer::setupFilesTable()
{
    m_filesTable->setColumnCount(5);
    QStringList headers = {"Filepath", "VRAM", "Size", "ID", "Symbols"};
    m_filesTable->setHorizontalHeaderLabels(headers);

    const auto& files = m_mapFile.files();
    m_filesTable->setRowCount(files.size());

    int row = 0;
    for (const auto& f : files) {
        const auto& file = f.value;
        m_filesTable->setItem(row, 0, new QTableWidgetItem(file.filename));
        m_filesTable->setItem(row, 1, new QTableWidgetItem(QString::number(file.vram, 16)));
        m_filesTable->setItem(row, 2, new QTableWidgetItem(QString::number(file.size)));
        m_filesTable->setItem(row, 3, new QTableWidgetItem(QString::number(file.id)));

        QComboBox* symbolsCombo = new QComboBox();
        for (const auto& symId : file.idSymbols) {
            const auto& sym = m_mapFile.symbols().at(symId);
            if (sym) {
                symbolsCombo->addItem(sym->name, symId);
            }
        }
        m_filesTable->setCellWidget(row, 4, symbolsCombo);

        row++;
    }

    //m_filesTable->resizeColumnsToContents();
    m_symbolsTable->resize(100, 10);
}

void MapFileViewer::setupSectionsTable()
{
    m_sectionsTable->setColumnCount(10);
    QStringList headers = {"Name", "VRAM", "VROM","Size", "ID", "from", "to", "size",  "Files", "Symbols"};
    m_sectionsTable->setHorizontalHeaderLabels(headers);

    const auto& sections = m_mapFile.sections();
    m_sectionsTable->setRowCount(sections.size());

    int row = 0;
    for (const auto& s : sections) {
        const auto& section = s.value;
        m_sectionsTable->setItem(row, 0, new QTableWidgetItem(section.name));
        m_sectionsTable->setItem(row, 1, new QTableWidgetItem(QString::number(section.vram.value_or(-1), 16)));
        m_sectionsTable->setItem(row, 2, new QTableWidgetItem(QString::number(section.vrom.value_or(-1), 16)));
        m_sectionsTable->setItem(row, 3, new QTableWidgetItem(QString::number(section.size)));
        m_sectionsTable->setItem(row, 4, new QTableWidgetItem(section.sizedev > 0 ? '+' + QString::number(section.sizedev) : QString::number(section.sizedev)));

        if(section.baddr < section.eaddr) {
            m_sectionsTable->setItem(row, 5, new QTableWidgetItem(QString::number(section.baddr, 16)));
            m_sectionsTable->setItem(row, 6, new QTableWidgetItem(QString::number(section.eaddr, 16)));
            m_sectionsTable->setItem(row, 7, new QTableWidgetItem(QString::number(section.eaddr - section.baddr)));
        }



        QComboBox* filesCombo = new QComboBox();
        for (const auto& fileId : section.idFiles) {
            const auto& file = m_mapFile.files().at(fileId);
            if (file) {
                filesCombo->addItem(file->filename, fileId);
            }
        }
        m_sectionsTable->setCellWidget(row, 8, filesCombo);

        QComboBox* symbolsCombo = new QComboBox();
        for (const auto& symId : section.idSymbols) {
            const auto& sym = m_mapFile.symbols().at(symId);
            if (sym) {
                symbolsCombo->addItem(sym->name, symId);
            }
        }
        m_sectionsTable->setCellWidget(row, 9, symbolsCombo);


        for (int i = 0; i < 3; ++i) {
            QTableWidgetItem* item = m_sectionsTable->item(row, i);

            if (section.isValid) {
                item->setBackground(QBrush(Qt::green));
            } else {
                item->setBackground(QBrush(Qt::red));
            }
        }

        row++;
    }

    //m_sectionsTable->resizeColumnsToContents();
    m_symbolsTable->resize(100, 10);
}
