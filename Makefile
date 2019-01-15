# Makefile for Linux, designed for Ubuntu 16.04. 
# You might need to install C/C++ development tools by typing :
#    sudo apt-get install build-essential
# in a terminal.
# For more information on the configuration used, see http://www.ensta-bretagne.fr/lebars/Share/Ubuntu.txt .
# Use dos2unix *.txt to ensure line endings are correct for Linux in the configuration files.

PROGS = Test_devices

CC = gcc
CXX = g++

CFLAGS += -g
#CFLAGS += -O3
CFLAGS += -Wall -Wno-unknown-pragmas -Wextra
#CFLAGS += -Winline

#CFLAGS += -D _DEBUG -D _DEBUG_DISPLAY 
#CFLAGS += -D _DEBUG_MESSAGES 
CFLAGS += -D DISABLE_THREADS_OSNET
CFLAGS += -D SIMULATED_INTERNET_SWARMONDEVICE
CFLAGS += -D DISABLE_AIS_SUPPORT
CFLAGS += -D ENABLE_DEFAULT_SAVE_RAW_DATA_MDM
CFLAGS += -D DISABLE_HOKUYOTHREAD
CFLAGS += -D DISABLE_IM483ITHREAD
CFLAGS += -D DISABLE_MDMTHREAD
CFLAGS += -D DISABLE_MINISSCTHREAD
CFLAGS += -D DISABLE_MTTHREAD
CFLAGS += -D DISABLE_NMEADEVICETHREAD
CFLAGS += -D DISABLE_P33XTHREAD
CFLAGS += -D DISABLE_PATHFINDERDVLTHREAD
CFLAGS += -D DISABLE_POLOLUTHREAD
CFLAGS += -D DISABLE_RAZORAHRSTHREAD
CFLAGS += -D DISABLE_RPLIDARTHREAD
CFLAGS += -D DISABLE_SEANETTHREAD
CFLAGS += -D DISABLE_SSC32THREAD
CFLAGS += -D DISABLE_SWARMONDEVICETHREAD
CFLAGS += -D DISABLE_UBLOXTHREAD

# Depending on your OS (old Linux or Mac OS)...
#CFLAGS += -DUSE_OLD_CHRONO

CXXFLAGS += $(CFLAGS) -fpermissive

# For Windows/MinGW
#LDFLAGS += -lws2_32

# For Linux, if static needed...
#LDFLAGS += -static-libgcc -static-libstdc++ -static

# Might need to remove -lrt for Mac OS...
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

Main.o: Main.cpp OSCore.h OSTime.h OSMisc.h OSNet.h OSComputerRS232Port.h Hokuyo.h IM483I.h MDM.h MiniSSC.h MT.h NMEADevice.h NMEAProtocol.h P33x.h PathfinderDVL.h Pololu.h RazorAHRS.h RPLIDAR.h RS232Port.h Seanet.h SSC32.h SwarmonDevice.h ublox.h UBXProtocol.h 
	$(CXX) $(CXXFLAGS) -c $<

Test_devices: Main.o OSNet.o OSComputerRS232Port.o OSMisc.o OSTime.o OSCore.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(PROGS) $(PROGS:%=%.elf) $(PROGS:%=%.exe) *.o *.obj core *.gch
