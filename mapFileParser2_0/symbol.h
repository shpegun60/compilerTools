#ifndef SYMBOL_H
#define SYMBOL_H

#include <QString>
#include <optional>
#include <QtContainerFwd> // Forward-declaration Qt-containers

class Symbol
{
public:
    Symbol() = default;
    Symbol(const QString& name, quint64 vram,
           const QString& file = QString(),
           std::optional<quint64> size = std::nullopt,
           std::optional<quint64> vrom = std::nullopt,
           std::optional<quint64> align = std::nullopt);

    // Getters
    inline const QString& name() const { return _name; }
    inline const QString& file() const { return _name; }
    inline quint64 vram() const { return _vram; }
    inline std::optional<quint64> size() const { return _size; }
    inline std::optional<quint64> vrom() const { return _vrom; }
    inline std::optional<quint64> align() const { return _align; }

    // Setters
    inline void setName(const QString& name) { _name = name; }
    inline void setName(QString&& name) { _name = std::move(name); }
    inline void setFile(const QString& file) { _file = file; }
    inline void setFile(QString&& file) { _file = std::move(file); }
    inline void setVram(const quint64 vram) { _vram = vram; }
    inline void setSize(const quint64 size) { _size = size; }
    inline void setVrom(const quint64 vrom) { _vrom = vrom; }
    inline void setAlign(const quint64 align) { _align = align; }

    // String representations (return QString)
    QString vramStr() const;
    QString sizeStr() const;
    QString vromStr() const;
    QString alignStr() const;

    // CSV functionality
    static QString toCsvHeader();
    QString toCsv() const;

    // Comparison
    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const;

    // Hash function for use in QHash/QSet
    friend uint qHash(const Symbol& key, uint seed);

private:
    QString _name {};   // name
    quint64 _vram = 0;  // address
    QString _file {};   // file
    std::optional<quint64> _size = std::nullopt;
    std::optional<quint64> _vrom = std::nullopt;
    std::optional<quint64> _align = std::nullopt;
};

#endif // SYMBOL_H
