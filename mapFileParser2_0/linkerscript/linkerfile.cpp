#include "linkerfile.h"
#include "linkersection.h"

#if QT_VERSION < QT_VERSION_CHECK(6,9,0)
#include <QJSValueIterator>
#endif

void LinkerFile::InstallDescriptor(LinkerDescriptor * const descr)
{
    if(LinkerDescriptor::validate(descr)) {
        _descr = descr;
    }
}


void LinkerFile::clear()
{
    _sections.clear();
    _regions.clear();
    _globals.clear();

    // Варіант B: ітерувати та видалити властивості
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

/**
 * @brief Reads and processes the linker script.
 *
 * This function performs the following steps:
 * 1. Removes unnecessary parts (comments) from the script.
 * 2. Reads raw data from the script.
 * 3. Reads and evaluates global variables.
 * 4. Reads and processes memory regions.
 * 5. Reads and processes sections, including subsections and variables.
 *
 * @param descr LinkerDescriptor object for configuration and logging.
 * @param script The linker script as a QString.
 */

void LinkerFile::read(QString& script)
{
    if(_descr == nullptr) {
        return;
    }

    LinkerRaw ld{};
    LinkerDescriptor& descr = *_descr;

    // Step 1: Remove comments and unnecessary parts
    descr.remove_unnecessary(script);
    descr << "[read] Comments and unnecessary parts removed from script." << Qt::endl;

    // Step 2: Read raw data from the script
    ld.read(descr, script);
    descr << "[read] Raw data read from script." << Qt::endl;

    // Step 3: Read and evaluate global variables
    {
        auto it = ld.data().find(descr.globalName);
        if (it != ld.data().end()) {
            _globals.read(descr, it.value());
            evaluateGlobals(descr);
            descr << "[read] Global variables read and evaluated." << Qt::endl;
        } else {
            descr << "[read] No global variables found in script." << Qt::endl;
        }
    }

    // Step 4: Read and process memory regions
    {
        LinkerMemory ld_mem;
        ld_mem.read(descr, ld);
        _regions.reserve(ld_mem.data().size());
        descr << "[read] Reserved space for " << ld_mem.data().size() << " memory regions." << Qt::endl;
        for (const auto& it : ld_mem.data()) {
            auto copy = it;
            evaluateNumber(descr, copy.length);
            evaluateNumber(descr, copy.origin);
            _regions.emplace(it.name, Region{
                                          std::move(copy),
                                          {}
                                      });
            descr << "[read] Processed memory region: \"" << it.name << "\"" << Qt::endl;
        }
        descr << "[read] Memory regions read and processed." << Qt::endl;
    }

    // Step 5: Read and process sections
    {
        LinkerSection ld_section{};
        ld_section.read(descr, ld);
        _sections.reserve(ld_section.data().size());
        descr << "[read] Reserved space for " << ld_section.data().size() << " sections." << Qt::endl;

        for (const auto& it : ld_section.data()) {
            // Read subsections
            LinkerSubSection sub{};
            sub.read(descr, it.body);
            sub.append(descr, it.name);
            descr << "[read] Subsections read for section: \"" << it.name << "\"" << Qt::endl;

            // Read variables
            LinkerVariable var;
            var.read(descr, it.body);
            descr << "[read] Variables read for section: \"" << it.name << "\"" << Qt::endl;

            _sections.emplace(it.name, Section{
                                           it.name,
                                           std::move(sub),
                                           std::move(var),
                                           nullptr,
                                           nullptr
                                       });
            sectionBind(descr, _sections[it.name], it.mem_region, it.attribute);
            descr << "[read] Section \"" << it.name << "\" processed and bound." << Qt::endl;
        }
        descr << "[read] Sections read and processed." << Qt::endl;
    }

    // Step 6: Check region adresses in memory
    {
        for (auto& addrIt : _regions) {
            bool originOk = false;
            bool lengthOk = false;
            const quint64 origin = addrIt.region.origin.toULongLong(&originOk, 16);
            const quint64 length = addrIt.region.length.toULongLong(&lengthOk, 16);

            if (!originOk || !lengthOk) {
                descr << "[Check memory] Invalid address format in region: \"" << addrIt.region.name
                      << "\" (origin: \"" << addrIt.region.origin
                      << "\", length: \"" << addrIt.region.length << "\")" << Qt::endl;

                addrIt.region.evaluated = false;
                continue;
            }

            if (origin + length < origin) { // Check for overflow
                descr << "[Check memory] Region address overflow in: \"" << addrIt.region.name
                      << "\" (origin: 0x" << QString::number(origin, 16)
                      << ", length: 0x" << QString::number(length, 16) << ")" << Qt::endl;

                addrIt.region.evaluated = false;
                continue;
            }

            addrIt.region.evaluated = true;
        }
    }
}


/**
 * @brief Binds a section to memory regions based on mem_region and attribute.
 *
 * This function processes the mem_region and attribute strings to assign
 * the section to the appropriate memory regions. It handles both name-based
 * and address-based region lookups.
 *
 * @param descr LinkerDescriptor object for configuration and logging.
 * @param sec The section to bind.
 * @param mem_region The mem_region string from the section.
 * @param attribute The attribute string from the section.
 */
void LinkerFile::sectionBind(LinkerDescriptor& descr,
                             Section& sec, const QString& mem_region, const QString& attribute)
{
    // Check if regular expressions are valid
    if (!descr.atRe.isValid()) {
        descr << "[sectionBind] ERROR: Invalid regular expression 'atRe' in LinkerDescriptor" << Qt::endl;
        return;
    }
    if (!descr.tokenRe.isValid()) {
        descr << "[sectionBind] ERROR: Invalid regular expression 'tokenRe' in LinkerDescriptor" << Qt::endl;
        return;
    }
    descr << "[sectionBind] Starting binding for section: \"" << sec.name << "\"" << Qt::endl;

    // Helper function to append section to unknown region
    const auto append_unknown_to_region = [&]() {
        auto it = _regions.find(descr.unknown);
        if (it == _regions.end()) {
            _regions.emplace(descr.unknown, Region {
                                                {descr.unknown, {}, {}, {}, false},
                                                {}
                                            });
            descr << "[sectionBind] Created unknown region: \"" << descr.unknown << "\"" << Qt::endl;
        }
        _regions[descr.unknown].sections.emplace_back(&sec);
        descr << "[sectionBind] Section \"" << sec.name << "\" added to unknown region." << Qt::endl;
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

        descr << "[sectionBind] Processing tokens from: \"" << txt << "\"" << Qt::endl;
        for (auto& token : tokens) {
            token = token.trimmed();
            if (token.isEmpty()) {
                descr << "[sectionBind] Empty token in: \"" << txt << "\" for section: \"" << sec.name << "\"" << Qt::endl;
                continue;
            }

            // Check for "AT(" or "AT>" and remove prefix
            if (descr.atRe.match(token).hasMatch()) {
                isLMA = true;
                descr << "[sectionBind] Detected LMA token: \"" << token << "\"" << Qt::endl;
                token = token.mid(3);
            }

            // Extract clean token using
            QString cleanToken = token.remove('<').remove('>').remove('|').remove(':')
                                     .remove('(').remove(')').trimmed();
            if (cleanToken.isEmpty()) {
                //descr << "[sectionBind] Empty token after cleaning in: \"" << txt << "\" for section: \"" << sec.name << "\"" << Qt::endl;
                continue;
            }

            // Search for region by name or address
            Region* targetRegion = nullptr;
            auto nameIt = _regions.find(cleanToken);
            if (nameIt != _regions.end()) {
                targetRegion = &(nameIt.value());
                descr << "[sectionBind] Found region by name: \"" << cleanToken << "\"" << Qt::endl;
            } else {
                evaluateNumber(descr, cleanToken);
                bool isAddress = false;
                const quint64 address = cleanToken.toULongLong(&isAddress, 16);
                if (isAddress) {
                    for (auto& addrIt : _regions) {
                        bool originOk = false;
                        bool lengthOk = false;
                        const quint64 origin = addrIt.region.origin.toULongLong(&originOk, 16);
                        const quint64 length = addrIt.region.length.toULongLong(&lengthOk, 16);

                        if (!originOk || !lengthOk) {
                            // descr << "[sectionBind] Invalid address format in region: \"" << addrIt.region.name
                            //       << "\" (origin: \"" << addrIt.region.origin
                            //       << "\", length: \"" << addrIt.region.length << "\")" << Qt::endl;
                            continue;
                        }

                        if (origin + length < origin) { // Check for overflow
                            descr << "[sectionBind] Region address overflow in: \"" << addrIt.region.name
                                  << "\" (origin: 0x" << QString::number(origin, 16)
                                  << ", length: 0x" << QString::number(length, 16) << ")" << Qt::endl;
                            continue;
                        }

                        const quint64 _begin = origin;
                        const quint64 _end = origin + length;
                        if (!(address < _begin || address >= _end)) {
                            targetRegion = &(addrIt);
                            descr << "[sectionBind] Found region by address: \"" << addrIt.region.name << "\"" << Qt::endl;
                            break;
                        }
                    }
                }
            }

            if (targetRegion) {

                segregated = true;
                targetRegion->sections.emplace_back(&sec);

                if (isLMA) {
                    if (!sec.lma) {
                        sec.lma = &(targetRegion->region);
                        descr << "[sectionBind] Assigned LMA to section: \"" << sec.name << "\" (region: \"" << targetRegion->region.name << "\")" << Qt::endl;
                    } else if (sec.lma != &(targetRegion->region)) {
                        descr << "[sectionBind] LMA conflict for section: \"" << sec.name
                              << "\" (existing: \"" << sec.lma->name
                              << "\", attempted: \"" << targetRegion->region.name << "\") in: \"" << txt << "\"" << Qt::endl;
                    }
                    isLMA = false;
                } else {
                    if (!sec.vma) {
                        sec.vma = &(targetRegion->region);
                        descr << "[sectionBind] Assigned VMA to section: \"" << sec.name << "\" (region: \"" << targetRegion->region.name << "\")" << Qt::endl;
                    } else if (sec.vma != &(targetRegion->region)) {
                        descr << "[sectionBind] VMA conflict for section: \"" << sec.name
                              << "\" (existing: \"" << sec.vma->name
                              << "\", attempted: \"" << targetRegion->region.name << "\") in: \"" << txt << "\"" << Qt::endl;
                    }
                }
            } else {
                descr << "[sectionBind] Unknown expression: \"" << cleanToken
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
        descr << "[sectionBind] No region found for section: \"" << sec.name
              << "\" (attribute1: \"" << attribute << "\", attribute2: \"" << mem_region << "\")" << Qt::endl;
        append_unknown_to_region();
    }
    descr << "[sectionBind] Completed binding for section: \"" << sec.name << "\"" << Qt::endl;
}

/**
 * @brief Evaluates global variables and sets them in the QJSEngine.
 *
 * This function processes each global variable, attempting to convert
 * its rvalue to a number or evaluate it as a JavaScript expression.
 *
 * @param descr LinkerDescriptor object for logging.
 */
void LinkerFile::evaluateGlobals(LinkerDescriptor& descr)
{
    descr << "[evaluateGlobals] Starting evaluation of global variables." << Qt::endl;
    for (auto& var : _globals.data()) {
        if (var.lvalue.isEmpty()) {
            descr << "[evaluateGlobals] Empty lvalue for rvalue: \"" << var.rvalue << "\"" << Qt::endl;
            continue;
        }

        descr << "[evaluateGlobals] Processing variable: \"" << var.lvalue << "\" with rvalue: \"" << var.rvalue << "\"" << Qt::endl;

        // Try to convert rvalue to number
        bool ok = false;
        quint64 value = var.rvalue.toULongLong(&ok, 16);
        if (ok) {
            engine.globalObject().setProperty(var.lvalue, QJSValue(static_cast<double>(value)));
            descr << "[evaluateGlobals] Set global variable: \"" << var.lvalue << "\" = " << value << Qt::endl;
            var.evaluated = true;
        } else {
            // Evaluate expression
            QJSValue result = engine.evaluate(var.rvalue);
            if (result.isError()) {
                descr << "[evaluateGlobals] Error in expression: \"" << var.rvalue << "\" [JS]: " << result.toString() << Qt::endl;
                var.evaluated = false;
                continue;
            }
            if (result.isNumber()) {
                var.rvalue = "0x" + QString::number(static_cast<quint64>(result.toNumber()), 16).toUpper();
                engine.globalObject().setProperty(var.lvalue, result);
                var.evaluated = true;
                descr << "[evaluateGlobals] Evaluated and set global variable: \"" << var.lvalue << "\" = " << var.rvalue << Qt::endl;
            } else {
                var.evaluated = false;
                descr << "[evaluateGlobals] Non-numeric result for: \"" << var.lvalue << "\" with expression: \"" << var.rvalue << "\"" << Qt::endl;
            }
        }
    }
    descr << "[evaluateGlobals] Completed evaluation of global variables." << Qt::endl;
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
void LinkerFile::evaluateNumber(LinkerDescriptor& descr, QString& expr)
{
    descr << "[evaluateNumber] Starting evaluation of expression: \"" << expr << "\"" << Qt::endl;

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
                descr << "[evaluateNumber] Invalid number format: \"" << m.captured() << "\" in expression: \"" << expr << "\"" << Qt::endl;
                expr.replace(m.capturedStart(), m.capturedLength(), QString(" @%1 ").arg(i));
                continue;
            }
            val *= descr.units.value(unit, 1ULL);
            QString replacement = QString::number(val);
            expr.replace(m.capturedStart(), m.capturedLength(), replacement);
            descr << "[evaluateNumber] Replaced \"" << m.captured() << "\" with \"" << replacement << "\"" << Qt::endl;
        }
    };

    // Apply replacements for hex and decimal numbers
    applyReplacements(descr.hexRe, true);
    applyReplacements(descr.decRe, false);

    // Evaluate the final expression using QJSEngine
    QJSValue result = engine.evaluate(expr);
    if (result.isError() || !result.isNumber()) {
        descr << "[evaluateNumber] Calculation error in expression: \"" << expr << "\" [JS]: " << result.toString() << Qt::endl;
        return;
    }
    const quint64 finalVal = static_cast<quint64>(result.toNumber());
    expr = QString("0x%1").arg(finalVal, 0, 16);
    descr << "[evaluateNumber] Evaluated expression to: \"" << expr << "\"" << Qt::endl;
}
