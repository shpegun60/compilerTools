#ifndef LINKERRAW_H
#define LINKERRAW_H

#include "linkerdescr.h"
#include <QRegularExpression>
#include <QString>
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
};

#endif // LINKERRAW_H
