#ifndef MAPDESCRIPTOR_H
#define MAPDESCRIPTOR_H

#include "sectioncategory.h"
#include <QRegularExpression>

class MapDescriptor
{
public:
    MapDescriptor() = default;
    ~MapDescriptor() = default;

    MapSection _linker{"._linker_", {}};
    MapSection _unknown_lines{"._unknown_", {}};

    SectionCategory _text{".text", ".isr_vector"};
    SectionCategory _data{".data", ".rodata", ".strings", "._pm" };
    SectionCategory _bss{".bss"/*, "COMMON"*/ };
    SectionCategory _unknown_sections{};

public: /* helpers */
    bool isEnd(const QString &s);
    bool isIgnore(const QString &s);
    bool isSection(const QString &s);
    bool containsSectionSize(const QString &line);
    std::pair<QString, quint64> readLineAddress(const QString &s);
    void push2category(const MapSection&);

public:
    const QVector<SectionCategory*> categories {&_text, &_data, &_bss};

    // map file splitters -----------------------------------------------
    QString C_MEM_LINKER_INFO_HEADER {"Memory Configuration"};
    QString C_MEM_MAP_HEADER {"Linker script and memory map"};

    // markers ----------------------------------------------------------
    QString C_FILL_IDENTIFIER {"*fill*"}; // All fill bytes are identified by this string in a Map file
    QStringList C_MEM_MAP_IGNORE_MARKERS {C_FILL_IDENTIFIER, "LOAD ", "START GROUP", "END GROUP"};
    QStringList C_MEM_MAP_END_MARKERS {"/DISCARD/", "OUTPUT", "LOAD linker stubs"};

    // RegExpr----------------------------------------------------------
    const QRegularExpression sectionRegex { R"(^\*\((.*?)\))" };
    const QRegularExpression tokenSplitter { R"(\s+)" };
    const QRegularExpression adressRegex {R"(\s*(0[xX][0-9a-fA-F]+))"}; //
};

#endif // MAPDESCRIPTOR_H
