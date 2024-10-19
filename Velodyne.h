// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#ifndef VELODYNE_H
#define VELODYNE_H

#include "OSMisc.h"
#include "RS232Port.h"

#ifndef DISABLE_VELODYNETHREAD
#include "OSThread.h"
#endif // !DISABLE_VELODYNETHREAD

// Need to be undefined at the end of the file...
// min and max might cause incompatibilities...
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif // !max
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif // !min

#define UDP_PACKET_SIZE_VELODYNE 1206

#define TIMEOUT_MESSAGE_VELODYNE 4.0 // In s.
// Should be at least 2 * number of bytes to be sure to contain entirely the biggest desired message (or group of messages) + 1.
#define MAX_NB_BYTES_VELODYNE (2*UDP_PACKET_SIZE_VELODYNE+1)

#define NB_MEASUREMENTS_VELODYNE 24

struct VELODYNE
{
	RS232PORT RS232Port;
	FILE* pfSaveFile; // Used to save raw data, should be handled specifically...
	//VELODYNEDATA LastVelodyneData;
	char szCfgFilePath[256];
	// Parameters.
	char szDevPath[256];
	int BaudRate;
	int timeout;
	int threadperiod;
	BOOL bSaveRawData;
	int DefaultLaserID;
	BOOL bDistanceProjectionWithElevation;
	int maxhist;
	double step_elevation;
	double min_elevation;
	double max_elevation;
	double alpha_max_err;
	double d_max_err;
};
typedef struct VELODYNE VELODYNE;

int ProcessRawDataVelodyne(VELODYNE* pVelodyne, const u_char* data, uint32_t data_len, double* pDistances, double* pAngles, double* pElevation)
{
	int k = 0;

	// Assuming buffer contains the received UDP packet.
	unsigned char* dataPacket = (unsigned char*)data;

	// Parse the data packet.
	unsigned char productID = dataPacket[data_len-1];
	unsigned char returnMode = dataPacket[data_len-2];
	unsigned int timeStamp = (dataPacket[data_len-3] << 24) | (dataPacket[data_len-4] << 16) | (dataPacket[data_len-5] << 8) | dataPacket[data_len-6];

	//if (productID != 0x22) {
	//	printf("Error reading data from a Velodyne : Unsupported productID. \n");
	//	return EXIT_FAILURE;
	//}

	int dual_mode = (returnMode == 0x39);
	//if (dual_mode) {
	//	printf("Error reading data from a Velodyne : Unsupported mode. \n");
	//	return EXIT_FAILURE;
	//}

	//memset(pAngles, 0, NB_MEASUREMENTS_VELODYNE);
	//memset(pDistances, 0, NB_MEASUREMENTS_VELODYNE);
	for (int i = 0; i < 12; i++) { // 12 blocks in one data packet.
		unsigned short flag = (dataPacket[i*100] << 8) | dataPacket[i*100 + 1]; // Each block starts with a flag.
		if (flag != 0xFFEE) {
			printf("Error reading data from a Velodyne : Invalid data packet. \n");
			return EXIT_FAILURE;
		}
		unsigned short rawAzimuth = (dataPacket[i*100 + 3] << 8) | dataPacket[i*100 + 2]; // Followed by azimuth data.
		double azimuth = rawAzimuth / 100.0; // Convert to degrees.

		for (int j = 0; j < 32; j++) { // Each block contains 32 data points.
			int dataBlockIndex = (dual_mode)? (i - (i % 2)) + (j / 16): (i * 2) + (j / 16); // From documentation.
			int laserID = j % 16; // There are 16 lasers

			// Assuming firing alternance, symetry...
			double elevation = pVelodyne->min_elevation+(laserID*pVelodyne->step_elevation);

			unsigned short rawDistance = (dataPacket[i*100 + 5 + j*3] << 8) | dataPacket[i*100 + 4 + j*3]; // Distance data.
			//if (rawDistance == 0) // Invalid data.
			//	continue;

			double distance = ((double)rawDistance)*2.0 / 1000.0; // Convert to meters.
			unsigned char intensity = dataPacket[i*100 + 6 + j*3]; // Intensity data.

			double timeOffset = (55.296 * dataBlockIndex) + (2.304 * laserID); // From documentation.

			//std::cout << "Laser ID: " << laserID << "Elevation: " << elevation << ", Azimuth: " << azimuth << ", Distance: " << distance << ", Intensity: " << int(intensity) << ", TimeStamp: " << std::fixed << timeStamp+timeOffset << std::endl;
			

			// TODO: there is a concept of "Vertical Correction" that should be applied...


			if (laserID == pVelodyne->DefaultLaserID)
			{
				*pElevation = fmod_2PI_deg2rad(elevation); // Convert in rad.
				pAngles[k] = fmod_2PI_pos_deg2rad(azimuth); // Convert in rad.
				if (pVelodyne->bDistanceProjectionWithElevation)
				{
					pDistances[k] = distance*cos(*pElevation); // Convert in m.
				}
				else 
				{
					pDistances[k] = distance; // Convert in m.
				}
				k++;
			}
			UNREFERENCED_PARAMETER(intensity);
			UNREFERENCED_PARAMETER(timeOffset);
		}
	}

	UNREFERENCED_PARAMETER(timeStamp);
	UNREFERENCED_PARAMETER(productID);

	return EXIT_SUCCESS;	
}

// NB_MEASUREMENTS_VELODYNE distances, angles...
inline int GetDataVelodyne(VELODYNE* pVelodyne, double* pDistances, double* pAngles, double* pElevation)
{
	unsigned char recvbuf[UDP_PACKET_SIZE_VELODYNE];

	// Receive the data response.
	//memset(recvbuf, 0, sizeof(recvbuf));
	if (ReadAllRS232Port(&pVelodyne->RS232Port, recvbuf, UDP_PACKET_SIZE_VELODYNE) != EXIT_SUCCESS)
	{
		printf("Error reading data from a Velodyne. \n");
		return EXIT_FAILURE;
	}
	if ((pVelodyne->bSaveRawData)&&(pVelodyne->pfSaveFile))
	{
		fwrite(recvbuf, UDP_PACKET_SIZE_VELODYNE, 1, pVelodyne->pfSaveFile);
		fflush(pVelodyne->pfSaveFile);
	}

	if (ProcessRawDataVelodyne(pVelodyne, recvbuf, UDP_PACKET_SIZE_VELODYNE, pDistances, pAngles, pElevation) != EXIT_SUCCESS)
	{ 
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// VELODYNE must be initialized to 0 before (e.g. VELODYNE velodyne; memset(&velodyne, 0, sizeof(VELODYNE));)!
inline int ConnectVelodyne(VELODYNE* pVelodyne, char* szCfgFilePath)
{
	FILE* file = NULL;
	char line[1024];

	memset(pVelodyne->szCfgFilePath, 0, sizeof(pVelodyne->szCfgFilePath));
	sprintf(pVelodyne->szCfgFilePath, "%.255s", szCfgFilePath);

	// If szCfgFilePath starts with "hardcoded://", parameters are assumed to be already set in the structure, 
	// otherwise it should be loaded from a configuration file.
	if (strncmp(szCfgFilePath, "hardcoded://", strlen("hardcoded://")) != 0)
	{
		memset(line, 0, sizeof(line));

		// Default values.
		memset(pVelodyne->szDevPath, 0, sizeof(pVelodyne->szDevPath));
		sprintf(pVelodyne->szDevPath, ":2368");
		pVelodyne->BaudRate = 115200;
		pVelodyne->timeout = 1000;
		pVelodyne->threadperiod = 50;
		pVelodyne->bSaveRawData = 1;
		pVelodyne->DefaultLaserID = 1;
		pVelodyne->bDistanceProjectionWithElevation = 0;
		pVelodyne->maxhist = 0;
		pVelodyne->step_elevation = 2;
		pVelodyne->min_elevation = -15;
		pVelodyne->max_elevation = 15;
		pVelodyne->alpha_max_err = 0.01;
		pVelodyne->d_max_err = 0.1;

		// Load data from a file.
		file = fopen(szCfgFilePath, "r");
		if (file != NULL)
		{
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%255s", pVelodyne->szDevPath) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%d", &pVelodyne->BaudRate) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%d", &pVelodyne->timeout) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%d", &pVelodyne->threadperiod) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%d", &pVelodyne->bSaveRawData) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%d", &pVelodyne->DefaultLaserID) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%d", &pVelodyne->bDistanceProjectionWithElevation) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%d", &pVelodyne->maxhist) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%lf", &pVelodyne->step_elevation) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%lf", &pVelodyne->min_elevation) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%lf", &pVelodyne->max_elevation) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%lf", &pVelodyne->alpha_max_err) != 1) printf("Invalid configuration file.\n");
			if (fgets3(file, line, sizeof(line)) == NULL) printf("Invalid configuration file.\n");
			if (sscanf(line, "%lf", &pVelodyne->d_max_err) != 1) printf("Invalid configuration file.\n");
			if (fclose(file) != EXIT_SUCCESS) printf("fclose() failed.\n");
		}
		else
		{
			printf("Configuration file not found.\n");
		}
	}

	if (pVelodyne->threadperiod < 0)
	{
		printf("Invalid parameter : threadperiod.\n");
		pVelodyne->threadperiod = 50;
	}

	// Used to save raw data, should be handled specifically...
	//pVelodyne->pfSaveFile = NULL;

	if (OpenRS232Port(&pVelodyne->RS232Port, pVelodyne->szDevPath) != EXIT_SUCCESS)
	{
		printf("Unable to connect to a VELODYNE.\n");
		return EXIT_FAILURE;
	}

	if (SetOptionsRS232Port(&pVelodyne->RS232Port, pVelodyne->BaudRate, NOPARITY, FALSE, 8, 
		ONESTOPBIT, (UINT)pVelodyne->timeout) != EXIT_SUCCESS)
	{
		printf("Unable to connect to a VELODYNE.\n");
		CloseRS232Port(&pVelodyne->RS232Port);
		return EXIT_FAILURE;
	}

	printf("Velodyne connected.\n");

	return EXIT_SUCCESS;
}

inline int DisconnectVelodyne(VELODYNE* pVelodyne)
{
	if (CloseRS232Port(&pVelodyne->RS232Port) != EXIT_SUCCESS)
	{
		printf("VELODYNE disconnection failed.\n");
		return EXIT_FAILURE;
	}

	printf("Velodyne disconnected.\n");

	return EXIT_SUCCESS;
}

#ifndef DISABLE_VELODYNETHREAD
THREAD_PROC_RETURN_VALUE VELODYNEThread(void* pParam);
#endif // !DISABLE_VELODYNETHREAD

// min and max might cause incompatibilities...
#ifdef max
#undef max
#endif // max
#ifdef min
#undef min
#endif // min

#endif // !VELODYNE_H
