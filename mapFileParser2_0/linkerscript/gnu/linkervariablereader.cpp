#include "linkervariablereader.h"
#include <algorithm>

namespace compiler_tools::gnu {

/// Parse out all the “VAR = VAL;” and “ATTR(VAR = VAL);” forms from @text
bool LinkerVariableReader::read(const LinkerDescriptor& descr,
                                Data& container,
                                const QString& text)
{
    // 1) ATTR(lvalue = rvalue);
    {
        QRegularExpressionMatchIterator it = descr.attrValueRegex.globalMatch(text);
        while (it.hasNext()) {
            const auto m = it.next();
            // push variable…
            container.emplace_back(ILinkerFile::Variable {
                m.captured(1).trimmed(),
                m.captured(2).trimmed(),
                m.captured(3).trimmed(),
                m.capturedStart(0),
                m.capturedEnd(0),
                false
            });
        }
    }

    // helper to test span overlap
    const auto overlapsAttr = [&](const qsizetype s, const qsizetype e) {
        for (auto &sp: std::as_const(container)) {
            if (!(e <= sp._start || s >= sp._end)) {  // ranges intersect
                return true;
            }
        }
        return false;
    };

    // 2) plain lvalue = rvalue;
    {
        QRegularExpressionMatchIterator it = descr.valueRegex.globalMatch(text);
        while (it.hasNext()) {
            const auto m = it.next();
            const auto _start = m.capturedStart(0);
            const auto _end = m.capturedEnd(0);
            if (overlapsAttr(_start, _end)) {
                continue;   // skip anything inside ATTR(...) spans
            }

            ;
            container.emplace_back(ILinkerFile::Variable {
                {},
                m.captured(1).trimmed(),
                m.captured(2).trimmed(),
                _start,
                _end,
                false
            });
        }
    }

    std::sort(container.begin(),container.end(),
              [](const ILinkerFile::Variable &a, const ILinkerFile::Variable &b) {
                  return a._start < b._start;
              });
    return true;
}

} /* namespace compiler_tools::gnu */
