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
            //const quint64 baseAddr = itAddr.key();
            const auto& symbolList = itAddr.value();

            // Process each symbol entry at this address
            for (const auto& symEntry : symbolList) {
                qsizetype nextpos = 0;
                int counter = 0;

                do {
                    auto some = processSymbol(rawParser.names(), symEntry, nextpos);

                    // is fill -------------------------------------
                    if(some.type == CursorType::isFill) {
                        if(some.size.has_value()) {
                            section.size += some.size.value();
                            fills.emplace_back(IMapFile::Fill{some.vram, some.size.value()});
                        }
                    }
                    // is symbol -----------------------------------
                    else if(some.type == CursorType::isSymbol) {

                        IMapFile::Symbol symbol{};
                        symbol.name      = some.name;
                        symbol.lable     = some.lable;
                        symbol.filepath  = some.filepath;
                        symbol.vram      = some.vram;
                        symbol.vrom      = some.vrom;
                        symbol.size      = some.size;
                        symbol.id        = IMapFile::Symbols::INVALID;
                        symbol.idSection = IMapFile::Sections::INVALID;
                        symbol.idFile    = IMapFile::Files::INVALID;


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
                            if (!some.filepath.isEmpty()) {
                                IMapFile::Files::Index fileIdx = files.indexOf(some.filepath);

                                if(fileIdx == IMapFile::Files::INVALID) {
                                    IMapFile::File f{};
                                    f.filepath = std::move(some.filepath);
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
                    else if(some.type == CursorType::isSectionInfo) {
                        qDebug() << "name: " << some.name;
                        qDebug() << "lable: " << some.lable;
                        qDebug() << "size: " << some.size;
                        qDebug() << "vram: " << some.vram;
                        qDebug() << "vrom: " << some.vrom;
                        qDebug() << '\n';

                        secInfo.emplace_back(std::move(some));
                    }
                    // out of range
                    else if(some.type == CursorType::isOutOfRange) {
                        break;
                    }
                    nextpos = some.nextPos;
                    ++counter;
                } while(nextpos < symEntry.lines.size() || counter > descr.evaluateCnt);
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

MapFileReader::Cursor MapFileReader::processSymbol(const QSet<QString>& names,
                                                     const MapSymbol::Symbol& data, const qsizetype pos)
{
    if(pos >= data.lines.size()) {
        Cursor some{};
        some.type         = CursorType::isOutOfRange;
        some.nextPos = data.lines.size();
        return some;
    } else if(pos != 0) {
        qDebug() << "try again-------";
        qDebug() << "pos: " << pos;
        qDebug() << data.lines.mid(pos);
        qDebug() << '\n';
    }

    bool isStrange              = false;
    qsizetype nextPosition      = 0;
    // Prepare symbol record
    Cursor some{};
    some.vram           = data.address_d;
    some.vrom           = std::nullopt;
    some.size           = std::nullopt;
    some.type           = CursorType::isUndef;
    some.nextPos = 0;

    // Skip fill entries quickly
    if (data.lines.size() == 1) {
        const quint64 fillSize = descr.readFillSize(data.lines.first());

        if(fillSize) {
            some.size         = fillSize;
            some.type         = CursorType::isFill;
            some.nextPos = data.lines.size();
            return some; // skip no-data entries
        }
    } else if(names.contains(data.name)) {
        isStrange = true;
    }

    // Determine name, size, path in one pass
    QString fullNameCandidate = data.name;
    QString nameCandidate{};
    QString filePathCandidate{};

    if(data.lines.size() > 3 && pos == 0) {
        nameCandidate = fullNameCandidate;
    }

    bool nameValid = validateName(nameCandidate);
    int statements = 0;

    // Loop lines once; break early when all info gathered
    for (qsizetype line_i = pos; line_i < data.lines.size(); ++line_i) {
        const auto& line = data.lines[line_i];

        // process fill ---
        {
            const quint64 fillSize = descr.readFillSize(line);

            if(fillSize) {
                if(line_i == pos) {
                    some.size         = fillSize;
                    some.type         = CursorType::isFill;
                    some.nextPos = line_i + 1;
                    return some;
                } else {
                    break;
                }
            }
        }

        // process full name & stranges elements
        {
            //Split the string by whitespace characters (remove empty elements)
            QStringList tokens = line.split(descr.tokenSplitter, Qt::SkipEmptyParts);
            if (tokens.isEmpty()) {
                nextPosition = line_i + 1;
                continue;
            }

            if(names.contains(tokens.first())) {
                isStrange = true;

                if(line_i != pos) {
                    break;
                }
            }

            if(validateName(tokens.first())) {
                ++statements;
                if(fullNameCandidate.isEmpty()) {
                    fullNameCandidate = tokens.first().trimmed();
                }
                if(statements > 1) {
                    break;
                }
            }
        }

        // process load addr
        {
            auto vrom = descr.readLoadAddr(line);
            if(vrom.first) {
                some.vrom = vrom.second;
                qDebug() << "Load:" << vrom;
            }
        }

        // process name
        {
            if (!nameValid) {
                QString n = descr.readLineName(line);
                if (validateName(n)) {
                    nameCandidate = std::move(n);
                    nameValid = true;
                }
            }
        }

        // process size
        {
            if (!some.size.has_value()) {
                const auto sz = descr.readLineSize(line);
                if (sz > 0) {
                    some.size = sz;
                }
            }
        }

        // process file
        {
            if (filePathCandidate.isEmpty()) {
                filePathCandidate = descr.readLinePath(line);
            }
        }

        // save next position
        {
            nextPosition = line_i + 1;
        }
    }

    // Assign final symbol name
    if (nameValid) {
        some.name = std::move(nameCandidate);
        some.lable = std::move(fullNameCandidate);
    } else if (!fullNameCandidate.isEmpty()) {
        some.name = std::move(fullNameCandidate);
        some.lable = some.name;
    } else {
        some.name = QString("sym_%1").arg(data.address);
        some.lable = std::move(nameCandidate);
    }

    if(isStrange) {
        if(filePathCandidate.isEmpty()) {
            qDebug() << "Ignore this";
            qDebug() << data.lines.mid(nextPosition - 1);
            some.nextPos = nextPosition;
            some.type = CursorType::isSectionInfo;
            return some; // skip no-data entries
        }
    }

    // Assign size or default
    some.filepath = filePathCandidate;
    some.size = some.size.value_or(0);
    some.type = CursorType::isSymbol;
    some.nextPos = nextPosition;
    return some;
}

}
