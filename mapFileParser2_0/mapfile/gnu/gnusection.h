#ifndef GNUSECTION_H
#define GNUSECTION_H

#include "gnusegment.h"
#include <QString>
#include <QStringList>

class GnuSection
{
public:
    GnuSection() = default;
    GnuSection(const QString& name) : _name(name) {}
    GnuSection(QString&& name) : _name(std::move(name)) {}

public: /* functions */
    /* name */
    inline const QString& name() const { return _name; }
    inline void setname(const QString& name) { _name = name; }
    inline void setname(QString&& name) { _name = std::move(name); }

    /* address */
    inline const QString& address() const { return _address; }
    inline void setaddress(const QString& address) { _address = address; }
    inline void setaddress(QString&& address) { _address = std::move(address); }

    /* lines */
    inline const auto& lines() const { return _lines; }
    inline void append(const QString& seg) { _lines.append(seg);}
    inline void append(QString&& seg) { _lines.append(std::move(seg)); }

    /* clear */
    inline void clear() { _name.clear(); _address.clear(); _lines.clear(); }

private: /* variables */
    QString     _name   {};
    QString     _address {};
    QStringList _lines  {};
};

struct GnuSectionContainer
{
    QString segmentName {};
    QHash<quint64, QList<GnuSection>> sections {};
};

class GnuSectionReder
{
public: /* constructors */
    GnuSectionReder() = default;

public: /* functions */
    void read(const GnuDescriptor&, const GnuSegmentList&);
    const QList<GnuSectionContainer>& data() const { return _segments; };
    const QStringList& ignored() const { return _ignored; }
    void clear();
private:
    QList<GnuSectionContainer> _segments{};
    QStringList _ignored {};
};

#endif // GNUSECTION_H
