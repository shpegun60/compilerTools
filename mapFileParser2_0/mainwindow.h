#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTableWidget>
#include <QSplitter>
#include <QHeaderView>
#include <QTextEdit>

#include "ilinkerfile.h"
#include "mapfilereader.h"
#include "mapfileviewer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

using namespace compiler_tools;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setupUI1();
    void populateData();

private:
    Ui::MainWindow *ui;
    QTreeWidget* m_sectionsTree;
    QTableWidget* m_regionsTable;
    QTableWidget* m_globalsTable;
    QTextEdit* m_logText;

    ILinkerFile* ld;
    IMapFile mapfile;
    MapFileViewer* mapView;

    // gnu::LinkerDescriptor ld_descr;
    // gnu::LinkerFile ld;
};
#endif // MAINWINDOW_H
