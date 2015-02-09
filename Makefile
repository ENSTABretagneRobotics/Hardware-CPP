
PROGS = Hovercraft_devices

CC = g++
CFLAGS += -g
#CFLAGS += -O3
CFLAGS += -Wall
#CFLAGS += -Wextra -Winline

#CFLAGS += -D _DEBUG -D _DEBUG_DISPLAY -D _DEBUG_MESSAGES 
CFLAGS += -D SIMULATED_INTERNET_SWARMONDEVICE
CFLAGS += -D DISABLE_MTTHREAD
CFLAGS += -D DISABLE_NMEADEVICETHREAD
CFLAGS += -D DISABLE_SWARMONDEVICETHREAD
CFLAGS += -D DISABLE_SSC32THREAD

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

OSThread.o: OSThread.c OSThread.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSTime.o: OSTime.c OSTime.h OSCore.o
	$(CC) $(CFLAGS) -c $<

############################# PROGS #############################

Main.o: Main.cpp MT.h NMEADevice.h SwarmonDevice.h SSC32.h
	$(CC) $(CFLAGS) -c $<

Hovercraft_devices: Main.o OSTime.o OSNet.o OSMisc.o OSCore.o OSComputerRS232Port.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(PROGS) $(PROGS:%=%.elf) $(PROGS:%=%.exe) *.o *.obj core *.gch
