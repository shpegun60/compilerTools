#ifndef LINKERSECTION_H
#define LINKERSECTION_H

#include "linkerraw.h"
#include <QList>

class LinkerSection
{
public: /* types */
    struct Section {
        QString name;
        QString body;
        QString attribute;
        QString mem_region;
    };
    using Data = QList<Section>; /* {name, body, attribute, mem_region} */
public: /* functions */
    LinkerSection();
    const Data& read(const LinkerDescriptor&, const LinkerRaw&);
    inline const Data& data() { return _sections; }
    inline void clear() { _sections.clear(); }

public: /* functions */
    const QPair<QString, QString> parseAttribute(const QString&);
private: /* values */
    Data _sections{};
    // delimiters
    static const inline QString name_delimiters = "\n\t\r\f\v;{}=<\"[]`";
    static const inline QString attribute_delimiters = " \n\t\r\f\v;{}=<>\"[]()+-*:`~|&";
};

#endif // LINKERSECTION_H
