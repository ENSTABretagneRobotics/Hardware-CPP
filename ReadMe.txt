If you have Visual Studio, open the .sln file.
If you have Qt (with MinGW), open the .pro file (and put the configuration files in the generated ../Test_devices-build-desktop folder).
If you have only gcc, change the Makefile to adapt to your OS if needed and run make.

Change the device path in the configuration files if necessary and comment/uncomment lines in Main.cpp depending on the device you wish to test.
