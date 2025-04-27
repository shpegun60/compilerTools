#ifndef LINKERRAW_H
#define LINKERRAW_H

#include "linkerdescr.h"
#include <QHash>

class LinkerRaw
{
public: /* types */
    using Data = QHash<QString, QString>;
public: /* functions */
    LinkerRaw() = default;
    const Data& read(const LinkerDescriptor&, const QString&);
    inline const Data& data() const { return _blocks; }
    inline void clear() { _blocks.clear(); }
private: /* values */
    Data _blocks{};
    static const inline QString delimiters = " \n\t\r\f\v;{}=<>\"[]()+-*:.\\/&|`~";
};

#endif // LINKERRAW_H
