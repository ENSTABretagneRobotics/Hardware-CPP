# Designed for Ubuntu 18.04. 
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
CFLAGS += -D DISABLE_MESTHREAD
CFLAGS += -D DISABLE_MINISSCTHREAD
CFLAGS += -D DISABLE_MS5837THREAD
CFLAGS += -D DISABLE_MTTHREAD
CFLAGS += -D DISABLE_NMEADEVICETHREAD
CFLAGS += -D DISABLE_P33XTHREAD
CFLAGS += -D DISABLE_PATHFINDERDVLTHREAD
CFLAGS += -D DISABLE_POLOLUTHREAD
CFLAGS += -D DISABLE_RAZORAHRSTHREAD
CFLAGS += -D DISABLE_ROBOTEQTHREAD
CFLAGS += -D DISABLE_RPLIDARTHREAD
CFLAGS += -D DISABLE_SBGTHREAD
CFLAGS += -D DISABLE_SEANETTHREAD
CFLAGS += -D DISABLE_SSC32THREAD
CFLAGS += -D DISABLE_SWARMONDEVICETHREAD
CFLAGS += -D DISABLE_UBLOXTHREAD
CFLAGS += -D DISABLE_VELODYNETHREAD

# Depending on your OS (old Linux or macOS)...
#CFLAGS += -D USE_OLD_CHRONO

CXXFLAGS += $(CFLAGS) -fpermissive

# For Linux, if static needed...
#LDFLAGS += -static-libgcc -static-libstdc++ -static

# For MinGW
#LDFLAGS += -lws2_32 -lm

# For macOS
#LDFLAGS += -lm

# For Linux
CFLAGS += -D ENABLE_COMPUTERI2CBUS_SUPPORT
LDFLAGS += -lrt -lm

default: $(PROGS)

############################# OSUtils #############################

OSCore.o: OSCore.c OSCore.h
	$(CC) $(CFLAGS) -c $<

OSMisc.o: OSMisc.c OSMisc.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSNet.o: OSNet.c OSNet.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSTime.o: OSTime.c OSTime.h OSCore.o
	$(CC) $(CFLAGS) -c $<

############################# PROGS #############################

Main.o: Main.cpp OSCore.h OSTime.h OSMisc.h OSNet.h OSComputerRS232Port.h OSComputerI2CBus.h Hokuyo.h IM483I.h MDM.h MES.h MiniSSC.h MS5837.h MT.h NMEADevice.h NMEAProtocol.h P33x.h PathfinderDVL.h Pololu.h RazorAHRS.h Roboteq.h RPLIDAR.h RS232Port.h SBG.h Seanet.h SSC32.h SwarmonDevice.h ublox.h UBXProtocol.h Velodyne.h 
	$(CXX) $(CXXFLAGS) -c $<

Test_devices: Main.o OSNet.o OSMisc.o OSTime.o OSCore.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(PROGS) $(PROGS:%=%.elf) $(PROGS:%=%.exe) *.o *.obj core *.gch
