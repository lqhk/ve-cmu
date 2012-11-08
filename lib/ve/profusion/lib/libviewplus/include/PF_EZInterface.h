// =============================================================================
//	PF_EZInterface.h
//
//	Revision History:
//	Rev.01 2007/03/03	The First edition
//	Rev.02 2008/01/12	Ver.1.1 - The following 3 functions were added.
//						(PF_EZGetImageAsync, PF_EZWaitImage, PF_EZWaitMultipleImages)
//	Rev.03 2008/08/04	Ver.1.2 - Added support for Linux
//	Rev.04 2008/11/06	Ver.1.3 - The following 3 functions were added.
//						(PF_EZSetActiveCameras, PF_EZGetActiveCameras, PF_EZGetActiveCameraNum)
//	Rev.05 2010/05/07	Change license to the modified BSD license
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
	\file		PF_EZInterface.h
	\author		Dairoku Sekiguchi
	\version	1.3
	\date		2008/11/06
	\brief		Header file for ProFUSION Library EZ-Interface

	This header file defines the functions of the ProFUSION Library EZ-Interface.

	\mainpage
	\section introduction Introduction
	ProFUSION Library EZ-Interface is a library designed specifically for use with
	multi-camera system such as ViewPLUS/PointGrey ProFUSION camera arrays.
	ProFUSION Library EZ-Interface is a part of the ProFUSION software package and
	provides users with a C language interface. ProFUSION Library EZ-Interface is
	C wrapping for the C++ ProFUSION Library(*1) and has been designed to be an
	easy-to-use interface by keeping its design as simple as possible compared
	with the C++ ProFUSION Library. Therefore ProFUSION Library EZ-Interface will
	be a good choice for those who want to start the development project of
	the ProFUSION quickly.

	(*1) The C++ ProFUSION Library is not available in the current release.

	\section function_group Function groups
	The functions included in the ProFUSION Library EZ-Interface can be grouped
	into modules, as shown below:

	- \ref VersionRelatedFunctions	"ProFUSION Library EZ-Interface Version functions"
	- \ref DeviceRelatedFunctions	"Device functions"
	- \ref ImageRelatedFunctions	"Image Data functions"
	- \ref CaptureRelatedFunctions	"Image Capture functions"
	- \ref CameraPropertyRelatedFunctions	"Camera Property functions"
	- \ref ColorProcessingRelatedFunctions	"Color Processing functions"
	- \ref OtherFunctions					"Miscellaneous functions"
*/


#ifndef __PF_EZ_INTERFACE_H_
#define __PF_EZ_INTERFACE_H_


// -----------------------------------------------------------------------------
// 	include files
// -----------------------------------------------------------------------------
#ifdef _WIN32	//	Win32 Specific Part

#include <windows.h>
#endif

#include <stdlib.h>

// -----------------------------------------------------------------------------
// 	macros
// -----------------------------------------------------------------------------
//!	API Version of the ProFUSION Library EZ-Interface
/*!
	This macro corresponds to the API Version that is defined with the header file.
	Allows the user to confirm whether dynamically linked library (DLL) has intended
	version of the interface or not by comparing the return values of
	the PF_EZGetAPIVersion(). This API Version value will be incremented when there
	are additions in functions. Therefore, if the PF_EZ_API_VERSION has the bigger
	value than the PF_EZGetAPIVersion() value at  application runtime check,
	this means that the loaded DLL version is too old.

	Example:
		if (PF_EZGetAPIVersion() >= PF_EZ_API_VERSION)
			printf("DLL version OK\n");
		else
			printf("DLL version is too old\n");
*/
#define	PF_EZ_API_VERSION					4


//!	Defines the camera index which means all cameras
/*!
	This macro is used for specifying all cameras in the camera array.
*/
#define	PF_EZ_ALL_CAMERA					-1


//!	Defines the active camera mask value which means all cameras
/*!
	This macro is used for specifying all cameras in the camera array.
*/
#define	PF_EZ_ALL_CAMERA_MASK				0x001FFFFFF


// -----------------------------------------------------------------------------
// PF_EZResult type
// -----------------------------------------------------------------------------
//!	Result Code of the ProFUSION Library EZ-Interface
/*!
	The result code returned by the functions in the ProFUSION Library EZ-Interface.
*/
typedef enum	PF_EZResult
{
	PF_EZ_OK			= 0,
	PF_EZ_OS_ERROR,					//!< OS error
	PF_EZ_MEMORY_ERROR,				//!< Memory allocation error
	PF_EZ_IO_TIMEOUT_ERROR,			//!< I/O operation timed out

	PF_EZ_INVALID_HANDLE_ERROR,		//!< Invalid handle passed
	PF_EZ_INVALID_IMAGE_ERROR,		//!< Invalid image format passed
	PF_EZ_BAD_ARGUMENT_ERROR,		//!< Invalid argument passed
	PF_EZ_BAD_PARAMETER_ERROR,		//!< Invalid parameter passed
	PF_EZ_INTERNAL_ERROR,			//!< Internal error
	PF_EZ_NOT_SUPPORTED_ERROR,		//!< Unsupported function called
	PF_EZ_INVALID_OPERATION_ERROR,	//!< Invalid function called. (difference with PF_NOT_SUPPORTED: may not be an error, depending on the circumstances)
	PF_EZ_NOT_IMPLEMENTED_ERROR,	//!< Unimplemented function called

	PF_EZ_DEVICE_NOT_FOUND_ERROR,	//!< No device found
	PF_EZ_DRIVER_MISMATCH_ERROR,	//!< There is version mismatch between the DLL and the driver
	PF_EZ_FIRMWARE_MISMATCH_ERROR,	//!< There is version mismatch between the library and the firmware

	PF_EZ_WAIT_ABANDONED,			//!< Waiting of the image was abandoned
	PF_EZ_WAIT_TIMEOUT				//!< Waiting of the image timeout

} PF_EZResult;


// -----------------------------------------------------------------------------
// PF_EZDeviceType type
// -----------------------------------------------------------------------------
//!	ProFUSION Library EZ-Interface Device type
/*!
	An enumeration of the devices that are supported by the ProFUSION Library EZ-Interface.
*/
typedef enum	PF_EZDeviceType
{
	PF_EZ_DEVICE_ANY				= 0,				//!< Used when the device type is not explicitly specified
	PF_EZ_DEVICE_ProFUSION_25,							//!< ProFUSION 25 (no distinction between color and monochrome)
	PF_EZ_DEVICE_ProFUSION_25M,
	PF_EZ_DEVICE_ProFUSION_25C,

	//	for CMU version hardware
	PF_EZ_DEVICE_ProFUSION_25_CMU	= 1024,
	PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU
} PF_EZDeviceType;


// -----------------------------------------------------------------------------
// PF_EZImageFormat type
// -----------------------------------------------------------------------------
//!	ProFUSION Library EZ-Interface image type
/*!
	An enumeration of the image formats that are supported by the ProFUSION Library EZ-Interface.
*/
typedef enum	PF_EZImageType
{
	PF_EZ_IMAGE_ANY					= 0,				//!< Used when the image type is not explicitly specified
	PF_EZ_IMAGE_MONO8				= 1,				//!< Monochrome 8bit (used with monochrome models)
	PF_EZ_IMAGE_S_MONO16,
	PF_EZ_IMAGE_BAYER_GBRG8,							//!< Color 8bit bayer pattern (used with color models)
	PF_EZ_IMAGE_BAYER_GRBG8,
	PF_EZ_IMAGE_BAYER_BGGR8,
	PF_EZ_IMAGE_BAYER_RGGB8,
	PF_EZ_IMAGE_BGR8				= 20,				//!< Color 8bitBGR format (used with images after color processing)
} PF_EZImageType;


// -----------------------------------------------------------------------------
// PF_EZCameraProperty type
// -----------------------------------------------------------------------------
//!	ProFUSION Library EZ-Interface Camera Property type
/*!
	An enumeration of the different camera properties that can be set via ProFUSION Library EZ-Interface.
*/
typedef enum	PF_EZCameraProperty
{
	PF_EZ_PROPERTY_ANY				= 0,			//!< Used when not explicitly specifying the property type

	PF_EZ_CAMERA_BRIGHTNESS,						//!< The brightness property of the camera
	PF_EZ_CAMERA_AUTO_EXPOSURE,						//!< The automatic exposure property of the camera
	PF_EZ_CAMERA_SHUTTER,							//!< The shutter speed property of the camera
	PF_EZ_CAMERA_GAIN								//!< The gain property of the camera

} PF_EZCameraProperty;


/*!
	\def _PF_API
	Whether appropriate functions are exported as DLL or imported from DLL is defined.
	Specifically, either __declspec (export) or __declspec (dllimport) is defined
*/
/*!
	\def _PF_CALL
	Defines the type of function call. __stdcall is defined.
*/

#ifdef _WIN32	//	Win32 Specific Part
 #ifdef PF_EZ_DLL_EXPORTS
  #define _PF_API	__declspec(dllexport)
  #define _PF_CALL	__stdcall
 #else
  #ifdef PF_EZ_STATIC_IMPORTS
   #define _PF_API
   #define _PF_CALL
  #else
   #define _PF_API	__declspec(dllimport)
   #define _PF_CALL	__stdcall
  #endif
 #endif
#else	//	#ifdef _WIN32
 #define _PF_API
 #define _PF_CALL
#endif	//	#ifdef _WIN32


// -----------------------------------------------------------------------------
// 	typedefs
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// PF_EZDeviceHandle type
// -----------------------------------------------------------------------------
//!	Handle of ProFUSION Device
/*!
	This is a handle for accessing any camera array that is supported with
	the ProFUSION Library EZ-Interface.
*/
typedef void	*PF_EZDeviceHandle;


// -----------------------------------------------------------------------------
// timestamp_t type
// -----------------------------------------------------------------------------
//!	Time Stamp type
/*!
	The time stamp type of the image data is expressed. The unit is us (1x10-6 sec).
*/
#ifdef _WIN32
typedef	unsigned __int64		timestamp_t;
#else
typedef	unsigned long long		timestamp_t;
#endif


// -----------------------------------------------------------------------------
// PF_EZImage type
// -----------------------------------------------------------------------------
//!	A structure for storing image data of the ProFUSION device
/*!
	This is a structure for storing image data of any device that is supported
	by the ProFUSION Library EZ-Interface. Multiple images from one camera array
	by one capturing are contained in one structure. All multiple camera images
	included in this have the same parameters (format, size of images, etc.).
*/
typedef struct 
{
	int				imageNum;		//!< Number of images included in the structure.
	PF_EZImageType	imageType;		//!< Image formats: Multiple images included in one structure are all in the same format and are indicated in this parameter.
	int				width;			//!< The width of images: Multiple images included in one structure have all the same width and are indicated in this parameter.
	int				height;			//!< The height of images: Multiple images included in one structure have all the same height and are indicated in this parameter.
	int				widthStep;		//!< The number of bytes in one line of an image: Multiple images included in one structure have all the same number of bytes in one line of a image and are indicated in this parameter.
	timestamp_t		timestamp;		//!< The time when an image was captured: The timestamps of multiple images included in one structure are all the same and are indicated in this parameter.
	unsigned char	**imageArray;	//!< The pointer list for each image.
} PF_EZImage;


#ifdef	__cplusplus
extern "C" {
#endif

/*!
\defgroup VersionRelatedFunctions	ProFUSION Library EZ-Interface Version functions
@{
*/
// -----------------------------------------------------------------------------
//	PF_EZGetAPIVersion
// -----------------------------------------------------------------------------
//!	A function for getting the version of the ProFUSION Library EZ-Interface
/*!
	This function returns the API Version of the ProFUSION Library
	EZ-Interface. By comparing it with the \ref PF_EZ_API_VERSION "PF_EZ_API_VERSION"
	defined by macro, it allows user to confirm whether dynamically linked library has the
	intended version of the interface or not, at runtime.

	\return API Version of the ProFUSION Library EZ-Interface
	\sa PF_EZ_API_VERSION, PF_EZGetAPIVersion
*/
_PF_API int	_PF_CALL			PF_EZGetAPIVersion();

// -----------------------------------------------------------------------------
//	PF_EZGetInterfaceDescription
// -----------------------------------------------------------------------------
//!	A function for getting a pointer for a character string that includes information related to the ProFUSION Library EZ-Interface
/*!
	This function returns a pointer for a character string that includes related
	information such as build numbers and dates of the ProFUSION Library
	EZ-Interface. The character code is in the Western European language
	(iso-8859-1) and the line feed character is platform-dependent. (CR+LF in the
	case of Windows.)

	\return The pointer to the character string that includes information related to the ProFUSION Library EZ-Interface
	\sa PF_EZGetAPIVersion
*/
_PF_API const char * _PF_CALL	PF_EZGetInterfaceDescription();
/*@}*/

/*!
\defgroup DeviceRelatedFunctions	Device functions
@{
*/
// -----------------------------------------------------------------------------
//	PF_EZGetDeviceNum
// -----------------------------------------------------------------------------
//!	A function for getting the number of camera arrays
/*!
	This function returns the number of specified type camera arrays found on the
	machine.

	Note: For example, in the case of the ProFUSION 25, with its 25
	image sensors, the return value of the PF_EzGetDeviceNum(PF_EZ_DEVICE_ANY) or
	the PF_EzGetDeviceNum(PF_EZ_DEVICE_ProFUSION_25) will become 1
	when one device is connected with the PC. Only the
	\ref PF_EZ_DEVICE_ProFUSION_25 "PF_EZ_DEVICE_ProFUSION_25" can be specified in
	the \ref inDeviceType "inDeviceType" as of the current version.
	(These limitations are scheduled to be resolved in the next version. Noted on 2007/03/15.)

	\param inDeviceType	Specify the \ref PF_EZDeviceType "type" of camera array
	\return Number of camera arrays
	\sa PF_EZOpenDevice
*/
_PF_API int	_PF_CALL				PF_EZGetDeviceNum(PF_EZDeviceType inDeviceType);

// -----------------------------------------------------------------------------
//	PF_EZOpenDevice
// -----------------------------------------------------------------------------
//!	A function for creating a device handle that is necessary for accessing the camera array
/*!
	This function creates a device handle that a handle to one of the camera arrays
	attached to the system. This call must be made before any other calls
	involving the context will work. Only the \ref PF_EZ_DEVICE_ProFUSION_25
	"PF_EZ_DEVICE_ProFUSION_25" can be specified in the \ref inDeviceType "inDeviceType" as
	of the current version. (These limitations are scheduled to be resolved in the
	next version. Noted on 2007/03/15.)

	\param inDeviceType	Specify the \ref PF_EZDeviceType "type" of camera array
	\param inDeviceIndex	Specify the device index (0, if it is the first device)
	\param outHandle	The device handle that was opened will be stored
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCloseDevice
*/
_PF_API PF_EZResult _PF_CALL		PF_EZOpenDevice(PF_EZDeviceType inDeviceType, int inDeviceIndex, PF_EZDeviceHandle *outHandle);

// -----------------------------------------------------------------------------
//	PF_EZCloseDevice
// -----------------------------------------------------------------------------
//!	A function for closing the device handle
/*!
	\param inHandle	inHandle Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZOpenDevice
*/
_PF_API PF_EZResult _PF_CALL		PF_EZCloseDevice(PF_EZDeviceHandle inHandle);

// -----------------------------------------------------------------------------
//	PF_EZGetDeviceType
// -----------------------------------------------------------------------------
//!	A function for getting the types of camera arrays
/*!
	This function returns the \ref PF_EZDeviceType "type" of the camera array
	specified by the device handle.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param outDeviceType	 The \ref PF_EZDeviceType "type" of the camera array of the specified camera array is stored
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZOpenDevice
*/
_PF_API PF_EZResult _PF_CALL		PF_EZGetDeviceType(PF_EZDeviceHandle inHandle, PF_EZDeviceType *outDeviceType);

_PF_API PF_EZResult _PF_CALL		PF_EZDeviceGetSerialNumber(PF_EZDeviceHandle inHandle, unsigned int *outSerialNumber);

// -----------------------------------------------------------------------------
//	PF_EZDeviceGetCameraNum
// -----------------------------------------------------------------------------
//!	A function for getting the number of cameras in the camera array
/*!
	This function returns the number of cameras that form the camera array specified by the device handle.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param outSerialNumber	The number of cameras of the specified camera array is stored
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZOpenDevice
*/
_PF_API PF_EZResult _PF_CALL		PF_EZDeviceGetCameraNum(PF_EZDeviceHandle inHandle, int *outCameraNum);
/*@}*/

/*!
\defgroup ImageRelatedFunctions	Image Data functions
@{
*/
// -----------------------------------------------------------------------------
//	PF_EZCreateDeviceImage
// -----------------------------------------------------------------------------
//!	A function for creating image data for image capturing
/*!
	This function creates \ref PF_EZImage "image data" for storing captured images from the specified
	camera array. \ref PF_EZImage "Image data" that has been created with this function is used in
	the \ref PF_EZGetImage "PF_EZGetImage()" function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param outImage	The \ref PF_EZImage "image data" that has been created will be stored
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZOpenDevice, PF_EZDisposeImage
*/
_PF_API PF_EZResult _PF_CALL		PF_EZCreateDeviceImage(PF_EZDeviceHandle inHandle, PF_EZImage **outImage);

// -----------------------------------------------------------------------------
//	PF_EZCreateBGR8Image
// -----------------------------------------------------------------------------
//!	A function for creating image data for storing an image after a color processing
/*!
	This function creates \ref PF_EZImage "image data" that is required by the specified camera array for storing a
	color processed image. \ref PF_EZImage "Image data" that has been created with this function is used in
	the \ref PF_EZColorProcessing "PF_EZColorProcessing()" function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param outImage	The \ref PF_EZImage "image data" that has been created will be stored
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZOpenDevice, PF_EZDisposeImage
*/
_PF_API PF_EZResult _PF_CALL		PF_EZCreateBGR8Image(PF_EZDeviceHandle inHandle, PF_EZImage **outImage);

// -----------------------------------------------------------------------------
//	PF_EZDisposeImage
// -----------------------------------------------------------------------------
//!	A function for discarding any image data that was created
/*!
	This function discards \ref PF_EZImage "image data" that has been created with functions such as
	the \ref PF_EZCreateDeviceImage "PF_EZCreateDeviceImage()" and
	\ref PF_EZCreateBGR8Image "PF_EZCreateBGR8Image()".
	Please discard all image data that has been used with this function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inImage	Specify the \ref PF_EZImage "image data" to be disposed
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZOpenDevice, PF_EZDisposeImage
*/
_PF_API PF_EZResult _PF_CALL		PF_EZDisposeImage(PF_EZDeviceHandle inHandle, PF_EZImage *inImage);
/*@}*/

/*!
\defgroup CaptureRelatedFunctions	Image Capture functions
@{
*/
// -----------------------------------------------------------------------------
//	PF_EZCaptureStart
// -----------------------------------------------------------------------------
//!	A function for starting the capturing of images
/*!
	This function starts the image capturing process from the camera array.
	It should be called just once before using the \ref PF_EZGetImage "PF_EZGetImage()" function.
	All the process that is needed to start the image capture will be done
	internally in this function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCaptureStop, PF_EZGetImage
*/
_PF_API PF_EZResult _PF_CALL		PF_EZCaptureStart(PF_EZDeviceHandle inHandle);

// -----------------------------------------------------------------------------
//	PF_EZCaptureStop
// -----------------------------------------------------------------------------
//!	A function for stopping the capturing of images
/*!
	This function stops the image capturing process from the camera array.
	It should be called just once when stopping the image capture. All the process
	that is needed to stop the image capture will be done internally in this function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCaptureStart
*/
_PF_API PF_EZResult _PF_CALL		PF_EZCaptureStop(PF_EZDeviceHandle inHandle);

// -----------------------------------------------------------------------------
//	PF_EZSetActiveCameras
// -----------------------------------------------------------------------------
//!	A function for setting the active cameras
/*!
	This function enables/disables image transferring of each camera during the capture process.
	The low 25bits of \ref inActiveCameraMask "inActiveCameraMask" parameter corresponds to
	each camera inside the camera array. "1" means that the image transferring of the corresponding
	camera is enabled (activated).

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param Specify the bit pattern of the enabled (activated) cameras
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCaptureStart
*/
_PF_API PF_EZResult _PF_CALL		PF_EZSetActiveCameras(PF_EZDeviceHandle inHandle, unsigned int inActiveCameraMask);

// -----------------------------------------------------------------------------
//	PF_EZGetActiveCameras
// -----------------------------------------------------------------------------
//!	A function for getting the activated cameras
/*!
	This function returns image transfer status of cameras.
	The low 25bits of \ref outActiveCameraMask "outActiveCameraMask" parameter corresponds to
	each camera inside the camera array. "1" means that the image transferring of the corresponding
	camera is enabled (activated).	

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param outActiveCameraMask The bit pattern of the enabled (activated) cameras will be stored.
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCaptureStart
*/
_PF_API PF_EZResult _PF_CALL		PF_EZGetActiveCameras(PF_EZDeviceHandle inHandle, unsigned int *outActiveCameraMask);

// -----------------------------------------------------------------------------
//	PF_EZGetActiveCameraNum
// -----------------------------------------------------------------------------
//!	A function for getting the activated camera numbers
/*!
	This function returns number of cameras, which will transfer a image.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param outActiveCameraNum The number of trasfered image will be stored.
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCaptureStart
*/
_PF_API PF_EZResult _PF_CALL		PF_EZGetActiveCameraNum(PF_EZDeviceHandle inHandle, int *outActiveCameraNum);

// -----------------------------------------------------------------------------
//	PF_EZGetImage
// -----------------------------------------------------------------------------
//!	A function for getting captured images
/*!
	This function returns captured images from the camera array. It is necessary to
	specify the \ref PF_EZImage "image data buffer" that has been created with
	the \ref PF_EZCreateDeviceImage "PF_EZCreateDeviceImage()" function to the second argument.
	In addition, the \ref PF_EZCaptureStart "PF_EZCaptureStart()" function should be called
	just once before using this function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inImage	Specify the \ref PF_EZImage "image data buffer" that is to store the captured images
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCreateDeviceImage, PF_EZCaptureStart
*/
_PF_API PF_EZResult _PF_CALL		PF_EZGetImage(PF_EZDeviceHandle inHandle, PF_EZImage *ioPF_EZImage);

// -----------------------------------------------------------------------------
//	PF_EZGetImageAsync
// -----------------------------------------------------------------------------
//!	A function for getting captured images asynchronously
/*!
	This function returns captured images from the camera array asynchronously.
	So this fucntion returns immediately after calling. It is necessary to
	specify the \ref PF_EZImage "image data buffer" that has been created with
	the \ref PF_EZCreateDeviceImage "PF_EZCreateDeviceImage()" function to the second argument.
	In addition, the \ref PF_EZCaptureStart "PF_EZCaptureStart()" function should be called
	just once before using this function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inImage	Specify the \ref PF_EZImage "image data buffer" that is to store the captured images
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCreateDeviceImage, PF_EZCaptureStart
*/
_PF_API PF_EZResult _PF_CALL		PF_EZGetImageAsync(PF_EZDeviceHandle inHandle, PF_EZImage *ioPF_EZImage);

// -----------------------------------------------------------------------------
//	PF_EZWaitImage
// -----------------------------------------------------------------------------
//!	A function for getting captured images
/*!
	This function returns captured images from the camera array. It is necessary to
	specify the \ref PF_EZImage "image data buffer" that has been created with
	the \ref PF_EZCreateDeviceImage "PF_EZCreateDeviceImage()" function to the second argument.
	In addition, the \ref PF_EZCaptureStart "PF_EZCaptureStart()" function should be called
	just once before using this function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inImage	Specify the \ref PF_EZImage "image data buffer" that is to store the captured images
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCreateDeviceImage, PF_EZCaptureStart
*/
_PF_API PF_EZResult _PF_CALL		PF_EZWaitImage(PF_EZImage *ioPF_EZImage, unsigned int inTimeout);

// -----------------------------------------------------------------------------
//	PF_EZWaitMultipleImages
// -----------------------------------------------------------------------------
//!	A function for getting captured images
/*!
	This function returns captured images from the camera array. It is necessary to
	specify the \ref PF_EZImage "image data buffer" that has been created with
	the \ref PF_EZCreateDeviceImage "PF_EZCreateDeviceImage()" function to the second argument.
	In addition, the \ref PF_EZCaptureStart "PF_EZCaptureStart()" function should be called
	just once before using this function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inImage	Specify the \ref PF_EZImage "image data buffer" that is to store the captured images
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZCreateDeviceImage, PF_EZCaptureStart
*/
_PF_API PF_EZResult _PF_CALL		PF_EZWaitMultipleImages(PF_EZImage **ioPF_EZImages, int inImageNum, bool inIsWaitAll, unsigned int inTimeout);
/*@}*/

/*!
\defgroup CameraPropertyRelatedFunctions	Camera Property functions
@{
*/
// -----------------------------------------------------------------------------
//	PF_EZIsValidProperty
// -----------------------------------------------------------------------------
//!	A function for checking whether a specified property can be used
/*!
	This function checks whether a specified \ref PF_EZCameraProperty "property"
	can be used at a specified camera.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inCameraIndex	Specify the index number of the camera that sees whether the property can be used or not
	\param inProperty	Specify the property that is to be checked as to whether it can be used
	\param outIsValie	The results will be stored; it can be used if found to be true
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZPropertySetValue
*/
_PF_API PF_EZResult _PF_CALL		PF_EZIsValidProperty(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, bool *outIsValie);

// -----------------------------------------------------------------------------
//	PF_EZPropertyHasAutoFunction
// -----------------------------------------------------------------------------
//!	A function for checking whether the specified property has an Auto function
/*!
	This function checks whether the given \ref PF_EZCameraProperty "property" by
	the \ref inProperty "inPropery" has an Auto function or not.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inCameraIndex	Specify the index number of the camera, with the value that is to be set
	\param inProperty	Specify the \ref PF_EZCameraProperty "property" with the value that is to be set
	\param outHasAutoFunction	The Auto function specifies the property that is to be checked as to whether it can be used
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZPropertySetAutoState, PF_EZPropertySetValue
*/
_PF_API PF_EZResult _PF_CALL		PF_EZPropertyHasAutoFunction(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, bool *outHasAutoFunction);

// -----------------------------------------------------------------------------
//	PF_EZPropertySetValue
// -----------------------------------------------------------------------------
//!	A function for setting the value of the property that was specified
/*!
	This function sets the absolute value of a given \ref PF_EZCameraProperty "property"
	by the \ref inProperty "inProperty" to the camera selected with the \ref inCameraIndex "inCameraIndex".
	It's valid value range can be acquired with the \ref PF_EZPropertyGetValueRange "PF_EZPropertyGetValueRange()" function,
	and the units of the values can be acquired with \ref PF_EZPropertyGetUnits "PF_EZPropertyGetUnits()" function.
	If \ref PF_EZ_ALL_CAMERA "PF_EZ_ALL_CAMERA" is specified in \ref inCameraIndex "inCameraIndex",
	the same value can be set in all of the cameras in the camera array.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inCameraIndex	Specify the index number of the camera, with the value that is to be set
	\param inProperty	Specify the \ref PF_EZCameraProperty "property" with the value that is to be set
	\param inValue	The value that is to be set is specified
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZPropertyGetValueRange, PF_EZPropertyGetUnits
*/
_PF_API PF_EZResult _PF_CALL		PF_EZPropertySetValue(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, double inValue);

// -----------------------------------------------------------------------------
//	PF_EZPropertyGetValue
// -----------------------------------------------------------------------------
//!	A function for getting the value of specified properties
/*!
	This function returns the current absolute value of a given \ref PF_EZCameraProperty "property"
	by the \ref inProperty "inProperty" from the camera selected with the \ref inCameraIndex "inCameraIndex".
	The value of each property is expressed with an absolute value, and it's unit can be
	acquired with the \ref PF_EZPropertyGetUnits "PF_EZPropertyGetUnits()" function.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inCameraIndex	Specify the index number of the camera, with the value that is to be set
	\param inProperty	Specify the \ref PF_EZCameraProperty "property" with the value that is to be get
	\param outValue	The acquired values will be stored
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZPropertySetValue
*/
_PF_API PF_EZResult _PF_CALL		PF_EZPropertyGetValue(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, double *outValue);

// -----------------------------------------------------------------------------
//	PF_EZPropertySetAutoState
// -----------------------------------------------------------------------------
//!	A function for setting On/Off status of an Auto function in a specified property
/*!
	This function sets On/Off status of the Auto function in a given \ref PF_EZCameraProperty "property" by
	the \ref inProperty "inProperty" of a camera that was selected with the \ref inCameraIndex "inCameraIndex".
	If \ref PF_EZ_ALL_CAMERA "PF_EZ_ALL_CAMERA" is specified in \ref inCameraIndex "inCameraIndex",
	the same value can be set in all of the cameras in the camera array.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inCameraIndex	Specify the index number of the camera, with the value that is to be set
	\param inProperty	Specify the \ref PF_EZCameraProperty "property" with the value that is to be set
	\param inAuto	Specify the On/Off of the Auto function. On if it is true
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZPropertySetValue
*/
_PF_API PF_EZResult _PF_CALL		PF_EZPropertySetAutoState(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, bool inAuto);

// -----------------------------------------------------------------------------
//	PF_EZPropertyGetAutoState
// -----------------------------------------------------------------------------
//!	A function for getting On/Off status of an Auto function in a specified property
/*!
	This function returns On/Off status of the Auto function in a given \ref PF_EZCameraProperty "property"
	by the \ref inProperty "inProperty" from a selected camera with the \ref inCameraIndex "inCameraIndex".

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inCameraIndex	Specify the index number of the camera, with the value that is to be set
	\param inProperty	Specify the \ref PF_EZCameraProperty "property" with the value that is to be set
	\param outAuto	The On/Off status of the Auto function will be stored (On if true)
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZPropertySetAutoState, PF_EZPropertySetValue
*/
_PF_API PF_EZResult _PF_CALL		PF_EZPropertyGetAutoState(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, bool *outAuto);

// -----------------------------------------------------------------------------
//	PF_EZPropertyGetValueRange
// -----------------------------------------------------------------------------
//!	A function for getting the value range of a specified property
/*!
	This function returns the minimum and maximum property value for the given
	\ref PF_EZCameraProperty "property" by the \ref inProperty "inProperty" from
	the camera selected with the \ref inCameraIndex "inCameraIndex"

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inCameraIndex	Specify the index number of the camera, with the value that is to be get
	\param inProperty	Specify the \ref PF_EZCameraProperty "property" with the value that is to be get
	\param outValueMin	The minimum value of the \ref PF_EZCameraProperty "property" will be stored
	\param outValueMax	The maximum value of the \ref PF_EZCameraProperty "property" will be stored
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZPropertyGetUnits
*/
_PF_API PF_EZResult _PF_CALL		PF_EZPropertyGetValueRange(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, double *outValueMin, double *outValueMax);

// -----------------------------------------------------------------------------
//	PF_EZPropertyGetUnits
// -----------------------------------------------------------------------------
//!	A function for getting the units of a specified property
/*!
	This function returns the units of a \ref PF_EZCameraProperty "property" that is specified with
	the \ref inProperty "inProperty" from the selected camera with the \ref inCameraIndex "inCameraIndex".
	The units of each property is expressed by the character string.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inCameraIndex	Specify the index number of the camera, with the value that is to be get
	\param inProperty	Specify the \ref PF_EZCameraProperty "property" with the value that is to be get
	\param outStringPtr	The pointer to the character string expressing units will be stored
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZPropertyGetValueRange
*/
_PF_API PF_EZResult _PF_CALL		PF_EZPropertyGetUnits(PF_EZDeviceHandle inHandle, int inCameraIndex, PF_EZCameraProperty inProperty, const char **outStringPtr);
/*@}*/

/*!
\defgroup ColorProcessingRelatedFunctions	Color Processing functions
@{
*/

// -----------------------------------------------------------------------------
//	PF_EZColorProcessing
// -----------------------------------------------------------------------------
//!	A function for converting raw images from a camera array to final color images
/*!
	This function converts raw images captured from the camera array to final color images.
	Specify captured images that were captured by \ref PF_EZGetImage "PF_EZGetImage" to
	the \ref inDeviceImage "inDeviceImage" and specify color image buffers that were created with
	\ref PF_EZCreateBGR8Image "PF_EZCreateBGR8Image" to the outColorImage.
	The format of the output image data is BGR8.

	\param inHandle	Specify the device handle that was created with \ref PF_EZOpenDevice "PF_EZOpenDevice()"
	\param inDeviceImage	Specify captured images
	\param outColorImage	Output color images are stored in specified color image buffers
	\return \ref PF_EZResult "Result Code"
	\sa PF_EZGetImage, PF_EZCreateBGR8Image
*/
_PF_API PF_EZResult _PF_CALL		PF_EZColorProcessing(PF_EZDeviceHandle inHandle, const PF_EZImage *inDeviceImage, const PF_EZImage *outColorImage);
/*@}*/

/*!
\defgroup OtherFunctions	Miscellaneous functions
@{
*/

// -----------------------------------------------------------------------------
//	PF_EZReadI2C
// -----------------------------------------------------------------------------
//!	A function for access I2C registers of the image sensor inside the camera array
/*!
	To use this function, please contact the support team.
*/
_PF_API PF_EZResult _PF_CALL	PF_EZReadI2C(PF_EZDeviceHandle inHandle, int inCameraIndex, unsigned int inI2CAddr, unsigned int *outData);

// -----------------------------------------------------------------------------
//	PF_EZWriteI2C
// -----------------------------------------------------------------------------
//!	A function for access I2C registers of the image sensor inside the camera array
/*!
	To use this function, please contact the support team.
*/
_PF_API PF_EZResult _PF_CALL	PF_EZWriteI2C(PF_EZDeviceHandle inHandle, int inCameraIndex, unsigned int inI2CAddr, unsigned int inData);

// -----------------------------------------------------------------------------
//	PF_EZCameraReset
// -----------------------------------------------------------------------------
//!	A function for resetting the camera array
/*!
	To use this function, please contact the support team.
*/
_PF_API PF_EZResult _PF_CALL	PF_EZCameraReset(PF_EZDeviceHandle inHandle);
/*@}*/

// returns the size of scatter gather list used in the last capture
_PF_API int	_PF_CALL	PF_EZGetSGListLength(PF_EZDeviceHandle inHandle);

#ifdef	__cplusplus
}	//	end of 'extern "C"'
#endif

#endif	// #ifdef __PF_EZ_INTERFACE_H_
