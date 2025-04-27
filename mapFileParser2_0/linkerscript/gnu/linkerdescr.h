#ifndef LINKERDESCR_H
#define LINKERDESCR_H

#include "macro.h"
#include <QRegularExpression>
#include <QString>
#include <QStringList>


class LinkerDescriptor {
    CONTROLLED_CREATION_CLASS(LinkerDescriptor);
public: /* values */
    /* -------------------- comments ---------------------------------*/
    static const inline QRegularExpression cCommentRegex = QRegularExpression(R"(/\*[\s\S]*?\*/)");
    static const inline QRegularExpression cppCommentRegex = QRegularExpression(R"(//[^\n]*)");

    /* ------------------- empty lines -------------------------------*/
    static const inline QRegularExpression emptyLines = QRegularExpression(R"(\n\s*\n)");

    /* global symbols name */
    static const inline QString globalName {"@GLOBAL"};
    static const inline QString noName {"@NONAME"};
    static const inline QString unknown {"@UNKNOWN"};

    /* specific symbols name */
    /* ------------------- section block -------------------------------*/
    static const inline QString sectionBlockName {"SECTIONS"};

    /* ------------------- Memory block -------------------------------*/
    static const inline QString memoryBlockName {"MEMORY"};
    static const inline QRegularExpression memoryBlockRegex = QRegularExpression(
        R"(^\s*(\w+)(?:\s*\(([^)]*?)\))?\s*:\s*ORIGIN\s*=\s*(.*?)\s*,\s*LENGTH\s*=\s*(.*?)\s*(?:,|\n|;|:|$))",
        QRegularExpression::CaseInsensitiveOption
            | QRegularExpression::MultilineOption
            | QRegularExpression::DotMatchesEverythingOption
        );

    /* ------------------- Variable block -------------------------------*/
    static const inline QRegularExpression attrValueRegex =
        QRegularExpression(R"(\b(\w+)\s*\(\s*(.*?)\s*=\s*(.*?)\s*\)\s*;)");
    static const inline QRegularExpression valueRegex =
        QRegularExpression(R"(\b([^()=;\s]+)\s*=\s*(.*?)\s*;)");
    static const inline QRegularExpression alignRegex =
        QRegularExpression(R"(\s*(.*?)\s*=\s*ALIGN\((.*?)\)\;)");

    /* ------------------- Global Variable block -------------------------------*/
    static const inline QRegularExpression enytryRegex =
        QRegularExpression(R"(\s*ENTRY\((.*?)\))");


    /* ------------------- Subsection block -------------------------------*/
    static const inline QRegularExpression subsectionRegex =
        QRegularExpression(R"(\s*\*\((.*?)\))");
    static const inline QStringList subsectionIgnored {globalName,
                                                      noName,
                                                      unknown,
                                                      sectionBlockName,
                                                      memoryBlockName,
                                                      "SORT("};

    /* ------------------- General File Block -------------------------------*/
    static const inline QRegularExpression hexRe = QRegularExpression{R"(\b(0[xX][0-9a-fA-F]+)\s*([KMGT]+)\b)",
                                                                      QRegularExpression::CaseInsensitiveOption};
    static const inline QRegularExpression decRe = QRegularExpression{R"(\b(?!0[xX])([0-9]+)\s*([KMGT]+)\b)",
                                                                      QRegularExpression::CaseInsensitiveOption};
    // Static structures for optimization
    inline static const QHash<QChar, quint64> units = {
        {'K', 1024ULL},
        {'M', 1024ULL * 1024},
        {'G', 1024ULL * 1024 * 1024},
        {'T', 1024ULL * 1024 * 1024 * 1024}
    };
    static constexpr int evaluateCnt = 100;

    /* regular expressions */
    static const inline QRegularExpression tokenRe{R"(\b(\w+)\b)"}; // Extracts words from letters, numbers, and underscores
    static const inline QRegularExpression atRe{R"(^AT\(|^AT>)"};   // Checks for "AT(" or "AT>"

public: /* functions */
    static void remove_unnecessary(QString&);
    static bool validate(LinkerDescriptor* descr);

public: /* log */
    template<typename T>
    LinkerDescriptor& operator<<(const T& value) {stream << value; return *this;}
    LinkerDescriptor& operator<<(QTextStreamManipulator manip) {stream << manip; return *this;}
    inline QString& log() { return _log; }
private:
    QString _log{};
    QTextStream stream;
};


#endif // LINKERDESCR_H
