#include "symbol.h"
#include <QTextStream>

Symbol::Symbol(const QString& name, quint64 vram,
               std::optional<quint64> size,
               std::optional<quint64> vrom,
               std::optional<quint64> align)
    : _name(name), _vram(vram), _size(size), _vrom(vrom), _align(align) {}


// Formatting strings using Qt
QString Symbol::getVramStr() const {
    return QString("0x%1").arg(_vram, 8, 16, QLatin1Char('0')).toUpper();
}

QString Symbol::getSizeStr() const {
    return _size.has_value() ? QString::number(*_size) : "None";
}

QString Symbol::getVromStr() const {
    return _vrom.has_value() ?
               QString("0x%1").arg(*_vrom, 6, 16, QLatin1Char('0')).toUpper() :
               "None";
}

QString Symbol::getAlignStr() const {
    return _align.has_value() ?
               QString("0x%1").arg(*_align, 0, 16).toUpper() :
               "None";
}

// CSV functionality
QString Symbol::toCsvHeader() {
    return "Symbol name,VRAM,Size in bytes";
}

QString Symbol::toCsv() const {
    return QString("%1,%2,%3").arg(_name, getVramStr(), getSizeStr());
}

void Symbol::printAsCsv() const {
    QTextStream out(stdout);
    out << toCsv() << Qt::endl;
}

// Comparison
bool Symbol::operator==(const Symbol& other) const {
    return _name == other._name && _vram == other._vram;
}

bool Symbol::operator!=(const Symbol& other) const {
    return !(*this == other);
}

// // Hash function for use in QHash/QSet
// uint qHash(const Symbol& key, uint seed) {
//     return qHash(key._name, seed) ^ qHash(key._vram, seed);
// }
