#ifndef MAPFILE_H
#define MAPFILE_H

#include "mapdescriptor.h"

class MapFile
{
public:
    MapFile() = default;

public:
    inline void installDescriptor(MapDescriptor& descriptor) { _descriptor = &descriptor; }
    bool read(const QString &mapData);

private: /* readers */
    void readLinker(const QStringList& lines);
    void readBody(const QStringList& lines);

private: /* variables */
    MapDescriptor* _descriptor = nullptr;
};

#endif // MAPFILE_H
