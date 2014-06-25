#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <sys\types.h>
#include <sys\time.h>
//#include <sys\resource.h>
//#include <sys\wait.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <netcdfcpp.h>

#include "netcdf_binary_codec.hh"
#include "empty_file.hh"

#include <string>
#include <map>

using namespace std;

extern void wav2nc(char *wav_file_name,char *netcdf_file_name);

extern string in_name;
extern string out_name;
extern map<string, string> attributes;




extern void H(int status);
extern void error(char *msg);


 
int main(int argc, char **argv)
{
    printf("Content-type: text/plain\n\n");

    char header_file[1024];
    if (argc == 1)
    {
        fscanf(stdin, "%s", header_file);
    }
    else
    {
        strcpy(header_file, argv[1]);
    }

    {
        NcFile header(header_file);
        if (!header.is_valid())
        {
            error("failed to open header");
        }

        in_name = header.get_var("in")->get_att("name")->as_string(0);
        out_name = header.get_var("out")->get_att("name")->as_string(0);
    }

    if (NetCDFBinaryCodec::is_encoded(in_name.c_str()))
    {
        int fd_out = open((out_name + ".tmp").c_str(), O_WRONLY | O_CREAT |
                O_TRUNC, 0666);
        if (fd_out == -1)
            error("failed to create a file");

        NetCDFBinaryCodec::decode(in_name.c_str(), fd_out, attributes);

        wav2nc(strdup((out_name + ".tmp").c_str()),
                strdup(out_name.c_str()));
        remove((out_name + ".tmp").c_str());
    }
    else
    {
        create_empty_netcdf_file(out_name.c_str(), "nothing");
    }

    printf("OK\n");

    return 0;
}
