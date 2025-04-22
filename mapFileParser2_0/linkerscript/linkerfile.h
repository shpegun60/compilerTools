#ifndef LINKERFILE_H
#define LINKERFILE_H

#include "linkermemory.h"
#include "linkersubsection.h"
#include "linkervariable.h"

class LinkerFile
{
public:
    struct Section {
        QString                     name;
        LinkerSubSection            subnames;
        LinkerVariable              vars;
        QString                     region;
        LinkerMemory::MemoryRegion* vma; // virtual memory address
        LinkerMemory::MemoryRegion* lma; // load memory address
    };
    using Sections = QHash<QString, Section>; /* { name, {name, subnames, vars, vma, lma} } */

    struct Region {
        LinkerMemory::MemoryRegion region;
        QList<Section*> sections;
    };

    using Regions = QHash<QString, Region>;
public:
    LinkerFile() = default;
    void parse(const LinkerDescriptor&, QString&);
    void evaluate(const LinkerDescriptor& descr);
private:
    Sections _sections{};
    Regions _regions{};
    LinkerVariable::Data _globals{};
};


#endif // LINKERFILE_H
