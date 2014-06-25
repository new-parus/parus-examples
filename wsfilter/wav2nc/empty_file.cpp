#include <netcdf.h>

#include "empty_file.hh"

using namespace std;

void create_empty_netcdf_file(string name, string type)
{
    NcFile result(name.c_str(), NcFile::Replace);
    if (!result.is_valid())
        exit(1);

    result.add_att("type", type.c_str());
    result.add_att("title", "empty file");
    result.add_att("empty", "empty");
}
