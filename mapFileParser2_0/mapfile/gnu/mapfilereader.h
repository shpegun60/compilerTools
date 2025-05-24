#ifndef MAPFILEREADER_H
#define MAPFILEREADER_H

#include "imapfilereader.h"
#include "mapsymbol.h"

namespace compiler_tools::gnu {

class MapFileReader : public IMapFileReader
{
public:
    MapFileReader() = default;
    ~MapFileReader() = default;

    struct StrangeMarker {
        qsizetype sectionId;
        quint64 addr;
        qsizetype symbolId;
        qsizetype lineId;
    };

    enum SymbolType
    {
        isUndef,
        isSymbol,
        isFill,
        isSectionInfo,
        isOutOfRange
    };

    struct Something {
        QString name;
        QString lable;
        QString filepath;
        quint64 vram;
        std::optional<quint64> vrom;
        std::optional<quint64> size;
        SymbolType type;
        qsizetype nextPos;
    };

public: /* IMapFileReader interface */
    virtual bool read(IMapFile&, QString&) override;
    virtual bool loadDescriptorFromFile(const QString&) override;
private:
    bool validateName(const QString&) const;
    Something processSymbol(const QSet<QString>& names, const MapSymbol::Symbol& data, const qsizetype pos);
private: /* fields */
    // descriptor of file -------------------
    MapDescriptor descr;
    QList<StrangeMarker> stranges;
};

}

#endif // MAPFILEREADER_H
