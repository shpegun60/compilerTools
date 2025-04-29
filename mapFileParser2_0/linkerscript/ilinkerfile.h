#ifndef ILINKERFILE_H
#define ILINKERFILE_H

#include <QtContainerFwd>
#include <QJSEngine>

namespace compiler_tools {

namespace gnu {
class LinkerFileReader;
}

namespace clang {
class LinkerFileReader;
}

class ILinkerFile
{
    friend class gnu::LinkerFileReader;
    friend class clang::LinkerFileReader;
public:
    /**
    * @brief Default constructor.
    *
    * Initializes an object with empty containers for sections, regions, and global variables.
    */
    ILinkerFile() : stream(&_log) {}
    ~ILinkerFile() = default;

    enum LinkerType {
        Empty,
        GNU,
        Clang
    };

    struct Variable {
        QString attribute;      // e.g. "PROVIDE_HIDDEN" or empty
        QString lvalue;         // e.g. "_Min_Heap_Size"
        QString rvalue;         // e.g. "0x300"
        qsizetype _start;       // start cursor
        qsizetype _end;         // end cursor
        bool isEvaluated;
    };

    using Variables = QList<Variable>;

    // Structure for storing data about a single memory region
    struct MemoryRegion {
        QString name;
        QString attributes; // Attributes (e.g., xrw)
        QString origin;     // Starting address (ORIGIN)
        QString length;     // Length in bytes (LENGTH)
        bool isEvaluated;
    };

    /**
    * @brief A structure for storing data about a linker script section.
    */
    using SubSections = QStringList;

    struct Section {
        QString name;                       ///< Section name.
        SubSections subnames;               ///< Subsections associated with this section.
        Variables vars;                     ///< Variables defined in the section.
        QList<const MemoryRegion*> vma;     ///< Pointers to a virtual memory region (VMA).
        QList<const MemoryRegion*> lma;     ///< Pointers to a load memory region (LMA).
    };

    using SectionsMap = QHash<QString, Section>; ///< Type for storing sections by name.

    /**
    * @brief Structure for storing data about a memory region.
    */
    enum RegionType {
        None,
        VMA,
        LMA
    };

    struct Region {
        MemoryRegion region;                                   ///< Memory region data.
        QList<std::pair<RegionType, const Section*>> sections; ///< List of sections bound to the region.
    };
    using RegionsMap = QHash<QString, Region>; ///< Type for storing regions by name.

public: /* functions */

    /**
    * @brief Clears all readed data.
    */
    void clear();
    /**
    * @brief Returns a constant reference to the section container.
    * @return A constant reference to the SectionMap.
    */
    inline const SectionsMap& sections() const { return _sections; }

    /**
    * @brief Returns a constant reference to the region container.
    * @return A constant reference to the RegionsMap.
    */
    inline const RegionsMap& regions() const { return _regions; }

    /**
    * @brief Returns a constant reference to global variables.
    * @return A constant reference to the Variables.
    */
    inline const Variables& globals() const { return _globals; }

    /**
    * @brief Returns a reference to the QJSEngine for evaluating expressions.
    * @return A reference to the QJSEngine.
    */
    QJSEngine& sandbox() { return engine; }

public: /* other functions */
    inline const QString& getlog() const { return _log; }
    LinkerType getType() const { return type; }

private: /* other functions */
    void setType(const LinkerType type_) { type = type_; }
    QTextStream& log() { return stream; }

private:
    SectionsMap _sections{};    ///< Container for sections.
    RegionsMap _regions{};      ///< Container for memory regions.
    Variables _globals{};       ///< Container for global variables.
    QJSEngine engine{};         ///< Engine for evaluating JavaScript expressions.
    LinkerType type = LinkerType::Empty;

    // logging ---------------------------------------
    QString _log{};
    QTextStream stream;
};


} //namespace compiler_tools

#endif // ILINKERFILE_H
