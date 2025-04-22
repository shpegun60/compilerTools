#ifndef GNUDESCRIPTOR_H
#define GNUDESCRIPTOR_H

#include <QString>
#include <QStringList>
#include <QRegularExpression>

class GnuDescriptor {
public: /* fields */
    const QString formatType; // "gnu" or "lld"
    const QString memLinkerInfoHeader;
    const QString memMapHeader;
    const QString fillIdentifier;
    QStringList memMapIgnoreMarkers;
    QStringList memMapEndMarkers;
    const QRegularExpression segmentRegex;
    const QRegularExpression tokenSplitter;
    const QRegularExpression addressRegex;

    /* ----- segment names ---------- */
    QStringList text;
    QStringList data;
    QStringList bss;
    /* ----- unknown segment name --- */
    const QString unknownName;

public: /* constructors */
    /* ----- segment names ---------- */
    static inline GnuDescriptor gnuDefault() {
        return GnuDescriptor {
            /* .formatType */           "gnu",
            /* .memLinkerInfoHeader */  "Memory Configuration",
            /* memMapHeader */          "Linker script and memory map",
            /* fillIdentifier */        "*fill*",
            /* memMapIgnoreMarkers */   {"LOAD ", "START GROUP", "END GROUP"},
            /* memMapEndMarkers */      {"/DISCARD/", "OUTPUT", "LOAD linker stubs"},
            /* segmentRegex */          QRegularExpression(R"(^\*\((.*?)\))"),
            /* tokenSplitter */         QRegularExpression(R"(\s+)"),
            /* addressRegex */          QRegularExpression(R"(\s*(0[xX][0-9a-fA-F]+))"),

            /* ---------------------- segment names ----------------------------------- */
            /* text */                  {".text", ".isr_vector"},
            /* data */                  {".data", ".rodata", ".strings", "._pm" },
            /* bss */                   {".bss", "COMMON" },
            /* ----- unknown segment name --- */
            /* unknownName */           {"._unknown_"}
        };
    }

public: /* datatypes */
    enum Segment : quint8 {
        Unknown = 0,
        Text    = 1,
        Data    = 2,
        Bss     = 3
    };

    struct SegmentSizes {
        quint64 text;
        quint64 data;
        quint64 bss;
    };

public: /* functions */
    /**
     * The function checks whether the string contains the end words, which indicates
     * the end of the desired block.
     */
    bool isEnd(const QString &line) const;
    bool isIgnore(const QString &line) const;
    bool isFill(const QString &line) const;
    /**
     * The function reads the first address found in the string as 0x... and returns it as a string.
     * If the address is not found, returns "-1".
     */
    std::pair<QString, quint64> readLineAddress(const QString &s) const;

    /**
     * Processes a line to detect segment size.
     * Returns segment type and size if successful.
     */
    std::pair<Segment, quint64> readSegmentSize(const QString &line) const;
    bool isSegmentSize(const QString &line, SegmentSizes& sz) const;
};


#endif // GNUDESCRIPTOR_H
