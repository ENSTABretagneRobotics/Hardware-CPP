#include "MT.h"
#include "RazorAHRS.h"
#include "NMEADevice.h"
#include "SwarmonDevice.h"
#include "P33x.h"
#include "SSC32.h"
#include "Maestro.h"
#include "MiniSSC.h"
#include "MDM.h"
#include "Hokuyo.h"

// Comment/uncomment lines depending on the device you wish to test...

int main(int argc, char* argv[]) 
{
	MT mt;
	MTDATA mtdata;
	RAZORAHRS razorahrs;
	RAZORAHRSDATA razorahrsdata;
	NMEADEVICE nmeadevice;
	NMEADATA nmeadata;
	SWARMONDEVICE swarmondevice;
	SWARMONDATA swarmondata;
	P33X p33x;
	double value = 0;
	int ivalue = 0;
	SSC32 ssc32;
	MAESTRO maestro;
	MINISSC minissc;
	double u1 = 0.25, u2 = -0.25;
	MDM mdm;
	char b = 0;
	HOKUYO hokuyo;
	double angles[MAX_SLITDIVISION_HOKUYO];
	double distances[MAX_SLITDIVISION_HOKUYO];

	// Initialize to 0 all the fields of the structure.
	memset(&mt, 0, sizeof(MT));
	memset(&razorahrs, 0, sizeof(RAZORAHRS));
	memset(&nmeadevice, 0, sizeof(NMEADEVICE));
	memset(&swarmondevice, 0, sizeof(SWARMONDEVICE));
	memset(&p33x, 0, sizeof(P33X));
	memset(&ssc32, 0, sizeof(SSC32));
	memset(&maestro, 0, sizeof(MAESTRO));
	memset(&minissc, 0, sizeof(MINISSC));
	memset(&mdm, 0, sizeof(MDM));
	memset(&hokuyo, 0, sizeof(HOKUYO));

	//ConnectMT(&mt, "MT0.txt");
	//ConnectRazorAHRS(&razorahrs, "RazorAHRS0.txt");
	//ConnectNMEADevice(&nmeadevice, "NMEADevice0.txt");
	//ConnectSwarmonDevice(&swarmondevice, "SwarmonDevice0.txt");
	//ConnectP33x(&p33x, "P33x0.txt");
	//ConnectSSC32(&ssc32, "SSC320.txt");
	//ConnectMaestro(&maestro, "Maestro0.txt");
	//ConnectMiniSSC(&minissc, "MiniSSC0.txt");
	//ConnectMDM(&mdm, "MDM0.txt");
	ConnectHokuyo(&hokuyo, "Hokuyo0.txt");

	for (;;)
	{
		// Wait a little bit...
		//mSleep(500);
		mSleep(20);

		//GetLatestDataMT(&mt, &mtdata);
		//printf("%f\n", mtdata.Yaw);

		//GetLatestDataRazorAHRS(&razorahrs, &razorahrsdata);
		//printf("%f\n", razorahrsdata.Yaw);

		//GetLatestDataNMEADevice(&nmeadevice, &nmeadata);
		//printf("%f;%f\n", nmeadata.Latitude, nmeadata.Longitude);

		//GetLatestDataSwarmonDevice(&swarmondevice, &swarmondata);
		//printf("%f;%f\n", swarmondata.Latitude, swarmondata.Longitude);

		//GetPressureP33x(&p33x, &value);
		//printf("%f\n", value);

		u1 = u2;
		u2 = -u1;
		//SetThrustersSSC32(&ssc32, u1, u2);
		//SetThrustersMaestro(&maestro, u1, u2);
		//SetThrustersMiniSSC(&minissc, u1, u2);
		//printf("%f;%f\n", u1, u2);
		//ivalue = 0;
		//GetValueMaestro(&maestro, 11, &ivalue);
		//printf("%f\n", ivalue*5.0/1024);

		//b = 0;
		//if ((EchoByteMDM(&mdm, (uint8*)&b) == EXIT_SUCCESS)&&(b != 0)) printf("%c", b);

		memset(angles, 0, sizeof(angles));
		memset(distances, 0, sizeof(distances));
		GetLatestDataHokuyo(&hokuyo, distances, angles);
		printf("Distance on the front = %f m\n", distances[angle2kHokuyo(&hokuyo, 0.0)]);
		//printf("Distance on the left = %f m\n", distances[angle2kHokuyo(&hokuyo, M_PI/2.0)]);
	}

	DisconnectHokuyo(&hokuyo);
	//DisconnectMDM(&mdm);
	//DisconnectMiniSSC(&minissc);
	//DisconnectMaestro(&maestro);
	//DisconnectSSC32(&ssc32);
	//DisconnectP33x(&p33x);
	//DisconnectSwarmonDevice(&swarmondevice);
	//DisconnectNMEADevice(&nmeadevice);
	//DisconnectRazorAHRS(&razorahrs);
	//DisconnectMT(&mt);

	return EXIT_SUCCESS;
}
