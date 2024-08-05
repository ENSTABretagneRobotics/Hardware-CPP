#include "MT.h"
#include "SBG.h"
#include "RazorAHRS.h"
#include "NMEADevice.h"
#include "ublox.h"
#include "SwarmonDevice.h"
#include "P33x.h"
#include "MS5837.h"
#include "SSC32.h"
#include "Pololu.h"
#include "MiniSSC.h"
#include "Roboteq.h"
#include "IM483I.h"
#include "MDM.h"
#include "MES.h"
#include "Hokuyo.h"
#include "RPLIDAR.h"
#include "Velodyne.h"
#include "Seanet.h"
#include "PathfinderDVL.h"

// Comment/uncomment lines depending on the device you wish to test.
// Change the device path and other parameters in the configuration files if necessary.
// If you are using an IDE, check that the configuration files are in the correct folder for that IDE 
// (e.g. for Qt you will probably need to put the configuration files in the generated 
// ../Test_devices-build-desktop folder or change "Working directory" in the project "Run Settings").

// min and max might cause incompatibilities...
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif // max
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif // min

/*
Return the distance to the first obstacle from a sonar scanline.
The function does not check any argument.

unsigned char* scanline : (IN) Pointer to the sonar scanline to process.
unsigned char threshold : (IN) Threshold that indicates an obstacle (from 0 to 255).
double minDist : (IN) Distance from which we begin the search of the first 
obstacle (in m).
double maxDist : (IN) Distance to which we stop the search of the first 
obstacle (in m).
int NBins : (IN) Number of bins per scanline.
int RangeScale : (IN) Sonar range scale.
double* pDist : (INOUT) Valid pointer that will receive the distance to the 
first obstacle in m. Return -1 if all the bins are under the threshold (i.e. 
no obstacle).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
int GetFirstObstacleDist(unsigned char* scanline, unsigned char threshold, double minDist, double maxDist, 
						 int NBins, int RangeScale, double* pDist)
{
	double r0 = (double)NBins/(double)RangeScale, r1 = (double)RangeScale/(double)NBins;
	int minBin = max(0, (int)(minDist*r0)), maxBin = min(NBins-1, (int)(maxDist*r0)), i = 0;

	i = minBin;
	while ((i <= maxBin) && (scanline[i] < threshold)) i++;
	if (i > maxBin) *pDist = -1; else *pDist = (double)i*r1; // Convert in m.

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) 
{
	MT mt;
	MTDATA mtdata;
	SBG sbg;
	SBGDATA sbgdata;
	RAZORAHRS razorahrs;
	RAZORAHRSDATA razorahrsdata;
	NMEADEVICE nmeadevice;
	NMEADATA nmeadata;
	UBLOX ublox;
	SWARMONDEVICE swarmondevice;
	SWARMONDATA swarmondata;
	P33X p33x;
	MS5837 ms5837;
	MS5837DATA ms5837data;
	double value = 0;
	int ivalue = 0;
	SSC32 ssc32;
	POLOLU pololu;
	MINISSC minissc;
	ROBOTEQ roboteq;
	IM483I im483i;
	double u1 = 0.25, u2 = -0.25;
	MDM mdm;
	MDM mes;
	char b = 0;
	int receivedbytes = 0;
	unsigned char buf[256];
	HOKUYO hokuyo;
	double angles[max(MAX_SLITDIVISION_HOKUYO,NB_MEASUREMENTS_EXPRESS_SCAN_DATA_RESPONSE_RPLIDAR)];
	double distances[max(MAX_SLITDIVISION_HOKUYO,NB_MEASUREMENTS_EXPRESS_SCAN_DATA_RESPONSE_RPLIDAR)];
	RPLIDAR rplidar;
	BOOL bNewScan = FALSE;
	int quality = 0;
	SEANET seanet;
	double angle = 0, d = 0;
	unsigned char scanline[MAX_NB_BYTES_SEANET];
	PATHFINDERDVL pathfinderdvl;

	// Disable buffering for printf()...
	setbuf(stdout, NULL);

#ifdef _WIN32
	// Prevent display/system sleep...
	SetThreadExecutionState(ES_CONTINUOUS|ES_DISPLAY_REQUIRED);
	//SetThreadExecutionState(ES_CONTINUOUS|ES_SYSTEM_REQUIRED);
#else
#ifndef DISABLE_IGNORE_SIGPIPE
	// See https://stackoverflow.com/questions/17332646/server-dies-on-send-if-client-was-closed-with-ctrlc...
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		PRINT_DEBUG_WARNING(("signal failed. \n"));
	}
#endif // DISABLE_IGNORE_SIGPIPE
#endif // _WIN32

	// Initialize to 0 all the fields of the structure.
	memset(&mt, 0, sizeof(MT));
	memset(&mtdata, 0, sizeof(mtdata));
	memset(&sbg, 0, sizeof(SBG));
	memset(&sbgdata, 0, sizeof(sbgdata));
	memset(&razorahrs, 0, sizeof(RAZORAHRS));
	memset(&razorahrsdata, 0, sizeof(razorahrsdata));
	memset(&nmeadevice, 0, sizeof(NMEADEVICE));
	memset(&nmeadata, 0, sizeof(nmeadata));
	memset(&ublox, 0, sizeof(UBLOX));
	memset(&swarmondevice, 0, sizeof(SWARMONDEVICE));
	memset(&swarmondata, 0, sizeof(swarmondata));
	memset(&p33x, 0, sizeof(P33X));
	memset(&ms5837, 0, sizeof(MS5837));
	memset(&ms5837data, 0, sizeof(MS5837DATA));
	memset(&ssc32, 0, sizeof(SSC32));
	memset(&pololu, 0, sizeof(POLOLU));
	memset(&minissc, 0, sizeof(MINISSC));
	memset(&roboteq, 0, sizeof(ROBOTEQ));
	memset(&im483i, 0, sizeof(IM483I));
	memset(&mdm, 0, sizeof(MDM));
	memset(&mes, 0, sizeof(MES));
	memset(&hokuyo, 0, sizeof(HOKUYO));
	memset(&rplidar, 0, sizeof(RPLIDAR));
	memset(&seanet, 0, sizeof(SEANET));
	memset(&pathfinderdvl, 0, sizeof(PATHFINDERDVL));

	//ConnectMT(&mt, "MT0.txt");
	//ConnectSBG(&sbg, "SBG0.txt");
	ConnectRazorAHRS(&razorahrs, "RazorAHRS0.txt");
	//ConnectNMEADevice(&nmeadevice, "NMEADevice0.txt");
	//Connectublox(&ublox, "ublox0.txt");
	//ConnectSwarmonDevice(&swarmondevice, "SwarmonDevice0.txt");
	//ConnectP33x(&p33x, "P33x0.txt");
	//ConnectMS5837(&ms5837, "MS58370.txt");
	//ConnectSSC32(&ssc32, "SSC320.txt");
	//ConnectPololu(&pololu, "Pololu0.txt");
	//ConnectMiniSSC(&minissc, "MiniSSC0.txt");
	//ConnectRoboteq(&roboteq, "Roboteq0.txt");
	//ConnectIM483I(&im483i, "IM483I0.txt");
	//ConnectMDM(&mdm, "MDM0.txt");
	//ConnectMES(&mes, "MES0.txt");
	//ConnectHokuyo(&hokuyo, "Hokuyo0.txt");
	//ConnectRPLIDAR(&rplidar, "RPLIDAR0.txt");
	//ConnectSeanet(&seanet, "Seanet0.txt");
	//ConnectPathfinderDVL(&pathfinderdvl, "PathfinderDVL0.txt");

	//mdm.pfSaveFile = fopen("rawmdm.txt", "wb");

	for (;;)
	{
		// Wait a little bit...
		mSleep(500);
		//mSleep(20000);

		//GetLatestDataMT(&mt, &mtdata);
		//printf("%f %f %f\n", mtdata.Yaw*180.0/M_PI, mtdata.Pitch*180.0/M_PI, mtdata.Roll*180.0/M_PI);

#ifdef ENABLE_SBG_SDK_SUPPORT
		//GetLatestDataSBG(&sbg, &sbgdata);
#else
		// Warning : need to be run in a loop without delays...
		//GetFrameSBG(&sbg, &sbgdata);
#endif // ENABLE_SBG_SDK_SUPPORT
		//printf("%f %f %f\n", sbgdata.Yaw*180.0/M_PI, sbgdata.Pitch*180.0/M_PI, sbgdata.Roll*180.0/M_PI);

		GetLatestDataRazorAHRS(&razorahrs, &razorahrsdata);
		printf("%f %f %f\n", razorahrsdata.Yaw*180.0/M_PI, razorahrsdata.Pitch*180.0/M_PI, razorahrsdata.Roll*180.0/M_PI);

		//GetLatestDataNMEADevice(&nmeadevice, &nmeadata);
		//printf("%f;%f\n", nmeadata.Latitude, nmeadata.Longitude);

		//GetNMEASentenceublox(&ublox, &nmeadata);
		//printf("%f;%f\n", nmeadata.Latitude, nmeadata.Longitude);

		//GetLatestDataSwarmonDevice(&swarmondevice, &swarmondata);
		//printf("%f;%f\n", swarmondata.Latitude, swarmondata.Longitude);

		//GetPressureP33x(&p33x, &value);
		//printf("%f\n", value);

		//GetLatestDataMS5837(&ms5837, &ms5837data);
		//printf("%f;%f\n", ms5837data.pressure, ms5837data.temperature);

		//u1 = u2;
		//u2 = -u1;
		//SetThrustersSSC32(&ssc32, u1, u2);
		//SetThrustersPololu(&pololu, u1, u2);
		//SetThrustersMiniSSC(&minissc, u1, u2);
		//SetThrustersRoboteq(&roboteq, u1, u2);
		//SetMaxAngleIM483I(&im483i, u1);
		//printf("%f;%f\n", u1, u2);
		//ivalue = 0;
		//GetValuePololu(&pololu, 11, &ivalue);
		//printf("%f\n", ivalue*5.0/1024);

		//b = 0;
		//if ((EchoByteMDM(&mdm, (uint8*)&b) == EXIT_SUCCESS)&&(b != 0)) printf("%c", b);
		//SendAllDataMDM(&mdm, (uint8*)"rng\n", 4);
		//mSleep(100);
		//PurgeDataMDM(&mdm);
		//mSleep(3000);
		//receivedbytes = 0;
		//memset(buf, 0, sizeof(buf));
		//RecvDataMDM(&mdm, buf, sizeof(buf)-1, &receivedbytes);
		//printf("%.255s", buf);

		//memset(angles, 0, sizeof(angles));
		//memset(distances, 0, sizeof(distances));
		//GetLatestDataHokuyo(&hokuyo, distances, angles);
		//printf("Distance on the front = %f m\n", distances[angle2kHokuyo(&hokuyo, 0.0)]);
		//printf("Distance on the left = %f m\n", distances[angle2kHokuyo(&hokuyo, M_PI/2.0)]);

		//d = 0;
		//angle = 0;
		//bNewScan = FALSE;
		//quality = 0;
		//GetScanDataResponseRPLIDAR(&rplidar, &d, &angle, &bNewScan, &quality);
		//printf("%f rad; %f m\n", angle, d);
		//GetExpressScanDataResponseRPLIDAR(&rplidar, distances, angles, &bNewScan);
		//printf("%f rad; %f m\n", angles[0], distances[0]);

		//angle = 0; // Will receive the angle of the ping in deg.
		//memset(scanline, 0, sizeof(scanline)); // Will receive the data of the ping (NBins values in [0..255]).
		//// Get a ping. Note that the sonar does not ping by default, therefore you should 
		//// request pings yourself to make the sonar ping and rotate...
		//GetHeadDataSeanet(&seanet, scanline, &angle);
		//// Simple detection of the first obstacle.
		//d = 0; // Will receive the distance in m of the first obstacle at angle.
		//GetFirstObstacleDist(scanline, 70, 0.5, seanet.RangeScale, seanet.NBins, seanet.RangeScale, &d);
		//printf("%f deg; %f m\n", angle, d);

		//GetLatestDataMES(&mes, &d);
		//printf("%f m\n", d);

		//GetNMEASentencePathfinderDVL(&pathfinderdvl, &nmeadata);
		//printf("%f;%f;%f;%c\n", nmeadata.vt_ship, nmeadata.vl_ship, nmeadata.vn_ship, nmeadata.vstatus_ship);
	}

	//fclose(mdm.pfSaveFile);

	//DisconnectPathfinderDVL(&pathfinderdvl);
	//DisconnectSeanet(&seanet);
	//DisconnectRPLIDAR(&rplidar);
	//DisconnectHokuyo(&hokuyo);
	//DisconnectMES(&mes);
	//DisconnectMDM(&mdm);
	//DisconnectIM483I(&im483i);
	//DisconnectRoboteq(&roboteq);
	//DisconnectMiniSSC(&minissc);
	//DisconnectPololu(&pololu);
	//DisconnectSSC32(&ssc32);
	//DisconnectMS5837(&ms5837);
	//DisconnectP33x(&p33x);
	//DisconnectSwarmonDevice(&swarmondevice);
	//Disconnectublox(&ublox);
	//DisconnectNMEADevice(&nmeadevice);
	DisconnectRazorAHRS(&razorahrs);
	//DisconnectSBG(&sbg);
	//DisconnectMT(&mt);

	return EXIT_SUCCESS;
}
