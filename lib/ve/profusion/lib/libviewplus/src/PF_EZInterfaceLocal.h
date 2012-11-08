// =============================================================================
//	PF_EZInterfaceLocal.h
//
//	Revision History:
//	Rev.01 2008/08/04	The First edition
//	Rev.02 2010/05/07	Change license to the modified BSD license
//
//	This file is licensed under the modified BSD license.
//
//	Copyright (C) 2008-2010 ViewPLUS Inc.
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
	\file		PF_EZInterfaceLocal.h
	\author		Dairoku Sekiguchi
	\version	1.0
	\date		2008/08/04
	\license	modified BSD license
	\brief		This file defines ProFUSION Library EZ-Interface routines.

	This file defines ProFUSION Library EZ-Interface routines.

	\note
		- None

	\todo
		- None
*/

#ifndef __PF_EZ_INTERFACE_LOCAL_H_
#define __PF_EZ_INTERFACE_LOCAL_H_


// -----------------------------------------------------------------------------
// 	include files
// -----------------------------------------------------------------------------
#include <math.h>
#include "VP1066Registers.h"
#include "PF_EZInterface.h"
#ifdef _WIN32	//	Win32 Specific Part
#include <windows.h>
#include <winioctl.h>
#include <setupapi.h>
#include <INITGUID.H>
#include "VP1066Interface.h"
#else
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <aio.h>
#include "VP1066Interface.h"
#endif

// -----------------------------------------------------------------------------
// 	macros
// -----------------------------------------------------------------------------
#define	PF_EZ_HANDLE_MAGIC_VALUE			0x95142772
#define	PF_EZ_IMAGE_MAGIC_VALUE				0x95142773
#define	PF_EZ_LOCAL_MULTIPLE_IMAGE_NUM		256
#define	PF_EZ_TOTAL_FRAME_TIME				(timestamp_t )33521		//	1 / 26.5 * 444154  * 2  = 33.521ms
#ifndef	NULL
#define NULL								0
#endif

// -----------------------------------------------------------------------------
// 	typedefs
// -----------------------------------------------------------------------------
typedef struct
{
	int				magicValue;
#ifdef _WIN32	//	Win32 Specific Part
	HANDLE			driverHandle;
#else	// Linux Specific Part
	int				deviceDesc;
#endif
	UInt32			serialNumber;
	int				cameraNum;
	PF_EZDeviceType	deviceType;

	int				readSize;

	unsigned int	activeCameraMask;
	int				activeCameraNum;

} PF_EZDeviceInternalData;

typedef struct 
{
	int				imageNum;
	PF_EZImageType	imageType;
	int				width;
	int				height;
	int				widthStep;
	timestamp_t		timestamp;
	unsigned char	**imageArray;

	int				magicValue;
	unsigned char	*imageBufPtr;
	bool			isDeviceImage;
#ifdef _WIN32	//	Win32 Specific Part
	OVERLAPPED		overlappedData;
#else	// Linux Specific Part
	struct aiocb	aiocb_data;
#endif
	unsigned int	frameNumber;
	PF_EZDeviceInternalData	*deviceDataPtr;

} PF_EZImageInternalData;


// -----------------------------------------------------------------------------
// 	prototypes
// -----------------------------------------------------------------------------
bool	IsCorrectDeviceHandle(const PF_EZDeviceHandle inHandle);
bool	IsCorrectImage(const PF_EZImage *inImage);
void	CalcTimestamp(PF_EZImageInternalData *inInternalImage);
void	Demosaic_GBRG(int inWidth, int inHeight, unsigned char *inBayer, unsigned char *outColor);
void	demosaiic_bgr_color_GBRG(int width, int height, unsigned char *bayer, unsigned char *color);


// For CMU
_PF_API PF_EZResult _PF_CALL	PF_EZVGAMode_CMU(PF_EZDeviceHandle inHandle);
_PF_API PF_EZResult _PF_CALL	PF_EZQVGAMode_CMU(PF_EZDeviceHandle inHandle);
_PF_API PF_EZResult _PF_CALL	PF_EZResync_CMU(PF_EZDeviceHandle inHandle);
_PF_API PF_EZResult _PF_CALL	PF_EZResyncAll_CMU(PF_EZDeviceHandle inHandle);
_PF_API PF_EZResult _PF_CALL	PF_EZCounterReset_CMU(PF_EZDeviceHandle inHandle);

#endif	// #ifdef __PF_EZ_INTERFACE_LOCAL_H_
