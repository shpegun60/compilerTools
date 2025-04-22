#include "linkerdescr.h"

void LinkerDescriptor::remove_unnecessary(QString& script)
{
    // Remove C-style comments /* ... */
    script.remove(cCommentRegex);
    // Remove C++-style comments // to the end of the line
    script.remove(cppCommentRegex);
    // Optional: remove remaining empty lines
    script.replace(emptyLines, "\n");
}
