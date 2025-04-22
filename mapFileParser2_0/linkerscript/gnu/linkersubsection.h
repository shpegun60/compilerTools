#ifndef LINKERSUBSECTION_H
#define LINKERSUBSECTION_H

#include "linkerdescr.h"
#include <QStringList>
#include <QList>

class LinkerSubSection
{
public: /* types */
    using Data = QStringList;
public: /* functions */
    LinkerSubSection();
    const Data& read(const LinkerDescriptor&, const QString&);
    void append(const LinkerDescriptor&, const QString&);
    inline void clear() { _subsections.clear(); }
    inline const Data& data() const { return _subsections; }
private: /* values */
    Data _subsections{};
};

#endif // LINKERSUBSECTION_H
