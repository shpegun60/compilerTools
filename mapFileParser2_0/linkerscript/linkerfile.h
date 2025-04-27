#ifndef LINKERFILE_H
#define LINKERFILE_H

#include "linkermemory.h"
#include "linkersubsection.h"
#include "linkervariable.h"
#include <QJSEngine>

/**
* @brief A class for processing linker scripts.
*
* This class is responsible for reading linker scripts, managing sections,
* memory regions, and global variables, and evaluating expressions
* using QJSEngine.
*/
class LinkerFile
{
public:
/**
* @brief A structure for storing data about a linker script section.
*/
    struct Section {
        QString name;                       ///< Section name.
        LinkerSubSection subnames;          ///< Subsections associated with this section.
        LinkerVariable vars;                ///< Variables defined in the section.
        LinkerMemory::MemoryRegion* vma;    ///< Pointer to a virtual memory region (VMA).
        LinkerMemory::MemoryRegion* lma;    ///< Pointer to a load memory region (LMA).
    };

    using SectionMap = QHash<QString, Section>; ///< Type for storing sections by name.

/**
* @brief Structure for storing data about a memory region.
*/
    struct Region {
        LinkerMemory::MemoryRegion region;  ///< Memory region data.
        QList<const Section*> sections;     ///< List of sections bound to the region.
    };
    using RegionMap = QHash<QString, Region>; ///< Type for storing regions by name.

public:
/**
* @brief Default constructor.
*
* Initializes an object with empty containers for sections, regions, and global variables.
*/
    LinkerFile() = default;
    explicit LinkerFile(LinkerDescriptor* const descr) : _descr(descr) {};
    void InstallDescriptor(LinkerDescriptor* const descr);

/**
* @brief Reads and processes the linker script.
* @param descr LinkerDescriptor object for configuration and logging.
* @param script The text of the linker script.
*/
    void read(QString& script);
    void clear();
/**
* @brief Returns a constant reference to the section container.
* @return A constant reference to the SectionMap.
*/
    inline const SectionMap& sections() const { return _sections; }

/**
* @brief Returns a constant reference to the region container.
* @return A constant reference to the RegionMap.
*/
    inline RegionMap& regions() { return _regions; }

/**
* @brief Returns a constant reference to global variables.
* @return A constant reference to the LinkerVariable.
*/
    inline LinkerVariable& globals() { return _globals; }

/**
* @brief Returns a reference to the QJSEngine for evaluating expressions.
* @return A reference to the QJSEngine.
*/
    QJSEngine& sandbox() { return engine; }

private:
/**
* @brief Binds a section to a memory region.
* @param descr A LinkerDescriptor object for logging.
* @param sec The section to bind to.
* @param mem_region A string describing the memory region.
* @param attribute A string with the section attributes.
*/
    void sectionBind(LinkerDescriptor& descr, Section& sec, const QString& mem_region, const QString& attribute);

/**
* @brief Evaluates global variables and sets them in the QJSEngine.
* @param descr LinkerDescriptor object to log.
*/
    void evaluateGlobals(LinkerDescriptor& descr);

/**
* @brief Evaluates a numeric expression with possible units (K, M, etc.).
* @param descr LinkerDescriptor object to log.
* @param expr Expression to evaluate (changes during execution).
*/
    void evaluateNumber(LinkerDescriptor& descr, QString& expr);

private:
    SectionMap _sections{};           ///< Container for sections.
    RegionMap _regions{};             ///< Container for memory regions.
    LinkerVariable _globals{};        ///< Container for global variables.
    QJSEngine engine{};               ///< Engine for evaluating JavaScript expressions.

    // descriptor of file -------------------
    LinkerDescriptor* _descr = nullptr;
};

#endif // LINKERFILE_H
