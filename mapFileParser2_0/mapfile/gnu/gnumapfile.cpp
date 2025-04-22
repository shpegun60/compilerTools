#include "gnumapfile.h"
#include "QDebug"

GnuMapFile::GnuMapFile(const GnuDescriptor& descr):
    _descr(&descr){}

GnuMapFile::GnuMapFile(const GnuDescriptor* const descr):
    _descr(descr){}


// Функція зчитує назву (частину рядка після адреси) та повертає її, обрізавши зайві пробіли.
QString GnuMapFile::readLineName(const QString &s)
{
    static QRegularExpression re(R"(0[xX][0-9a-fA-F]+\s+(.*))");
    //static QRegularExpression re("0[xX][0-9a-fA-F]+(.*)");
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }
    return QString();
}




bool GnuMapFile::read(const QString& txt)
{
    if(_descr == nullptr) {
        return false;
    }
    const GnuDescriptor& descr = *_descr;

    _sizes = _segments.read(descr, txt);
    _sections.read(descr, _segments.data());
    return true;
}

void GnuMapFile::parse(MapFile& map)
{
    const auto& data = _sections.data();
    if(_descr == nullptr || data.size() == 0) {
        return;
    }

    for(const auto& segment : data) {
        if(segment.sections.empty()) {
            continue;
        }

        qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        qDebug() << "segment:" << segment.segmentName;

        Segment mapSeg = Segment(segment.segmentName, 0);

        for (const auto & [key, list] : segment.sections.asKeyValueRange()) {
            if(list.isEmpty()) {
                continue;
            }
            qDebug() << "Key:" << QString("0x%1").arg(key, 8, 16, QLatin1Char('0')).toLower();

            Symbol symbol;
            symbol.setVram(key);
            for (const auto &section : list) {
                if(section.lines().size() == 1 &&
                    _descr->isFill(section.lines().first())) {
                    qDebug() << "Fillll:" << section.lines().first();
                    continue;
                }

                if(symbol.name().isEmpty()) {
                    symbol.setName(section.name());
                }
                qDebug() << "Section name:" << section.name();

                for (const QString &line : section.lines()) {
                    QString currentName = readLineName(line);

                    if(!currentName.isEmpty()) {
                        if(symbol.name().isEmpty() ||
                            (!currentName.startsWith("./") && !currentName.startsWith("0x", Qt::CaseInsensitive))) {
                            symbol.setName(std::move(currentName));
                        }
                    }
                }

                if(!section.name().isEmpty()) {
                    qDebug() << "NameCheck: " << section.name().contains(symbol.name());
                }

                qDebug() << "Symbol name:" << symbol.name();

            }
        }


        // for (QHash<quint64, QList<GnuSection>>::const_iterator it = segment.sections.constBegin();
        //      it != segment.sections.constEnd(); ++it) {
        //     const auto key = it.key();
        //     const auto& list = it.value();

        //     qDebug() << "Key:" << key;

        //     for (const GnuSection &section : list) {
        //         qDebug() << "Section:" << section.name();
        //     }
        // }
    }


}
