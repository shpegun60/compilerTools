#include "gnumapparser.h"

bool GnuMapParser::parse()
{
    MapDescriptor& descr = *_descriptor;
    SubSection saved {};
    auto& text = descr._text;
    quint64 key = 0;
    QString lastSubName {};

    for(const auto& sec : text.getSections()) {
        const QString sectionName = sec.sectionName.trimmed().remove('*');

        for(const auto& line : sec.lines) {
            // Розбиваємо рядок за пробільними символами (видаляємо порожні елементи)
            const QStringList tokens = line.split(descr.tokenSplitter, Qt::SkipEmptyParts);

            if (tokens.isEmpty()) {
                continue;
            }

            ////////////////////////////////////////////////////////

            if(tokens.size() == 1) {
                QString firstToken = tokens[0].trimmed();

                if(firstToken.contains(sectionName)) {
                    lastSubName = std::move(firstToken);
                } else {
                    unknownLines.append(line);
                }
                continue;
            }
            //////////////////////////////////////////////////////


            auto addr = descr.readLineAddress(line);
            if (addr.first == "-1") {
                unknownLines.append(line);
                continue;
            }

            if(!saved.adress.isEmpty()) {
                if(saved.adress == addr.first) {
                    saved.lines.append(line);

                    if(!lastSubName.isEmpty()) {
                        lastSubName.clear();
                    }
                } else {

                    addressMap[key].append(std::move(saved));
                    saved.adress.clear();
                    saved.lines.clear();

                    if(!lastSubName.isEmpty()) {
                        saved.subName = std::move(lastSubName);
                        lastSubName.clear();
                    } else {
                        saved.subName.clear();
                    }

                    key = addr.second;
                    saved.adress = std::move(addr.first);
                    saved.lines.append(line);
                }
            } else {

                if(!lastSubName.isEmpty()) {
                    saved.subName = std::move(lastSubName);
                    lastSubName.clear();
                } else {
                    saved.subName.clear();
                }

                key = addr.second;
                saved.adress = std::move(addr.first);
                saved.lines.append(line);
            }
        }
    }

    if(!saved.adress.isEmpty()) {
        addressMap[key].append(std::move(saved));
    }

    return true;
}
