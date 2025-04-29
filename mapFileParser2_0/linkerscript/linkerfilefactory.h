#ifndef LINKERFILEFACTORY_H
#define LINKERFILEFACTORY_H

#include "ilinkerfile.h"

namespace compiler_tools {

class LinkerFileFactory {
public:
    static ILinkerFile* create(const QString& filePath);
    static ILinkerFile* create();
    static bool read(ILinkerFile&, const QString& filePath, bool clear = false);
    static bool loadDescriptorFromFile(const QString& path);
    static void clearCache();
};

} // namespace compiler_tools

#endif // LINKERFILEFACTORY_H
