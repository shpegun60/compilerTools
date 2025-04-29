#include "linkerreaderfactory.h"
#include "gnu/linkerfilereader.h"

#include <QFile>
//#include "clang/linkerfilereader.h"

namespace compiler_tools {

ILinkerReader* LinkerReaderFactory::getReader(ILinkerFile::LinkerType type)
{
    std::lock_guard<std::mutex> lock(_cacheMutex);

    if (_readersCache.find(type) == _readersCache.end()) {

        switch (type) {

        case ILinkerFile::LinkerType::GNU: {
            ILinkerReader* reader = new gnu::LinkerFileReader();
            if(reader) {
                readDescrProcess(reader);
                _readersCache[type] = reader;
            }
            break;}

        case ILinkerFile::LinkerType::Clang: {
            //_readersCache[type] = new clang::LinkerFileReader();
            ILinkerReader* reader = new gnu::LinkerFileReader();
            if(reader) {
                readDescrProcess(reader);
                _readersCache[type] = reader;
            }
            break;}

        default:
            return nullptr;
        }
    }
    return _readersCache[type];
}

bool LinkerReaderFactory::installDescriptor(const QString &path)
{
    std::lock_guard<std::mutex> lock(_cacheMutex);

    _descriptorPath = path;

    if(_descriptorPath.isEmpty()) {
        return false;
    }

    QFile file(_descriptorPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QString descriptor = QString::fromUtf8(file.readAll());
    file.close();

    bool ok = true;
    for (auto& pair : _readersCache) {
        if(!pair.second->loadDescriptorFromFile(descriptor)) {
            ok = false;
        }
    }

    return ok;
}

void LinkerReaderFactory::clearCache()
{
    std::lock_guard<std::mutex> lock(_cacheMutex);

    for (auto& pair : _readersCache) {
        delete pair.second;
    }
    _readersCache.clear();
}

void LinkerReaderFactory::readDescrProcess(ILinkerReader* const reader)
{
    if(_descriptorPath.isEmpty()) {
        return;
    }

    QFile file(_descriptorPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString descriptor = QString::fromUtf8(file.readAll());
    file.close();
    reader->loadDescriptorFromFile(descriptor);
}

} // namespace compiler_tools
