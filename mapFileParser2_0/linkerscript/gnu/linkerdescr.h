#ifndef LINKERDESCR_H
#define LINKERDESCR_H

#include "macro.h"
#include <QRegularExpression>
#include <QString>

class LinkerDescriptor {
    CONTROLLED_CREATION_CLASS(LinkerDescriptor, = default);
public: /* values */
    /* -------------------- comments ---------------------------------*/
    static const inline QRegularExpression cCommentRegex = QRegularExpression(R"(/\*[\s\S]*?\*/)");
    static const inline QRegularExpression cppCommentRegex = QRegularExpression(R"(//[^\n]*)");
    /* ------------------- empty lines -------------------------------*/
    static const inline QRegularExpression emptyLines = QRegularExpression(R"(\n\s*\n)");

    /* global symbols name */
    static const inline QString globalName {"@GLOBAL"};
    static const inline QString noName {"@NONAME"};
    /* specific symbols name */
    /* ------------------- section block -------------------------------*/
    static const inline QString sectionBlockName {"SECTIONS"};
    /* ------------------- Memory block -------------------------------*/
    static const inline QString memoryBlockName {"MEMORY"};
    static const inline QRegularExpression memoryBlockRegex = QRegularExpression (
        R"(^\s*(\w+)\s*\(([^)]+)\)\s*:\s*ORIGIN\s*=\s*(.*?)\s*,\s*LENGTH\s*=\s*(.*?)\s*(?:,|\n|;|:|$))",
        QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::MultilineOption |
            QRegularExpression::DotMatchesEverythingOption
        );
    /* ------------------- Variable block -------------------------------*/
    static const inline QRegularExpression attrValueRegex =
        QRegularExpression(R"(\b(\w+)\s*\(\s*(.*?)\s*=\s*(.*?)\s*\)\s*;)");
    static const inline QRegularExpression valueRegex =
        QRegularExpression(R"(\b([^()=;\s]+)\s*=\s*(.*?)\s*;)");
    static const inline QRegularExpression alignRegex =
        QRegularExpression(R"(\s*(.*?)\s*=\s*ALIGN\((.*?)\)\;)");
    static const inline QRegularExpression enytryRegex =
        QRegularExpression(R"(\s*ENTRY\((.*?)\))");
    /* ------------------- Subsection block -------------------------------*/
    static const inline QRegularExpression subsectionRegex =
        QRegularExpression(R"(\s*\*\((.*?)\))");
    static const inline QStringList subsectionIgnored {globalName,
                                                      noName,
                                                      sectionBlockName,
                                                      memoryBlockName,
                                                      "SORT("};
public: /* functions */
    static void remove_unnecessary(QString&);
};


#endif // LINKERDESCR_H
