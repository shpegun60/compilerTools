#include "imapfile.h"

void compiler_tools::IMapFile::clear()
{
    _symbols.clear();
    _files.clear();
    _sections.clear();
    type = MapType::Empty;
    stream << "------------->>Clear<<----------------" << Qt::endl;
}
