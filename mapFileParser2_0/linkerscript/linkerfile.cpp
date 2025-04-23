#include "linkerfile.h"
#include "linkersection.h"


void LinkerFile::read(LinkerDescriptor& descr, QString& script)
{
    LinkerRaw ld{};

    // read all
    {
        // remove comments
        descr.remove_unnecessary(script);
        // raw read
        ld.read(descr, script);
    }

    // read global variables
    {
        _globals.read(descr, ld.data().value(descr.globalName));
        evaluateGlobals(descr);
    }

    // memory read
    {
        LinkerMemory ld_mem;
        ld_mem.read(descr, ld);
        _regions.reserve(ld_mem.data().size());
        for (const auto& it : ld_mem.data()) {
            auto copy = it;
            evaluateNumber(descr, copy.length);
            evaluateNumber(descr, copy.origin);
            Region reg {
                std::move(copy),
                {}
            };

            _regions.emplace(it.name, std::move(reg));
        }
    }

    // sections read
    {
        LinkerSection ld_section{};
        ld_section.read(descr, ld);
        _sections.reserve(ld_section.data().size());

        // sub-sections read
        for (const auto& it : ld_section.data()) {
            // read subnames
            LinkerSubSection sub{};
            sub.read(descr, it.body);
            sub.append(descr, it.name);

            // read variables
            LinkerVariable var;
            var.read(descr, it.body);
            Section data {
                it.name,
                std::move(sub),
                std::move(var),
                nullptr,
                nullptr
            };

            _sections.emplace(it.name, std::move(data));
            sectionBind(descr, _sections[it.name], it.mem_region, it.attribute);
        }
    }
}

void LinkerFile::sectionBind(LinkerDescriptor& descr,
                             Section& sec, const QString& mem_region, const QString& attribute)
{
    auto append_unknown_to_region = [&](const Section& sec){
        auto it = _regions.find(descr.unknown);
        if (it == _regions.end()) {
            Region reg {
                {descr.unknown, {}, {}, {}},
                {}
            };
            _regions.emplace(descr.unknown, std::move(reg));
            _regions[descr.unknown].sections.emplace_back(&sec);
        } else {
            it->sections.emplace_back(&sec);
        }
    };

    // Skip sections without defining memory
    if (mem_region.isEmpty() && attribute.isEmpty()) {
        descr << "[bind] Unknown Region:  " << sec.name << Qt::endl;
        append_unknown_to_region(sec);
        return;
    }

    bool segregated = false;

    if(!mem_region.isEmpty()) {
        // Split the memory string into tokens, ignoring empty ones
        QStringList regions = mem_region.split(' ', Qt::SkipEmptyParts);
        bool isLMA = false; // Flag for defining LMA

        for (auto& regionToken : regions) {
            // LMA processing (Load Memory Address)
            if (regionToken.startsWith("AT>")) {
                isLMA = true;
                regionToken = regionToken.mid(3); // Cut out "AT>"
                continue; // Go to the next token
            }

            // Remove special characters (e.g., '>' in ">REGION")
            regionToken = regionToken.replace(">", "").trimmed();

            // Find a region in the map
            auto it = _regions.find(regionToken);
            if (it != _regions.end()) {
                it->sections.emplace_back(&sec);
                segregated = true;
                if (isLMA) {
                    sec.lma = &(it->region); // Set LMA
                    isLMA = false; // Uncheck
                } else {
                    sec.vma = &(it->region); // Set VMA
                }
            }
        }
    }


    if (!attribute.isEmpty()) {
        // todo realize this function
    }

    if(!segregated) {
        descr << "Not find region for section: "<< sec.name << " attribute1: " << mem_region <<" attribute2: " << attribute << Qt::endl;
        append_unknown_to_region(sec);
    }
}

void LinkerFile::evaluateGlobals(LinkerDescriptor& descr)
{
    // Handling global variables
    for (auto& var : _globals.data()) {
        if (var.lvalue.isEmpty()) {
            descr << "[evaluateGlobals] Empty lvalue:" << var.lvalue << "rvalue: " << var.rvalue << Qt::endl;
            continue;
        }

        // Attempting to convert rvalue to number
        bool ok = false;
        quint64 value = var.rvalue.toULongLong(&ok, 16);
        if (ok) {
            engine.globalObject().setProperty(var.lvalue, QJSValue(static_cast<double>(value)));
        } else {
            // Evaluating expression
            QJSValue result = engine.evaluate(var.rvalue);
            if (result.isError()) {
                descr << "[evaluateGlobals] Error in expression " << var.rvalue << " [JS]: " << result.toString() << Qt::endl;
                continue;
            }
            if (result.isNumber()) {
                var.rvalue = "0x" + QString::number(static_cast<quint64>(result.toNumber()), 16).toUpper();
                engine.globalObject().setProperty(var.lvalue, result);
            } else {
                descr << "[evaluateGlobals] Non-numeric result for " << var.lvalue << " : " << var.rvalue << Qt::endl;
            }
        }
    }
}

void LinkerFile::evaluateNumber(LinkerDescriptor& descr, QString& expr)
{
    auto applyReplacements = [&](const QRegularExpression &re, bool isHex){
        int i = 0;
        while (i < descr.evaluateCnt) {
            QRegularExpressionMatch m = re.match(expr);

            if (!m.hasMatch()) {
                break;
            }

            bool ok = false;
            quint64 val = m.captured(1).toULongLong(&ok, isHex ? 16 : 10);
            const QChar unit = m.captured(2).isEmpty() ? QChar('\0') : m.captured(2).front().toUpper();;
            ++i;
            if (!ok) {
                descr << "[WARN] Invalid number format: " << m.captured() << Qt::endl;
                expr.replace(m.capturedStart(), m.capturedLength(), QString(" @%1 ").arg(i));
                continue;
            }

            // застосувати множник
            val *= descr.units.value(unit, 1ULL);

            // текст заміни — десяткове число (для JS) або можна в hex
            QString replacement = QString::number(val);

            // compute replacement…
            expr.replace(m.capturedStart(), m.capturedLength(), replacement);
        }
    };

    // Виконати три проходи
    applyReplacements(descr.hexRe, true);
    applyReplacements(descr.decRe, false);

    // Оцінити остаточний вираз через JS
    QJSValue result = engine.evaluate(expr);                                  //
    if (result.isError() || !result.isNumber()) {
        descr << "[evaluateNumber] Calc error: " << expr << " [JS]: " << result.toString() << Qt::endl;
        return;
    }

    const quint64 finalVal = static_cast<quint64>(result.toNumber());
    expr = QString("0x%1").arg(finalVal, 0, 16);
}
