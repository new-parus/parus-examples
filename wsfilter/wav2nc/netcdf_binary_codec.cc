#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <string>
#include <map>

#include <netsdfcpp.h>

#include "netcdf_binary_codec.hh"

namespace NetCDFBinaryCodec
{
    static int read_at_once = 1048576;

    void encode(int from, const char *nc_file,
            const std::map<std::string, std::string> &_global_attributes)
    {
        std::map<std::string, std::string> global_attributes =
            _global_attributes;
        ncbyte *buffer = new ncbyte[read_at_once];

        NcFile to(nc_file, NcFile::Replace);
        if (!to.is_valid())
            throw NetCDFFileError("trouble creating netcdf file");

        off_t from_available;
        {
            struct stat stat_data;
            if (fstat(from, &stat_data) == -1)
                throw FileError();

            from_available = stat_data.st_size;
            if (from_available == 0)
                from_available = 1;
        }

        NcDim *dim = to.add_dim("index", (long) from_available);

        to.add_att("plain_data", "MAGIC");
        NcVar *data_var = to.add_var("data", ncByte, dim);

        if (global_attributes.find("type") == global_attributes.end())
            global_attributes["type"] = "plain_data";
        if (global_attributes.find("title") == global_attributes.end())
            global_attributes["title"] = "Some data";
        for (std::map<std::string, std::string>::iterator i =
                global_attributes.begin(); i != global_attributes.end(); i++)
        {
            to.add_att(i->first.c_str(), i->second.c_str());
        }

        {
            long cursor = 0;
            ssize_t got_from_file = 0;
            while ((got_from_file = read(from, buffer, read_at_once)) != 0)
            {
                if (got_from_file == -1)
                    throw FileError();

                data_var->set_cur(cursor);
                if (!data_var->put(buffer, got_from_file))
                    throw NetCDFFileError(
                            "failed to write to netcdf file");

                cursor += got_from_file;
            }

            to.close();
        }

        close(from);

        delete[] buffer;
    }

    void decode(const char *nc_file, int to,
            std::map<std::string, std::string> &global_attributes)
    {
        NcFile from_file(nc_file, NcFile::ReadOnly);
        if (!from_file.is_valid())
            throw NetCDFFileError("failed to open netcdf file");

        if (!is_encoded(nc_file))
            throw new NetCDFFileError("Not a binary encoded file");

        NcVar *data = from_file.get_var("data");

        long *shape = data->edges();

        long cursor = 0;
        ncbyte *buffer = new ncbyte[read_at_once];
        for (long to_get = shape[0]; to_get > 0; to_get -= read_at_once)
        {
            long shape_to_read = to_get < read_at_once ? to_get :
                read_at_once;

            data->set_cur(cursor);
            if (!data->get(buffer, shape_to_read))
                throw NetCDFFileError(
                        "failed to read from netcdf file");

            if (write(to, buffer, shape_to_read) != shape_to_read)
                throw FileError();

            cursor += read_at_once;
        }

        for (int i = 0; i < from_file.num_atts(); i++)
            if (from_file.get_att(i)->type() == ncChar ||
                    from_file.get_att(i)->type() == ncByte)
            {
                global_attributes[from_file.get_att(i)->name()] =
                    from_file.get_att(i)->as_string(0);
            }

        close(to);

        delete[] buffer;
    }

    bool is_netcdf_file(const char *nc_file)
    {
        NcFile from_file(nc_file, NcFile::ReadOnly);
        if (!from_file.is_valid())
            return false;

        return true;
    }

    bool is_encoded(const char *nc_file)
    {
        NcFile from_file(nc_file, NcFile::ReadOnly);

        if (!from_file.is_valid())
            return false;

        bool found_magic = false;
        for (int i = 0; i < from_file.num_atts(); i++)
            if (from_file.get_att(i)->name() == std::string("plain_data"))
                found_magic = true;

        if (!found_magic)
            return false;

        NcAtt *magic = from_file.get_att("plain_data");
        if (magic->type() != ncByte && magic->type() != ncChar)
            return false;
        if (!(std::string(magic->as_string(0)) == "MAGIC"))
            return false;

        if (from_file.num_vars() != 1)
            return false;
            // File has more than one variable!

        NcVar *data = from_file.get_var("data");
        if (data == NULL)
            return false;
            // File doesn't have the 'data' variable!

        if (data->num_dims() != 1)
            return false;
            // Variable data has more than one dimension!

        if (data->type() != ncByte)
            return false;

        if (std::string(data->get_dim(0)->name()) != "index")
            return false;

        return true;
    }
}
