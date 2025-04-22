#include "linkersection.h"

LinkerSection::LinkerSection() {}

const QPair<QString, QString> LinkerSection::parseAttribute(const QString& str)
{
    // Define the unit of measurement
    const QString str_tmp = str.trimmed();
    QString name{};
    QString attr{};
    bool isAttribute = false;

    for (const QChar& c : str) {
        if(c.isSpace()) {
            isAttribute = true;
        }

        if (isAttribute) {
            attr += c; // Add a character to the attribute
        } else {
            name += c; // Add a character to the name
        }
    }

    return {name.trimmed(), attr.trimmed()};
}

const LinkerSection::Data &LinkerSection::read(const LinkerDescriptor& descr, const LinkerRaw& raw)
{
    if (!raw.data().contains(descr.sectionBlockName)) {
        return _sections;
    }

    QString sectionsContent = raw.data().value(descr.sectionBlockName);
    QString globalContent{};
    int sectionStart = -1;
    int braceLevel = 0;
    QString currentSectionName{};
    QString currentSectionBody{};
    QString memoryRegion{};

    for (int i = 0; i < sectionsContent.length(); ++i) {
        const QChar c = sectionsContent[i];

        // Start of block: found '{' and not yet inside a block
        if (c == '{' && braceLevel == 0) {
            // Look for the block name before '{'
            int nameStart = i - 1;
            while (nameStart >= 0 && sectionsContent[nameStart].isSpace()) {
                nameStart--;
            }
            int nameEnd = nameStart;
            const QString delimiters = "\n\t\r\f\v;{}=<>\"[]";
            while (nameStart >= 0 && !delimiters.contains(sectionsContent[nameStart])) {
                nameStart--;
            }
            currentSectionName.clear();
            currentSectionBody.clear();
            memoryRegion.clear();

            currentSectionName = sectionsContent.mid(nameStart + 1, nameEnd - nameStart).trimmed();
            braceLevel = 1;
            sectionStart = i + 1;
            currentSectionBody += "{";
            continue;
        }

        // Handling nested curly braces
        else if (sectionStart != -1 && braceLevel > 0) {
            currentSectionBody += c;
            if (c == '{') braceLevel++;
            else if (c == '}') braceLevel--;

            // End of section
            if (braceLevel == 0) {
                // Memory definition (e.g. ">FLASH")
                int memoryPos = i + 1;

                // Skip spaces after '}'
                while (memoryPos < sectionsContent.length() && sectionsContent[memoryPos].isSpace()) {
                    ++memoryPos;
                }

                // Check for '>' and get the memory name
                if (memoryPos < sectionsContent.length() && sectionsContent[memoryPos] == '>') {
                    int memoryEnd = sectionsContent.indexOf("\n", memoryPos);
                    if (memoryEnd == -1) {
                        memoryEnd = sectionsContent.length();
                    }
                    memoryRegion = sectionsContent.mid(memoryPos, memoryEnd - memoryPos).trimmed();
                }

                // Save the section with memory
                const auto name = parseAttribute(currentSectionName.remove(':'));
                _sections.insert(std::move(name.first), /* name */
                                 {std::move(currentSectionBody), std::move(name.second), std::move(memoryRegion)} /* {body, attribute, mem_region} */);

                currentSectionName.clear();
                currentSectionBody.clear();
                memoryRegion.clear();
                sectionStart = -1;
            }
        } else {
            globalContent += c;
        }
    }

    if(!currentSectionBody.isEmpty()) {
        // Save the section with memory
        const auto name = parseAttribute(currentSectionName.remove(':'));
        _sections.insert(std::move(name.first), /* name */
                         {std::move(currentSectionBody), std::move(name.second), std::move(memoryRegion)} /* {body, attribute, mem_region} */);

    }

    _sections.insert(descr.globalName, /* name */
                     {globalContent.trimmed(), "", ""} /* {body, attribute, mem_region} */);
    return _sections;
}
