#ifndef LINKERREADERFACTORY_H
#define LINKERREADERFACTORY_H

#include "ilinkerreader.h"
#include "ilinkerfile.h"
#include <map>
#include <mutex>

namespace compiler_tools {

class LinkerReaderFactory {
public:
    static ILinkerReader* getReader(ILinkerFile::LinkerType type);
    static bool installDescriptor(const QString& path);
    static void clearCache();

private:
    static void readDescrProcess(ILinkerReader* const);
private:
    static inline std::map<ILinkerFile::LinkerType, ILinkerReader*> _readersCache{};
    static inline std::mutex _cacheMutex{};
    static inline QString _descriptorPath = {};
};

}

#endif // LINKERREADERFACTORY_H
