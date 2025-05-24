#ifndef MAPRAW_H
#define MAPRAW_H

#include "mapdescriptor.h"

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

    using Data = QList<Section>;
    using Names = QSet<QString>;

public: /* functions */
    bool read(const MapDescriptor&, const QString&);
    inline const Data& data() const { return _sections; }
    inline const Names& names() const { return _names; }
    inline const QStringList& ignored() const { return _ignored; }
    inline void clear() { _sections.clear(); _ignored.clear(); }
private:
    void readBody(const MapDescriptor&, const QString&);

private: /* variables */
    Data _sections {};
    Names _names{};
    QStringList _ignored {};
};

}

#endif // MAPRAW_H
