#ifndef ILINKERREADER_H
#define ILINKERREADER_H

#include "ilinkerfile.h"

namespace compiler_tools {

class ILinkerReader {
public:
    ILinkerReader() = default;
    virtual ~ILinkerReader() = default;
    virtual bool read(ILinkerFile& linkerFile, QString& script) = 0;
    virtual bool loadDescriptorFromFile(const QString& file) = 0;
};

}

#endif // ILINKERREADER_H
