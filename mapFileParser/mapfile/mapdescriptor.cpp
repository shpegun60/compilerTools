#include "mapdescriptor.h"


// Функція перевіряє, чи містить рядок слово "OUTPUT", що позначає кінець потрібного блоку.
bool MapDescriptor::isEnd(const QString &s)
{
    for (const QString &name : C_MEM_MAP_END_MARKERS) {
        if (s.startsWith(name)) {
            return true;
        }
    }
    return false;
}

bool MapDescriptor::isIgnore(const QString &s)
{
    for (const QString &name : C_MEM_MAP_IGNORE_MARKERS) {
        if (s.startsWith(name)) {
            return true;
        }
    }

    return false;
}

// Функція isSection перевіряє, чи належить секція до списку дозволених.
// Допустимі значення: "text*", "data*", "bss*"
bool MapDescriptor::isSection(const QString &s)
{
    for (const auto category : categories) {
        if(category->contains(s)) {
            return true;
        }
    }
    return false;
}

// Функція зчитує першу знайдену адресу у рядку у вигляді 0x... і повертає її як рядок.
// Якщо адреса не знайдена, повертає "-1".
std::pair<QString, quint64> MapDescriptor::readLineAddress(const QString &s)
{
    const QRegularExpressionMatch match = adressRegex.match(s);
    if (match.hasMatch()) {
        bool ok;
        const quint64 addr = match.captured(1).toULongLong(&ok, 16);

        if(ok) {
            return {QString("0x%1").arg(addr, 0, 16), addr};
        } else {
            return {"-1", 0};
        }
    }
    return {"-1", 0};
}


bool MapDescriptor::containsSectionSize(const QString &line)
{
    // Розбиваємо рядок за пробільними символами (видаляємо порожні елементи)
    const QStringList tokens = line.split(tokenSplitter, Qt::SkipEmptyParts);

    if (tokens.size() > 2) {
        const QString firstToken = tokens[0].trimmed();
        bool ok = false;
        const quint64 size = tokens[2].trimmed().toULongLong(&ok, 16);

        if(ok) {
            for (auto category : categories) {
                for (const auto& name : category->names()) {
                    if(firstToken.length() == name.length() && firstToken.contains(name)) {
                        category->sizeInc(size);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void MapDescriptor::push2category(const MapSection& seq)
{
    if(seq.sectionName.isEmpty()) {
        if(seq.lines.isEmpty()) {
            return;
        }

        _unknown_sections.addMapSection(seq);
        return;
    } else if(seq.lines.isEmpty()) {
        return;
    }

    for (const auto category : categories) {
        if(category->contains(seq.sectionName)) {
            category->addMapSection(seq);
            return;
        }
    }

    // if not found seqtions
    _unknown_sections.addMapSection(seq);
}
