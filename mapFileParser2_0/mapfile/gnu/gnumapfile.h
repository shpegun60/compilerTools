#ifndef GNUMAPFILE_H
#define GNUMAPFILE_H

#include "mapfile.h"
#include "gnudescriptor.h"
#include "gnusegment.h"
#include "gnusection.h"
#include <QList>

class GnuMapFile
{
public:
    GnuMapFile() = default;
    GnuMapFile(const GnuDescriptor& descr);
    GnuMapFile(const GnuDescriptor* const descr);

    inline void installDescriptor(const GnuDescriptor& descr) { _descr = &descr; }

public: /* types */
    bool read(const QString&);
    void parse(MapFile&);

private:
    QString readLineName(const QString &s);

private /* variables */:
    const GnuDescriptor* _descr = nullptr;
    GnuDescriptor::SegmentSizes _sizes {};
    GnuSegmentReader _segments;
    GnuSectionReder _sections;
    //QStringList _linker;
};

#endif // GNUMAPFILE_H
