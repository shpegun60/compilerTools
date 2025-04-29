#include "linkerdescr.h"

namespace compiler_tools::gnu {

void LinkerDescriptor::remove_unnecessary(QString& script)
{
    // Remove C-style comments /* ... */
    script.remove(cCommentRegex);
    // Remove C++-style comments // to the end of the line
    script.remove(cppCommentRegex);
    // Optional: remove remaining empty lines
    script.replace(emptyLines, "\n");
}

bool LinkerDescriptor::validate(LinkerDescriptor* const descr, QTextStream& log) {
    if (!descr) {
        qCritical() << "[ERROR] Null descriptor pointer";
        return false;
    }

    if(descr->isValidated) {
        return true;
    }

    bool ok = true;

    log << "----------------------------------------------------" << Qt::endl;
    log << "[INFO] Validate LinkerDescriptor" << Qt::endl;
    log << "----------------------------------------------------" << Qt::endl;

    log << "1) Check all QRegularExpression: ";

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
            log << "[ERROR] Invalid regex " << r.name
                   << ": " << r.re.errorString() << Qt::endl;
            ok = false;
        }
    }

    log << ok << Qt::endl;

    log << "2) Checking string constants: ";

    const struct S { const QString& s; const char* name; } strs[] = {
        { descr->globalName, "globalName" },
        { descr->noName, "noName" },
        { descr->unknown, "unknown" },
        { descr->sectionBlockName,"sectionBlockName"},
        { descr->memoryBlockName, "memoryBlockName" }
    };
    for (auto &s : strs) {
        if (s.s.isEmpty()) {
            log << "[ERROR] Empty string " << s.name << Qt::endl;
            ok = false;
        }
    }

    log << ok << Qt::endl;

    log << "3) Checking units: ";

    const QChar needed[] = { 'K','M','G','T' };
    for (QChar u : needed) {
        if (!descr->units.contains(u)) {
            log << "[ERROR] units missing key " << u << Qt::endl;
            ok = false;
        }
    }

    if (descr->units.isEmpty()) {
        log << "[ERROR] units map is empty: ";
        ok = false;
    }

    log << ok << Qt::endl;

    log << "4) Other constants: ";

    if (LinkerDescriptor::evaluateCnt <= 0) {
        log << "[ERROR] evaluateCnt must be >0" << Qt::endl;
        ok = false;
    }

    log << ok << Qt::endl;

    log << "----------------------------------------------------" << Qt::endl;
    log << "[INFO] END Validate LinkerDescriptor" << Qt::endl;
    log << "----------------------------------------------------" << Qt::endl << Qt::endl;
    descr->isValidated = ok;
    return ok;
}


} /* namespace compiler_tools::gnu */
