// =============================================================================
//	VP1066Registers.h
//
//	Revision History:
//	Rev.01 2006/11/06	The First edition
//	Rev.02 2007/03/14	VP1066_REGS structure was modified for supporting New VP1066A
//	Rev.03 2007/08/16	Added support for Scatter Gather DMA
//	Rev.04 2008/11/06	Added macros for CAM_TRANS register
//	Rev.05 2010/05/07	Change license to the modified BSD license
//	Rev.06 2010/11/02	Added VP1066_QVGA_TRANSFER_LENGTH
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
	\file		VP1066Registers.h
	\author		Dairoku Sekiguchi
	\version	1.2
	\date		2008/11/06
	\license	modified BSD license
	\brief		VP1066A Register Definitions

	This file defines all the VP1066 Registers.
*/

#ifndef __VP1066_REGISTERS_H_
#define __VP1066_REGISTERS_H_

// -----------------------------------------------------------------------------
// PCI Device/Vendor IDs.
// -----------------------------------------------------------------------------
#define VP_PCI_VENDOR_ID					0x1A63
#define VP_PCI_DEVICE_ID					0x1066


// -----------------------------------------------------------------------------
// VP1066 Parameters
// -----------------------------------------------------------------------------
#define VP1066_CAMERA_NUM					25
#define VP1066_IMAGE_WIDTH					640
#define VP1066_IMAGE_HEIGHT					480
#define VP1066_QVGA_IMAGE_WIDTH				320
#define VP1066_QVGA_IMAGE_HEIGHT			240
#define VP1066_FRAME_NO_OFFSET				32
#define	VP1066_SINGLE_FRAME_SIZE			(640*480)


// -----------------------------------------------------------------------------
// Maximum DMA transfer size (in bytes).
//
// NOTE: This value is rather abritrary for this drive, 
//       You can play with the value to see how requests are sequenced as a
//       set of one or more DMA operations.
// -----------------------------------------------------------------------------
#define VP1066_MAXIMUM_TRANSFER_LENGTH		(640*480*25+32)
#define VP1066_QVGA_TRANSFER_LENGTH			(320*240*25+32)

#define VP1066_MAXIMUM_TRANSFER_LENGTH_CMU		(640*480*24+32)
#define VP1066_QVGA_TRANSFER_LENGTH_CMU			(320*240*24+32)


// -----------------------------------------------------------------------------
// Number of DMA channels supported by VP1066
// -----------------------------------------------------------------------------
#define VP1066_DMA_CHANNELS					(1) 


// -----------------------------------------------------------------------------
// Driver version macros
// -----------------------------------------------------------------------------
#define	VP1066_DRIVER_VERSION_MASK			0xFFFFF000
#define	VP1066_DRIVER_BUILD_MASK			0x00000FFF


// -----------------------------------------------------------------------------
// FPGA version macros
// -----------------------------------------------------------------------------
#define	VP1066_FPGA_VERSION					0x00001000
#define	VP1066_FPGA_VERSION_MASK			0xFFFFF000
#define	VP1066_FPGA_BUILD_MASK				0x00000FFF

// -----------------------------------------------------------------------------
// CAM TRANS macros
// -----------------------------------------------------------------------------
#define	VP1066_CAM_TRANS_MASK				0x001FFFFFF

// -----------------------------------------------------------------------------
// VP1066 Registers offset macros
// -----------------------------------------------------------------------------
#define	VP1066_REG_FPGA_VERSION				0x000
#define	VP1066_REG_CAM_SERIAL_NO			0x004
#define	VP1066_REG_DMA_ADDR32				0x010
#define	VP1066_REG_DMA_ADDR64				0x014
#define	VP1066_REG_DMA_SIZE					0x018
#define	VP1066_REG_DMA_LOCAL_OFFSET			0x01C
#define	VP1066_REG_DMA_CTRL					0x020
#define	VP1066_REG_DMA_STATUS				0x024
#define	VP1066_REG_INT_REG					0x030
#define	VP1066_REG_CAM_RESET				0x040
#define	VP1066_REG_CAM_MODE					0x044
#define	VP1066_REG_CAM_IMG_POS				0x048
#define	VP1066_REG_CAM_TRANS_MASK_0			0x050
#define	VP1066_REG_CAM_I2C_ADDR				0x060
#define	VP1066_REG_CAM_I2C_SEL				0x064
#define	VP1066_REG_CAM_I2C_CTRL				0x068
#define	VP1066_REG_CAM_FRAME_NO				0x070
#define	VP1066_REG_CAM_TRANS_OFFSET			0x074


// -----------------------------------------------------------------------------
// VP1066 Registers structure
// -----------------------------------------------------------------------------
#ifndef DARWIN		// should be defined in Win32 and Linux not in Mac OS X
typedef unsigned int UInt32;
#endif

typedef struct _VP1066DMADescriptor
{
	UInt32			MEM_PAGE_ADDR32;		// 0x00
	UInt32			MEM_PAGE_ADDR64;		// 0x04
	UInt32			MEM_PAGE_SIZE;			// 0x08
	UInt32			NEXT_DESC32;			// 0x0C
	UInt32			NEXT_DESC64;			// 0x10
} VP1066DMADescriptor, *VP1066DMADescriptorPtr;


typedef struct _VP1066Registers_
{
	UInt32			FPGA_VERSON;			// 0x000
	UInt32			CAM_SERIAL_NO;			// 0x004

	UInt32			Padding01[2];			// Range [0x008 - 0x00C]

	UInt32			DMA_ADDR32;				// 0x010 
	UInt32			DMA_ADDR64;				// 0x014
	UInt32			DMA_SIZE;				// 0x018 
	UInt32			DMA_LOCAL_OFFSET;		// 0x01C
	UInt32			DMA_CTRL;				// 0x020
	UInt32			DMA_STATUS;				// 0x024

	UInt32			Padding02[2];			// Range [0x028 - 0x02C]

	UInt32			INT_REG;				// 0x030 

	UInt32			Padding03[3];			// Range [0x034 - 0x03C]

	UInt32			CAM_RESET;				// 0x040
	UInt32			CAM_MODE;				// 0x044
	UInt32			CAM_IMG_POS;			// 0x048

	UInt32			Padding04[3];			// Range [0x04C]

	UInt32			CAM_TRANS_MASK_0;		// 0x050
	UInt32			CAM_TRANS_MASK_1;		// 0x054 (Not used)
	UInt32			CAM_TRANS_MASK_2;		// 0x058 (Not used)
	UInt32			CAM_TRANS_MASK_3;		// 0x05C (Not used)

	UInt32			CAM_I2C_ADDR;			// 0x060
	UInt32			CAM_I2C_SEL;			// 0x064
	UInt32			CAM_I2C_CTRL;			// 0x068

	UInt32			Padding05[1];			// Range [0x06C]

	UInt32			CAM_FRAME_NO;			// 0x070
	UInt32			CAM_TRANS_OFFSET;		// 0x074

	UInt32			Padding06[2];			// Range [0x078 - 0x07C]
	UInt32			Padding07[4];			// Range [0x080 - 0x08C]
	UInt32			Padding08[4];			// Range [0x090 - 0x09C]
	UInt32			Padding09[4];			// Range [0x0A0 - 0x0AC]

	UInt32			Padding10[76];			// Range [0x0B0 - 0x0FC]

	UInt32			INTER_SYNC_RESET;		// 0x100
	UInt32			INTER_SYNC_MODE;		// 0x104
	UInt32			INTER_SYNC_COUNTER;		// 0x108
	UInt32			INTER_SYNC_OFFSET;		// 0x10C

} VP1066Registers, *VP1066RegistersPtr;


#endif	// #ifdef __VP1066_REGISTERS_H_
