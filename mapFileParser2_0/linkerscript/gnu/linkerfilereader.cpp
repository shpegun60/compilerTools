#include "linkerfilereader.h"
#include "linkermemory.h"
#include "linkersubsectionreader.h"
#include "linkervariablereader.h"
#include "linkersection.h"

namespace compiler_tools::gnu {

bool LinkerFileReader::loadDescriptorFromFile(const QString& file)
{
    (void)file;
    LinkerDescriptor::invalidate(&descr);
    return false;
}


bool LinkerFileReader::read(ILinkerFile& file, QString& script)
{
    if(!LinkerDescriptor::validate(&descr, file.log())) {
        return false;
    }

    LinkerRaw ld{};

    file.log() << "----------------------------------------------------" << Qt::endl;
    file.log() << "[INFO] Start GNU-Ld read" << Qt::endl;
    file.log() << "----------------------------------------------------" << Qt::endl;

    // Step 0: Clear file
    file.clear();
    // Step 1: Remove comments and unnecessary parts
    descr.remove_unnecessary(script);
    file.log() << "[read] Comments and unnecessary parts removed from script." << Qt::endl;

    // Step 2: Read raw data from the script
    ld.read(descr, script);
    file.log() << "[read] Raw data read from script." << Qt::endl;

    // Step 3: Read and evaluate global variables
    {
        auto it = ld.data().find(descr.globalName);
        if (it != ld.data().end()) {
            LinkerVariableReader::read(descr, file._globals, it.value());
            evaluateGlobals(file);
            file.log() << "[read] Global variables read and evaluated." << Qt::endl;
        } else {
            file.log() << "[read] No global variables found in script." << Qt::endl;
        }
    }

    // Step 4: Read and process memory regions
    {
        LinkerMemory ld_mem;
        ld_mem.read(descr, ld);
        file._regions.reserve(ld_mem.data().size());
        file.log() << "[read] Reserved space for " << ld_mem.data().size() << " memory regions." << Qt::endl;
        for (const auto& it : ld_mem.data()) {
            auto copy = it;
            evaluateNumber(file, copy.length);
            evaluateNumber(file, copy.origin);
            file._regions.emplace(it.name, ILinkerFile::Region{
                                               std::move(copy),
                                               {}
                                           });
            file.log() << "[read] Processed memory region: \"" << it.name << "\"" << Qt::endl;
        }
        file.log() << "[read] Memory regions read and processed." << Qt::endl;
    }

    // Step 5: Read and process sections
    {
        LinkerSection ld_section{};
        ld_section.read(descr, ld);
        file._sections.reserve(ld_section.data().size());
        file.log() << "[read] Reserved space for " << ld_section.data().size() << " sections." << Qt::endl;

        // first iteration build Qhash data
        file.log() << "[read] Build sections -------------" << Qt::endl;
        for (const auto& it : ld_section.data()) {
            // Read subsections
            ILinkerFile::SubSections sub{};
            LinkerSubSectionReader::read(descr, sub, it.body);
            LinkerSubSectionReader::append(descr, sub, it.name);
            file.log() << "[read] Subsections read for section: \"" << it.name << "\"" << Qt::endl;

            // Read variables
            ILinkerFile::Variables var{};
            LinkerVariableReader::read(descr, var, it.body);
            file.log() << "[read] Variables read for section: \"" << it.name << "\"" << Qt::endl;

            file._sections.emplace(it.name, ILinkerFile::Section{
                                                it.name,
                                                std::move(sub),
                                                std::move(var),
                                                {},
                                                {}
                                            });
        }

        file.log() << "[read] Bind sections -------------" << Qt::endl;
        for (const auto& it : ld_section.data()) {
            sectionBind(file, file._sections[it.name], it.mem_region, it.attribute);
            file.log() << "[read] Section \"" << it.name << "\" processed and bound." << Qt::endl;
        }
        file.log() << "[read] Sections read and processed." << Qt::endl;
    }

    // Step 6: Check region adresses in memory
    {
        for (auto& addrIt : file._regions) {
            bool originOk = false;
            bool lengthOk = false;
            const quint64 origin = addrIt.region.origin.toULongLong(&originOk, 16);
            const quint64 length = addrIt.region.length.toULongLong(&lengthOk, 16);

            if (!originOk || !lengthOk) {
                file.log() << "[Check memory] Invalid address format in region: \"" << addrIt.region.name
                           << "\" (origin: \"" << addrIt.region.origin
                           << "\", length: \"" << addrIt.region.length << "\")" << Qt::endl;

                addrIt.region.isEvaluated = false;
                continue;
            }

            if (origin + length < origin) { // Check for overflow
                file.log() << "[Check memory] Region address overflow in: \"" << addrIt.region.name
                           << "\" (origin: 0x" << QString::number(origin, 16)
                           << ", length: 0x" << QString::number(length, 16) << ")" << Qt::endl;

                addrIt.region.isEvaluated = false;
                continue;
            }

            addrIt.region.isEvaluated = true;
        }
    }

    file.setType(ILinkerFile::LinkerType::GNU);
    return !file._sections.isEmpty() ||
           !file._regions.isEmpty() ||
           !file._globals.isEmpty();
}



void LinkerFileReader::sectionBind(ILinkerFile& file, ILinkerFile::Section& sec, const QString& mem_region, const QString& attribute)
{
    // Check if regular expressions are valid
    if (!descr.atRe.isValid()) {
        file.log() << "[sectionBind] ERROR: Invalid regular expression 'atRe' in LinkerDescriptor" << Qt::endl;
        return;
    }
    if (!descr.tokenRe.isValid()) {
        file.log() << "[sectionBind] ERROR: Invalid regular expression 'tokenRe' in LinkerDescriptor" << Qt::endl;
        return;
    }
    file.log() << "[sectionBind] Starting binding for section: \"" << sec.name << "\"" << Qt::endl;

    // Helper function to append section to unknown region
    const auto append_unknown_to_region = [&]() {
        auto it = file._regions.find(descr.unknown);
        if (it == file._regions.end()) {
            file._regions.emplace(descr.unknown, ILinkerFile::Region {
                                                     {descr.unknown, {}, {}, {}, false},
                                                     {}
                                                 });
            file.log() << "[sectionBind] Created unknown region: \"" << descr.unknown << "\"" << Qt::endl;
        }
        file._regions[descr.unknown].sections.emplace_back(std::make_pair(ILinkerFile::None, &sec));
        file.log() << "[sectionBind] Section \"" << sec.name << "\" added to unknown region." << Qt::endl;
    };

    // Helper function to process tokens and assign regions
    const auto processTokens = [&](const QString& txt) -> bool {

        // skip empty token
        if(txt.size() == 1 && txt[0] == ':') {
            return false;
        }

        // need update ----------------------------------------------
        QStringList tokens;
        //QStringList tokens = txt.split(':', Qt::SkipEmptyParts); //-------------------------------- need add handling
        // QStringList tokens = txt.split(' ', Qt::SkipEmptyParts);
        if(txt.contains(':')) {
            tokens = txt.split(':', Qt::SkipEmptyParts);
        } else {
            tokens = txt.split(' ', Qt::SkipEmptyParts);
        }
        // need update ----------------------------------------------


        bool isLMA = false;
        bool segregated = false;

        file.log() << "[sectionBind] Processing tokens from: \"" << txt << "\"" << Qt::endl;
        for (auto& token : tokens) {
            token = token.trimmed();
            if (token.isEmpty()) {
                file.log() << "[sectionBind] Empty token in: \"" << txt << "\" for section: \"" << sec.name << "\"" << Qt::endl;
                continue;
            }

            // Check for "AT(" or "AT>" and remove prefix
            if (descr.atRe.match(token).hasMatch()) {
                isLMA = true;
                file.log() << "[sectionBind] Detected LMA token: \"" << token << "\"" << Qt::endl;
                token = token.mid(3);
            }

            // Extract clean token using
            QString cleanToken = token.remove('<').remove('>').remove('|').remove(':')
                                     .remove('(').remove(')').trimmed();
            if (cleanToken.isEmpty()) {
                //file.log() << "[sectionBind] Empty token after cleaning in: \"" << txt << "\" for section: \"" << sec.name << "\"" << Qt::endl;
                continue;
            }

            // Search for region by name or address
            ILinkerFile::Region* targetRegion = nullptr;
            auto nameIt = file._regions.find(cleanToken);
            if (nameIt != file._regions.end()) {
                targetRegion = &(nameIt.value());
                file.log() << "[sectionBind] Found region by name: \"" << cleanToken << "\"" << Qt::endl;
            } else {
                evaluateNumber(file, cleanToken);
                bool isAddress = false;
                const quint64 address = cleanToken.toULongLong(&isAddress, 16);
                if (isAddress) {
                    for (auto& addrIt : file._regions) {
                        bool originOk = false;
                        bool lengthOk = false;
                        const quint64 origin = addrIt.region.origin.toULongLong(&originOk, 16);
                        const quint64 length = addrIt.region.length.toULongLong(&lengthOk, 16);

                        if (!originOk || !lengthOk) {
                            // file.log() << "[sectionBind] Invalid address format in region: \"" << addrIt.region.name
                            //       << "\" (origin: \"" << addrIt.region.origin
                            //       << "\", length: \"" << addrIt.region.length << "\")" << Qt::endl;
                            continue;
                        }

                        if (origin + length < origin) { // Check for overflow
                            file.log() << "[sectionBind] Region address overflow in: \"" << addrIt.region.name
                                       << "\" (origin: 0x" << QString::number(origin, 16)
                                       << ", length: 0x" << QString::number(length, 16) << ")" << Qt::endl;
                            continue;
                        }

                        const quint64 _begin = origin;
                        const quint64 _end = origin + length;
                        if (!(address < _begin || address >= _end)) {
                            targetRegion = &(addrIt);
                            file.log() << "[sectionBind] Found region by address: \"" << addrIt.region.name << "\"" << Qt::endl;
                            break;
                        }
                    }
                }
            }

            if (targetRegion) {

                segregated = true;
                targetRegion->sections.emplace_back(std::make_pair(isLMA ? ILinkerFile::LMA : ILinkerFile::VMA, &sec));

                if (isLMA) {

                    if (sec.lma.isEmpty()) {
                        file.log() << "[sectionBind] Assigned LMA to section: \"" << sec.name << "\" (region: \"" << targetRegion->region.name << "\")" << Qt::endl;
                    } else if (sec.lma[0] != &(targetRegion->region)) {
                        file.log() << "[sectionBind] LMA conflict for section: \"" << sec.name
                                   << "\" (existing: \"" << sec.lma[0]->name
                                   << "\", attempted: \"" << targetRegion->region.name << "\") in: \"" << txt << "\"" << Qt::endl;
                    }
                    sec.lma.emplace_back(&(targetRegion->region));
                    isLMA = false;
                } else {
                    if (sec.vma.empty()) {
                        file.log() << "[sectionBind] Assigned VMA to section: \"" << sec.name << "\" (region: \"" << targetRegion->region.name << "\")" << Qt::endl;
                    } else if (sec.vma[0] != &(targetRegion->region)) {
                        file.log() << "[sectionBind] VMA conflict for section: \"" << sec.name
                                   << "\" (existing: \"" << sec.vma[0]->name
                                   << "\", attempted: \"" << targetRegion->region.name << "\") in: \"" << txt << "\"" << Qt::endl;
                    }
                    sec.vma.emplace_back(&(targetRegion->region));
                }
            } else {
                file.log() << "[sectionBind] Unknown expression: \"" << cleanToken
                           << "\" in token: \"" << token << "\" for section: \"" << sec.name << "\"" << Qt::endl;
            }
        }
        return segregated;
    };

    bool segregated1 = false;
    bool segregated2 = false;

    // Process attribute
    if (!attribute.isEmpty()) {
        segregated1 = processTokens(attribute);
    }

    // Process mem_region
    if (!mem_region.isEmpty()) {
        segregated2 = processTokens(mem_region);
    }

    // If no region is found, append to unknown
    if (!segregated1 && !segregated2) {
        file.log() << "[sectionBind] No region found for section: \"" << sec.name
                   << "\" (attribute1: \"" << attribute << "\", attribute2: \"" << mem_region << "\")" << Qt::endl;
        append_unknown_to_region();
    }
    file.log() << "[sectionBind] Completed binding for section: \"" << sec.name << "\"" << Qt::endl;
}


void LinkerFileReader::evaluateGlobals(ILinkerFile& file)
{
    file.log() << "[evaluateGlobals] Starting evaluation of global variables." << Qt::endl;
    for (auto& var : file._globals) {
        if (var.lvalue.isEmpty()) {
            file.log() << "[evaluateGlobals] Empty lvalue for rvalue: \"" << var.rvalue << "\"" << Qt::endl;
            continue;
        }

        file.log() << "[evaluateGlobals] Processing variable: \"" << var.lvalue << "\" with rvalue: \"" << var.rvalue << "\"" << Qt::endl;

        // Try to convert rvalue to number
        bool ok = false;
        quint64 value = var.rvalue.toULongLong(&ok, 16);
        if (ok) {
            file.engine.globalObject().setProperty(var.lvalue, QJSValue(static_cast<double>(value)));
            file.log() << "[evaluateGlobals] Set global variable: \"" << var.lvalue << "\" = " << value << Qt::endl;
            var.isEvaluated = true;
        } else {
            // Evaluate expression
            QJSValue result = file.engine.evaluate(var.rvalue);
            if (result.isError()) {
                file.log() << "[evaluateGlobals] Error in expression: \"" << var.rvalue << "\" [JS]: " << result.toString() << Qt::endl;
                var.isEvaluated = false;
                continue;
            }
            if (result.isNumber()) {
                var.rvalue = "0x" + QString::number(static_cast<quint64>(result.toNumber()), 16).toUpper();
                file.engine.globalObject().setProperty(var.lvalue, result);
                var.isEvaluated = true;
                file.log() << "[evaluateGlobals] Evaluated and set global variable: \"" << var.lvalue << "\" = " << var.rvalue << Qt::endl;
            } else {
                var.isEvaluated = false;
                file.log() << "[evaluateGlobals] Non-numeric result for: \"" << var.lvalue << "\" with expression: \"" << var.rvalue << "\"" << Qt::endl;
            }
        }
    }
    file.log() << "[evaluateGlobals] Completed evaluation of global variables." << Qt::endl;
    (void)descr;
}

/**
 * @brief Evaluates a numeric expression with units (e.g., K, M).
 *
 * This function processes the expression by replacing unit-multiplied
 * numbers and then evaluates the final expression using QJSEngine.
 *
 * @param descr LinkerDescriptor object for logging.
 * @param expr The expression to evaluate.
 */
void LinkerFileReader::evaluateNumber(ILinkerFile& file, QString& expr)
{
    file.log() << "[evaluateNumber] Starting evaluation of expression: \"" << expr << "\"" << Qt::endl;

    auto applyReplacements = [&](const QRegularExpression &re, bool isHex) {
        int i = 0;
        while (i < descr.evaluateCnt) {
            const auto m = re.match(expr);
            if (!m.hasMatch()) {
                break;
            }
            bool ok = false;
            quint64 val = m.captured(1).toULongLong(&ok, isHex ? 16 : 10);
            const QChar unit = m.captured(2).isEmpty() ? QChar('\0') : m.captured(2).front().toUpper();
            ++i;
            if (!ok) {
                file.log() << "[evaluateNumber] Invalid number format: \"" << m.captured() << "\" in expression: \"" << expr << "\"" << Qt::endl;
                expr.replace(m.capturedStart(), m.capturedLength(), QString(" @%1 ").arg(i));
                continue;
            }
            val *= descr.units.value(unit, 1ULL);
            QString replacement = QString::number(val);
            expr.replace(m.capturedStart(), m.capturedLength(), replacement);
            file.log() << "[evaluateNumber] Replaced \"" << m.captured() << "\" with \"" << replacement << "\"" << Qt::endl;
        }
    };

    // Apply replacements for hex and decimal numbers
    applyReplacements(descr.hexRe, true);
    applyReplacements(descr.decRe, false);

    // Evaluate the final expression using QJSEngine
    QJSValue result = file.engine.evaluate(expr);
    if (result.isError() || !result.isNumber()) {
        file.log() << "[evaluateNumber] Calculation error in expression: \"" << expr << "\" [JS]: " << result.toString() << Qt::endl;
        return;
    }
    const quint64 finalVal = static_cast<quint64>(result.toNumber());
    expr = QString("0x%1").arg(finalVal, 0, 16);
    file.log() << "[evaluateNumber] Evaluated expression to: \"" << expr << "\"" << Qt::endl;
}

} //namespace compiler_tools::gnu
