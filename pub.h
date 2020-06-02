/*
 * pub.h
 *
 *  Created on: 2013-4-15
 *      Author: Administrator
 */

#ifndef PUB_H_
#define PUB_H_
#include <stdbool.h>
typedef unsigned char  		INT8U;
typedef signed   char  		INT8S;
typedef unsigned short 		INT16U;
typedef signed   short 		INT16S;
typedef unsigned int   		INT32U;
typedef signed   int   		INT32S;
typedef unsigned long long 	INT64U;
typedef signed   long long  INT64S;
//typedef unsigned short 		MSG;
typedef void*		   		WPARA;
typedef void*		   		LPARA;
//typedef unsigned char  		bool;

#define NORMAL 	0
#define EMPTY  	1
#define FULL	2

#define pr_err_0   0
#define pr_err_1   1
#define eph_err    2
#define conv_err   3

#define true		   1
#define false 		   0
#define null   		  (void*)0

#define HIGH		  0
#define MID			  1
#define LOW_0		  2
#define LOW_1		  3
#define LOW_2		  4
#define LOWEST		  5

#define ERROR_DEBUG   0

#define __eph_txt_debug__  1

#define dim(x)		  (sizeof(x)/sizeof(x[0]))

#define SETBIT(dest,mask)     (dest |= mask)
#define CLRBIT(dest,mask)     (dest &= ~mask)
#define TGLBIT(dest,mask)     (dest ^= mask)
#define CHKBIT(dest,mask)     (dest & mask)

//#define sign(x) (x > 0 ? 1 : (x == 0) ? 0 : -1)
#define lrotl(x,y)        ((x << y) |(x >> (32 - y))) //左循环移位

#ifdef __pub_private__
const INT32U m_tmask[] = {1 << 0, 1<<1, 1<< 2,1<< 3,1<< 4,1<< 5,1<< 6,1<< 7,1<< 8,1<<9,1<< 10,1<<11,1<<12,1<<13,1<<14,1<<15,
						  1<<16,  1<<17,1<<18,1<<19,1<<20,1<<21,1<<22,1<<23,1<<24,1<<25,1<<26,1<<27,1<<28,1<<29,1<<30,1<<31};
const INT32U m_tbit_msk_tbl[] =
{
	0x1,0x3,0x7,0xf,0x1f,0x3f,0x7f,0xff,0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff,0xffff,
	0x1ffff,0x3ffff,0x7ffff,0xfffff,0x1fffff,0x3fffff,0x7fffff,0xffffff,
	0x1ffffff,0x3ffffff,0x7ffffff,0xfffffff,0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff
};
#else
extern INT32U m_tmask[];
extern const INT32U m_tbit_msk_tbl[];
#define setBit(mask,k)    (mask |= m_tmask[k])
#define clrBit(mask,k)    (mask &= ~m_tmask[k]);
#define chkBit(mask,k)	  (mask & m_tmask[k])
#define setMask(mask,k)   (mask = m_tmask[k])
#endif

#define BD2SV_BASE   32
#define GLOSV_BASE   96
#define GPSSV_BASE   0

#define PUB_DEBUG    1

#define MCU_DSP		 0
#define MCU_ARM		 1
#define MCU_TYPE     MCU_DSP

#define max_freq_n   16
#define max_sys_n    4

//#define GPS_L1     0
//#define BDS_B1     1
//#define BDS_B2     2
//#define BDS_B3     3
//#define GPS_L2		4
//#define GLS_L1     5
//#define GLS_L2     6
//#define BDS_B2A     7
//#define BDS_B2B     8
//#define BDS_B1C     9
//#define GAL_E5A     10
//#define GAL_E5B     11
//#define GAL_E1C     12
//#define GPS_L5      13
//#define GPS_L1C     14
//#define SBAS		15    // SBAS星

//#define GPS_SAT		0     // gps星
//#define BDS_SAT     1     // 北斗星
//#define GLS_SAT		2     // glonass星
//#define GAL_SAT		3     // galileo
//#define SBAS_SAT    4

#define ARM_LOAD  0

#define glonass_debug    0

#define rf_3902_dev      1

#define measure_machine  1
#define small_ver  		 1
#define dac_adj_clk      1
#define rf_mix_one       1
#define glon_l1l2		 0

#define dual_antenna	 1

#define b2_lpf      	 0    // b2频点为lpf模式

#define rf_l1orb1        1

#define base_rtcm_debug  0

#define dual_7_freq      1

#define pr_smooth  		 1
#if measure_machine
#define pr_smooth_max	 8000
#else
#define pr_smooth_max	 6000
#endif

#define rdss_en   0

#define usr_define_v10   0

#define crpr_enable      1
#define __pvt_rtcm_en__  1

#define __air_plane__    0

#define CLK_25M

#define __nmea_0183_en__  0

#define fpga_nj28    0

#if measure_machine
#define __PRE_SET_SVID__
#define __RTCM_PRO__
#define __FFT_ACQ__     1
#if __air_plane__
#define rtcm_com   0
#else
#define rtcm_com   0
#endif
#else
#define __FPGA_150__   0
#define __HIGH_PLANE__
#define __FPGA_ACQ__
#define __RTC_EXISIT__
#endif
#endif /* PUB_H_ */
