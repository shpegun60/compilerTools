#include "mapsymbol.h"

namespace compiler_tools::gnu {

bool MapSymbol::read(const MapDescriptor& descr, const MapRaw& mapraw)
{
    if(mapraw.data().size() == 0) {
        return false;
    }
    _sections.reserve(mapraw.data().size());
    _names.reserve(mapraw.data().size());

    Symbol currentSymbol {};
    QString symbolName {};
    quint64 key = 0;

    // ------------------------------------------------------
    for(auto& raw : mapraw.data()) {
        Section section{};
        section.name = raw.name;
        section.addresses.reserve(4);
        section.fills.reserve(4);

        // push names to QSet
        {
            QString nameCopy = raw.name;
            QStringList names1 = nameCopy.split(' ', Qt::SkipEmptyParts);
            nameCopy.remove('*');
            QStringList names2 = nameCopy.split(' ', Qt::SkipEmptyParts);
            _names.insert(std::move(nameCopy));
            _names.insert(raw.name);

            for (int i = 0; i < names1.size(); ++i) {
                _names.insert(names1[i].trimmed());
            }

            for (int i = 0; i < names2.size(); ++i) {
                _names.insert(names2[i].trimmed());
            }
        }

        for(auto& line : raw.lines) {
            if (line.isEmpty()) {
                continue;
            }

            // Split the string by whitespace characters (remove empty elements)
            const QStringList tokens = line.split(descr.tokenSplitter, Qt::SkipEmptyParts);

            if (tokens.isEmpty()) {
                continue;
            }

            // proceed fills -----------------------------
            {
                const auto fsize = descr.readFillSize(line);
                if(fsize.first) {

                    const auto faddr = descr.readLineAddress(line);
                    if (faddr.first == "-1") {
                        _ignored.append(std::move(line));
                        continue;
                    }

                    section.fills.emplace_back(IMapFile::Fill{faddr.second, fsize.second});
                    continue;
                }
            }

            ////////////////Read symbol name////////////////////////////////////////
            {
                // .text.Default_Handler
                //     0x0800252c        0x2 ./Core/Startup/startup_stm32h723zgtx.o
                //     0x0800252c                LTDC_ER_IRQHandler
                // name is --> .text.Default_Handler
                if(tokens.size() == 1) {
                    symbolName = tokens.first().trimmed();
                    continue;
                }
            }

            //////////////////////////////////////////////////////


            ////////////////Read Address name////////////////////////////////////////
            //     0x0800063c      0x130 ./Core/Src/dma.o
            //     0x0800063c                MX_DMA_Init
            // address is --> 0x0800063c
            auto addr = descr.readLineAddress(line);
            if (addr.first == "-1") {
                _ignored.emplace_back(line);
                continue;
            }

            ////////////////Sort elements////////////////////////////////////////
            // 1) .text.MX_DMA_Init
            //     0x0800063c      0x130 ./Core/Src/dma.o
            //     0x0800063c                MX_DMA_Init
            //-------------------------------------------------------------------------
            // 2) .text.main     0x08000d30       0x50 ./Core/Src/main.o
            //     0x08000d30                main
            //-------------------------------------------------------------------------
            // 3) .text.Default_Handler
            //     0x0800252c        0x2 ./Core/Startup/startup_stm32h723zgtx.o
            //     0x0800252c                LTDC_ER_IRQHandler
            //-------------------------------------------------------------------------
            // 4)  *fill*         0x0800252e        0x2
            {
                if(!currentSymbol.address.isEmpty()) {
                    if(currentSymbol.address == addr.first) {
                        currentSymbol.lines.emplace_back(line);

                        if(!symbolName.isEmpty()) {
                            _ignored.emplace_back(symbolName);
                            symbolName.clear();
                        }

                    } else {
                        section.addresses[key].emplace_back(std::move(currentSymbol));
                        ++_estimatedSymbols;

                        currentSymbol.name = std::move(symbolName);
                        currentSymbol.address = std::move(addr.first);
                        currentSymbol.address_d = addr.second;
                        currentSymbol.lines.clear();
                        currentSymbol.lines.reserve(3);
                        currentSymbol.lines.emplace_back(line);

                        symbolName.clear();
                        key = addr.second;
                        section.addresses[key].reserve(2);
                    }
                } else {
                    currentSymbol.name = std::move(symbolName);
                    currentSymbol.address = std::move(addr.first);
                    currentSymbol.address_d = addr.second;
                    currentSymbol.lines.clear();
                    currentSymbol.lines.reserve(3);
                    currentSymbol.lines.emplace_back(line);

                    key = addr.second;
                    section.addresses[key].reserve(2);
                    symbolName.clear();
                }
            }
        }

        if(!currentSymbol.address.isEmpty() && !currentSymbol.lines.isEmpty()) {
            section.addresses[key].emplace_back(std::move(currentSymbol));
            ++_estimatedSymbols;

            currentSymbol.name.clear();
            currentSymbol.address.clear();
            currentSymbol.address_d = 0;
            currentSymbol.lines.clear();

            symbolName.clear();
            key = 0;
        }

        _sections.emplace_back(std::move(section));

        if(!symbolName.isEmpty()) {
            _ignored.emplace_back(symbolName);
        }
    }

    return !_sections.empty();
}

}
