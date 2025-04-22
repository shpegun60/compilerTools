#include "symbol.h"
#include <QHash>

Symbol::Symbol(const QString& name, quint64 vram,
               const QString& file,
               std::optional<quint64> size,
               std::optional<quint64> vrom,
               std::optional<quint64> align)
    : _name(name), _vram(vram), _file(file), _size(size), _vrom(vrom), _align(align) {}


// Formatting strings using Qt
QString Symbol::vramStr() const {
    return QString("0x%1").arg(_vram, 8, 16, QLatin1Char('0')).toUpper();
}

QString Symbol::sizeStr() const {
    return _size.has_value() ? QString::number(*_size) : "None";
}

QString Symbol::vromStr() const {
    return _vrom.has_value() ?
               QString("0x%1").arg(*_vrom, 6, 16, QLatin1Char('0')).toUpper() :
               "None";
}

QString Symbol::alignStr() const {
    return _align.has_value() ?
               QString("0x%1").arg(*_align, 0, 16).toUpper() :
               "None";
}

// CSV functionality
QString Symbol::toCsvHeader() {
    return "Symbol name,VRAM,Size in bytes,File";
}

QString Symbol::toCsv() const {
    return QString("%1,%2,%3,4").arg(_name, vramStr(), sizeStr(), file());
}

// Comparison
bool Symbol::operator==(const Symbol& other) const {
    return _name == other._name && _file == other._file && _vram == other._vram;
}

bool Symbol::operator!=(const Symbol& other) const {
    return !(*this == other);
}

// Hash function for use in QHash/QSet
uint qHash(const Symbol& key, uint seed) {
    return qHash(key._name, seed) ^ qHash(key._file, seed) ^ qHash(key._vram, seed);
}
