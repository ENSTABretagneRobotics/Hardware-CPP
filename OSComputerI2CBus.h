/***************************************************************************************************************:')

OSComputerI2CBus.h

Computer I2C bus handling (open, close, read, write,...).

Fabrice Le Bars

Created : 2019-01-30

***************************************************************************************************************:)*/

// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#ifndef OSCOMPUTERI2CBUS_H
#define OSCOMPUTERI2CBUS_H

#include "OSTime.h"

/*
Debug macros specific to OSComputerI2CBus.
*/
#ifdef _DEBUG_MESSAGES_OSUTILS
#	define _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
#endif // _DEBUG_MESSAGES_OSUTILS

#ifdef _DEBUG_WARNINGS_OSUTILS
#	define _DEBUG_WARNINGS_OSCOMPUTERI2CBUS
#endif // _DEBUG_WARNINGS_OSUTILS

#ifdef _DEBUG_ERRORS_OSUTILS
#	define _DEBUG_ERRORS_OSCOMPUTERI2CBUS
#endif // _DEBUG_ERRORS_OSUTILS

#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
#	define PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(params) PRINT_DEBUG_MESSAGE(params)
#else
#	define PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(params)
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS

#ifdef _DEBUG_WARNINGS_OSCOMPUTERI2CBUS
#	define PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(params) PRINT_DEBUG_WARNING(params)
#else
#	define PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(params)
#endif // _DEBUG_WARNINGS_OSCOMPUTERI2CBUS

#ifdef _DEBUG_ERRORS_OSCOMPUTERI2CBUS
#	define PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(params) PRINT_DEBUG_ERROR(params)
#else
#	define PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(params)
#endif // _DEBUG_ERRORS_OSCOMPUTERI2CBUS

#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
#else 
#include <sys/ioctl.h>
//#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#ifndef DISABLE_SELECT_OSCOMPUTERI2CBUS
#include <sys/select.h>
#endif // !DISABLE_SELECT_OSCOMPUTERI2CBUS
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))

#define MAX_COMPUTERI2CBUS_NAME_LENGTH (128-8)

/*
	// https://www.kernel.org/doc/Documentation/i2c/dev-interface

	__u8 reg = 0x10; // Device register to access
	__s32 res;
	char buf[10];

	// Using SMBus commands
	res = i2c_smbus_read_word_data(file, reg);
	if (res < 0) {
		// ERROR HANDLING: i2c transaction failed
	}
	else {
		// res contains the read word
	}

	// Using I2C Write, equivalent of
	// i2c_smbus_write_word_data(file, reg, 0x6543)

	buf[0] = reg;
	buf[1] = 0x43;
	buf[2] = 0x65;
	if (write(file, buf, 3) != 3) {
		// ERROR HANDLING: i2c transaction failed
	}

	// Using I2C Read, equivalent of i2c_smbus_read_byte(file)
	if (read(file, buf, 1) != 1) {
		// ERROR HANDLING: i2c transaction failed
	}
	else {
		// buf[0] contains the read byte
	}
*/

/*
Open a computer I2C bus. Use CloseComputerI2CBus() to close it at the end.

HANDLE* phDev : (INOUT) Valid pointer that will receive an identifier of the
computer I2C bus opened.
char* szDevice : (IN) Computer I2C bus to open.

Return : EXIT_SUCCESS, EXIT_NAME_TOO_LONG or EXIT_FAILURE.
*/
inline int OpenComputerI2CBus(HANDLE* phDev, char* szDevice)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(szDevice);
	*phDev = (HANDLE)-1;
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("OpenComputerI2CBus error (%s) : %s"
		"(szDevice=%s)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		szDevice));
	return EXIT_NOT_IMPLEMENTED;
#else 
	int fd = open(szDevice, O_RDWR);

	if (fd == -1)
	{
		PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("OpenComputerI2CBus error (%s) : %s"
			"(szDevice=%s)\n",
			strtime_m(),
			GetLastErrorMsg(),
			szDevice));
		return EXIT_FAILURE;
	}

	// The read function can be made to return immediately by
	// doing the following :
	// fcntl(fd, F_SETFL, FNDELAY);
	// The FNDELAY option causes the read function to return 0 if no characters are
	// available on the port. To restore normal (blocking) behavior, call fcntl() without
	// the FNDELAY option:
	//if (fcntl(fd, F_SETFL, 0) == (-1))
	//{
	//	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("OpenComputerI2CBus error (%s) : %s"
	//		"(szDevice=%s)\n", 
	//		strtime_m(), 
	//		GetLastErrorMsg(), 
	//		szDevice));
	//	close(fd);
	//	return EXIT_FAILURE;
	//}

#ifndef DISABLE_IGNORE_SIGPIPE
	// See https://stackoverflow.com/questions/17332646/server-dies-on-send-if-client-was-closed-with-ctrlc...
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("OpenComputerI2CBus warning (%s) : %s"
			"(szDevice=%s)\n",
			strtime_m(),
			"signal failed ",
			szDevice));
	}
#endif // DISABLE_IGNORE_SIGPIPE

	*phDev = (HANDLE)(intptr_t)fd;
	return EXIT_SUCCESS;
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

/*
Close a computer I2C bus opened by OpenComputerI2CBus().

HANDLE hDev : (IN) Identifier of the computer I2C bus.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int CloseComputerI2CBus(HANDLE hDev)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(hDev);
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("CloseComputerI2CBus error (%s) : %s(hDev=%#x)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		hDev));
	return EXIT_NOT_IMPLEMENTED;
#else
	if (close((intptr_t)hDev) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("CloseComputerI2CBus error (%s) : %s(hDev=%#x)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

/*
Set the options of a computer I2C bus.

HANDLE hDev : (IN) Identifier of the computer I2C bus.
UINT addr : (IN) Slave address.
BOOL bTenBit : (IN) Select ten bit addresses if TRUE, normal 7 bit addresses if FALSE.
BOOL bPEC : (IN) Select SMBus PEC (packet error checking) generation and verification if TRUE, disable if FALSE.
UINT nbretries : (IN) Number of times a device address should be polled when not acknowledging.
UINT timeout : (IN) Time to wait to get at least 1 byte in ms.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int SetSlaveComputerI2CBus(HANDLE hDev, UINT addr, BOOL bTenBit, BOOL bPEC, UINT nbretries, UINT timeout)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(hDev);
	UNREFERENCED_PARAMETER(addr);
	UNREFERENCED_PARAMETER(bTenBit);
	UNREFERENCED_PARAMETER(bPEC);
	UNREFERENCED_PARAMETER(nbretries);
	UNREFERENCED_PARAMETER(timeout);
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("SetSlaveComputerI2CBus error (%s) : %s"
		"(hDev=%#x, addr=%u, bTenBit=%d, bPEC=%d, nbretries=%u, timeout=%u)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		hDev, addr, bTenBit, bPEC, nbretries, timeout));
	return EXIT_NOT_IMPLEMENTED;
#else 
	if (ioctl((intptr_t)hDev, I2C_SLAVE, addr) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("SetSlaveComputerI2CBus error (%s) : %s"
			"(hDev=%#x, addr=%u, bTenBit=%d, bPEC=%d, nbretries=%u, timeout=%u)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev, addr, bTenBit, bPEC, nbretries, timeout));
		return EXIT_FAILURE;
	}
	if (ioctl((intptr_t)hDev, I2C_TENBIT, bTenBit) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("SetSlaveComputerI2CBus error (%s) : %s"
			"(hDev=%#x, addr=%u, bTenBit=%d, bPEC=%d, nbretries=%u, timeout=%u)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev, addr, bTenBit, bPEC, nbretries, timeout));
	}
	if (ioctl((intptr_t)hDev, I2C_PEC, bPEC) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("SetSlaveComputerI2CBus error (%s) : %s"
			"(hDev=%#x, addr=%u, bTenBit=%d, bPEC=%d, nbretries=%u, timeout=%u)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev, addr, bTenBit, bPEC, nbretries, timeout));
	}
	if (ioctl((intptr_t)hDev, I2C_RETRIES, nbretries) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("SetSlaveComputerI2CBus error (%s) : %s"
			"(hDev=%#x, addr=%u, bTenBit=%d, bPEC=%d, nbretries=%u, timeout=%u)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev, addr, bTenBit, bPEC, nbretries, timeout));
	}
	if (ioctl((intptr_t)hDev, I2C_TIMEOUT, timeout/10) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("SetSlaveComputerI2CBus error (%s) : %s"
			"(hDev=%#x, addr=%u, bTenBit=%d, bPEC=%d, nbretries=%u, timeout=%u)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev, addr, bTenBit, bPEC, nbretries, timeout));
	}
	return EXIT_SUCCESS;
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

/*
Check for any data available to read on a computer I2C bus.

HANDLE hDev : (IN) Identifier of the computer I2C bus.

Return : EXIT_SUCCESS if there is data to read, EXIT_TIMEOUT if there is currently no data
 available or EXIT_FAILURE if there is an error.
*/
inline int CheckAvailableBytesComputerI2CBus(HANDLE hDev)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(hDev);
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("CheckAvailableBytesComputerI2CBus error (%s) : %s(hDev=%#x)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		hDev));
	return EXIT_NOT_IMPLEMENTED;
#else
	int bytes_avail = 0;

	if (ioctl((intptr_t)hDev, FIONREAD, &bytes_avail) != EXIT_SUCCESS)
	{
		PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("CheckAvailableBytesComputerI2CBus error (%s) : %s(hDev=%#x)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev));
		return EXIT_FAILURE;
	}
	if (bytes_avail <= 0) return EXIT_TIMEOUT;
	return EXIT_SUCCESS;
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

/*
Wait for data to read on a computer I2C bus.

HANDLE hDev : (IN) Identifier of the computer I2C bus.
int timeout : (IN) Max time to wait before returning in ms.
int checkingperiod : (IN) Checking period in ms.

Return : EXIT_SUCCESS if there is data to read, EXIT_TIMEOUT if there is currently no data
 available or EXIT_FAILURE if there is an error.
*/
inline int WaitForComputerI2CBus(HANDLE hDev, int timeout, int checkingperiod)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(hDev);
	UNREFERENCED_PARAMETER(timeout);
	UNREFERENCED_PARAMETER(checkingperiod);
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("WaitForComputerI2CBus error (%s) : %s"
		"(hDev=%#x, timeout=%d, checkingperiod=%d)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		hDev, timeout, checkingperiod));
	return EXIT_NOT_IMPLEMENTED;
#else
#ifdef DISABLE_SELECT_OSCOMPUTERI2CBUS
	CHRONO chrono;

	StartChrono(&chrono);
	do
	{
		int bytes_avail = 0;

		if (ioctl((intptr_t)hDev, FIONREAD, &bytes_avail) == EXIT_SUCCESS)
		{
			if (bytes_avail <= 0) mSleep(checkingperiod);
			else
			{
				StopChronoQuick(&chrono);
				return EXIT_SUCCESS;
			}
		}
		else
		{
			StopChronoQuick(&chrono);
			PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("WaitForComputerI2CBus error (%s) : %s"
				"(hDev=%#x, timeout=%d, checkingperiod=%d)\n",
				strtime_m(),
				GetLastErrorMsg(),
				hDev, timeout, checkingperiod));
			return EXIT_FAILURE;
		}
	} while (GetTimeElapsedChronoQuick(&chrono) <= timeout);
	StopChronoQuick(&chrono);
	return EXIT_TIMEOUT;
#else
	fd_set dev_set;
	int iResult = -1;
	struct timeval tv;

	UNREFERENCED_PARAMETER(checkingperiod);

	// Initialize a fd_set and add the socket to it.
	FD_ZERO(&dev_set);
	FD_SET((intptr_t)hDev, &dev_set);

	tv.tv_sec = (long)(timeout/1000);
	tv.tv_usec = (long)((timeout%1000)*1000);

	// Wait for the readability of the device in the fd_set, with a timeout.
	iResult = select((int)(intptr_t)hDev+1, &dev_set, NULL, NULL, &tv);

	// Remove the device from the set.
	FD_CLR((intptr_t)hDev, &dev_set);

	if (iResult == -1)
	{
		PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("WaitForComputerI2CBus error (%s) : %s"
			"(hDev=%#x, timeout=%d, checkingperiod=%d)\n",
			strtime_m(),
			"select failed. ",
			hDev, timeout, checkingperiod));
		return EXIT_FAILURE;
	}

	if (iResult == 0)
	{
		// The timeout on select() occured.
		return EXIT_TIMEOUT;
	}

	return EXIT_SUCCESS;
#endif // DISABLE_SELECT_OSCOMPUTERI2CBUS
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

/*
Write data to a computer I2C bus.

HANDLE hDev : (IN) Identifier of the computer I2C bus.
uint8* writebuf : (IN) Valid pointer to the data to write.
UINT writebuflen : (IN) Number of bytes to write.
int* pWrittenBytes : (INOUT) Valid pointer that will receive the number of bytes written.

Return : EXIT_SUCCESS if some bytes are written, EXIT_TIMEOUT if a timeout occurs or
EXIT_FAILURE if there is an error.
*/
inline int WriteComputerI2CBus(HANDLE hDev, uint8* writebuf, UINT writebuflen, int* pWrittenBytes)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(hDev);
	UNREFERENCED_PARAMETER(writebuf);
	UNREFERENCED_PARAMETER(writebuflen);
	*pWrittenBytes = -1;
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("WriteComputerI2CBus error (%s) : %s"
		"(hDev=%#x, writebuf=%#x, writebuflen=%u)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		hDev, writebuf, writebuflen));
	return EXIT_NOT_IMPLEMENTED;
#else
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	int i = 0;
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS

#ifdef _WIN32
	if (WriteFile(hDev, writebuf, writebuflen, (LPDWORD)pWrittenBytes, NULL))
	{
#else 
	*pWrittenBytes = write((intptr_t)hDev, writebuf, writebuflen);
	if (*pWrittenBytes >= 0)
	{
#endif // _WIN32
		if (*pWrittenBytes == 0)
		{
			PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("WriteComputerI2CBus warning (%s) : %s"
				"(hDev=%#x, writebuf=%#x, writebuflen=%u)\n",
				strtime_m(),
				szOSUtilsErrMsgs[EXIT_TIMEOUT],
				hDev, writebuf, writebuflen));
			return EXIT_TIMEOUT;
		}
		else
		{
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
			for (i = 0; i < *pWrittenBytes; i++)
			{
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("%.2x ", (int)writebuf[i]));
			}
			PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("\n"));
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
			PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Bytes written : %d\n", *pWrittenBytes));
		}
	}
	else
	{
		PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("WriteComputerI2CBus error (%s) : %s"
			"(hDev=%#x, writebuf=%#x, writebuflen=%u)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev, writebuf, writebuflen));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

/*
Read data from a computer I2C bus.

HANDLE hDev : (IN) Identifier of the computer I2C bus.
uint8* readbuf : (INOUT) Valid pointer that will receive the data read.
UINT readbuflen : (IN) Number of bytes to read.
int* pReadBytes : (INOUT) Valid pointer that will receive the number of bytes read.

Return : EXIT_SUCCESS if some bytes are read, EXIT_TIMEOUT if a timeout occurs or
EXIT_FAILURE if there is an error.
*/
inline int ReadComputerI2CBus(HANDLE hDev, uint8* readbuf, UINT readbuflen, int* pReadBytes)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(hDev);
	UNREFERENCED_PARAMETER(readbuf);
	UNREFERENCED_PARAMETER(readbuflen);
	*pReadBytes = -1;
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("ReadComputerI2CBus error (%s) : %s"
		"(hDev=%#x, readbuf=%#x, readbuflen=%u)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		hDev, readbuf, readbuflen));
	return EXIT_NOT_IMPLEMENTED;
#else
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	int i = 0;
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS

#ifdef _WIN32
	if (ReadFile(hDev, readbuf, readbuflen, (LPDWORD)pReadBytes, NULL))
	{
#else 
	*pReadBytes = read((intptr_t)hDev, readbuf, readbuflen);
	if (*pReadBytes >= 0)
	{
#endif // _WIN32
		if (*pReadBytes == 0)
		{
			PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("ReadComputerI2CBus warning (%s) : %s"
				"(hDev=%#x, readbuf=%#x, readbuflen=%u)\n",
				strtime_m(),
				szOSUtilsErrMsgs[EXIT_TIMEOUT],
				hDev, readbuf, readbuflen));
			return EXIT_TIMEOUT;
		}
		else
		{
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
			for (i = 0; i < *pReadBytes; i++)
			{
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("%.2x ", (int)readbuf[i]));
			}
			PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("\n"));
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
			PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Bytes read : %d\n", *pReadBytes));
		}
	}
	else
	{
		PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("ReadComputerI2CBus error (%s) : %s"
			"(hDev=%#x, readbuf=%#x, readbuflen=%u)\n",
			strtime_m(),
			GetLastErrorMsg(),
			hDev, readbuf, readbuflen));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

/*
Write data to a computer I2C bus. Retry automatically if all the bytes were not written.

HANDLE hDev : (IN) Identifier of the computer I2C bus.
uint8* writebuf : (IN) Valid pointer to the data to write.
UINT writebuflen : (IN) Number of bytes to write.

Return : EXIT_SUCCESS if all the bytes are written, EXIT_TIMEOUT if a timeout occurs or
EXIT_FAILURE if there is an error.
*/
inline int WriteAllComputerI2CBus(HANDLE hDev, uint8* writebuf, UINT writebuflen)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(hDev);
	UNREFERENCED_PARAMETER(writebuf);
	UNREFERENCED_PARAMETER(writebuflen);
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("WriteAllComputerI2CBus error (%s) : %s"
		"(hDev=%#x, writebuf=%#x, writebuflen=%u)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		hDev, writebuf, writebuflen));
	return EXIT_NOT_IMPLEMENTED;
#else
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	UINT i = 0;
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	UINT BytesWritten = 0;
	int Bytes = 0;

	while (BytesWritten < writebuflen)
	{
#ifdef _WIN32
		if (WriteFile(hDev, writebuf + BytesWritten, writebuflen - BytesWritten, (LPDWORD)&Bytes, NULL))
		{
#else 
		Bytes = write((intptr_t)hDev, writebuf + BytesWritten, writebuflen - BytesWritten);
		if (Bytes >= 0)
		{
#endif // _WIN32
			if (Bytes == 0)
			{
				PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("WriteAllComputerI2CBus warning (%s) : %s"
					"(hDev=%#x, writebuf=%#x, writebuflen=%u)\n",
					strtime_m(),
					szOSUtilsErrMsgs[EXIT_TIMEOUT],
					hDev, writebuf, writebuflen));
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
				for (i = 0; i < BytesWritten; i++)
				{
					PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("%.2x ", (int)writebuf[i]));
				}
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("\n"));
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Total bytes written : %u\n", BytesWritten));
				return EXIT_TIMEOUT;
			}
			else
			{
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Bytes written : %d\n", Bytes));
			}
		}
		else
		{
			PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("WriteAllComputerI2CBus error (%s) : %s"
				"(hDev=%#x, writebuf=%#x, writebuflen=%u)\n",
				strtime_m(),
				GetLastErrorMsg(),
				hDev, writebuf, writebuflen));
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
			for (i = 0; i < BytesWritten; i++)
			{
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("%.2x ", (int)writebuf[i]));
			}
			PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("\n"));
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
			PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Total bytes written : %u\n", BytesWritten));
			return EXIT_FAILURE;
		}

		BytesWritten += Bytes;
		}

#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	for (i = 0; i < BytesWritten; i++)
	{
		PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("%.2x ", (int)writebuf[i]));
	}
	PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("\n"));
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Total bytes written : %u\n", BytesWritten));
	return EXIT_SUCCESS;
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

/*
Read data from a computer I2C bus. Retry automatically if all the bytes were not read.

HANDLE hDev : (IN) Identifier of the computer I2C bus.
uint8* readbuf : (INOUT) Valid pointer that will receive the data read.
UINT readbuflen : (IN) Number of bytes to read.

Return : EXIT_SUCCESS if all the bytes are read, EXIT_TIMEOUT if a timeout occurs or
EXIT_FAILURE if there is an error.
*/
inline int ReadAllComputerI2CBus(HANDLE hDev, uint8* readbuf, UINT readbuflen)
{
#if (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
	UNREFERENCED_PARAMETER(hDev);
	UNREFERENCED_PARAMETER(readbuf);
	UNREFERENCED_PARAMETER(readbuflen);
	PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("ReadAllComputerI2CBus error (%s) : %s"
		"(hDev=%#x, readbuf=%#x, readbuflen=%u)\n",
		strtime_m(),
		szOSUtilsErrMsgs[EXIT_NOT_IMPLEMENTED],
		hDev, readbuf, readbuflen));
	return EXIT_NOT_IMPLEMENTED;
#else
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	UINT i = 0;
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	UINT BytesRead = 0;
	int Bytes = 0;

	while (BytesRead < readbuflen)
	{
#ifdef _WIN32
		if (ReadFile(hDev, readbuf + BytesRead, readbuflen - BytesRead, (LPDWORD)&Bytes, NULL))
		{
#else 
		Bytes = read((intptr_t)hDev, readbuf + BytesRead, readbuflen - BytesRead);
		if (Bytes >= 0)
		{
#endif // _WIN32
			if (Bytes == 0)
			{
				PRINT_DEBUG_WARNING_OSCOMPUTERI2CBUS(("ReadAllComputerI2CBus warning (%s) : %s"
					"(hDev=%#x, readbuf=%#x, readbuflen=%u)\n",
					strtime_m(),
					szOSUtilsErrMsgs[EXIT_TIMEOUT],
					hDev, readbuf, readbuflen));
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
				for (i = 0; i < BytesRead; i++)
				{
					PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("%.2x ", (int)readbuf[i]));
				}
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("\n"));
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Total bytes read : %u\n", BytesRead));
				return EXIT_TIMEOUT;
			}
			else
			{
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Bytes read : %d\n", Bytes));
			}
		}
		else
		{
			PRINT_DEBUG_ERROR_OSCOMPUTERI2CBUS(("ReadAllComputerI2CBus error (%s) : %s"
				"(hDev=%#x, readbuf=%#x, readbuflen=%u)\n",
				strtime_m(),
				GetLastErrorMsg(),
				hDev, readbuf, readbuflen));
#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
			for (i = 0; i < BytesRead; i++)
			{
				PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("%.2x ", (int)readbuf[i]));
			}
			PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("\n"));
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
			PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Total bytes read : %u\n", BytesRead));
			return EXIT_FAILURE;
		}

		BytesRead += Bytes;
		}

#ifdef _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	for (i = 0; i < BytesRead; i++)
	{
		PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("%.2x ", (int)readbuf[i]));
	}
	PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("\n"));
#endif // _DEBUG_MESSAGES_OSCOMPUTERI2CBUS
	PRINT_DEBUG_MESSAGE_OSCOMPUTERI2CBUS(("Total bytes read : %u\n", BytesRead));
	return EXIT_SUCCESS;
#endif // (defined(_WIN32) || (!defined(ENABLE_COMPUTERI2CBUS_SUPPORT)))
}

#endif // !OSCOMPUTERI2CBUS_H
