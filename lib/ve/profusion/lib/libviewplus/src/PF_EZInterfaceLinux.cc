// =============================================================================
//	PF_EZInterfaceLiux.cpp
//
//	Revision History:
//	Rev.01 2008/08/04	The First edition
//	Rev.02 2010/05/07	Change License to LGPL v2.1
//
//	Copyright (C) 2008-2010 ViewPLUS Inc. All Rights Reserved.
//
//	This library is free software; you can redistribute it and/or
//	modify it under the terms of the GNU Lesser General Public
//	License as published by the Free Software Foundation; either
//	version 2.1 of the License, or (at your option) any later version.
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//	Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public
//	License along with this library; if not, write to the Free Software
//	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
// =============================================================================
/*!
	\file		PF_EZInterfaceLinux.cpp
	\author		Dairoku Sekiguchi
	\version	1.0
	\date		2008/08/04
	\license	LGPL v2.1
	\brief		This file defines ProFUSION Library EZ-Interface routines.

	This file defines ProFUSION Library EZ-Interface routines.

	\note
		- None

	\todo
		- None
*/


// -----------------------------------------------------------------------------
// 	include files
// -----------------------------------------------------------------------------
#include <errno.h>
#include "PF_EZInterfaceLocal.h"
#include "MyUnixUtils.hpp"

// -----------------------------------------------------------------------------
// 	macros
// -----------------------------------------------------------------------------
#define	VP1066_STR_BUF_SIZE		256
#define VP1066_MAXIMUM_TRANSFER_LENGTH_CMU		(640*480*24+32)
#define VP1066_QVGA_TRANSFER_LENGTH_CMU		(320*240*24+32)


// -----------------------------------------------------------------------------
// 	prototypes
// -----------------------------------------------------------------------------
int	VP1066_ReadI2C(int inDeviceDesc, int inCameraIndex, UInt32 inI2CAddr, UInt32 *outData);
int	VP1066_WriteI2C(int inDeviceDesc, int inCameraIndex, UInt32 inI2CAddr, UInt32 inData);
int	VP1066_ReadBAR0(int inDeviceDesc, UInt32 inAddressOffset, UInt32 *outData);
int	VP1066_WriteBAR0(int inDeviceDesc, UInt32 inAddressOffset, UInt32 inData);


//  Device Related Functions ===================================================
// -----------------------------------------------------------------------------
//	PF_EZGetDeviceNum
// -----------------------------------------------------------------------------
//
_PF_API int	_PF_CALL	PF_EZGetDeviceNum(PF_EZDeviceType inDeviceType)
{
	return 4;
}


// -----------------------------------------------------------------------------
//	PF_EZOpenDevice
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZOpenDevice(PF_EZDeviceType inDeviceType, int inDeviceIndex, PF_EZDeviceHandle *outHandle)
{
	*outHandle = NULL;

	if (inDeviceIndex > PF_EZGetDeviceNum(inDeviceType))
		return PF_EZ_DEVICE_NOT_FOUND_ERROR;

	int	deviceDesc;
	char	buf[VP1066_STR_BUF_SIZE];

	sprintf(buf, "/dev/vpcpro%d", inDeviceIndex);

	deviceDesc = open(buf, O_RDONLY);
	if (deviceDesc < 0)
	{
		return PF_EZ_OS_ERROR;
	}

	//	Check Driver Version
	int	version;
	int	result = ioctl(deviceDesc, VP1066_IOC_GET_VER, &version);
	if (result != 0)
	{
		close(deviceDesc);
		return PF_EZ_OS_ERROR;
	}

	if ((version & VP1066_DRIVER_VERSION_MASK) !=
		(VP1066_DRIVER_VERSION & VP1066_DRIVER_VERSION_MASK) &&
		(version & VP1066_DRIVER_BUILD_MASK) <
		(VP1066_DRIVER_VERSION & VP1066_DRIVER_BUILD_MASK))
	{
		close(deviceDesc);
		return PF_EZ_DRIVER_MISMATCH_ERROR;
	}

	//	Check FPGA Version
	UInt32	regValue;

	if (VP1066_ReadBAR0(deviceDesc, VP1066_REG_FPGA_VERSION, &regValue) != 0)
	{
		close(deviceDesc);
		return PF_EZ_OS_ERROR;
	}

	if ((regValue & VP1066_FPGA_VERSION_MASK) !=
		(VP1066_FPGA_VERSION & VP1066_FPGA_VERSION_MASK) ||
		(regValue & VP1066_FPGA_BUILD_MASK) <
		(VP1066_FPGA_VERSION & VP1066_FPGA_BUILD_MASK))
	{
		close(deviceDesc);
		return PF_EZ_FIRMWARE_MISMATCH_ERROR;
	}

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)malloc(sizeof(PF_EZDeviceInternalData));
	if (deviceDataPtr == NULL)
	{
		close(deviceDesc);
		return PF_EZ_MEMORY_ERROR;
	}
	memset(deviceDataPtr, 0, sizeof(PF_EZDeviceInternalData));
	deviceDataPtr->deviceDesc = deviceDesc;
	deviceDataPtr->magicValue = PF_EZ_HANDLE_MAGIC_VALUE;
	*outHandle = (void *)deviceDataPtr;

	//	Get Serial Number
	if (VP1066_ReadBAR0(deviceDesc, VP1066_REG_CAM_SERIAL_NO, &regValue) != 0)
	{
		PF_EZCloseDevice(*outHandle);
		return PF_EZ_OS_ERROR;
	}
	deviceDataPtr->serialNumber = regValue;

	if (inDeviceType != PF_EZ_DEVICE_ProFUSION_25_CMU && inDeviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
	{
		if ((deviceDataPtr->serialNumber >> 16) == 0x01)
			deviceDataPtr->deviceType = PF_EZ_DEVICE_ProFUSION_25M;
		else
			deviceDataPtr->deviceType = PF_EZ_DEVICE_ProFUSION_25C;

		deviceDataPtr->cameraNum = VP1066_CAMERA_NUM;
		deviceDataPtr->readSize = VP1066_MAXIMUM_TRANSFER_LENGTH;

	}
	else
	{
		if (inDeviceType == PF_EZ_DEVICE_ProFUSION_25_CMU)
		{
			deviceDataPtr->deviceType = PF_EZ_DEVICE_ProFUSION_25_CMU;
			deviceDataPtr->cameraNum = VP1066_CAMERA_NUM;
			deviceDataPtr->readSize = VP1066_MAXIMUM_TRANSFER_LENGTH_CMU;
		}
		else
		{
			deviceDataPtr->deviceType = PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU;
			deviceDataPtr->cameraNum = VP1066_CAMERA_NUM;
			deviceDataPtr->readSize = VP1066_QVGA_TRANSFER_LENGTH_CMU;
		}
	}

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZCloseDevice
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZCloseDevice(PF_EZDeviceHandle inHandle)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	close(deviceDataPtr->deviceDesc);

	memset(deviceDataPtr, 0, sizeof(PF_EZDeviceInternalData));
	free(deviceDataPtr);

	return PF_EZ_OK;
}


//  Image Related Functions ====================================================
// -----------------------------------------------------------------------------
//	PF_EZCreateDeviceImage
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZCreateDeviceImage(PF_EZDeviceHandle inHandle, PF_EZImage **outImage)
{
	*outImage = NULL;

	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	size_t	size = sizeof(PF_EZImageInternalData);
	PF_EZImageInternalData	*imageDataPtr;
	imageDataPtr = (PF_EZImageInternalData *)malloc(size);
	if (imageDataPtr == NULL)
	{
		return PF_EZ_MEMORY_ERROR;
	}
	memset(imageDataPtr, 0, size);

	size = sizeof(unsigned char *) * deviceDataPtr->cameraNum;
	imageDataPtr->imageArray = (unsigned char **)malloc(size);
	if (imageDataPtr->imageArray == NULL)
	{
		free(imageDataPtr);
		return PF_EZ_MEMORY_ERROR;
	}
	memset(imageDataPtr->imageArray, 0, size);

	if (deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
		size = VP1066_MAXIMUM_TRANSFER_LENGTH;
	else
		size = VP1066_QVGA_TRANSFER_LENGTH;
	imageDataPtr->imageBufPtr = (unsigned char *)malloc(size);
	if (imageDataPtr->imageBufPtr == NULL)
	{
		free(imageDataPtr->imageArray);
		free(imageDataPtr);
		return PF_EZ_MEMORY_ERROR;
	}
	memset(imageDataPtr->imageBufPtr, 0, size);

	unsigned char	*ptr = imageDataPtr->imageBufPtr + VP1066_FRAME_NO_OFFSET;
	for (int i = 0; i < deviceDataPtr->cameraNum; i++)
	{
		imageDataPtr->imageArray[i] = ptr;
		if (deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
			ptr += VP1066_IMAGE_WIDTH * VP1066_IMAGE_HEIGHT;
		else
			ptr += VP1066_QVGA_IMAGE_WIDTH * VP1066_QVGA_IMAGE_HEIGHT;
	}

	imageDataPtr->imageNum = deviceDataPtr->cameraNum;
	if (deviceDataPtr->deviceType == PF_EZ_DEVICE_ProFUSION_25M)
		imageDataPtr->imageType = PF_EZ_IMAGE_MONO8;
	else
		imageDataPtr->imageType = PF_EZ_IMAGE_BAYER_GBRG8;
	if (deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
	{
		imageDataPtr->width = VP1066_IMAGE_WIDTH;
		imageDataPtr->height = VP1066_IMAGE_HEIGHT;
		imageDataPtr->widthStep = VP1066_IMAGE_WIDTH;
	}
	else
	{
		imageDataPtr->width = VP1066_QVGA_IMAGE_WIDTH;
		imageDataPtr->height = VP1066_QVGA_IMAGE_HEIGHT;
		imageDataPtr->widthStep = VP1066_QVGA_IMAGE_WIDTH;
	}
	imageDataPtr->magicValue = PF_EZ_IMAGE_MAGIC_VALUE;
	imageDataPtr->isDeviceImage = true;
	imageDataPtr->deviceDataPtr = deviceDataPtr;

	*outImage = (PF_EZImage *)imageDataPtr;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZCreateBGR8Image
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZCreateBGR8Image(PF_EZDeviceHandle inHandle, PF_EZImage **outImage)
{
	*outImage = NULL;

	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	size_t	size = sizeof(PF_EZImageInternalData);
	PF_EZImageInternalData	*imageDataPtr;
	imageDataPtr = (PF_EZImageInternalData *)malloc(size);
	if (imageDataPtr == NULL)
	{
		return PF_EZ_MEMORY_ERROR;
	}
	memset(imageDataPtr, 0, size);

	size = sizeof(unsigned char *) * deviceDataPtr->cameraNum;
	imageDataPtr->imageArray = (unsigned char **)malloc(size);
	if (imageDataPtr->imageArray == NULL)
	{
		free(imageDataPtr);
		return PF_EZ_MEMORY_ERROR;
	}
	memset(imageDataPtr->imageArray, 0, size);

	if (deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
		size = VP1066_IMAGE_WIDTH * VP1066_IMAGE_HEIGHT * 3 * deviceDataPtr->cameraNum;
	else
		size = VP1066_QVGA_IMAGE_WIDTH * VP1066_QVGA_IMAGE_HEIGHT * 3 * deviceDataPtr->cameraNum;
	imageDataPtr->imageBufPtr = (unsigned char *)malloc(size);
	if (imageDataPtr->imageBufPtr == NULL)
	{
		free(imageDataPtr->imageArray);
		free(imageDataPtr);
		return PF_EZ_MEMORY_ERROR;
	}
	memset(imageDataPtr->imageBufPtr, 0, size);

	unsigned char	*ptr = imageDataPtr->imageBufPtr;
	for (int i = 0; i < deviceDataPtr->cameraNum; i++)
	{
		imageDataPtr->imageArray[i] = ptr;
		if (deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
			ptr += VP1066_IMAGE_WIDTH * VP1066_IMAGE_HEIGHT * 3;
		else
			ptr += VP1066_QVGA_IMAGE_WIDTH * VP1066_QVGA_IMAGE_HEIGHT * 3;
	}

	imageDataPtr->imageNum = deviceDataPtr->cameraNum;
	imageDataPtr->imageType = PF_EZ_IMAGE_BGR8;
	if (deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
	{
		imageDataPtr->width = VP1066_IMAGE_WIDTH;
		imageDataPtr->height = VP1066_IMAGE_HEIGHT;
		imageDataPtr->widthStep = VP1066_IMAGE_WIDTH * 3;
	}
	else
	{
		imageDataPtr->width = VP1066_QVGA_IMAGE_WIDTH;
		imageDataPtr->height = VP1066_QVGA_IMAGE_HEIGHT;
		imageDataPtr->widthStep = VP1066_QVGA_IMAGE_WIDTH * 3;
	}
	imageDataPtr->magicValue = PF_EZ_IMAGE_MAGIC_VALUE;
	imageDataPtr->isDeviceImage = false;

	*outImage = (PF_EZImage *)imageDataPtr;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZDisposeImage
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZDisposeImage(PF_EZDeviceHandle inHandle, PF_EZImage *inImage)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	if (!IsCorrectImage(inImage))
		return PF_EZ_INVALID_IMAGE_ERROR;

	PF_EZImageInternalData	*imageDataPtr;
	imageDataPtr = (PF_EZImageInternalData *)inImage;

	free(imageDataPtr->imageBufPtr);
	free(imageDataPtr->imageArray);

	memset(imageDataPtr, 0, sizeof(PF_EZImageInternalData));
	free(imageDataPtr);

	return PF_EZ_OK;
}


//  Capture Related Functions ==================================================
// -----------------------------------------------------------------------------
//	PF_EZCaptureStart
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZCaptureStart(PF_EZDeviceHandle inHandle)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZResult	result;
	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_CMU &&
		deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
	{
		result = PF_EZCameraReset(inHandle);
		if (result != PF_EZ_OK)
			return result;
	}
	else
	{
		result = PF_EZCameraReset(inHandle);
		if (result != PF_EZ_OK)
			return result;

		if (deviceDataPtr->deviceType == PF_EZ_DEVICE_ProFUSION_25_CMU)
		{
			result = PF_EZVGAMode_CMU(inHandle);
			if (result != PF_EZ_OK)
				return result;
		}
		else
		{
			result = PF_EZQVGAMode_CMU(inHandle);
			if (result != PF_EZ_OK)
				return result;
		}
	}

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZCaptureStop
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZCaptureStop(PF_EZDeviceHandle inHandle)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZGetImage
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZGetImage(PF_EZDeviceHandle inHandle, PF_EZImage *ioPF_EZImage)
{
	PF_EZResult	result;

	result = PF_EZGetImageAsync(inHandle, ioPF_EZImage);
	if (result != PF_EZ_OK)
		return result;

	return PF_EZWaitImage(ioPF_EZImage, 0);
}


// -----------------------------------------------------------------------------
//	PF_EZGetImageAsync
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZGetImageAsync(PF_EZDeviceHandle inHandle, PF_EZImage *ioPF_EZImage)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	if (!IsCorrectImage(ioPF_EZImage))
		return PF_EZ_INVALID_IMAGE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	PF_EZImageInternalData	*imageDataPtr;

	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;
	imageDataPtr = (PF_EZImageInternalData *)ioPF_EZImage;

	if (imageDataPtr->isDeviceImage == false ||
		(imageDataPtr->imageType != PF_EZ_IMAGE_MONO8 &&
		imageDataPtr->imageType != PF_EZ_IMAGE_BAYER_GBRG8))
	{
		return PF_EZ_INVALID_IMAGE_ERROR;
	}

	imageDataPtr->aiocb_data.aio_fildes = deviceDataPtr->deviceDesc;
	imageDataPtr->aiocb_data.aio_buf = imageDataPtr->imageBufPtr;
	imageDataPtr->aiocb_data.aio_nbytes = deviceDataPtr->readSize;
	imageDataPtr->aiocb_data.aio_offset = 0;

	int	result = aio_read(&(imageDataPtr->aiocb_data));
	if (result < 0)
		return PF_EZ_OS_ERROR;
	
	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZWaitImage
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZWaitImage(PF_EZImage *ioPF_EZImage, unsigned int inTimeout)
{
	if (!IsCorrectImage(ioPF_EZImage))
		return PF_EZ_INVALID_IMAGE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	PF_EZImageInternalData	*imageDataPtr;

	imageDataPtr = (PF_EZImageInternalData *)ioPF_EZImage;
	deviceDataPtr = imageDataPtr->deviceDataPtr;

	if (imageDataPtr->isDeviceImage == false ||
		(imageDataPtr->imageType != PF_EZ_IMAGE_MONO8 &&
		imageDataPtr->imageType != PF_EZ_IMAGE_BAYER_GBRG8))
	{
		return PF_EZ_INVALID_IMAGE_ERROR;
	}

	while (aio_error(&(imageDataPtr->aiocb_data)) == EINPROGRESS) ;
	int result = aio_return(&(imageDataPtr->aiocb_data));
	if (result <= 0)
		return PF_EZ_OS_ERROR;

	CalcTimestamp(imageDataPtr);

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZWaitMultipleImages
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZWaitMultipleImages(PF_EZImage **ioPF_EZImages, int inImageNum, bool inIsWaitAll, unsigned int inTimeout)
{
	return PF_EZ_OS_ERROR;
}


//  I2C Access Related Functions ===============================================
// -----------------------------------------------------------------------------
//	PF_EZReadI2C
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZReadI2C(PF_EZDeviceHandle inHandle, int inCameraIndex, unsigned int inI2CAddr, unsigned int *outData)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (inCameraIndex >= deviceDataPtr->cameraNum)
			return PF_EZ_BAD_PARAMETER_ERROR;

	if (VP1066_ReadI2C(deviceDataPtr->deviceDesc, inCameraIndex, inI2CAddr, outData) != 0)
		return PF_EZ_OS_ERROR;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZWriteI2C
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZWriteI2C(PF_EZDeviceHandle inHandle, int inCameraIndex, unsigned int inI2CAddr, unsigned int inData)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (inCameraIndex >= deviceDataPtr->cameraNum)
			return PF_EZ_BAD_PARAMETER_ERROR;

	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, inCameraIndex, inI2CAddr, inData) != 0)
		return PF_EZ_OS_ERROR;

	return PF_EZ_OK;
}


// Camera Reset Related Functions ==============================================
// -----------------------------------------------------------------------------
//	PF_EZCameraReset
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZCameraReset(PF_EZDeviceHandle inHandle)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, VP1066_REG_CAM_RESET, 0x00000001) != 0)
		return PF_EZ_OS_ERROR;
	MyUnixUtils::Sleep(1500);
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, VP1066_REG_CAM_RESET, 0x00000000) != 0)
		return PF_EZ_OS_ERROR;

	return PF_EZ_OK;
}

// -----------------------------------------------------------------------------
//	PF_EZVGAMode_CMU
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZVGAMode_CMU(PF_EZDeviceHandle inHandle)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0x05, (1696-752)) != 0)
		return PF_EZ_OS_ERROR;
	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0x03, 480) != 0)
		return PF_EZ_OS_ERROR;
	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0x06, 56) != 0)
		return PF_EZ_OS_ERROR;
	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0xBD, (0x01e0+9)) != 0)
		return PF_EZ_OS_ERROR;

	// Set Transfer Mode
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, VP1066_REG_CAM_MODE, 0x00000006) != 0)
		return PF_EZ_OS_ERROR;

	// Set Image Position
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, VP1066_REG_CAM_IMG_POS, 0x80 | 1) != 0)
		return PF_EZ_OS_ERROR;

	return PF_EZResync_CMU(inHandle);
}


// -----------------------------------------------------------------------------
//	PF_EZQVGAMode_CMU
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZQVGAMode_CMU(PF_EZDeviceHandle inHandle)
{
	PF_EZResult	result;

	result = PF_EZPropertySetAutoState(inHandle, PF_EZ_ALL_CAMERA, PF_EZ_CAMERA_BRIGHTNESS, false);
	if (result != PF_EZ_OK)
		return result;
	result = PF_EZPropertySetAutoState(inHandle, PF_EZ_ALL_CAMERA, PF_EZ_CAMERA_SHUTTER, false);
	if (result != PF_EZ_OK)
		return result;
	result = PF_EZPropertySetAutoState(inHandle, PF_EZ_ALL_CAMERA, PF_EZ_CAMERA_GAIN, false);
	if (result != PF_EZ_OK)
		return result;
	result = PF_EZPropertySetValue(inHandle, PF_EZ_ALL_CAMERA, PF_EZ_CAMERA_SHUTTER, 15.0);
	if (result != PF_EZ_OK)
		return result;
	result = PF_EZPropertySetValue(inHandle, PF_EZ_ALL_CAMERA, PF_EZ_CAMERA_GAIN, 0.0);
	if (result != PF_EZ_OK)
		return result;

	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0x05, (1696/2-752)) != 0)
		return PF_EZ_OS_ERROR;
	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0x03, 240) != 0)
		return PF_EZ_OS_ERROR;
	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0x06, 28) != 0)
		return PF_EZ_OS_ERROR;
	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0xBD, (0x0100)) != 0)
		return PF_EZ_OS_ERROR;

	// Set Transfer Mode
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, VP1066_REG_CAM_MODE, 0x00000106) != 0)
		return PF_EZ_OS_ERROR;

	// Set Image Position
	if (VP1066_WriteI2C(deviceDataPtr->deviceDesc, PF_EZ_ALL_CAMERA, 0x02, 184) != 0)
		return PF_EZ_OS_ERROR;

	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, VP1066_REG_CAM_IMG_POS, 0x80 | 111) != 0)
		return PF_EZ_OS_ERROR;

	return PF_EZResync_CMU(inHandle);
}


// -----------------------------------------------------------------------------
//	PF_EZResync_CMU
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZResync_CMU(PF_EZDeviceHandle inHandle)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, 0x40, 0x80000000) != 0)
		return PF_EZ_OS_ERROR;
	MyUnixUtils::Sleep(1500);
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, 0x40, 0x00000000) != 0)
		return PF_EZ_OS_ERROR;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZResyncAll_CMU
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZResyncAll_CMU(PF_EZDeviceHandle inHandle)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	// Issue Resync All to the MCG
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, 0x40, 0x40000000) != 0)
		return PF_EZ_OS_ERROR;
	MyUnixUtils::Sleep(1500);
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, 0x40, 0x00000000) != 0)
		return PF_EZ_OS_ERROR;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZCounterReset_CMU
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZCounterReset_CMU(PF_EZDeviceHandle inHandle)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	// Issue Resync All to the MCG
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, 0x40, 0x20000000) != 0)
		return PF_EZ_OS_ERROR;
	MyUnixUtils::Sleep(1500);
	if (VP1066_WriteBAR0(deviceDataPtr->deviceDesc, 0x40, 0x00000000) != 0)
		return PF_EZ_OS_ERROR;

	return PF_EZ_OK;
}


// Linux Specific Local Functions ==============================================
// -----------------------------------------------------------------------------
//	VP1066_ReadI2C
// -----------------------------------------------------------------------------
//
int	VP1066_ReadI2C(int inDeviceDesc, int inCameraIndex, UInt32 inI2CAddr, UInt32 *outData)
{
	UInt32	camsel = 0x1ffffff;

	if (inCameraIndex < 0)
		inCameraIndex = 0;

	camsel &= ~(1 << inCameraIndex);

	if (VP1066_WriteBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_SEL, camsel) != 0)
		return -1;

	if (VP1066_WriteBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_ADDR, inI2CAddr << 16) != 0)
		return -1;
	if (VP1066_WriteBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_CTRL, 0x00000002) != 0)
		return -1;
	if (VP1066_WriteBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_CTRL, 0x00000000) != 0)
		return -1;

	MyUnixUtils::Sleep(50);

	if (VP1066_ReadBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_ADDR, outData) != 0)
		return -1;

	(*outData) &= 0xFFFF;

	return 0;
}


// -----------------------------------------------------------------------------
//	VP1066_WriteI2C
// -----------------------------------------------------------------------------
//
int	VP1066_WriteI2C(int inDeviceDesc, int inCameraIndex, UInt32 inI2CAddr, UInt32 inData)
{
	UInt32	camsel = 0x1ffffff;

	if (inCameraIndex < 0)
		camsel = 0;
	else
		camsel &= ~(1 << inCameraIndex);

	if (VP1066_WriteBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_SEL, camsel) != 0)
		return -1;

	if (VP1066_WriteBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_ADDR,
						(inI2CAddr << 16) + (inData & 0xFFFF)) != 0)
		return -1;
	if (VP1066_WriteBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_CTRL, 0x00000001) != 0)
		return -1;
	MyUnixUtils::Sleep(150);
	if (VP1066_WriteBAR0(inDeviceDesc, VP1066_REG_CAM_I2C_CTRL, 0x00000000) != 0)
		return -1;
	MyUnixUtils::Sleep(150);

	return 0;
}


// -----------------------------------------------------------------------------
//	VP1066_ReadBAR0
// -----------------------------------------------------------------------------
//
int	VP1066_ReadBAR0(int inDeviceDesc, UInt32 inAddressOffset, UInt32 *outData)
{
	VP1066_BAR_DIRECT_ACCESS_DATA	directAccessData;
	int	result;

	directAccessData.addressOffset = inAddressOffset;
	directAccessData.data = 0;

	result = ioctl(inDeviceDesc, VP1066_IOC_READ_BAR, &directAccessData);
	if (result != 0)
		return -1;

	*outData = directAccessData.data;

	return 0;
}


// -----------------------------------------------------------------------------
//	VP1066_WriteBAR0
// -----------------------------------------------------------------------------
//
int	VP1066_WriteBAR0(int inDeviceDesc, UInt32 inAddressOffset, UInt32 inData)
{
	VP1066_BAR_DIRECT_ACCESS_DATA	directAccessData;
	int	result;

	directAccessData.addressOffset = inAddressOffset;
	directAccessData.data = inData;

	result = ioctl(inDeviceDesc, VP1066_IOC_WRITE_BAR, &directAccessData);
	if (result != 0)
		return -1;

	return 0;
}

int	PF_EZGetSGListLength(PF_EZDeviceHandle inHandle)
{
	VP1066_BAR_DIRECT_ACCESS_DATA	directAccessData;
	int	result;

	PF_EZDeviceInternalData * deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	result = ioctl(deviceDataPtr->deviceDesc, VP1066_IOC_GET_SG_LEN, &directAccessData);
	if (result != 0)
		return -2;

	return directAccessData.data;
}
