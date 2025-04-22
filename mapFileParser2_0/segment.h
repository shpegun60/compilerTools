#ifndef SEGMENT_H
#define SEGMENT_H

#include "symbol.h"
#include <QString>
#include <QList>
#include <QHash>

class Segment {
public:
    Segment() = default;
    Segment(const QString& name, const quint64 vram,
            const quint64 size = 0,
            const std::optional<quint64> vrom = std::nullopt,
            const std::optional<quint64> align = std::nullopt);

    // Getters
    inline const QString& name() const { return _name; }
    inline quint64 vram() const { return _vram; }
    inline quint64 size() const { return _size; }
    inline std::optional<quint64> vrom() const { return _vrom; }
    inline std::optional<quint64> align() const { return _align; }
    inline const auto& symbols() const { return _symbols; }

    // Setters
    inline void setName(const QString& name) { _name = name; }
    inline void setName(QString&& name) { _name = std::move(name); }
    inline void setVram(const quint64 vram) { _vram = vram; }
    inline void setSize(const quint64 size) { _size = size; }
    inline void setVrom(const quint64 vrom) { _vrom = vrom; }
    inline void setAlign(const quint64 align) { _align = align; }

    // String representations (return QString)
    QString vramStr() const;
    QString vromStr() const;
    QString sizeStr() const;

    inline void push(const Symbol& sym) { _symbols[sym.vram()] = sym; };
    inline void push(Symbol&& sym) { _symbols[sym.vram()] = std::move(sym); };

private:
    QString _name {};
    quint64 _vram = 0;
    quint64 _size = 0;
    std::optional<quint64> _vrom = std::nullopt;
    std::optional<quint64> _align = std::nullopt;
    QHash<quint64, Symbol> _symbols {};
};

#endif // SEGMENT_H
