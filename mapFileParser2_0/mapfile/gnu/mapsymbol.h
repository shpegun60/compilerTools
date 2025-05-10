#ifndef MAPSYMBOL_H
#define MAPSYMBOL_H

#include "mapraw.h"

namespace compiler_tools::gnu {


class MapSymbol
{
public:
    MapSymbol() = default;
    ~MapSymbol() = default;

    struct Symbol {
        QString     name;
        QString     address;
        QStringList lines;
    };

    struct Section {
        QString name;
        QHash<quint64, QList<Symbol>> addresses;
    };

    using Data = QList<Section>;

public: /* functions */
    bool read(const MapDescriptor&, const MapRaw&);
    inline const Data& data() const { return _sections; }
    inline const QStringList& ignored() const { return _ignored; }
    inline void clear() { _sections.clear(); _ignored.clear(); }

    int estimateSymbolCount() { return _estimatedSymbols; };

private:
    Data _sections{};
    QStringList _ignored {};
    int _estimatedSymbols = 0;
};

}
#endif // MAPSYMBOL_H
