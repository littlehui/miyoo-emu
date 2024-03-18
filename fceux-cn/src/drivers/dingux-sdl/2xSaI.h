//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __2XSAI_H__
#define __2XSAI_H__

int Init_2xSaI(u32 BitFormat);
void Super2xSaI (u8 *srcPtr, u32 srcPitch,  u8 *deltaPtr,		u8 *dstPtr, u32 dstPitch, int width, int height);
void SuperEagle (u8 *srcPtr, u32 srcPitch,  u8 *deltaPtr,		u8 *dstPtr, u32 dstPitch, int width, int height);
void _2xSaI (u8 *srcPtr, u32 srcPitch,	    u8 *deltaPtr,		u8 *dstPtr, u32 dstPitch, int width, int height);
void Scale_2xSaI (u8 *srcPtr, u32 srcPitch, u8 * /*deltaPtr */,  u8 *dstPtr, u32 dstPitch,  u32 dstWidth, u32 dstHeight, int width, int height);

#endif