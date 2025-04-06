#include "mapparser1.h"
#include <QDebug>
#include <QtMath>


// Функція approvedSection перевіряє, чи належить секція до списку дозволених.
// Допустимі значення: "text*", "data*", "bss*"
bool MapFileParser::approvedSection(const QString &s)
{
    for (const QString &name : C_TEXT_ID) {
        if (s.contains(name)) {
            return true;
        }
    }

    for (const QString &name : C_DATA_ID) {
        if (s.contains(name)) {
            return true;
        }
    }

    for (const QString &name : C_BSS_ID) {
        if (s.contains(name)) {
            return true;
        }
    }
    return false;
}

bool MapFileParser::approvedLines(const QString &s)
{
    for (const QString &name : C_MEM_MAP_IGNORE_MARKERS) {
        if (s.startsWith(name)) {
            return false;
        }
    }

    return true;
}

// Функція перевіряє, чи є рядок заголовком секції.
// Використовується регулярний вираз для пошуку шаблону *(.<section_name>)
bool MapFileParser::isSectionTitle(const QString &s)
{
    static QRegularExpression re(R"(\*\(\.([\w\.\-\*]+)\))"); //\*\(([\w\.\-\*]+)\)
    return re.match(s).hasMatch();
}

// Функція перевіряє, чи містить рядок слово "OUTPUT", що позначає кінець потрібного блоку.
bool MapFileParser::isEnd(const QString &s)
{
    for (const QString &name : C_MEM_MAP_END_MARKERS) {
        if (s.startsWith(name)) {
            return true;
        }
    }

    return false;
}

// Функція зчитує першу знайдену адресу у рядку у вигляді 0x... і повертає її як рядок.
// Якщо адреса не знайдена, повертає "-1".
QString MapFileParser::readLineAddress(const QString &s)
{
    static QRegularExpression re(R"(\s*(0x[0-9a-fA-F]+))"); //\s*(0[xX][0-9a-fA-F]+)
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        bool ok;
        quint64 addr = match.captured(1).toULongLong(&ok, 16);
        return ok ? QString("0x%1").arg(addr, 0, 16) : "-1";
    }
    return "-1";
}

// Функція повертає друге входження шаблону адреси (розмір) у рядку.
// Якщо не знайдено – повертає "0".
QString MapFileParser::readLineSize(const QString &s)
{
    static QRegularExpression re("0[xX][0-9a-fA-F]+");
    QRegularExpressionMatchIterator it = re.globalMatch(s);
    int count = 0;
    QString size;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        count++;
        if (count == 2) {
            size = match.captured(0);
            break;
        }
    }
    return size.isEmpty() ? "0" : size;
}

// Функція зчитує назву (частину рядка після адреси) та повертає її, обрізавши зайві пробіли.
QString MapFileParser::readLineName(const QString &s)
{
    static QRegularExpression re(R"(0[xX][0-9a-fA-F]+\s+(.*))");
    //static QRegularExpression re("0[xX][0-9a-fA-F]+(.*)");
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }
    return QString();
}

// Функція перевіряє, чи містить рядок заповнювач (*fill*). Якщо так – повертає Item з ім'ям "*fill*".
Item MapFileParser::readFill(const QString &s)
{
    if (s.contains(C_FILL_IDENTIFIER)) {
        return Item{0, C_FILL_IDENTIFIER, {}, readLineAddress(s), readLineSize(s)};
    }
    return Item{};
}

// Функція групує рядки, що мають однакову адресу, і створює для них Item.
// Якщо в групі лише один рядок – додатково перевіряється, чи не є він заповнювачем.
QVector<Item> MapFileParser::parseItemLines(const QStringList &lines, const QString sectionName)
{
    QVector<Item> items;
    int index = 0;
    //QStringList unknownLines;
    QString headName;

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];

        // Розбиваємо рядок за пробільними символами (видаляємо порожні елементи)
        const QStringList tokens = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (tokens.isEmpty()) {
            //unknownLines.append(line);
            continue;
        }

        const QString firstToken = tokens[0].trimmed();

        if(tokens.size() == 1) {
            if(firstToken.contains(sectionName)) {
                headName = firstToken;
            } else {
                //unknownLines.append(line);
            }
            continue;
        } else if(headName.isEmpty()) {

            if(firstToken.contains(sectionName) && firstToken.size() != sectionName.size() && firstToken != sectionName) {
                headName = firstToken;
            } else {
                for(auto& name : tokens) {
                    headName = readLineName(name);
                    if(!headName.startsWith("./") && !headName.startsWith("0x", Qt::CaseInsensitive)) {
                        break;
                    }
                }
            }
        }

        const QString addr = readLineAddress(line);
        if (addr == "-1") {
            //unknownLines.append(line);
            continue;
        }

        // Групуємо всі сусідні рядки з тією ж адресою
        QStringList group{line};
        while (i + 1 < lines.size() && readLineAddress(lines[i+1]) == addr) {
            group.append(lines[++i]);
        }

        if (group.size() == 1) {
            Item fill = readFill(group.first());
            if (!fill.name.isEmpty() && fill.address != "-1") { // Додано перевірку адреси
                fill.index = index++;
                items.append(fill);
                continue;
            }
        }

        QString name;
        if(group.size() > 3) {
            name = headName;
        } else {
            for (const QString &line : group) {
                QString currentName = readLineName(line);

                if(!currentName.isEmpty()) {
                    if(headName.isEmpty() ||
                        (!currentName.startsWith("./") && !currentName.startsWith("0x", Qt::CaseInsensitive))) {
                        name = currentName;
                    } else {
                        name = headName;
                    }
                } else {
                    name = headName;
                }
            }
        }

        items.append(Item{
            index++,
            name.isEmpty() ? "unnamed" : name,
            {},
            addr,
            readLineSize(group.first())
        });

        headName = QString();
    }
    return items;
}

// Головна функція парсингу.
// Вона створює кореневу секцію, перебирає рядки файлу, шукає заголовки секцій за допомогою регулярних виразів,
// після чого для кожної секції збирає рядки елементів (items) та обчислює сумарну висоту елементів.
ParseResult MapFileParser::parseMapFile(const QString &mapData)
{
    int sectionIndex = 0;
    ParseResult result;
    Section root;

    const QStringList memConfig = mapData.split(C_MEM_LINKER_INFO_HEADER, Qt::SkipEmptyParts);
    const QStringList mapStart = memConfig[1].split(C_MEM_MAP_HEADER, Qt::SkipEmptyParts);

    if(mapStart.size() < 2) {
        return result;
    }


    const QStringList lines = mapStart.last().split('\n', Qt::SkipEmptyParts);

    for (int i = 0; i < lines.size(); ++i) {
        const QString line = lines[i].trimmed();
        if(isEnd(line)) {
            break;
        } else if (line.isEmpty() || !approvedLines(line)) {
            continue;
        }

        const QRegularExpressionMatch sectionMatch = sectionRegex.match(line);

        if (sectionMatch.hasMatch()) {
            const QString sectionName = sectionMatch.captured(1);
            if (!approvedSection(sectionName)) {
                continue;
            }

            QStringList itemLines;

            while (++i < lines.size() && !isSectionTitle(lines[i]) && !isEnd(lines[i])) {
                const QString corrected = lines[i].trimmed();
                if(!corrected.isEmpty()) {
                    itemLines.append(corrected);
                }
            }
            --i;

            // Додавання секції
            Section newSection {
                sectionIndex++,
                sectionName,
                0,
                {},
                parseItemLines(itemLines, sectionName.trimmed().remove('*'))
            };
            root.subSections.append(newSection);
        }
    }

    root.name = "root";
    result.rootSection = root;
    return result;
}
