#include "linkersubsection.h"

LinkerSubSection::LinkerSubSection() {}

void LinkerSubSection::append(const LinkerDescriptor& descr, const QString& name)
{
    for (auto &ignored: descr.subsectionIgnored) {
        if(name.startsWith(ignored)) {
            return;
        }
    }

    for (auto &sub: _subsections) {
        if(sub == name) {
            return;
        }
    }
    _subsections.emplace_back(name);
}

const LinkerSubSection::Data& LinkerSubSection::read(const LinkerDescriptor& descr, const QString& text)
{
    QRegularExpressionMatchIterator it = descr.subsectionRegex.globalMatch(text);
    while (it.hasNext()) {
        const auto m = it.next();
        const QStringList names = m.captured(1).split(' ', Qt::SkipEmptyParts);

        for(auto& it : names) {
            append(descr, it.trimmed());
        }
    }
    return _subsections;
}
