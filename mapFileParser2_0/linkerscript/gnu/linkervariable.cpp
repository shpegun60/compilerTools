#include "linkervariable.h"
#include <algorithm>

/// Parse out all the “VAR = VAL;” and “ATTR(VAR = VAL);” forms from @text
const LinkerVariable::Data &LinkerVariable::read(const LinkerDescriptor& descr, const QString& text)
{
    // 1) ATTR(lvalue = rvalue);
    {
        QRegularExpressionMatchIterator it = descr.attrValueRegex.globalMatch(text);
        while (it.hasNext()) {
            auto m = it.next();
            // extract variable…
            Variable v {
                m.captured(1).trimmed(),
                m.captured(2).trimmed(),
                m.captured(3).trimmed(),
                m.capturedStart(0),
                m.capturedEnd(0)
            };
            _variables.emplace_back(std::move(v));
        }
    }

    // helper to test span overlap
    auto overlapsAttr = [&](int s, int e) {
        for (auto &sp: _variables) {
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
            auto m = it.next();
            const auto _start = m.capturedStart(0);
            const auto _end = m.capturedEnd(0);
            if (overlapsAttr(_start, _end)) {
                continue;   // skip anything inside ATTR(...) spans
            }

            Variable v {
                {},
                m.captured(1).trimmed(),
                m.captured(2).trimmed(),
                _start,
                _end
            };
            _variables.emplace_back(std::move(v));
        }
    }

    std::sort(_variables.begin(),_variables.end(),
              [](const Variable &a, const Variable &b) {
                  return a._start < b._start;
              });
    return _variables;
}
