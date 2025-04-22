#ifndef MAPFILE_H
#define MAPFILE_H

#include "segment.h"

class MapFile
{
public:
    MapFile();

    // Getters
    inline const QString& name() const { return _name; }
    inline const QString& path() const { return _path; }
    inline const auto& segments() const { return _segments; }
    inline void push(const Segment& sym) { _segments.append(sym); };
    inline void push(Segment&& sym) { _segments.append(std::move(sym)); };

    // Setters
    inline void setName(const QString& name) { _name = name; }
    inline void setName(QString&& name) { _name = std::move(name); }
    inline void setPath(const QString& path) { _path = path; }
    inline void setPath(QString&& path) { _path = std::move(path); }

private:
    QString _name {};
    QString _path {};
    QList<Segment> _segments {};
};

#endif // MAPFILE_H
