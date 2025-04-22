#include "segment.h"


Segment::Segment(const QString &name, const quint64 vram,
                 const quint64 size,
                 const std::optional<quint64> vrom,
                 const std::optional<quint64> align)
    :    _name(name), _vram(vram), _size(size), _vrom(vrom), _align(align)  {};

