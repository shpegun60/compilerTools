#ifndef MAPFILEVIEWER_H
#define MAPFILEVIEWER_H

#include <QWidget>
#include <QSplitter>
#include <QTableWidget>
#include "imapfile.h"

class MapFileViewer : public QWidget
{
    Q_OBJECT

public:
    explicit MapFileViewer(const compiler_tools::IMapFile& mapFile, QWidget* parent = nullptr);
    ~MapFileViewer();

private:
    void setupSymbolsTable();
    void setupFilesTable();
    void setupSectionsTable();

    const compiler_tools::IMapFile& m_mapFile;
    QSplitter* m_splitter;
    QTableWidget* m_symbolsTable;
    QTableWidget* m_filesTable;
    QTableWidget* m_sectionsTable;
};


#endif // MAPFILEVIEWER_H
