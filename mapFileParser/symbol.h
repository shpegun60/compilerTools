#ifndef SYMBOL_H
#define SYMBOL_H

#include <QString>
#include <optional>
#include <QtContainerFwd> // Для forward-декларацій Qt-контейнерів

class Symbol
{
public:
    Symbol(const QString& name, quint64 vram,
           std::optional<quint64> size = std::nullopt,
           std::optional<quint64> vrom = std::nullopt,
           std::optional<quint64> align = std::nullopt);

    // Getters
    const QString& getName() const { return _name; }
    quint64 getVram() const { return _vram; }
    std::optional<quint64> getSize() const { return _size; }
    std::optional<quint64> getVrom() const { return _vrom; }
    std::optional<quint64> getAlign() const { return _align; }

    // Setters
    void setName(const QString& name) { _name = name; }
    void setVram(quint64 vram) { _vram = vram; }
    void setSize(quint64 size) { _size = size; }
    void setVrom(quint64 vrom) { _vrom = vrom; }
    void setAlign(quint64 align) { _align = align; }

    // String representations (return QString)
    QString getVramStr() const;
    QString getSizeStr() const;
    QString getVromStr() const;
    QString getAlignStr() const;

    // CSV functionality
    static QString toCsvHeader();
    QString toCsv() const;
    void printAsCsv() const;

    // Comparison
    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const;

    // Hash function for use in QHash/QSet
    friend uint qHash(const Symbol& key, uint seed);

    enum Scope {
        TYPE_UNKNOWN = 0,
        TYPE_STATIC = 1,
        TYPE_GLOBAL = 2,
        TYPE_HIDDEN = 3,
    };

private:
    QString _name {};   // name
    quint64 _vram = 0;  // address
    std::optional<quint64> _size = std::nullopt;
    std::optional<quint64> _vrom = std::nullopt;
    std::optional<quint64> _align = std::nullopt;
    Scope _scope = TYPE_UNKNOWN;// For now we have Global, Static and Hidden
};

#endif // SYMBOL_H
