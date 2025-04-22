#ifndef LINKERVARIABLE_H
#define LINKERVARIABLE_H

#include "linkerdescr.h"
#include <QString>
#include <QList>

class LinkerVariable
{
public: /* types */
    struct Variable {
        QString attribute;      // e.g. "PROVIDE_HIDDEN" or empty
        QString lvalue;         // e.g. "_Min_Heap_Size"
        QString rvalue;         // e.g. "0x300"
        qsizetype _start;       // start cursor
        qsizetype _end;         // end cursor
    };
    using Data = QList<Variable>;
public: /* functions */
    LinkerVariable() = default;
    const Data& read(const LinkerDescriptor&, const QString &);
    inline void clear() { _variables.clear(); }
    inline const Data& data() const { return _variables; }
private: /* values */
    Data _variables{};
};


#endif // LINKERVARIABLE_H
