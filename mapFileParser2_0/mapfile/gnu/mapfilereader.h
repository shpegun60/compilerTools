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

    enum CursorType
    {
        isUndef,
        isSymbol,
        isSectionInfo,
        isOutOfRange
    };

    struct Cursor {
        QString name;
        QString lable;
        QString filepath;
        quint64 vram;
        std::optional<quint64> vrom;
        std::optional<quint64> size;
        CursorType type;
        qsizetype nextPos;
    };

public: /* IMapFileReader interface */
    virtual bool read(IMapFile&, QString&) override;
    virtual bool loadDescriptorFromFile(const QString&) override;
private:
    bool validateName(const QString&) const;
    Cursor processSymbol(const QSet<QString>& names, const MapSymbol::Symbol& data, const qsizetype pos);
    void processFill(IMapFile&, IMapFile::Section&, const IMapFile::Fills&);
    void processSectionInfos(IMapFile& mapFile);
    quint64 selectMedianVariant(const quint64 var1, const quint64 var2, const quint64 var3);
private: /* fields */
    // descriptor of file -------------------
    MapDescriptor descr;
};

}

#endif // MAPFILEREADER_H
