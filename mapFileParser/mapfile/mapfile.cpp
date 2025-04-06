#include "mapfile.h"


bool MapFile::read(const QString &mapData)
{
    if(_descriptor == nullptr) {
        return false;
    }

    const QStringList linkerConfig = mapData.split(_descriptor->C_MEM_LINKER_INFO_HEADER, Qt::SkipEmptyParts);

    if(linkerConfig.size() < 2) {
        return false;
    }

    const QStringList mapStart = linkerConfig[1].split(_descriptor->C_MEM_MAP_HEADER, Qt::SkipEmptyParts);

    if(mapStart.size() < 2) {
        return false;
    }

    // get linker script info
    {
        const QStringList lines = mapStart.first().split('\n', Qt::SkipEmptyParts);
        readLinker(lines);
    }

    // get sections info
    {
        const QStringList lines = mapStart.last().split('\n', Qt::SkipEmptyParts);
        readBody(lines);
    }

    return true;
}

void MapFile::readLinker(const QStringList &lines)
{
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        if(line.isEmpty()) {
            continue;
        } else if(_descriptor->isEnd(line)) {
            break;
        } else if (_descriptor->isIgnore(line)) {
            continue;
        }

        _descriptor->_linker.lines.append(std::move(line));
    }
}

void MapFile::readBody(const QStringList &lines)
{
    MapSection sec {};
    MapDescriptor& descr = *_descriptor;

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();

        if (line.isEmpty()) {
            continue;
        } else if(descr.isEnd(line)) {
            break;
        } else if(descr.containsSectionSize(line)) {
            //continue;
        } else if(descr.isIgnore(line)) {
            descr._unknown_lines.lines.append(std::move(line));
            continue;
        }

        // split all lines
        {
            const QRegularExpressionMatch sectionMatch = descr.sectionRegex.match(line);

            if (sectionMatch.hasMatch()) {

                QString sectionName = sectionMatch.captured(1);
                descr.push2category(sec);
                sec.lines.clear();
                sec.sectionName = std::move(sectionName);
            } else if(!sec.sectionName.isEmpty()) {
                sec.lines.append(std::move(line));
            } else {
                descr._unknown_lines.lines.append(std::move(line));
            }
        }
    }

    // append last element if exists
    descr.push2category(sec);
}
