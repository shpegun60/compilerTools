#ifndef MAPRAW_H
#define MAPRAW_H

#include "mapdescriptor.h"
#include "imapfile.h"

namespace compiler_tools::gnu {

class MapRaw
{
public:
    MapRaw() = default;
    ~MapRaw() = default;

    struct Section {
        QString     name;
        QStringList lines;
    };

    struct Fill {
        quint64 addr;
        quint64 size;
    };

    using Data = QList<Section>;

public: /* functions */
    bool read(const MapDescriptor&, const QString&);
    inline const Data& data() const { return _sections; }
    inline auto& fills() { return _fills; }
    inline const QStringList& ignored() const { return _ignored; }
    inline void clear() { _sections.clear(); _ignored.clear(); }
private:
    void readBody(const MapDescriptor&, const QString&);

private: /* variables */
    Data _sections {};
    IMapFile::Fills _fills{};
    QStringList _ignored {};
};

}

#endif // MAPRAW_H
