
PROGS = Test_devices

CC = g++
CFLAGS += -g
#CFLAGS += -O3
CFLAGS += -Wall
#CFLAGS += -Wextra -Winline

#CFLAGS += -D _DEBUG -D _DEBUG_DISPLAY -D _DEBUG_MESSAGES 
CFLAGS += -D DISABLE_THREADS_OSNET
CFLAGS += -D SIMULATED_INTERNET_SWARMONDEVICE
CFLAGS += -D DISABLE_MTTHREAD
CFLAGS += -D DISABLE_RAZORAHRSTHREAD
CFLAGS += -D DISABLE_NMEADEVICETHREAD
CFLAGS += -D DISABLE_SWARMONDEVICETHREAD
CFLAGS += -D DISABLE_P33XTHREAD
CFLAGS += -D DISABLE_SSC32THREAD
CFLAGS += -D DISABLE_MAESTROTHREAD
CFLAGS += -D DISABLE_MINISSCTHREAD
CFLAGS += -D DISABLE_MDMTHREAD

# For Windows/MinGW
LDFLAGS += -lws2_32
# For Linux
#LDFLAGS += -lrt -lm

default: $(PROGS)

############################# OSUtils #############################

OSComputerRS232Port.o: OSComputerRS232Port.c OSComputerRS232Port.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSCore.o: OSCore.c OSCore.h
	$(CC) $(CFLAGS) -c $<

OSCriticalSection.o: OSCriticalSection.c OSCriticalSection.h OSThread.o
	$(CC) $(CFLAGS) -c $<

OSMisc.o: OSMisc.c OSMisc.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSNet.o: OSNet.c OSNet.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSTime.o: OSTime.c OSTime.h OSCore.o
	$(CC) $(CFLAGS) -c $<

############################# PROGS #############################

Main.o: Main.cpp OSCore.h OSTime.h OSMisc.h OSNet.h OSComputerRS232Port.h RS232Port.h MT.h RazorAHRS.h NMEADevice.h SwarmonDevice.h P33x.h SSC32.h Maestro.h MiniSSC.h MDM.h 
	$(CC) $(CFLAGS) -c $<

Test_devices: Main.o OSNet.o OSComputerRS232Port.o OSMisc.o OSTime.o OSCore.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(PROGS) $(PROGS:%=%.elf) $(PROGS:%=%.exe) *.o *.obj core *.gch
