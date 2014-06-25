//Config header

//Standard headers
#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <ctype.h>
//#include <fcntl.h>
//#include <errno.h>
//#include <math.h>

//Platform-depended headers
//#ifdef _WIN32
//	#include <sys\types.h>
//	#include <sys\time.h>
//	#include <sys\stat.h>
//#else
//	#include <sys/resource.h>
//	#include <sys/wait.h>
//	#include <sys/types.h>
//	#include <sys/time.h>
//	#include <sys/stat.h>
//#endif

//STL headers
#include <map>
#include <string>

//NetCDF headers
//#include <netcdf.h>

#include <netcdfcpp.h>
#include "config.h"

//Namespace conjunction directives
using namespace std;

//Type definitions
//Description of WAV file format see at the end of file
typedef	struct    //ChunkHdr - Wave File Header
{
	char id[4];     //Have to be "RIFF"  - Resource Interchange File Format
	long len;       //Size of rest of file
	char waveID[4]; //Have to be "WAVE" - We expect wave data
} ChunkHdr;

typedef struct               //Wave format structure
{
	char chunkID[4];           //Chunk ID - "fmt "
	int32_t chunkSize;         //Size of chunk - 16
	int16_t wFormatTag;        //Wave format. 1 is PCM and we expect it.
	uint16_t wChannels;        //Number of channels. Only mono (1 channel) is now supported
	uint32_t dwSamplesPerSec;  //Number of samples per second. Have to be 44100
	uint32_t dwAvgBytesPerSec; //Average byterate. Calculated as (dwSamplesPerSec*wChannels*wBitsPerSample)/8
	uint16_t wBlockAlign;      //Alignment of block. Calculated as (wChannels*wBitsPerSample)/8
	uint16_t wBitsPerSample;   //Bits Per sample. Value of 16 is expected.
} FormatChunk;

typedef struct               //Chunk header structure
{
	char chunkID[4];           //Chunk ID, e. g. "data"
	uint32_t chunkSize;        //Size of remaining part of chunk
} DataChunk;

//Constant definitions
const int ERROR_GENERAL_INVALID_PARAMS = 1; //Number of params is less than 2 (input and output files)
const int ERROR_GENERAL_INVALID_FORMAT = 2; //Invalid format of input file
const int ERROR_GENERAL_NETCDF_ERROR = 100; //NetCDF error codes are in -64..-33 range

const int ERROR_WAV2NC_CANT_OPEN_WAV = 11;      //Error occured dyring trying to open input wav file for reading
const int ERROR_WAV2NC_INVALID_WAV_FORMAT = 12; //Invalid file format. Can happen if some file params (bits per sample, number of channel) are not supported

const int ERROR_NC2WAV_CANT_OPEN_WAV = 21; //Error occured dyring trying to open output wav file for writing

//Chunk headers
const char * WAV_HEADER_FILE_SIGNATURE = "RIFF";
const char * WAV_HEADER_TYPE_SIGNATURE = "WAVE";
const char * WAV_HEADER_FMT_SIGNATURE  = "fmt ";
const char * WAV_HEADER_DATA_SIGNATURE = "data";

//Global variable definitions
map <string, string> attributes;
int error_happened = 0; //Used to indicate happening of non-fatal errors during processing. It is useful to output all file format errors (not only first) before terminating

//Functionf declarations
//Error reporting functions
void CheckNCStatus(int status, char * hint = 0); //Checks NetCDF function call and exits, if error happened. Also prints some info if error occured
void ErrorOut(char * msg);                       //Outputs error on stderr and increment number of errors (error_happened global variable)

//Utility functions for wav2nc and nc2wav
void AssignID(char a[4], const char * b); //Assigns first 4 chars of string b to string a

//Utility parameter parsing function
char * GetExtension(char * path); //Return extension of file which is given as parameter. Note, that this function returns part of given string, not creates new string
char * StrToUpper(char * str);   //Upcases given string

//Main processing functions
void wav2nc(char * wav, char * nc); //Converts from WAV to NetCDF format
void nc2wav(char * nc, char * wav); //Converts from NetCDF to WAV format

//Function to handle NetCDF errors
void CheckNCStatus(int status, char * hint)
{
	if (status!=NC_NOERR)
	{
		fprintf(stderr, "NetCDF error: %s\n", nc_strerror(status));
		if (hint)
		{
			fprintf(stderr, "Additional info: %s\n", hint);
		}
		//create_empty_netcdf_file(out_name.c_str(), "nothing");
		exit(ERROR_GENERAL_NETCDF_ERROR+status);
	}
}

//Function to print error information
void ErrorOut(char * msg)
{
	fprintf(stderr,"%s\n",msg);
	error_happened++;
}

//Assigns first 4 chars of string b to string a
void AssignID(char a[4], const char * b)
{
	a[0] = b[0];
	a[1] = b[1];
	a[2] = b[2];
	a[3] = b[3];
}

//Convert wav -> nc
void wav2nc(char * wav,char * nc)
{
	//Variables
	FILE * fwav = 0;
	ChunkHdr chnkhdr;
	FormatChunk fmtchnk;
	DataChunk datchnk;

	size_t i = 0, index = 0;
	short iBuf = 0;

	int ncid = 0, pid = 0, timeid = 0, timevarid = 0;
	float t0 = 0, dt = 1;

	char * type = "uniform timescale process";
	char * title = strdup(attributes["title"].c_str());
	/* TODO (eSyr#1#): Выяснить, зачем нужны title и attributes */

	//Start of function code

	//Reading...
	//Opening file for reading
	if (!(fwav=fopen(wav,"rb")))
	{
	  ErrorOut("Can't open wav file for reading");
	  exit(ERROR_WAV2NC_CANT_OPEN_WAV);
	}

	//Reading headers
	fread(&chnkhdr,sizeof(ChunkHdr),1,fwav);
	fread(&fmtchnk,sizeof(FormatChunk),1,fwav);
	fread(&datchnk,sizeof(DataChunk),1,fwav);

	#ifdef _CONFIG_DEBUG_
		printf("File type '%c%c%c%c'\n", chnkhdr.id[0], chnkhdr.id[1], chnkhdr.id[2], chnkhdr.id[3]);
	#endif

	/*do //I don`t know for what this code is needed, because in WAV format description data chunk is right after fmt chunk
	{
		fread(&datchnk,sizeof(DataChunk),1,fwav);

		#ifdef _CONFIG_DEBUG_
			printf("Feld type '%c%c%c%c'\n", datchnk.chunkID[0], datchnk.chunkID[1], datchnk.chunkID[2], datchnk.chunkID[3]);
		#endif

		if (strncmp(datchnk.chunkID, "data", 4))
		{
			size_t i;
			char buf;
			for (i = 0; i < datchnk.chunkSize; i++)
				fread(&buf, 1, 1, fwav);
		}
	}
	while (strncmp(datchnk.chunkID, "data", 4));*/

	//File format analyzing...

	//Some additional variables
	char temp[5], * buf;

	memcpy(temp, chnkhdr.waveID, 4);
	temp[4] = 0;

	//Checking file type (WAVE or other RIFF format; WAVE is expected)
	#ifdef _CONFIG_DEBUG_
		printf("File type '%s'\n", temp);
	#endif

	if (strncmp("WAVE",chnkhdr.waveID,4))
	  ErrorOut("Format error: 'WAVE' data signature expected");

	//Checking WAVE encoding format (1 - PCM expected)
	#ifdef _CONFIG_DEBUG_
	  printf("Format type (1 - PCM) = %d\n",fmtchnk.wFormatTag);
	#endif

	if (fmtchnk.wFormatTag!=1)
	  ErrorOut("Format error: PCM wave file expected");

	//Number of channels in file (1 channel expected)
	#ifdef _CONFIG_DEBUG_
	  printf("Number of channels = %d\n",fmtchnk.wChannels);
	#endif

	if (fmtchnk.wChannels!=1)
	  ErrorOut("Format error: single-channel wave file expected");

	//Checking Bits Per Sample, 16 expected
	#ifdef _CONFIG_DEBUG_
	  printf("Bits per sample = %d\n",fmtchnk.wBitsPerSample);
	#endif

	if (fmtchnk.wBitsPerSample!=16)
	  ErrorOut("Format error: 16 bits per sample wave file expected");

	//Some debug info
	#ifdef _CONFIG_DEBUG_
	  printf("Samples per second = %d\n",fmtchnk.dwSamplesPerSec);
	  fprintf(stderr, "chunkSize is %lu\n", datchnk.chunkSize);
	#endif

	//If errors during check occured, show info about number of errors and terminate
	if (error_happened)
	{
		printf("%d file format errors total.\n",error_happened);
		//create_empty_netcdf_file(out_name.c_str(), "nothing");
		exit(ERROR_WAV2NC_INVALID_WAV_FORMAT);
	}

	dt = 1.0 / (double)fmtchnk.dwSamplesPerSec;

	//Converting...

	//Creating NetCDF file and preparing it for writing WAV data. Also some data parameters are written
	CheckNCStatus(nc_create(nc,NC_CLOBBER,&ncid), "Error occured while trying to create output NetCDF file");

	CheckNCStatus(nc_def_dim(ncid,"time",datchnk.chunkSize/fmtchnk.wBlockAlign,&timeid), "Error occured while trying to define new dimension 'time'");

	CheckNCStatus(nc_def_var(ncid,"time",NC_SHORT,0,NULL,&timevarid), "Error occured while trying to define new variable 'time'");
	CheckNCStatus(nc_put_att_text(ncid,timevarid,"units",3,"sec"), "Error occured while trying to put attribute 'units' to variable 'time'");

	CheckNCStatus(nc_def_var(ncid,"amplitude",NC_SHORT,1,&timeid,&pid), "Error occured while trying to define new variable 'amplitude'");

	CheckNCStatus(nc_put_att_text(ncid,NC_GLOBAL,"type",strlen(type),type), "Error occured while trying to put global attribute 'type'");
	CheckNCStatus(nc_put_att_text(ncid,NC_GLOBAL,"title",strlen(title),title), "Error occured while trying to put global attribute 'title'");
	CheckNCStatus(nc_put_att_float(ncid,NC_GLOBAL,"t0",NC_FLOAT,1,&t0), "Error occured while trying to put global attribute 't0'");
	CheckNCStatus(nc_put_att_float(ncid,NC_GLOBAL,"dt",NC_FLOAT,1,&dt), "Error occured while trying to put global attribute 'dt'");

	CheckNCStatus(nc_enddef(ncid), "Error occured while trying to exit definition mode");

	//Writing wave data itself
	for(i=index=0; i<datchnk.chunkSize; i+=fmtchnk.wBlockAlign, index++)
	{
		/*#ifdef _CONFIG_DEBUG_
		  printf("ftell: %d %d %d\n",ftell(fwav), i, datchnk.chunkSize);
		#endif*/

		fread(&iBuf,2,1,fwav);
		nc_put_var1_short(ncid,pid,&index,&iBuf);
	}

	//Cleanup
	fclose(fwav);
	CheckNCStatus(nc_close(ncid));
}

/* Convert nc -> wav */
void nc2wav(char * nc,char * wav)
{
	//Variables
	FILE * fwav = 0;
	ChunkHdr chnkhdr;
	FormatChunk fmtchnk;
	DataChunk datchnk;

	short iBuf = 0;

	int ncid = 0, pid = 0, timeid = 0, timevarid = 0;
	size_t timeidlen = 0;
	float t0 = 0, dt = 1;

	size_t units_len = 0, type_len = 0;
	char * units = 0, * type = 0;

	//Opening NetCDF file end reading required metadata
	CheckNCStatus(nc_open(nc, NC_NOWRITE, &ncid), "Error occured while trying to open input file");
	CheckNCStatus(nc_inq_dimid(ncid,"time",&timeid), "Error occured while trying to access 'time' dimension");
	CheckNCStatus(nc_inq_dimlen(ncid, timeid, &timeidlen), "Error occured while trying to get 'time' dimension length");
	CheckNCStatus(nc_inq_varid(ncid,"time",&timevarid), "Error occured while trying to access 'time' dimension");
	CheckNCStatus(nc_inq_attlen(ncid,timevarid,"units",&units_len), "Error occured while trying to get 'units' attribute length");
	units = (char *)malloc(units_len+1);
	CheckNCStatus(nc_get_att_text(ncid,timevarid,"units",units), "Error occured while trying to get 'units' attribute value");
	CheckNCStatus(nc_inq_attlen(ncid,NC_GLOBAL,"type",&type_len), "Error occured while trying to get 'type' attribute length");
	type = (char *)malloc(type_len+1);
	CheckNCStatus(nc_get_att_text(ncid,NC_GLOBAL,"type",type), "Error occured while trying to get 'type' attributer value");
	CheckNCStatus(nc_get_att_float(ncid,NC_GLOBAL,"dt",&dt), "Error occured while trying to get 'dt' attribute");

	//Opening WAV file for writing
	fwav = fopen(wav, "wb");
	
	if (!fwav)
	{
	  ErrorOut("Can`t open wavefile in write mode.");
	  exit(ERROR_NC2WAV_CANT_OPEN_WAV);
	}

	//Filling WAV file header
	AssignID(fmtchnk.chunkID, WAV_HEADER_FMT_SIGNATURE);
	fmtchnk.chunkSize = 16;
	fmtchnk.wFormatTag = 1; //PCM
	fmtchnk.wChannels = 1; //Mono
	fmtchnk.wBitsPerSample = 16;
	fmtchnk.wBlockAlign = (fmtchnk.wChannels * fmtchnk.wBitsPerSample) >> 3;
  fmtchnk.dwSamplesPerSec = (uint32_t)round((double)1.0 / dt);
  fmtchnk.dwAvgBytesPerSec = (fmtchnk.wChannels * fmtchnk.dwSamplesPerSec * fmtchnk.wBitsPerSample) >> 3;

  AssignID(chnkhdr.id, WAV_HEADER_FILE_SIGNATURE);
	chnkhdr.len = strlen(WAV_HEADER_TYPE_SIGNATURE)+strlen(WAV_HEADER_FMT_SIGNATURE)+16+strlen(WAV_HEADER_DATA_SIGNATURE)+timeidlen*fmtchnk.wBlockAlign;
  AssignID(chnkhdr.waveID, WAV_HEADER_TYPE_SIGNATURE);

	AssignID(datchnk.chunkID, WAV_HEADER_DATA_SIGNATURE);
	datchnk.chunkSize = timeidlen*fmtchnk.wBlockAlign;

	//Writing header to file
	fwrite(&chnkhdr, sizeof(ChunkHdr), 1, fwav);
	fwrite(&fmtchnk, sizeof(FormatChunk), 1, fwav);
	fwrite(&datchnk, sizeof(DataChunk), 1, fwav);

	//Preparing to read dara
	CheckNCStatus(nc_inq_varid(ncid,"amplitude",&pid), "Error occured while trying to access 'amplitude' variable");

	//Reading samples information from NetCDF file and writing it to WAV file
	for (size_t i = 0, index = 0; i<timeidlen; i++, index++)
	{
		nc_get_var1_short(ncid,pid,&index,&iBuf);
		fwrite(&iBuf, 2, 1, fwav);
	}

	//Cleanup
	fclose(fwav);
	free(units);
	free(type);
}

//Return extension of file which is given as parameter. Note, that this function returns part of given string, not creates new string
char * GetExtension(char * path)
{
	
	/*

	char * pos = path, * buf = 0;
	while ((buf=strstr(pos,".")))
	{
		pos=buf+1;
	}
	return pos;
	*/
	
	int i;
	
	char *extension=NULL;
	
	int len=strlen(path);
	
	for(i=len;i>=0;i--)
	{
		if(path[i]=='.')
		 break;
	}
	
	extension=(char *)malloc((len-i+1)*sizeof(char));
	strcpy(extension,&path[i+1]);
	return extension;
}

//Warning: this procedure changes contents of string!
char * StrToUpper(char * str)
{
	for (int i = 0; i<strlen(str); i++)
	{
		str[i] = toupper(str[i]);
	}
	
	return str;
}

int main(int argc, char ** argv)
{
	//Checking number of arguments
	if(argc<3)
	{
		fprintf(stderr, "Usage:\n\t%s <input wav file name> <output NetCDF file name>\n\t<input NetCDF file name> <output wav file name>\n", argv[0]);
		return ERROR_GENERAL_INVALID_PARAMS;
	}

	if(argc>3)
	{
		fprintf(stderr, "Only first two parameters used, following ignored.\n");
	}

	//Parsing arguments
	if (!strcmp(StrToUpper(GetExtension(argv[1])),"WAV"))
	{
		wav2nc(argv[1],argv[2]);
	}
	else
	{
		if (!strcmp(StrToUpper(GetExtension(argv[1])),"NC"))
		{
			nc2wav(argv[1],argv[2]);
		}
		else
		{
			fprintf(stderr, "Invalid format of input file (*.wav or *.nc expected). Aborting.\n");
			return ERROR_GENERAL_INVALID_FORMAT;
		}
	}
	
	return 0;	
}

/*
WAVE format description --------------------------------------------------------

WAVE file, as every RIFF file, consists of chunks. Every chunk has ID and size,
where size do not include size of 4-byte ID and DWORD size. Chunks organized in
hierachical structure:

RIFF
|-WAVE   - Wave data
| |-fmt  - format
| |-data - content
|-LIST  \
| |...   } Optional data
|-INFO  /
|
...


Offset   Name        Type   Value Description
00h      RIFF Header DWORD  RIFF  RIFF format header. Signalizes that file is in
                                  Resource Interchange File Format
04h      File size   DWORD        Size of file excluding RIFF Header and File
                                  size records, so can be from 00000000h up to
                                  FFFFFFFFh
08h      WAVE Header DWORD  WAVE  WAVE format headr. Indicates that file
                                  contains WAVE data
0Ch      WAVE format struct       WAVE format structure/ Contains paramters of
                                  WAVE data (size of sample, channels etc.)
  +00h   Identifyer  DWORD  fmt   structure ID. Has space (20h) as forth
                                  character.
  +04h   Chunck size DWORD  10h   size of structure
  +08h   Format tage WORD         Format type of wave data. 1 for PCM.
  +0Ah   Channels    WORD         Number of channels. 01h for mono, 02h for
                                  stereo
  +0Ch   Sample rate DWORD        Number of samples per second. Standard values
                                  are 2000, 4000, 8000, 12000, 16000, 32000,
                                  48000, 96000, 192000, 11025, 22050, 44100. Can
                                  be nonstandard, but not all hardware and
                                  applications support nonstandard sample rate.
  +10h   Byterate    DWORD        Average stream size. Calculated as
                                  (dwSamplesPerSec*wChannels*wBitsPerSample)/8
  +14h   Block align WORD         Alignment of block (alignment between samples
                                  of each channel). Calculated as
                                  (wChannels*wBitsPerSample)/8
  +16h   Sample size WORD         Size of sample in bits. Can be 8 or 16.
24h      WAVE data   struct       WAVE data itself (samples). Note, that stereo
                                  is stored as Left channel sample, Right
                                  channel sample, Left channel sample and so on
	+00h   Identifyer  DWORD  data  Chunk ID
	+04h   Chunck size DWORD        Size of remaining part chunk
	+08h   Samples                  Samples themselves

*/
