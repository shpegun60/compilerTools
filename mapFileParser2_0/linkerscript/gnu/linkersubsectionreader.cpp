#include "linkersubsectionreader.h"

namespace compiler_tools::gnu {

void LinkerSubSectionReader::append(const LinkerDescriptor& descr,
                              Data& container,
                              const QString& name)
{
    for (auto &ignored: descr.subsectionIgnored) {
        if(name.startsWith(ignored)) {
            return;
        }
    }

    for (auto &sub: container) {
        if(sub == name) {
            return;
        }
    }
    container.emplace_back(name);
}
bool LinkerSubSectionReader::read(const LinkerDescriptor& descr,
                            Data& container,
                            const QString& text)
{
    QRegularExpressionMatchIterator it = descr.subsectionRegex.globalMatch(text);
    while (it.hasNext()) {
        const auto m = it.next();
        const QStringList names = m.captured(1).split(' ', Qt::SkipEmptyParts);

        for(auto& it : names) {
            append(descr, container, it.trimmed());
        }
    }
    return true;
}

} /* namespace compiler_tools::gnu */
