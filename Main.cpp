#include "MT.h"
#include "NMEADevice.h"
#include "SwarmonDevice.h"
#include "SSC32.h"

int main(int argc, char* argv[]) 
{
	MT mt;
	MTDATA mtdata;
	NMEADEVICE nmeadevice;
	NMEADATA nmeadata;
	SWARMONDEVICE swarmondevice;
	SWARMONDATA swarmondata;
	SSC32 ssc32;
	double u1 = 0, u2 = 0;

	// Initialize to 0 all the fields of the structure.
	memset(&mt, 0, sizeof(MT));
	memset(&nmeadevice, 0, sizeof(NMEADEVICE));
	memset(&swarmondevice, 0, sizeof(SWARMONDEVICE));
	memset(&ssc32, 0, sizeof(SSC32));

	//ConnectMT(&mt, "MT0.txt");
	//ConnectNMEADevice(&nmeadevice, "NMEADevice0.txt");
	//ConnectSwarmonDevice(&swarmondevice, "SwarmonDevice0.txt");
	ConnectSSC32(&ssc32, "SSC320.txt");

	for (;;)
	{
		// Wait a little bit...
		mSleep(1000);

		//GetLatestDataMT(&mt, &mtdata);
		//printf("%f\n", mtdata.Yaw);

		//GetLatestDataNMEADevice(&nmeadevice, &nmeadata);
		//printf("%f;%f\n", nmeadata.Latitude, nmeadata.Longitude);

		//GetLatestDataSwarmonDevice(&swarmondevice, &swarmondata);
		//printf("%f;%f\n", swarmondata.Latitude, swarmondata.Longitude);

		u1 = 0.25;
		u2 = -0.25;
		SetThrustersSSC32(&ssc32, u1, u2);
	}

	DisconnectSSC32(&ssc32);
	//DisconnectSwarmonDevice(&swarmondevice);
	//DisconnectNMEADevice(&nmeadevice);
	//DisconnectMT(&mt);

	return EXIT_SUCCESS;
}
