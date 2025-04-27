#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTableWidget>
#include <QSplitter>
#include <QHeaderView>
#include <QTextEdit>

#include "linkerfile.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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

    LinkerDescriptor ld_descr;
    LinkerFile ld;
};
#endif // MAINWINDOW_H
