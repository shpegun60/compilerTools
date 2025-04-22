#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "symbol.h"
#include "QDebug"
#include <QFile>
#include <QSet>

#include "gnumapfile.h"
// #include "gnusegment.h"
// #include "gnusection.h"
#include <QRegularExpression>

#include "linkerraw.h"
#include "linkersection.h"
#include "linkermemory.h"

#include "linkervariable.h"
#include "linkersubsection.h"
#include "linkerfile.h"


// Assume you've installed LLD and its headers are in your INCLUDEPATH

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    {
        LinkerDescriptor ld_descr = LinkerDescriptor::create();
        LinkerFile ld;
        // LinkerRaw linker;
        // LinkerSection ld_section;
        // LinkerSubSection ld_subsection;
        // LinkerMemory ld_mem;
        // LinkerVariable ld_var;

        QFile file("C:\\Users\\admin\\Documents\\Work\\Qt\\compilerTools\\mapFileParser2_0\\linker.ld");
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "do not open";
        }
        QString linkerData = file.readAll();
        ld.parse(ld_descr, linkerData);
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

MainWindow::~MainWindow()
{
    delete ui;
}
