#include "linkersubsection.h"

LinkerSubSection::LinkerSubSection() {}

void LinkerSubSection::append(const LinkerDescriptor& descr, const QString& txt)
{
    const QString name = txt.trimmed();
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
    _subsections.append(name);
}

const LinkerSubSection::Data &LinkerSubSection::read(const LinkerDescriptor& descr, const QString& text)
{
    QRegularExpressionMatchIterator it = descr.subsectionRegex.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        append(descr, m.captured(1));
    }
    return _subsections;
}
