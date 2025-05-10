#ifndef IMAPFILEREADER_H
#define IMAPFILEREADER_H

#include "imapfile.h"

namespace compiler_tools {

class IMapFileReader
{
public:
    IMapFileReader() = default;
    virtual ~IMapFileReader() = default;
    virtual bool read(IMapFile& mapFile, QString& file) = 0;
    virtual bool loadDescriptorFromFile(const QString& file) = 0;
};

}



#endif // IMAPFILEREADER_H
