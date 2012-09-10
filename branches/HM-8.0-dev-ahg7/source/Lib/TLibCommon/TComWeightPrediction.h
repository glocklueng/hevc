/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.  
 *
 * Copyright (c) 2010-2012, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TComWeightPrediction.h
    \brief    weighting prediction class (header)
*/

#ifndef __TCOMWEIGHTPREDICTION__
#define __TCOMWEIGHTPREDICTION__


// Include files
#include "TComPic.h"
#include "TComMotionInfo.h"
#include "TComPattern.h"
#include "TComTrQuant.h"
#include "TComInterpolationFilter.h"

// ====================================================================================================================
// Class definition
// ====================================================================================================================
/// weighting prediction class
class TComWeightPrediction
{
  Int             m_ibdi;

public:
  TComWeightPrediction();

  Void  getWpScaling( TComDataCU* pcCU, const Int iRefIdx0, const Int iRefIdx1, wpScalingParam *&wp0 , wpScalingParam *&wp1 , const Int ibdi=(g_uiBitDepth+g_uiBitIncrement));

  Void  addWeightBi( const TComYuv* pcYuvSrc0, const TComYuv* pcYuvSrc1, const UInt iPartUnitIdx, const UInt uiWidth, const UInt uiHeight, const wpScalingParam *wp0, const wpScalingParam *wp1, TComYuv* rpcYuvDst, const Bool bRoundLuma=true );
  Void  addWeightUni( const TComYuv* pcYuvSrc0, const UInt iPartUnitIdx, const UInt uiWidth, const UInt uiHeight, const  wpScalingParam *wp0, TComYuv* rpcYuvDst );

  Void  xWeightedPredictionUni( TComDataCU* pcCU, const TComYuv* pcYuvSrc, const UInt uiPartAddr, const Int iWidth, const Int iHeight,
                                const RefPicList eRefPicList, TComYuv*& rpcYuvPred, const Int iPartIdx, const Int iRefIdx=-1 );
  Void  xWeightedPredictionBi( TComDataCU* pcCU, const TComYuv* pcYuvSrc0, const TComYuv* pcYuvSrc1, const Int iRefIdx0, const Int iRefIdx1,
                                const UInt uiPartIdx, const Int iWidth, const Int iHeight, TComYuv* rpcYuvDst );

  static __inline  Pel   xClip       ( const Int x );
  static __inline  Pel   weightBidir( const Int w0, const Pel P0, const Int w1, const  Pel P1, const Int round, const Int shift, const  Int offset);
  static __inline  Pel   weightUnidir( const Int w0, const Pel P0, const Int round, const Int shift, const  Int offset);

};

inline  Pel TComWeightPrediction::xClip( const Int x ) // static member
{ 
  const Int max = (Int)g_uiIBDI_MAX;
  const Pel pel = (Pel)( (x < 0) ? 0 : (x > max) ? max : x );
  return( pel );
}

inline  Pel TComWeightPrediction::weightBidir( const Int w0, const Pel P0, Int w1, const Pel P1, const Int round, const Int shift, const Int offset) // static member
{
  return xClip( ( (w0*(P0 + IF_INTERNAL_OFFS) + w1*(P1 + IF_INTERNAL_OFFS) + round + (offset << (shift-1))) >> shift ) );
}
inline  Pel TComWeightPrediction::weightUnidir( const Int w0, const Pel P0, const Int round, const Int shift, const Int offset)
{
  return xClip( ( (w0*(P0 + IF_INTERNAL_OFFS) + round) >> shift ) + offset ); // static member
}

#endif 
