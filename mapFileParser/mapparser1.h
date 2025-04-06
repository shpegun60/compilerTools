#pragma once

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QVector>

// Структура, що описує елемент (Item) – рядок з інформацією (назва, адреса, розмір)
struct Item {
    int index = 0;
    QString name;
    QString file;
    QString address;
    QString size;
};

// Структура, що описує секцію.
// Верхній рівень (root) має порожню назву, а його підсекции зберігаються у векторі subSections.
// Крім того, секція може містити "листові" елементи – items.
struct Section {
    int index = 0;
    QString name;
    quint64 sectionSize;
    QVector<Section> subSections;
    QVector<Item> items;
};

// Результат парсингу містить сумарну висоту (itemHeight) та кореневу секцію.
struct ParseResult {
    Section rootSection;
};

// Клас, що реалізує логіку парсингу map-файлу
class MapFileParser {
public:
    // Головна функція для парсингу тексту map-файлу
    ParseResult parseMapFile(const QString &mapData);

private:
    struct Lines {
        QString sectionName;
        QStringList lines;
    };

    QStringList linkerInfo;
    QVector<Lines> sections;
    QStringList unknownLines;

private:
    // Допоміжні функції
    bool approvedSection(const QString &s);
    bool approvedLines(const QString &s);
    bool isSectionTitle(const QString &s);
    bool isEnd(const QString &s);
    QString readLineAddress(const QString &s);
    QString readLineSize(const QString &s);
    QString readLineName(const QString &s);
    QVector<Item> parseItemLines(const QStringList &lines, const QString sectionName);
    // Якщо рядок містить заповнювач (*fill*), повертає відповідний Item, інакше – порожній Item.
    Item readFill(const QString &s);
private:
    // map file splitters -----------------------------------------------
    QString C_MEM_LINKER_INFO_HEADER = "Memory Configuration";
    QString C_MEM_MAP_HEADER = "Linker script and memory map";

    // sections----------------------------------------------------------
    const QRegularExpression sectionRegex { R"(^\*\((.*?)\))" };
    QStringList C_TEXT_ID = { ".text" };
    QStringList C_DATA_ID = { ".data", ".rodata", ".strings", "._pm" };
    QStringList C_BSS_ID = { ".bss", "COMMON" };

    // marlers ----------------------------------------------------------
    QString C_FILL_IDENTIFIER = "*fill*"; // All fill bytes are identified by this string in a Map file
    QStringList C_MEM_MAP_IGNORE_MARKERS = {"LOAD "};
    QStringList C_MEM_MAP_END_MARKERS = {"OUTPUT", "LOAD linker stubs"};
};
