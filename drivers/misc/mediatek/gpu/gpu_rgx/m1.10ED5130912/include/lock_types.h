/*************************************************************************/ /*!
@File           lock_types.h
@Title          Locking types
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    Locking specific enums, defines and structures
@License        Dual MIT/GPLv2

The contents of this file are subject to the MIT license as set out below.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Alternatively, the contents of this file may be used under the terms of
the GNU General Public License Version 2 ("GPL") in which case the provisions
of GPL are applicable instead of those above.

If you wish to allow use of your version of this file only under the terms of
GPL, and not to allow others to use your version of this file under the terms
of the MIT license, indicate your decision by deleting the provisions above
and replace them with the notice and other provisions required by GPL as set
out in the file called "GPL-COPYING" included in this distribution. If you do
not delete the provisions above, a recipient may use your version of this file
under the terms of either the MIT license or GPL.

This License is also included in this distribution in the file called
"MIT-COPYING".

EXCEPT AS OTHERWISE STATED IN A NEGOTIATED AGREEMENT: (A) THE SOFTWARE IS
PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT; AND (B) IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ /**************************************************************************/

#ifndef _LOCK_TYPES_H_
#define _LOCK_TYPES_H_

/* In Linux kernel mode we are using the kernel mutex implementation directly
 * with macros. This allows us to use the kernel lockdep feature for lock
 * debugging. */
#if defined(LINUX) && defined(__KERNEL__)

#include <linux/types.h>
#include <linux/mutex.h>
/* The mutex is defined as a pointer to be compatible with the other code. This
 * isn't ideal and usually you wouldn't do that in kernel code. */
typedef struct mutex *POS_LOCK;
typedef struct rw_semaphore *POSWR_LOCK;
typedef atomic_t ATOMIC_T;

#else /* defined(LINUX) && defined(__KERNEL__) */
#include "img_types.h" /* needed for IMG_INT */
typedef struct _OS_LOCK_ *POS_LOCK;

#if defined(LINUX) || defined(__QNXNTO__) || defined (INTEGRITY_OS)
typedef struct _OSWR_LOCK_ *POSWR_LOCK;
#else /* defined(LINUX) || defined(__QNXNTO__) || defined (INTEGRITY_OS) */
typedef struct _OSWR_LOCK_ {
	IMG_UINT32 ui32Dummy;
} *POSWR_LOCK;
#endif /* defined(LINUX) || defined(__QNXNTO__) || defined (INTEGRITY_OS) */

#if defined(LINUX)
	typedef struct _OS_ATOMIC {IMG_INT counter;} ATOMIC_T;
#elif defined(__QNXNTO__)
	typedef struct _OS_ATOMIC {IMG_INT counter;} ATOMIC_T;
#elif defined(_WIN32)
	/*
	 * Dummy definition. WDDM doesn't use Services, but some headers
	 * still have to be shared. This is one such case.
	 */
	typedef struct _OS_ATOMIC {IMG_INT counter;} ATOMIC_T;
#elif defined(INTEGRITY_OS)
	/* Fixed size data type to hold the largest value */
	typedef struct _OS_ATOMIC {IMG_UINT64 counter;} ATOMIC_T;
#else
	#error "Please type-define an atomic lock for this environment"
#endif

#endif /* defined(LINUX) && defined(__KERNEL__) */

typedef enum
{
	LOCK_TYPE_NONE 			= 0x00,

	LOCK_TYPE_MASK			= 0x0F,
	LOCK_TYPE_PASSIVE		= 0x01,		/* Passive level lock e.g. mutex, system may promote to dispatch */
	LOCK_TYPE_DISPATCH		= 0x02,		/* Dispatch level lock e.g. spin lock, may be used in ISR/MISR */

	LOCK_TYPE_INSIST_FLAG	= 0x80,		/* When set caller can guarantee lock not used in ISR/MISR */
	LOCK_TYPE_PASSIVE_ONLY	= LOCK_TYPE_INSIST_FLAG | LOCK_TYPE_PASSIVE

} LOCK_TYPE;
#endif	/* _LOCK_TYPES_H_ */
