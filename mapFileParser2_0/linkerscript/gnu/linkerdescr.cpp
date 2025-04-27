#include "linkerdescr.h"

LinkerDescriptor::LinkerDescriptor() : stream(&_log)
{}

void LinkerDescriptor::remove_unnecessary(QString& script)
{
    // Remove C-style comments /* ... */
    script.remove(cCommentRegex);
    // Remove C++-style comments // to the end of the line
    script.remove(cppCommentRegex);
    // Optional: remove remaining empty lines
    script.replace(emptyLines, "\n");
}

bool LinkerDescriptor::validate(LinkerDescriptor* descr) {
    if (!descr) {
        qCritical() << "[ERROR] Null descriptor pointer";
        return false;
    }
    bool ok = true;

    *descr << "----------------------------------------------------" << Qt::endl;
    *descr << "[INFO] Validate LinkerDescriptor" << Qt::endl;
    *descr << "----------------------------------------------------" << Qt::endl;

    *descr << "1) Check all QRegularExpression: ";

    struct R { const QRegularExpression& re; const char* name; };
    const R regs[] = {
        { descr->cCommentRegex, "cCommentRegex" },
        { descr->cppCommentRegex, "cppCommentRegex" },
        { descr->emptyLines, "emptyLines" },
        { descr->memoryBlockRegex,"memoryBlockRegex"},
        { descr->attrValueRegex, "attrValueRegex" },
        { descr->valueRegex, "valueRegex" },
        { descr->alignRegex, "alignRegex" },
        { descr->enytryRegex, "enytryRegex" },
        { descr->subsectionRegex, "subsectionRegex" },
        { descr->hexRe, "hexRe" },
        { descr->decRe, "decRe" },
        { descr->tokenRe, "tokenRe" },
        { descr->atRe, "atRe" }
    };
    for (auto &r : regs) {
        if (!r.re.isValid()) {
            *descr << "[ERROR] Invalid regex " << r.name
                   << ": " << r.re.errorString() << Qt::endl;
            ok = false;
        }
    }

    *descr << ok << Qt::endl;

    *descr << "2) Checking string constants: ";

    const struct S { const QString& s; const char* name; } strs[] = {
        { descr->globalName, "globalName" },
        { descr->noName, "noName" },
        { descr->unknown, "unknown" },
        { descr->sectionBlockName,"sectionBlockName"},
        { descr->memoryBlockName, "memoryBlockName" }
    };
    for (auto &s : strs) {
        if (s.s.isEmpty()) {
            *descr << "[ERROR] Empty string " << s.name << Qt::endl;
            ok = false;
        }
    }

    *descr << ok << Qt::endl;

    *descr << "3) Checking units: ";

    const QChar needed[] = { 'K','M','G','T' };
    for (QChar u : needed) {
        if (!descr->units.contains(u)) {
            *descr << "[ERROR] units missing key " << u << Qt::endl;
            ok = false;
        }
    }

    if (descr->units.isEmpty()) {
        *descr << "[ERROR] units map is empty: ";
        ok = false;
    }

    *descr << ok << Qt::endl;

    *descr << "4) Other constants: ";

    if (LinkerDescriptor::evaluateCnt <= 0) {
        *descr << "[ERROR] evaluateCnt must be >0" << Qt::endl;
        ok = false;
    }

    *descr << ok << Qt::endl;

    *descr << "----------------------------------------------------" << Qt::endl;
    *descr << "[INFO] END Validate LinkerDescriptor" << Qt::endl;
    *descr << "----------------------------------------------------" << Qt::endl << Qt::endl;
    return ok;
}
