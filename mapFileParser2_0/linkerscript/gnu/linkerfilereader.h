#ifndef LINKERFILEREADER_H
#define LINKERFILEREADER_H

#include "linkerdescr.h"
#include "ilinkerreader.h"

namespace compiler_tools::gnu {

class LinkerFileReader : public ILinkerReader
{
public:
    LinkerFileReader() = default;
    ~LinkerFileReader() = default;
    virtual bool read(ILinkerFile& file, QString& script) override;
    virtual bool loadDescriptorFromFile(const QString& file) override;
private:
    void sectionBind(ILinkerFile& file, ILinkerFile::Section& sec, const QString& mem_region, const QString& attribute);
    void evaluateGlobals(ILinkerFile& file);
    void evaluateNumber(ILinkerFile& file, QString& expr);
private:
    // descriptor of file -------------------
    LinkerDescriptor descr;
};

} //namespace compiler_tools::gnu

#endif // LINKERFILEREADER_H
