# Makefile for Linux, tested with Ubuntu 15.10. 
# You might need to install C/C++ development tools by typing :
#    sudo apt-get install build-essential
# in a terminal.
# For more information on the configuration used, see www.ensta-bretagne.fr/lebars/Share/Ubuntu.txt .
# Use dos2unix *.txt to ensure line endings are correct for Linux in the configuration files.

PROGS = Test_devices

CC = gcc
CXX = g++

CFLAGS += -g
#CFLAGS += -O3
CFLAGS += -Wall -Wno-unknown-pragmas
#CFLAGS += -Wextra -Winline

#CFLAGS += -D _DEBUG -D _DEBUG_DISPLAY 
#CFLAGS += -D _DEBUG_MESSAGES 
CFLAGS += -D DISABLE_THREADS_OSNET
CFLAGS += -D SIMULATED_INTERNET_SWARMONDEVICE
CFLAGS += -D DISABLE_AIS_SUPPORT
CFLAGS += -D ENABLE_DEFAULT_SAVE_RAW_DATA_MDM
CFLAGS += -D DISABLE_MTTHREAD
CFLAGS += -D DISABLE_RAZORAHRSTHREAD
CFLAGS += -D DISABLE_NMEADEVICETHREAD
CFLAGS += -D DISABLE_SWARMONDEVICETHREAD
CFLAGS += -D DISABLE_P33XTHREAD
CFLAGS += -D DISABLE_SSC32THREAD
CFLAGS += -D DISABLE_MAESTROTHREAD
CFLAGS += -D DISABLE_MINISSCTHREAD
CFLAGS += -D DISABLE_MDMTHREAD
CFLAGS += -D DISABLE_HOKUYOTHREAD
CFLAGS += -D DISABLE_RPLIDARTHREAD
CFLAGS += -D DISABLE_SEANETTHREAD

CXXFLAGS += $(CFLAGS) -fpermissive

# For Windows/MinGW
#LDFLAGS += -lws2_32
# For Linux
LDFLAGS += -lrt -lm

default: $(PROGS)

############################# OSUtils #############################

OSComputerRS232Port.o: OSComputerRS232Port.c OSComputerRS232Port.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSCore.o: OSCore.c OSCore.h
	$(CC) $(CFLAGS) -c $<

OSMisc.o: OSMisc.c OSMisc.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSNet.o: OSNet.c OSNet.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSTime.o: OSTime.c OSTime.h OSCore.o
	$(CC) $(CFLAGS) -c $<

############################# PROGS #############################

Main.o: Main.cpp OSCore.h OSTime.h OSMisc.h OSNet.h OSComputerRS232Port.h RS232Port.h MT.h RazorAHRS.h NMEADevice.h NMEAProtocol.h SwarmonDevice.h P33x.h SSC32.h Maestro.h MiniSSC.h MDM.h Hokuyo.h RPLIDAR.h Seanet.h 
	$(CXX) $(CXXFLAGS) -c $<

Test_devices: Main.o OSNet.o OSComputerRS232Port.o OSMisc.o OSTime.o OSCore.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(PROGS) $(PROGS:%=%.elf) $(PROGS:%=%.exe) *.o *.obj core *.gch
