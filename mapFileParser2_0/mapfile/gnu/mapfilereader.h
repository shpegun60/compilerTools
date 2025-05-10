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
public: /* IMapFileReader interface */
    virtual bool read(IMapFile&, QString&) override;
    virtual bool loadDescriptorFromFile(const QString&) override;
private:
    bool validateName(const QString&) const;
private: /* fields */
    // descriptor of file -------------------
    MapDescriptor descr;
};

}

#endif // MAPFILEREADER_H
