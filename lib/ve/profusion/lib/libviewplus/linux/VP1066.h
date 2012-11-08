// =============================================================================
//	VP1066.h
//
//	Revision History:
//
//	Revision History:
//	Rev.01 2006/12/20	The First edition
//	Rev.02 2008/07/30	Add Scatter Gather Support
//	Rev.03 2010/05/07	Change License to GPL v2
//
//	Copyright (C) 2006-2010 ViewPLUS Inc. All Rights Reserved.
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.

//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// =============================================================================
/*!
	\file		Private.h
	\author		Dairoku Sekiguchi
	\version	1.3
	\date		2010/05/07
	\license	GPL v2
	\brief		VP1066 Driver functions

	This file defines VP1066 Driver functions.
*/


#ifndef _VP1066_H_
#define _VP1066_H_

#include <linux/ioctl.h>	//	Needed for the _IOW etc stuff used later

//typedef u32	UInt32;
#include "../share/VP1066Registers.h"
#include "../share/Linux/VP1066Interface.h"


//  Base Macro Definitions =====================================================
#define	VP1066_MODULE_NAME		"VP1066"
#define	VP1066_DEVICE_NAME		"vpcpro"
#define	PFX						VP1066_MODULE_NAME ": "

//  Macros to help debugging ===================================================
#define VP1066_DEBUG		//	define it, just in case
#undef PDEBUG				//	undef it, just in case
#ifdef VP1066_DEBUG
 #ifdef __KERNEL__
 //	This one if debugging is on, and kernel space
  #define PDEBUG(fmt, args...) printk(KERN_DEBUG PFX fmt "\n", ## args)
 #else
  //	This one for user space
  #define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
 #endif
#else
 #define PDEBUG(fmt, args...)		//	not debugging: nothing
#endif
#undef PDEBUGG
#define PDEBUGG(fmt, args...)		//	nothing: it's a placeholder

#define ERR_REPORT(fmt, args...) printk(KERN_ERR PFX fmt "\n", ## args)


//  General Macros  =============================================================
#ifndef VP1066_MAJOR
#define VP1066_MAJOR 0		//	dynamic major by default
#endif

#ifndef VP1066_NR_DEVS
#define VP1066_NR_DEVS 4	//	vpcpro0 through vpcpro3
#endif

#define	VP1066_MAXIMUM_PAGE_NUM	(VP1066_MAXIMUM_TRANSFER_LENGTH / PAGE_SIZE + 2)
#define	VP1066_HW_SG_LIST_SIZE	(VP1066_MAXIMUM_PAGE_NUM * sizeof(VP1066DMADescriptor))

typedef struct VP1066_DMA
{
	size_t		dma_size;
	size_t		transfer_len;
	int		page_count;
	int		count_sg;
	struct page	*map[VP1066_MAXIMUM_PAGE_NUM];
	// VP1066DMADescriptor	hw_sg_list[VP1066_MAXIMUM_PAGE_NUM];
	VP1066DMADescriptorPtr	hw_sg_list;
	dma_addr_t			hw_sg_list_phy_addr;
	struct scatterlist		os_sg_list[VP1066_MAXIMUM_PAGE_NUM];
} VP1066_DMA;

typedef struct VP1066_DEV
{
	//unsigned int	access_key;		//	used by vp1066uid and vp1066priv
	struct semaphore	sem;		//	mutual exclusion semaphore
	struct cdev			cdev;		//	Char device structure

	struct pci_dev		*pci_dev;
	VP1066RegistersPtr	regs;

	VP1066_DMA			dma;

	int					irq_enabled;
	wait_queue_head_t	irq_waitq;
} VP1066_DEV;

/*
 * Split minors in two parts
 */
#define TYPE(minor)	(((minor) >> 4) & 0xf)	/* high nibble */
#define NUM(minor)	((minor) & 0xf)		/* low  nibble */


#endif /* _VP1066_H_ */
