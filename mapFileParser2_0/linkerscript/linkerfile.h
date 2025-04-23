#ifndef LINKERFILE_H
#define LINKERFILE_H

#include "linkermemory.h"
#include "linkersubsection.h"
#include "linkervariable.h"
#include <QJSEngine>

class LinkerFile
{
public:
    struct Section {
        QString                     name;
        LinkerSubSection            subnames;
        LinkerVariable              vars;
        LinkerMemory::MemoryRegion* vma; // virtual memory address
        LinkerMemory::MemoryRegion* lma; // load memory address
    };
    using Sections = QHash<QString, Section>; /* { name, {name, subnames, vars, vma, lma} } */

    struct Region {
        LinkerMemory::MemoryRegion region;
        QList<const Section*> sections;
    };

    using Regions = QHash<QString, Region>;
public:
    LinkerFile() = default;
    void read(LinkerDescriptor&, QString&);
    inline const auto& sections() const { return _sections; }
    auto& regions() { return _regions; }
    auto& globals() { return _globals; }
    auto& regions_undef() { return _regions_undef; }
    QJSEngine& sandbox() { return engine; }
private:
    void sectionBind(LinkerDescriptor& descr,
                     Section& sec, const QString& mem_region, const QString& attribute);

    void evaluateGlobals(LinkerDescriptor&);
    void evaluateNumber(LinkerDescriptor&, QString&);
private:
    Sections _sections{};
    Regions _regions{};
    LinkerVariable _globals{};
    QList<Region*> _regions_undef{};
    QJSEngine engine;
};


#endif // LINKERFILE_H
