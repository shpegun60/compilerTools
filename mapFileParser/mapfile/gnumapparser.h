#ifndef GNUMAPPARSER_H
#define GNUMAPPARSER_H

#include "mapdescriptor.h"
#include <QHash>
#include <QList>

// MapSections descriptor
struct SubSection {
    QString subName;
    QString adress;
    QStringList lines;
};

class GnuMapParser
{
public:
    GnuMapParser() = default;
    ~GnuMapParser() = default;

public:
    inline void installDescriptor(MapDescriptor& descriptor) { _descriptor = &descriptor; }
    bool parse();

private: /* variables */
    MapDescriptor* _descriptor = nullptr;
    QHash<quint64, QList<SubSection>> addressMap {};
    QStringList unknownLines {};
};

#endif // GNUMAPPARSER_H
