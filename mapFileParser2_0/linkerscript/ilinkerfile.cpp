#include "ilinkerfile.h"

#if QT_VERSION < QT_VERSION_CHECK(6,9,0)
#include <QJSValueIterator>
#endif

namespace compiler_tools {

void ILinkerFile::clear()
{
    _sections.clear();
    _regions.clear();
    _globals.clear();

    // clear sandbox
    QJSValue go = engine.globalObject();
#if QT_VERSION >= QT_VERSION_CHECK(6,9,0)
    for (const QString &name : go.propertyNames()) {
        go.deleteProperty(name);
    }
#else
    QJSValueIterator it(go);
    while (it.hasNext()) {
        it.next();
        go.deleteProperty(it.name());
    }
#endif
    engine.collectGarbage();
}

} //namespace compiler_tools
