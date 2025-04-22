#include "gnusection.h"

void GnuSectionReder::read(const GnuDescriptor& descr, const GnuSegmentList& segments)
{
    GnuSection section {};
    quint64 key = 0;
    QString sectionName {};

    // clear all before read --------------------------------
    clear();
    _segments.reserve(segments.size());
    // ------------------------------------------------------
    for(const auto& seg : segments) {
        GnuSectionContainer sectionCont{};
        sectionCont.segmentName = seg.name().trimmed();

        for(const auto& line : seg.lines()) {
            if (line.isEmpty()) {
                continue;
            }

            // Split the string by whitespace characters (remove empty elements)
            const QStringList tokens = line.split(descr.tokenSplitter, Qt::SkipEmptyParts);

            if (tokens.isEmpty()) {
                continue;
            }

            ////////////////////////////////////////////////////////
            if(tokens.size() == 1) {
                QString firstToken = tokens[0].trimmed();

                if(firstToken.contains(sectionName)) {
                    sectionName = std::move(firstToken);
                } else {
                    _ignored.append(line);
                }
                continue;
            }
            //////////////////////////////////////////////////////

            auto addr = descr.readLineAddress(line);
            if (addr.first == "-1") {
                _ignored.append(line);
                continue;
            }

            if(!section.address().isEmpty()) {
                if(section.address() == addr.first) {
                    section.append(line);

                    if(!sectionName.isEmpty()) {
                        sectionName.clear();
                    }
                } else {
                    sectionCont.sections[key].append(std::move(section));
                    section.clear();
                    section.setname(std::move(sectionName));
                    sectionName.clear();

                    key = addr.second;
                    section.setaddress(std::move(addr.first));
                    section.append(line);
                }
            } else {
                section.setname(std::move(sectionName));
                sectionName.clear();

                key = addr.second;
                section.setaddress(std::move(addr.first));
                section.append(line);
            }
        }

        if(!section.address().isEmpty() && !section.lines().isEmpty()) {
            sectionCont.sections[key].append(std::move(section));
            section.clear();
            sectionName.clear();
        }
        _segments.append(std::move(sectionCont));
    }
}

void GnuSectionReder::clear()
{
    _segments.clear();
    _ignored.clear();
}
