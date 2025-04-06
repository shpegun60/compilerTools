#ifndef MAP_SECTION_CATEGORY_H
#define MAP_SECTION_CATEGORY_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <initializer_list>

// MapSections descriptor
struct MapSection {
    QString sectionName;
    QStringList lines;
};

class SectionCategory {
public:
    SectionCategory() = default;
    // Конструктор, що приймає список QString через std::initializer_list
    SectionCategory(std::initializer_list<QString> names)
        : _names(names) {}

public:
    // Геттер для списку назв
    inline const QStringList& names() const { return _names; }
    inline quint64 sectionSize() const { return _sec_size; }
    inline void sizeInc(const int next) { _sec_size += next; }

    // Додає нове ім'я до категорії, якщо його ще немає
    inline void addName(const QString &name) {
        if (!_names.contains(name)) {
            _names.append(name);
        }
    }

    // Перевіряє, чи міститься ім'я в категорії
    inline bool contains(const QString &name) const {
        for(const auto& s : _names) {
            if(name.contains(s)) {
                return true;
            }
        }
        return false;
    }
    // Видаляє ім'я з категорії
    inline void removeName(const QString &name) { _names.removeAll(name); }

    inline const auto& getSections() const { return sections; }
    inline void addMapSection(const MapSection& sec) { sections.push_back(sec); ++_sec_count; }

private:
    QStringList _names {};
    quint64 _sec_size = 0;
    quint64 _sec_count = 0;
    QList<MapSection> sections {};
};

#endif // MAP_SECTION_CATEGORY_H
