#include "linkersection.h"

namespace compiler_tools::gnu {

LinkerSection::LinkerSection() {}

const QPair<QString, QString> LinkerSection::parseAttribute(const QString& str)
{
    // Define the unit of measurement
    QString name{};
    QString attr{};
    bool isAttribute = false;

    for (const QChar& c : str) {
        if(attribute_delimiters.contains(c)) {
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
    auto it = raw.data().find(descr.sectionBlockName);
    if(it == raw.data().end()) {
        return _sections;
    }

    const QString& sectionsContent = it.value();
    QString globalContent{};
    QString currentSectionName{};
    QString currentSectionBody{};
    QString memoryRegion{};
    int sectionStart = -1;
    int braceLevel = 0;

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
            while (nameStart >= 0 && !name_delimiters.contains(sectionsContent[nameStart])) {
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
                if(!(currentSectionBody.isEmpty() && currentSectionName.isEmpty())) {
                    auto name = parseAttribute(currentSectionName);
                    _sections.emplace_back(Section {
                                                   std::move(name.first),
                                                   std::move(currentSectionBody),
                                                   std::move(name.second),
                                                   std::move(memoryRegion)});
                }

                currentSectionName.clear();
                currentSectionBody.clear();
                memoryRegion.clear();
                sectionStart = -1;
            }
        } else {
            globalContent += c;
        }
    }

    if(!(currentSectionBody.isEmpty() && currentSectionName.isEmpty())) {
        // Save the section with memory
        auto name = parseAttribute(currentSectionName);
        _sections.emplace_back(Section {
                                       std::move(name.first),
                                       std::move(currentSectionBody),
                                       std::move(name.second),
                                       std::move(memoryRegion)});
    }

    if(!globalContent.isEmpty()) {
        _sections.emplace_back(Section {descr.globalName, globalContent.trimmed(), "", ""});
    }
    return _sections;
}

} /* namespace compiler_tools::gnu */
