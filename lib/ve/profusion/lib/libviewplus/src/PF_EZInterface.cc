// =============================================================================
//	PF_EZInterface.cpp
//
//	Revision History:
//	Rev.01 2007/03/03	The First edition
//	Rev.02 2008/01/12	Ver.1.1 - The following 3 functions were added.
//						(PF_EZGetImageAsync, PF_EZWaitImage, PF_EZWaitMultipleImages)
//	Rev.03 2008/08/04	Ver.1.2 - Added support for Linux
//	Rev.04 2008/11/06	Ver.1.3 - The following 3 functions were added.
//						(PF_EZSetActiveCameras, PF_EZGetActiveCameras, PF_EZGetActiveCameraNum)
//	Rev.05 2010/05/07	Change license to the modified BSD license
//	Rev.06 2010/11/02	Added support for CMU version hardware (Branched version)
//
//	This file is licensed under the modified BSD license.
//
//	Copyright (C) 2006-2010 ViewPLUS Inc.
//	All Rights Reserved.
//
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions
//	are met:
//
//	*	Redistributions of source code must retain the above copyright
//		notice, this list of conditions and the following disclaimer.
//
//	*	Redistributions in binary form must reproduce the above
//		copyright notice, this list of conditions and the following
//		disclaimer in the documentation and/or other materials provided
//		with the distribution.
//
//	*	Neither the name of ViewPLUS Inc. nor the names of
//		its contributors may be used to endorse or promote products
//		derived from this software without specific prior written
//		permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// =============================================================================
/*!
	\file		PF_EZInterface.cpp
	\author		Dairoku Sekiguchi
	\version	1.3
	\date		2008/11/06
	\license	modified BSD license
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
#include "PF_EZInterfaceLocal.h"


// -----------------------------------------------------------------------------
// 	globals
// -----------------------------------------------------------------------------
const char	*gInterfaceDescription =
	"ProFUSION Library EZ-Interface: API Ver.5, DLL Ver.1.4.1.0, 2010/11/02";

// -----------------------------------------------------------------------------
// 	prototypes
// -----------------------------------------------------------------------------
bool	IsCorrectDeviceHandle(const PF_EZDeviceHandle inHandle);
bool	IsCorrectImage(const PF_EZImage *inImage);
void	CalcTimestamp(PF_EZImageInternalData *inInternalImage);
void	Demosaic_GBRG(int inWidth, int inHeight, unsigned char *inBayer, unsigned char *outColor);
void	demosaiic_bgr_color_GBRG(int width, int height, unsigned char *bayer, unsigned char *color);


//  ProFUSION Library EZ-Interfac Version Related Functions ====================
// -----------------------------------------------------------------------------
//	PF_EZGetAPIVersion
// -----------------------------------------------------------------------------
//
_PF_API int	_PF_CALL	PF_EZGetAPIVersion()
{
	return PF_EZ_API_VERSION;
}


// -----------------------------------------------------------------------------
//	PF_EZGetInterfaceDescription
// -----------------------------------------------------------------------------
//
_PF_API const char * _PF_CALL	PF_EZGetInterfaceDescription()
{
	return gInterfaceDescription;
}


//  ProFUSION Device Related Functions =========================================
// -----------------------------------------------------------------------------
//	PF_EZGetDeviceType
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZGetDeviceType(PF_EZDeviceHandle inHandle, PF_EZDeviceType *outDeviceType)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	*outDeviceType = deviceDataPtr->deviceType;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZDeviceGetSerialNumber
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZDeviceGetSerialNumber(PF_EZDeviceHandle inHandle, unsigned int *outSerialNumber)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	*outSerialNumber = deviceDataPtr->serialNumber;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZDeviceGetCameraNum
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZDeviceGetCameraNum(PF_EZDeviceHandle inHandle, int *outCameraNum)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	*outCameraNum = deviceDataPtr->cameraNum;

	return PF_EZ_OK;
}


//  Camera Property Functions ==================================================
// -----------------------------------------------------------------------------
//	PF_EZIsValidProperty
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZIsValidProperty(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, bool *outIsValie)
{
	*outIsValie = false;

	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	if (inProperty > PF_EZ_PROPERTY_ANY && inProperty <= PF_EZ_CAMERA_GAIN)
		*outIsValie = true;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZPropertyHasAutoFunction
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZPropertyHasAutoFunction(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, bool *outHasAutoFunction)
{
	*outHasAutoFunction = false;

	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	if (inProperty > PF_EZ_PROPERTY_ANY && inProperty <= PF_EZ_CAMERA_GAIN)
		*outHasAutoFunction = true;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZPropertySetValue
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZPropertySetValue(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, double inValue)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (inCameraIndex >= deviceDataPtr->cameraNum)
			return PF_EZ_BAD_PARAMETER_ERROR;

	UInt32	addr, regValue;
	double	valueMin, valueMax;
	unsigned char	ucharValue;

	PF_EZPropertyGetValueRange(inHandle, inCameraIndex, inProperty, &valueMin, &valueMax);
	if (inValue < valueMin)
		inValue = valueMin;
	if (inValue > valueMax)
		inValue = valueMax;

	switch (inProperty)
	{
		case PF_EZ_CAMERA_BRIGHTNESS:
			addr = 0x48;
			*((char *)(&ucharValue)) = (char )inValue;
			regValue = ucharValue;
			break;
		case PF_EZ_CAMERA_AUTO_EXPOSURE:
			addr = 0xA5;
			regValue = (UInt32 )inValue;
			break;
		case PF_EZ_CAMERA_SHUTTER:
			addr = 0x0B;
			regValue = (UInt32 )(inValue / valueMin);
			break;
		case PF_EZ_CAMERA_GAIN:
			addr = 0x35;
			regValue = (UInt32 )(pow(10.0, (inValue / 20)) * 16.0);
			break;
		default:
			return PF_EZ_BAD_PARAMETER_ERROR;
			break;
	}

	return PF_EZWriteI2C(inHandle, inCameraIndex, addr ,regValue);
}


// -----------------------------------------------------------------------------
//	PF_EZPropertyGetValue
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZPropertyGetValue(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, double *outValue)
{
	UInt32	regValue;
	unsigned char	value;
	double	valueMin, valueMax;
	PF_EZResult	result ;
	
	switch (inProperty)
	{
		case PF_EZ_CAMERA_BRIGHTNESS:
			result = PF_EZReadI2C(inHandle, inCameraIndex, 0x48, &regValue);
			if (result != PF_EZ_OK)
				return result;

			value = (0xFF & regValue);
			*outValue = *((char *)&value);
			break;
		case PF_EZ_CAMERA_AUTO_EXPOSURE:
			result = PF_EZReadI2C(inHandle, inCameraIndex, 0xA5, &regValue);
			if (result != PF_EZ_OK)
				return result;

			*outValue = regValue;
			break;
		case PF_EZ_CAMERA_SHUTTER:
			PF_EZPropertyGetValueRange(inHandle, inCameraIndex, inProperty, &valueMin, &valueMax);
			result = PF_EZReadI2C(inHandle, inCameraIndex, 0xBB, &regValue);
			if (result != PF_EZ_OK)
				return result;

			*outValue = (double )regValue * valueMin;
			break;
		case PF_EZ_CAMERA_GAIN:
			result = PF_EZReadI2C(inHandle, inCameraIndex, 0xBA, &regValue);
			if (result != PF_EZ_OK)
				return result;

			*outValue = 20.0 * log10((double )regValue / 16.0);
			break;
		default:
			return PF_EZ_BAD_PARAMETER_ERROR;
			break;
	}

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZPropertySetAutoState
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZPropertySetAutoState(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, bool inAuto)
{
	UInt32	addr, regValue;
	PF_EZResult	result ;

	switch (inProperty)
	{
		case PF_EZ_CAMERA_BRIGHTNESS:
			addr = 0x47;
			if (inAuto)
				regValue = 0x8080;
			else
				regValue = 0x8081;
			break;
		case PF_EZ_CAMERA_AUTO_EXPOSURE:
			addr = 0xA5;
			if (inAuto)
				regValue = 0x3A;
			else
				regValue = 0x3A;
			break;
		case PF_EZ_CAMERA_SHUTTER:
			addr = 0xAF;
			result = PF_EZReadI2C(inHandle, inCameraIndex, addr, &regValue);
			if (result != PF_EZ_OK)
				return result;

			if (inAuto)
				regValue = (0xFFFE & regValue) + 1;
			else
				regValue = (0xFFFE & regValue);
			break;
		case PF_EZ_CAMERA_GAIN:
			addr = 0xAF;
			result = PF_EZReadI2C(inHandle, inCameraIndex, addr, &regValue);
			if (result != PF_EZ_OK)
				return result;

			if (inAuto)
				regValue = (0xFFFD & regValue) + 2;
			else
				regValue = (0xFFFD & regValue);
			break;
		default:
			return PF_EZ_BAD_PARAMETER_ERROR;
			break;
	}

	result = PF_EZWriteI2C(inHandle, inCameraIndex, addr, regValue);
	if (result != PF_EZ_OK)
		return result;

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZPropertyGetAutoState
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZPropertyGetAutoState(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, bool *outAuto)
{
	UInt32	regValue;
	PF_EZResult	result ;

	switch (inProperty)
	{
		case PF_EZ_CAMERA_BRIGHTNESS:
			result = PF_EZReadI2C(inHandle, inCameraIndex, 0x47, &regValue);
			if (result != PF_EZ_OK)
				return result;

			if ((regValue & 0x0001) == 0x0000)
				*outAuto = true;
			else
				*outAuto = false;
			break;
		case PF_EZ_CAMERA_AUTO_EXPOSURE:
			result = PF_EZReadI2C(inHandle, inCameraIndex, 0xA5, &regValue);
			if (result != PF_EZ_OK)
				return result;

			if (regValue == 0x3A)
				*outAuto = true;
			else
				*outAuto = false;
			break;
		case PF_EZ_CAMERA_SHUTTER:
			result = PF_EZReadI2C(inHandle, inCameraIndex, 0xAF, &regValue);
			if (result != PF_EZ_OK)
				return result;

			if ((regValue & 0x0001) == 0x0001)
				*outAuto = true;
			else
				*outAuto = false;
			break;
		case PF_EZ_CAMERA_GAIN:
			result = PF_EZReadI2C(inHandle, inCameraIndex, 0xAF, &regValue);
			if (result != PF_EZ_OK)
				return result;

			if ((regValue & 0x0002) == 0x0002)
				*outAuto = true;
			else
				*outAuto = false;
			break;
		default:
			return PF_EZ_BAD_PARAMETER_ERROR;
			break;
	}

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZPropertyGetValueRange
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZPropertyGetValueRange(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, double *outValueMin, double *outValueMax)
{
	*outValueMin = 0;
	*outValueMax = 0;

	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (inCameraIndex >= deviceDataPtr->cameraNum)
			return PF_EZ_BAD_PARAMETER_ERROR;

	switch (inProperty)
	{
		case PF_EZ_CAMERA_BRIGHTNESS:
			*outValueMin = -128.0;
			*outValueMax = 127.0;
			break;
		case PF_EZ_CAMERA_AUTO_EXPOSURE:
			*outValueMin = 7.0;		// 1.0 original
			*outValueMax = 62.0;	// 64.0 original
			break;
		case PF_EZ_CAMERA_SHUTTER:
			if (deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
			{
				*outValueMin = 6.349E-2;	// 33.33 / (480 + 45)
				*outValueMax = 33.0;
			}
			else
			{
				*outValueMin = 6.349E-2;	// 33.33 / (480 + 45)
				*outValueMax = 16.5;
			}
			break;
		case PF_EZ_CAMERA_GAIN:
			*outValueMin = 0.0;
			*outValueMax = 12.04;
			break;
		default:
			return PF_EZ_BAD_PARAMETER_ERROR;
			break;
	}

	return PF_EZ_OK;
}


// -----------------------------------------------------------------------------
//	PF_EZPropertyGetUnits
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL	PF_EZPropertyGetUnits(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, const char **outStringPtr)
{
	const static char	*sBrightnessUnit = "";
	const static char	*sAutoExposureUnit = "";
	const static char	*sShutterUnit = "ms";
	const static char	*sGainUnit = "dB";
	const static char	*sUnknownUnit = "Unknown";

	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (inCameraIndex >= deviceDataPtr->cameraNum)
			return PF_EZ_BAD_PARAMETER_ERROR;

	switch (inProperty)
	{
		case PF_EZ_CAMERA_BRIGHTNESS:
			*outStringPtr = sBrightnessUnit;
			break;
		case PF_EZ_CAMERA_AUTO_EXPOSURE:
			*outStringPtr = sAutoExposureUnit;
			break;
		case PF_EZ_CAMERA_SHUTTER:
			*outStringPtr = sShutterUnit;
			break;
		case PF_EZ_CAMERA_GAIN:
			*outStringPtr = sGainUnit;
			break;
		default:
			return PF_EZ_BAD_PARAMETER_ERROR;
			*outStringPtr = sUnknownUnit;
			break;
	}

	return PF_EZ_OK;
}


//  Color Processing Functions =================================================
// -----------------------------------------------------------------------------
//	PF_EZColorProcessing
// -----------------------------------------------------------------------------
//
_PF_API PF_EZResult _PF_CALL		PF_EZColorProcessing(PF_EZDeviceHandle inHandle, const PF_EZImage *inDeviceImage, const PF_EZImage *outColorImage)
{
	if (!IsCorrectDeviceHandle(inHandle))
		return PF_EZ_INVALID_HANDLE_ERROR;

	if (!IsCorrectImage(inDeviceImage))
		return PF_EZ_INVALID_IMAGE_ERROR;

	if (!IsCorrectImage(outColorImage))
		return PF_EZ_INVALID_IMAGE_ERROR;

	PF_EZDeviceInternalData	*deviceDataPtr;
	PF_EZImageInternalData	*deviceImagePtr;
	PF_EZImageInternalData	*colorImagePtr;

	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;
	deviceImagePtr = (PF_EZImageInternalData *)inDeviceImage;
	colorImagePtr = (PF_EZImageInternalData *)outColorImage;

	for (int i = 0; i < deviceImagePtr->imageNum; i++)
	{
//		demosaiic_bgr_color_GBRG(
		Demosaic_GBRG(
			deviceImagePtr->width,
			deviceImagePtr->height,
			deviceImagePtr->imageArray[i],
			colorImagePtr->imageArray[i]);
	}

	return PF_EZ_OK;
}


//  Private Functions ==========================================================
// -----------------------------------------------------------------------------
//	IsCorrectDeviceHandle
// -----------------------------------------------------------------------------
//
bool	IsCorrectDeviceHandle(const PF_EZDeviceHandle inHandle)
{
	if (inHandle == 0)
		return false;

	PF_EZDeviceInternalData	*deviceDataPtr;
	deviceDataPtr = (PF_EZDeviceInternalData *)inHandle;

	if (deviceDataPtr->magicValue != (int)PF_EZ_HANDLE_MAGIC_VALUE)
		return false;

	return true;
}


// -----------------------------------------------------------------------------
//	IsCorrectImage
// -----------------------------------------------------------------------------
//
bool	IsCorrectImage(const PF_EZImage *inImage)
{
	if (inImage == 0)
		return false;

	PF_EZImageInternalData	*imageDataPtr;
	imageDataPtr = (PF_EZImageInternalData *)inImage;

	if (imageDataPtr->magicValue != (int)PF_EZ_IMAGE_MAGIC_VALUE)
		return false;

	return true;
}


// -----------------------------------------------------------------------------
//	CalcTimestamp
// -----------------------------------------------------------------------------
//
void	CalcTimestamp(PF_EZImageInternalData *inInternalImage)
{
	if (inInternalImage->deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_CMU &&
		inInternalImage->deviceDataPtr->deviceType != PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU)
	{
		inInternalImage->frameNumber = *((unsigned int *)inInternalImage->imageBufPtr);
		inInternalImage->timestamp = inInternalImage->frameNumber * PF_EZ_TOTAL_FRAME_TIME;
	}
	else
	{
		inInternalImage->frameNumber = *((unsigned int *)inInternalImage->imageBufPtr);
		inInternalImage->timestamp =  *((unsigned int *)inInternalImage->imageBufPtr);
	}
}


// -----------------------------------------------------------------------------
//	Demosaic_GBRG (Nearest Neighbor ultra fast - Sekiguchi's routine)
// -----------------------------------------------------------------------------
//
void	Demosaic_GBRG(int inWidth, int inHeight, unsigned char *inBayer, unsigned char *outColor)
{
	int	x, y;
	unsigned int	*in1, *in2, *out1, *out2;
	unsigned int	a, b;

	in1 = (unsigned int *)inBayer;
	in2 = (unsigned int *)(inBayer + inWidth);
	out1 = (unsigned int *)outColor;
	out2 = (unsigned int *)(outColor + inWidth * 3);

	for (y = 0; y < inHeight; y+=2)
	{
		for (x = 0; x < inWidth; x+=4)
		{
			a = *in1;
			in1++;
			b = *in2;
			in2++;
			
			*out1 = ((a >> 8) & 0xff) + ((a & 0xff) << 8) + ((b & 0xff) << 16) + ((a << 16) & 0xff000000);
			out1++;
			*out1 = (a & 0xff) + ((b & 0xff) << 8) + ((a << 8) & 0xffff0000);
			out1++;
			*out1 = ((b >> 16) & 0xff) + ((a >> 16) & 0xff00) + (a & 0xff0000) + ((b << 8) & 0xff000000);
			out1++;

			*out2 = ((a >> 8) & 0xff) + (b & 0xff00) + ((b << 16) & 0xff0000) + ((a << 16) & 0xff000000);
			out2++;
			*out2 = ((b >> 8) & 0xff) + ((b << 8) & 0xff00) + ((a << 8) & 0xff0000) + ((b << 16) & 0xff000000);
			out2++;
			*out2 = ((b >> 16) & 0xff) + ((a >> 16) & 0xff00) + ((b >> 8) & 0xff0000) + ((b << 8) & 0xff000000);
			out2++;
		}

		in1 += (inWidth / 4);
		in2 += (inWidth / 4);

		out1 += (inWidth / 4 * 3);
		out2 += (inWidth / 4 * 3);
	}
}


// -----------------------------------------------------------------------------
//	demosaiic_bgr_color_GBRG (Mori's routine)
// -----------------------------------------------------------------------------
//
void demosaiic_bgr_color_GBRG(int width, int height, unsigned char *bayer, unsigned char *color)
{
	// Cener part
	for ( int y = 2; y < height-2; y+=2){
		for ( int x = 2; x < width-2; x+=2){
			
			int ptr = y * width + x;
			
			int c1 = bayer[ ptr]; // G1
			int c2 = bayer[ ptr + 1]; // B
			int c3 = bayer[ ptr + width]; // R
			int c4 = bayer[ ptr + width + 1]; // G2
			
			int c5 = bayer[ ptr - width]; // R
			int c14 = bayer[ ptr - width + 1]; // G
			int c6 = bayer[ ptr - width + 2]; // R
			
			int c7 = bayer[ ptr - 1]; // B
			int c8 = bayer[ ptr + 2]; // G

			int c9 = bayer[ ptr + width - 1]; // G
			int c10 = bayer[ ptr + width + 2]; // R

			int c11 = bayer[ ptr + width * 2 - 1]; // B
			int c12 = bayer[ ptr + width * 2]; // G
			int c13 = bayer[ ptr + width * 2 + 1]; // B


			color[ ptr * 3] = ( c2 + c7)/2;
			color[ ptr * 3 + 1] = c1;
			color[ ptr * 3 + 2] = ( c3 + c5)/2;

			color[ (ptr+1) * 3] = c2;
			color[ (ptr+1) * 3 + 1] = (c1+c4+c8+c14)/4;
			color[ (ptr+1) * 3 + 2] = ( c3+c5+c6+c10)/4;

			color[ (ptr+width) * 3] = (c2+c7+c11+c13)/4;
			color[ (ptr+width) * 3 + 1] = (c1+c4+c9+c12)/4;
			color[ (ptr+width) * 3 + 2] = c3;

			color[ (ptr+width+1) * 3] = (c2+c13)/2;
			color[ (ptr+width+1) * 3 + 1] = c4;
			color[ (ptr+width+1) * 3 + 2] = (c3+c10)/2;

		}
	}

	// Top and bottom 2lines
	for ( int y = 0; y < height; y+= height-2){
		for ( int x = 0; x < width; x+=2){
			int ptr = y * width + x;
			int c1 = bayer[ ptr]; // G1
			int c2 = bayer[ ptr + 1]; // B
			int c3 = bayer[ ptr + width]; // R
			int c4 = bayer[ ptr + width + 1]; // G2

			color[ ptr * 3] = c2;
			color[ ptr * 3 + 1] = c1;
			color[ ptr * 3 + 2] = c3;

			color[ (ptr+1) * 3] = c2;
			color[ (ptr+1) * 3 + 1] = (c1+c4)/2;
			color[ (ptr+1) * 3 + 2] = c3;

			color[ (ptr+width) * 3] = c2;
			color[ (ptr+width) * 3 + 1] = (c1+c4)/2;
			color[ (ptr+width) * 3 + 2] = c3;

			color[ (ptr+width+1) * 3] = c2;
			color[ (ptr+width+1) * 3 + 1] = c4;
			color[ (ptr+width+1) * 3 + 2] = c3;
		}
	}

	// Left and right 2lines
	for ( int x = 0; x < width; x+= width-2){
		for ( int y = 0; y < height; y+=2){
			int ptr = y * width + x;
			int c1 = bayer[ ptr]; // G1
			int c2 = bayer[ ptr + 1]; // B
			int c3 = bayer[ ptr + width]; // R
			int c4 = bayer[ ptr + width + 1]; // G2

			color[ ptr * 3] = c2;
			color[ ptr * 3 + 1] = c1;
			color[ ptr * 3 + 2] = c3;

			color[ (ptr+1) * 3] = c2;
			color[ (ptr+1) * 3 + 1] = (c1+c4)/2;
			color[ (ptr+1) * 3 + 2] = c3;

			color[ (ptr+width) * 3] = c2;
			color[ (ptr+width) * 3 + 1] = (c1+c4)/2;
			color[ (ptr+width) * 3 + 2] = c3;

			color[ (ptr+width+1) * 3] = c2;
			color[ (ptr+width+1) * 3 + 1] = c4;
			color[ (ptr+width+1) * 3 + 2] = c3;
		}
	}
}
