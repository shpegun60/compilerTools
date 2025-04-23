#include "linkerraw.h"


const LinkerRaw::Data& LinkerRaw::read(const LinkerDescriptor& descr, const QString& script)
{
    QString globalContent{};
    QString currentBlockName{};
    int braceLevel = 0;
    int blockStartPos = -1;

    for (int i = 0; i < script.length(); ++i) {
        const QChar c = script[i];

        // Start of block: found '{' and not yet inside a block
        if (c == '{' && braceLevel == 0) {
            // Look for the block name before '{'
            int nameStart = i - 1;
            while (nameStart >= 0 && script[nameStart].isSpace()) {
                nameStart--;
            }
            int nameEnd = nameStart;
            while (nameStart >= 0 && !script[nameStart].isSpace()) {
                nameStart--;
            }
            currentBlockName = script.mid(nameStart + 1, nameEnd - nameStart).trimmed().toUpper();

            braceLevel = 1;
            blockStartPos = i + 1;
            continue;
        }

        // Inside the block
        else if (braceLevel > 0) {
            if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
            }

            // End of block
            if (braceLevel == 0) {
                QString currentBlockContent = script.mid(blockStartPos, i - blockStartPos).trimmed();
                _blocks.emplace(currentBlockName.isEmpty() ? descr.noName : std::move(currentBlockName), std::move(currentBlockContent));
                currentBlockName.clear();
                blockStartPos = -1;
            }
        }

        // Global content (outside blocks)
        else if (blockStartPos == -1) {
            globalContent += c;
        }
    }

    // Handling an unclosed block at the end of the file
    if (braceLevel != 0 && blockStartPos != -1) {
        QString currentBlockContent = script.mid(blockStartPos, script.length() - blockStartPos).trimmed();
        _blocks.emplace(currentBlockName.isEmpty() ? descr.noName : std::move(currentBlockName), std::move(currentBlockContent));
    }

    // Add global content
    if(!globalContent.isEmpty()) {
        _blocks.emplace(descr.globalName, globalContent.trimmed());
    }
    return _blocks;
}
