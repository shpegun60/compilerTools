#ifndef LINKERSECTION_H
#define LINKERSECTION_H

#include "linkerraw.h"
#include <QString>
#include <QHash>

class LinkerSection
{
public: /* types */
    struct Section {
        QString body;
        QString attribute;
        QString mem_region;
    };
    using Data = QHash<QString, Section>; /* name, {body, attribute, mem_region} */
public: /* functions */
    LinkerSection();
    const Data& read(const LinkerDescriptor&, const LinkerRaw&);
    inline const Data& data() { return _sections; }
    inline void clear() { _sections.clear(); }

public: /* functions */
    const QPair<QString, QString> parseAttribute(const QString&);
private: /* values */
    Data _sections{};
};

#endif // LINKERSECTION_H
