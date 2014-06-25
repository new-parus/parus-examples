include ../../../config

EXECS= wav2nc module

INCLUDES= -I $(NETCDF_HOME)/include -I .
LIB_DIR= -L $(NETCDF_HOME)/lib

LIBS= -lnetcdf_c++ -lnetcdf

all: $(EXECS)

wav2nc: converter.cpp
	echo $(NETCDF_HOME)
	$(MPI_CC) $(INCLUDES) -o wav2nc converter.cpp $(FILES) $(LIB_DIR) $(LIBS)

module: module.cpp
	$(MPI_CC) $(INCLUDES) -o module  module.cpp $(FILES) $(LIB_DIR) $(LIBS)

clean:
	rm -f $(EXECS) *.o

