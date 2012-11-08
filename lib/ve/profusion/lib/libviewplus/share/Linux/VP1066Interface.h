// =============================================================================
//	VP1066Interface.h
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
	\file		VP1066Interface.h
	\author		Dairoku Sekiguchi
	\version	1.0
	\date		2008/08/04
	\license	LGPL v2.1
	\brief		VP1066 Driver User Mode interface header file

	This is a VP1066 Driver User Mode interface header file
*/

#ifndef __VP1066_INTERFACE_H__
#define __VP1066_INTERFACE_H__


// -----------------------------------------------------------------------------
//	Driver version macros (Linux version)
// -----------------------------------------------------------------------------
#define	VP1066_DRIVER_VERSION				0x00001001


// -----------------------------------------------------------------------------
//	Type definitions
// -----------------------------------------------------------------------------
typedef struct _VP1066_BAR_DIRECT_ACCESS_DATA
{
	UInt32		addressOffset;
	UInt32		data;

	unsigned char		*bufPtr;
	UInt32		length;
} VP1066_BAR_DIRECT_ACCESS_DATA;


// -----------------------------------------------------------------------------
//	Ioctl definitions
// -----------------------------------------------------------------------------
#define VP1066_IOC_MAGIC	'P'	// Use 'P' as magic number

#define VP1066_IOC_GET_VER		_IOR(VP1066_IOC_MAGIC, 0, int)
#define VP1066_IOC_READ_BAR		_IOWR(VP1066_IOC_MAGIC, 1, VP1066_BAR_DIRECT_ACCESS_DATA)
#define VP1066_IOC_WRITE_BAR	_IOW(VP1066_IOC_MAGIC, 2, VP1066_BAR_DIRECT_ACCESS_DATA)
#define VP1066_IOC_ADD_CUE		_IOW(VP1066_IOC_MAGIC, 3, VP1066_BAR_DIRECT_ACCESS_DATA)
#define VP1066_IOC_GET_SG_LEN		_IOWR(VP1066_IOC_MAGIC, 5, VP1066_BAR_DIRECT_ACCESS_DATA)
#define VP1066_IOC_MAXNR 5

#endif  // __VP1066_INTERFACE_H__

