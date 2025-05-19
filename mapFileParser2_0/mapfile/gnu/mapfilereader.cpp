#include "mapfilereader.h"

namespace compiler_tools::gnu {

// Optimized MapFileReader::read with inline comments and performance improvements
bool MapFileReader::read(IMapFile& mapFile, QString& file)
{
    MapRaw   rawParser{};
    MapSymbol symbolParser{};

    // Read raw data and symbol table
    rawParser.read(descr, file);
    symbolParser.read(descr, rawParser);

    // Aliases to containers for faster access
    auto& sections = mapFile._sections;       // HashIndex<QString, Section>
    auto& symbols  = mapFile._symbols;        // HashIndex<QString, Symbol>
    auto& files    = mapFile._files;          // HashIndex<QString, File>
    auto& fills    = mapFile._fills;          // QList<Fill>

    const auto& sectionDataList = symbolParser.data();
    sections.reserve(sectionDataList.size()); // avoid rehash during section inserts

    // Optionally reserve expected symbols/files to reduce rehash
    symbols.reserve(symbolParser.estimateSymbolCount());
    files.reserve(sectionDataList.size() * 2); // heuristic

    int unnamedSymbolCount = 0;

    // Iterate each section
    for (const auto& sectionData : sectionDataList) {
        IMapFile::Section section{};
        section.name   = sectionData.name;
        section.vram   = 0;
        section.size   = 0;
        section.id     = IMapFile::Sections::INVALID;
        section.idSymbols.reserve(sectionData.addresses.size());
        section.idFiles.reserve(4);

        // For each address bucket in this section
        for (auto itAddr = sectionData.addresses.cbegin();
             itAddr != sectionData.addresses.cend(); ++itAddr)
        {
            const quint64 baseAddr = itAddr.key();
            const auto& symbolList = itAddr.value();

            // Process each symbol entry at this address
            for (const auto& symEntry : symbolList) {
                // Skip fill entries quickly
                if (symEntry.lines.size() == 1) {
                    const quint64 fillSize = descr.readFillSize(symEntry.lines.first());

                    if(fillSize) {
                        fills.emplace_back(IMapFile::Fill{baseAddr, fillSize});
                        section.size += fillSize;
                        continue; // skip no-data entries
                    }
                }

                // Prepare symbol record
                IMapFile::Symbol symbol{};
                symbol.vram      = baseAddr;
                symbol.vrom      = std::nullopt;
                symbol.size      = std::nullopt;
                symbol.id        = IMapFile::Symbols::INVALID;
                symbol.idSection = IMapFile::Sections::INVALID;
                symbol.idFile    = IMapFile::Files::INVALID;

                // Determine name, size, path in one pass
                QString fullNameCandidate = symEntry.name;
                QString nameCandidate;
                QString filePathCandidate;

                if(symEntry.lines.size() > 3) {
                    nameCandidate = fullNameCandidate;
                }

                bool nameValid = validateName(nameCandidate);

                // Loop lines once; break early when all info gathered
                for (const auto& line : symEntry.lines) {
                    const quint64 fillSize = descr.readFillSize(line);

                    if(fillSize) {
                        qDebug() << line << " <<- fill";
                        fills.emplace_back(IMapFile::Fill{baseAddr, fillSize});
                        section.size += fillSize;
                        continue; // skip no-data entries
                    }

                    if (!nameValid) {
                        QString n = descr.readLineName(line);
                        if (validateName(n)) {
                            nameCandidate = std::move(n);
                            nameValid = true;
                        }
                    }

                    if(fullNameCandidate.isEmpty()) {
                        //Split the string by whitespace characters (remove empty elements)
                        QStringList tokens = line.split(descr.tokenSplitter, Qt::SkipEmptyParts);
                        if (tokens.isEmpty()) {
                            continue;
                        }

                        for(auto& name : tokens) {
                            if(validateName(name)) {
                                fullNameCandidate = std::move(name);
                                break;
                            }
                        }
                    }

                    if (!symbol.size.has_value()) {
                        const auto sz = descr.readLineSize(line);
                        if (sz > 0) {
                            symbol.size = sz;
                        }
                    }
                    if (filePathCandidate.isEmpty()) {
                        filePathCandidate = descr.readLinePath(line);
                    }
                    if (nameValid && symbol.size.has_value() && !filePathCandidate.isEmpty() && !fullNameCandidate.isEmpty()) {
                        break; // all required info found, exit early
                    }
                }

                if(symbolParser.names().contains(fullNameCandidate)) {
                    qDebug() << fullNameCandidate << " <<- skip";
                    qDebug() << nameCandidate;
                    qDebug() << symEntry.lines;
                    continue;
                }

                // Assign final symbol name
                if (nameValid) {
                    symbol.name = std::move(nameCandidate);
                    symbol.fullName = std::move(fullNameCandidate);
                } else if (!fullNameCandidate.isEmpty()) {
                    symbol.name = std::move(fullNameCandidate);
                    symbol.fullName = symbol.name;
                } else {
                    symbol.name = QString("sym_%1@_%2").arg(symEntry.address).arg(unnamedSymbolCount++);
                    symbol.fullName = std::move(nameCandidate);
                }


                // Assign size or default
                symbol.size = symbol.size.value_or(0);

                // Insert symbol into base
                IMapFile::Symbols::Index symIdx = IMapFile::Symbols::INVALID;
                {
                    int atemps = 0;
                    symIdx = symbols.emplace(symbol.name, std::move(symbol));
                    while(symIdx == IMapFile::Symbols::INVALID) {
                        symbol.name += QString("@_%1").arg(unnamedSymbolCount++);
                        symIdx = symbols.emplace(symbol.name, std::move(symbol));

                        ++atemps;
                        if(atemps > 3) {
                            break;
                        }
                    }

                    // Link symbol to id
                    auto s = symbols.at(symIdx);
                    if(s) {
                        s->id = symIdx;
                        // Link symbol to section
                        section.idSymbols.insert(symIdx);

                        if(s->size.has_value()) {
                            section.size += s->size.value();
                        }
                    }
                }


                // Handle file association if path found
                {
                    if (!filePathCandidate.isEmpty()) {
                        IMapFile::Files::Index fileIdx = files.indexOf(filePathCandidate);

                        if(fileIdx == IMapFile::Files::INVALID) {
                            IMapFile::File f{};
                            f.filepath = std::move(filePathCandidate);
                            f.vram     = 0;
                            f.size     = 0;
                            f.id       = IMapFile::Files::INVALID;
                            fileIdx = files.emplace(f.filepath, std::move(f));
                        }

                        // record file-symbol link
                        auto f = files.at(fileIdx);
                        if(f) {
                            // Link symbol to file
                            auto s = symbols.at(symIdx);
                            if(s) {
                                s->idFile = fileIdx;
                                if(s->size.has_value()) {
                                    f->size += s->size.value();
                                }
                            }

                            f->id = fileIdx;
                            f->filename = descr.extractFileName(f->filepath);
                            f->idSymbols.insert(symIdx);
                            section.idFiles.insert(fileIdx);
                        }
                    }
                }
            }
        }

        // Finally insert section
        {
            int atemps = 0;
            IMapFile::Sections::Index secIdx = sections.emplace(section.name, std::move(section));
            while(secIdx == IMapFile::Sections::INVALID) {
                section.name += QString("@_%1").arg(unnamedSymbolCount++);
                secIdx = sections.emplace(section.name, std::move(section));

                ++atemps;
                if(atemps > 3) {
                    break;
                }
            }

            // Update section reference in container
            auto sec = sections.at(secIdx);
            if(sec) {
                sec->id = secIdx;
                for(const auto& symIdx : std::as_const(sec->idSymbols)) {
                    auto s = symbols.at(symIdx);
                    if(s) {
                        s->idSection = secIdx;
                    }
                }
            }
        }
    }

    return true;
}


bool MapFileReader::loadDescriptorFromFile(const QString&)
{
    return false;
}

bool MapFileReader::validateName(const QString & name) const
{
    if(!name.isEmpty() && name.size() != 1 &&
        !name.startsWith("./") && !name.startsWith("0x", Qt::CaseInsensitive) && !name.contains('=')) {
        return true;
    }
    return false;
}

}
