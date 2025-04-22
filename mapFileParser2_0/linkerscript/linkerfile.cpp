#include "linkerfile.h"
#include "linkersection.h"

void LinkerFile::parse(const LinkerDescriptor& descr, QString& script)
{
    LinkerRaw ld{};
    LinkerSection ld_section{};

    // remove comments
    descr.remove_unnecessary(script);
    // raw read
    ld.read(descr, script);
    // sections read
    ld_section.read(descr, ld);
    // sub-sections read
    for (auto it = ld_section.data().begin(); it != ld_section.data().end(); ++it) {
        // read subnames
        LinkerSubSection sub{};
        sub.read(descr, it.value().body);
        sub.append(descr, it.key());

        // read variables
        LinkerVariable var;
        var.read(descr, it.value().body);

        Section data{
            it.key(),
            std::move(sub),
            std::move(var),
            it.value().mem_region,
            nullptr,
            nullptr
        };

        _sections.insert(it.key(), std::move(data));
    }

    // memory read
    LinkerMemory ld_mem;
    ld_mem.read(descr, ld);
    for (auto it = ld_mem.data().begin(); it != ld_mem.data().end(); ++it) {
        Region reg {
            it.value(),
            {}
        };

        _regions.insert(it.key(), std::move(reg));
    }


    LinkerVariable global_var{};
    global_var.read(descr, ld.data().value(descr.globalName));
    _globals = std::move(global_var.data());
    evaluate(descr);
    return;
}

void LinkerFile::evaluate(const LinkerDescriptor& descr)
{
    for (auto& sec : _sections) {
        // Пропускаємо секції без визначення пам'яті
        if (sec.region.isEmpty()) {
            auto it = _regions.find(descr.noName);
            if (it == _regions.end()) {
                _regions.insert(descr.noName, {});
                _regions.value(descr.noName).sections.append(&sec);
            } else {
                it->sections.append(&sec);
            }
            continue;
        }

        // Розбиваємо рядок пам'яті на токени, ігноруючи порожні
        QStringList regions = sec.region.split(' ', Qt::SkipEmptyParts);
        bool isLMA = false; // Прапорець для визначення LMA
        bool segregated = false;

        for (auto& regionToken : regions) {
            // Обробка LMA (Load Memory Address)
            if (regionToken.startsWith("AT>")) {
                isLMA = true;
                regionToken = regionToken.mid(3); // Вирізаємо "AT>"
                continue; // Переходимо до наступного токена
            }

            // Видаляємо спецсимволи (наприклад, '>' у ">REGION")
            regionToken = regionToken.replace(">", "").trimmed();

            // Шукаємо регіон у мапі
            auto it = _regions.find(regionToken);
            if (it != _regions.end()) {
                it->sections.append(&sec);
                segregated = true;
                if (isLMA) {
                    sec.lma = &(it->region); // Встановлюємо LMA
                    isLMA = false; // Скидаємо прапорець
                } else {
                    sec.vma = &(it->region); // Встановлюємо VMA
                }
            }
        }

        if(!segregated) {
            _regions.value(descr.noName).sections.append(&sec);
        }
    }
}
