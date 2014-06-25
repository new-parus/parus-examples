#ifndef __NETCDF_BINARY_CODEC_HH__
#define __NETCDF_BINARY_CODEC_HH__

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <string>
#include <map>

#include <netcdf.h>

namespace NetCDFBinaryCodec
{
    extern void encode(int fd, const char *nc_file,
            const std::map<std::string, std::string> &global_attributes);

    extern void decode(const char *nc_file, int fd,
            std::map<std::string, std::string> &global_attributes);

    extern bool is_netcdf_file(const char *nc_file);

    extern bool is_encoded(const char *nc_file);

    class NetCDFFileError
    {
    public:
        std::string description;

        NetCDFFileError(std::string _description) :
            description(_description)
        {
        }
    };

    class FileError
    {
    public:
        std::string description;

        FileError() :
            description(strerror(errno))
        {
        }
    };
}

#endif
