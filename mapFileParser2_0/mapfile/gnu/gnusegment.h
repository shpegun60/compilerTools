#ifndef GNUSEGMENT_H
#define GNUSEGMENT_H

#include "gnudescriptor.h"
#include <QString>
#include <QStringList>
#include <QList>

/*
 * Basic component of map file
 */
class GnuSegment
{
public:
    GnuSegment() = default;
    GnuSegment(const QString& name) : _name(name) {}
    GnuSegment(QString&& name) : _name(std::move(name)) {}

public: /* functions */
    inline const QString& name() const { return _name; }
    inline void setname(const QString& name) { _name = name; }
    inline void setname(QString&& name) { _name = std::move(name); }

    inline const auto& lines() const { return _lines; }

    inline void append(const QString& seg) { _lines.append(seg); }
    inline void append(QString&& seg) { _lines.append(std::move(seg)); }

    inline void clear() { _name.clear(); _lines.clear(); }
private:
    QString     _name {};
    QStringList _lines {};
};

using GnuSegmentList = QList<GnuSegment>;

/*
 * segment container & reader
 */
class GnuSegmentReader
{
public: /* constructors */
    GnuSegmentReader() = default;
public: /* functions */
    GnuDescriptor::SegmentSizes read(const GnuDescriptor& descr, const QString &mapData);
    const GnuSegmentList& data() const { return _readed; }
    const QStringList& ignored() const { return _ignored; }
    void clear();
private:
    GnuDescriptor::SegmentSizes readBody(const GnuDescriptor& descr, const QStringList &lines);
private: /* variables */
    GnuSegmentList _readed {};
    QStringList _ignored {};
};
#endif // GNUSEGMENT_H
