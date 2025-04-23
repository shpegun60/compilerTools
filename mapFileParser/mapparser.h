#ifndef MAPPARSER_H
#define MAPPARSER_H

#include <QVector>
#include <QString>
#include <QRegularExpression>
#include <QFileInfo>

struct Symbol {
    QString name;
    quint64 vram = 0;
    quint64 size = 0;
    quint64 vrom = 0;
    quint64 align = 0;
};

struct File {
    QFileInfo path;
    QString sectionType;
    quint64 vram = 0;
    quint64 size = 0;
    quint64 vrom = 0;
    QVector<Symbol> symbols;
};

struct Segment {
    QString name;
    quint64 vram = 0;
    quint64 size = 0;
    quint64 vrom = 0;
    QVector<File> files;

    static Segment newPlaceholder();
    bool isPlaceholder() const;
};

class MapParser
{
public:
    MapParser();
};

#endif // MAPPARSER_H
