#include "linkerfilefactory.h"
#include "linkerreaderfactory.h"
#include <QFile>
#include <QFileInfo>

namespace compiler_tools {

ILinkerFile* LinkerFileFactory::create(const QString& filePath)
{
    ILinkerFile* linkerFile = new ILinkerFile();
    if(linkerFile) {
        read(*linkerFile, filePath);
    }

    return linkerFile;
}

ILinkerFile* LinkerFileFactory::create()
{
    return new ILinkerFile();
}

bool LinkerFileFactory::read(ILinkerFile& linkerFile, const QString &filePath)
{
    if(filePath.isEmpty()) {
        return false;
    }

    // Відкриваємо файл для читання
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false; // Помилка відкриття файлу
    }

    QFileInfo fileInfo(filePath);
    QString script = QString::fromUtf8(file.readAll());
    file.close();

    // Визначте тип лінкера на основі розширення
    ILinkerFile::LinkerType type = ILinkerFile::LinkerType::Empty;
    if (fileInfo.suffix().compare("ld", Qt::CaseInsensitive) == 0) {
        type = ILinkerFile::LinkerType::GNU;
    } else if (fileInfo.suffix().compare("lds", Qt::CaseInsensitive) == 0) {
        type = ILinkerFile::LinkerType::Clang;
    } else {
        return false;
    }

    linkerFile.clear();

    // Отримайте читача з кешу
    if (auto* reader = LinkerReaderFactory::getReader(type)) {
        return reader->read(linkerFile, script);
    }
    return false;
}

bool LinkerFileFactory::loadDescriptorFromFile(const QString &path)
{
    return LinkerReaderFactory::installDescriptor(path);
}

void LinkerFileFactory::clearCache()
{
    LinkerReaderFactory::clearCache();
}

}
