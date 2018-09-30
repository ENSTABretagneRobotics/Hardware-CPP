Hardware-CPP
============
- If you have Visual Studio, open the .sln file.
- If you have Qt (with MinGW for Windows), open the .pro file (and possibly put the configuration files in the generated ../Test_devices-build-desktop folder).
- If you have only gcc, change the Makefile to adapt to your OS if needed and run make.

Change the device path and other parameters in the configuration files if necessary and comment/uncomment lines in Main.cpp depending on the device you wish to test. Mind the line endings in the configuration files depending on the OS (use e.g. the command dos2unix *.txt to convert line endings for Linux)!

Hardware support : 
- Hokuyo : Hokuyo URG-04LX-UG01 laser telemeter.
- IM483I : Intelligent Motion Systems IM483I step motor controller.
- MDM : Tritech Micron Data Modem (or other kinds of simple RS232 modems).
- MT : Xsens MTi, MTi-G AHRS.
- NMEADevice (superseded by ublox) : GPS, Furuno WS200 weather station.
- P33x : Keller pressure sensor PAA-33x.
- PathfinderDVL : TRDI Pathfinder DVL.
- Pololu : Pololu Mini Maestro 6, 18, 24 servo controllers, Pololu Jrk (preliminary support).
- RazorAHRS : SparkFun 9DOF Razor IMU (flash firmware from https://github.com/lebarsfa/razor-9dof-ahrs if needed).
- RPLIDAR : RPLIDAR A1 and A2 laser telemeters.
- Seanet : Tritech Micron Sonar, Tritech MiniKing Sonar.
- SSC-32 : Lynxmotion SSC-32, SSC-32u servo controllers.
- ublox : ublox GPS, Furuno WS200 weather station, or other NMEA-compatible devices with supported NMEA sentences.

See also https://github.com/ENSTABretagneRobotics/Hardware-MATLAB, https://github.com/ENSTABretagneRobotics/Hardware-Python, https://github.com/ENSTABretagneRobotics/Hardware-Java, https://github.com/ENSTABretagneRobotics/Android.
