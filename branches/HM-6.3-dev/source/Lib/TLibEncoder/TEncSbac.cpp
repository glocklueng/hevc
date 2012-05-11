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

/** \file     TEncSbac.cpp
    \brief    SBAC encoder class
*/

#include "TEncTop.h"
#include "TEncSbac.h"

#include <map>
#include <algorithm>

//! \ingroup TLibEncoder
//! \{

// ====================================================================================================================
// Constructor / destructor / create / destroy
// ====================================================================================================================

TEncSbac::TEncSbac()
// new structure here
: m_pcBitIf                   ( NULL )
, m_pcSlice                   ( NULL )
, m_pcBinIf                   ( NULL )
#if !AHG6_ALF_OPTION2
, m_bAlfCtrl                  ( false )
#endif
, m_uiCoeffCost               ( 0 )
#if !AHG6_ALF_OPTION2
, m_uiMaxAlfCtrlDepth         ( 0 )
#endif
, m_numContextModels          ( 0 )
, m_cCUSplitFlagSCModel       ( 1,             1,               NUM_SPLIT_FLAG_CTX            , m_contextModels + m_numContextModels, m_numContextModels )
, m_cCUSkipFlagSCModel        ( 1,             1,               NUM_SKIP_FLAG_CTX             , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUMergeFlagExtSCModel    ( 1,             1,               NUM_MERGE_FLAG_EXT_CTX        , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUMergeIdxExtSCModel     ( 1,             1,               NUM_MERGE_IDX_EXT_CTX         , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUPartSizeSCModel        ( 1,             1,               NUM_PART_SIZE_CTX             , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUPredModeSCModel        ( 1,             1,               NUM_PRED_MODE_CTX             , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUAlfCtrlFlagSCModel     ( 1,             1,               NUM_ALF_CTRL_FLAG_CTX         , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUIntraPredSCModel       ( 1,             1,               NUM_ADI_CTX                   , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUChromaPredSCModel      ( 1,             1,               NUM_CHROMA_PRED_CTX           , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUDeltaQpSCModel         ( 1,             1,               NUM_DELTA_QP_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUInterDirSCModel        ( 1,             1,               NUM_INTER_DIR_CTX             , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCURefPicSCModel          ( 1,             1,               NUM_REF_NO_CTX                , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUMvdSCModel             ( 1,             1,               NUM_MV_RES_CTX                , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUQtCbfSCModel           ( 1,             2,               NUM_QT_CBF_CTX                , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUTransSubdivFlagSCModel ( 1,             1,               NUM_TRANS_SUBDIV_FLAG_CTX     , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUQtRootCbfSCModel       ( 1,             1,               NUM_QT_ROOT_CBF_CTX           , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUSigCoeffGroupSCModel   ( 1,             2,               NUM_SIG_CG_FLAG_CTX           , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUSigSCModel             ( 1,             1,               NUM_SIG_FLAG_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCuCtxLastX               ( 1,             2,               NUM_CTX_LAST_FLAG_XY          , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCuCtxLastY               ( 1,             2,               NUM_CTX_LAST_FLAG_XY          , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUOneSCModel             ( 1,             1,               NUM_ONE_FLAG_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUAbsSCModel             ( 1,             1,               NUM_ABS_FLAG_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cMVPIdxSCModel            ( 1,             1,               NUM_MVP_IDX_CTX               , m_contextModels + m_numContextModels, m_numContextModels)
, m_cALFFlagSCModel           ( 1,             1,               NUM_ALF_FLAG_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cALFUvlcSCModel           ( 1,             1,               NUM_ALF_UVLC_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cALFSvlcSCModel           ( 1,             1,               NUM_ALF_SVLC_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cCUAMPSCModel             ( 1,             1,               NUM_CU_AMP_CTX                , m_contextModels + m_numContextModels, m_numContextModels)
, m_cSaoFlagSCModel           ( 1,             1,               NUM_SAO_FLAG_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cSaoUvlcSCModel           ( 1,             1,               NUM_SAO_UVLC_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cSaoSvlcSCModel           ( 1,             1,               NUM_SAO_SVLC_CTX              , m_contextModels + m_numContextModels, m_numContextModels)
, m_cSaoMergeLeftSCModel      ( 1,             1,               NUM_SAO_MERGE_LEFT_FLAG_CTX   , m_contextModels + m_numContextModels, m_numContextModels)
, m_cSaoMergeUpSCModel        ( 1,             1,               NUM_SAO_MERGE_UP_FLAG_CTX     , m_contextModels + m_numContextModels, m_numContextModels)
, m_cSaoTypeIdxSCModel        ( 1,             1,               NUM_SAO_TYPE_IDX_CTX          , m_contextModels + m_numContextModels, m_numContextModels)
{
  assert( m_numContextModels <= MAX_NUM_CTX_MOD );
  m_iSliceGranularity = 0;
}

TEncSbac::~TEncSbac()
{
}

// ====================================================================================================================
// Public member functions
// ====================================================================================================================

Void TEncSbac::resetEntropy           ()
{
  Int  iQp              = m_pcSlice->getSliceQp();
  SliceType eSliceType  = m_pcSlice->getSliceType();
  
  Int  encCABACTableIdx = m_pcSlice->getPPS()->getEncCABACTableIdx();
  if (!m_pcSlice->isIntra() && (encCABACTableIdx==B_SLICE || encCABACTableIdx==P_SLICE) && m_pcSlice->getPPS()->getCabacInitPresentFlag())
  {
    eSliceType = (SliceType) encCABACTableIdx;
  }

  m_cCUSplitFlagSCModel.initBuffer       ( eSliceType, iQp, (UChar*)INIT_SPLIT_FLAG );
  
  m_cCUSkipFlagSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_SKIP_FLAG );
  m_cCUAlfCtrlFlagSCModel.initBuffer     ( eSliceType, iQp, (UChar*)INIT_ALF_CTRL_FLAG );
  m_cCUMergeFlagExtSCModel.initBuffer    ( eSliceType, iQp, (UChar*)INIT_MERGE_FLAG_EXT);
  m_cCUMergeIdxExtSCModel.initBuffer     ( eSliceType, iQp, (UChar*)INIT_MERGE_IDX_EXT);
  m_cCUPartSizeSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_PART_SIZE );
  m_cCUAMPSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_CU_AMP_POS );
  m_cCUPredModeSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_PRED_MODE );
  m_cCUIntraPredSCModel.initBuffer       ( eSliceType, iQp, (UChar*)INIT_INTRA_PRED_MODE );
  m_cCUChromaPredSCModel.initBuffer      ( eSliceType, iQp, (UChar*)INIT_CHROMA_PRED_MODE );
  m_cCUInterDirSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_INTER_DIR );
  m_cCUMvdSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_MVD );
  m_cCURefPicSCModel.initBuffer          ( eSliceType, iQp, (UChar*)INIT_REF_PIC );
  m_cCUDeltaQpSCModel.initBuffer         ( eSliceType, iQp, (UChar*)INIT_DQP );
  m_cCUQtCbfSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_QT_CBF );
  m_cCUQtRootCbfSCModel.initBuffer       ( eSliceType, iQp, (UChar*)INIT_QT_ROOT_CBF );
  m_cCUSigCoeffGroupSCModel.initBuffer   ( eSliceType, iQp, (UChar*)INIT_SIG_CG_FLAG );
  m_cCUSigSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_SIG_FLAG );
  m_cCuCtxLastX.initBuffer               ( eSliceType, iQp, (UChar*)INIT_LAST );
  m_cCuCtxLastY.initBuffer               ( eSliceType, iQp, (UChar*)INIT_LAST );
  m_cCUOneSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_ONE_FLAG );
  m_cCUAbsSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_ABS_FLAG );
  m_cMVPIdxSCModel.initBuffer            ( eSliceType, iQp, (UChar*)INIT_MVP_IDX );
  m_cALFFlagSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_ALF_FLAG );
  m_cALFUvlcSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_ALF_UVLC );
  m_cALFSvlcSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_ALF_SVLC );
  m_cCUTransSubdivFlagSCModel.initBuffer ( eSliceType, iQp, (UChar*)INIT_TRANS_SUBDIV_FLAG );
  m_cSaoFlagSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_SAO_FLAG );
  m_cSaoUvlcSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_SAO_UVLC );
  m_cSaoSvlcSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_SAO_SVLC );
  m_cSaoMergeLeftSCModel.initBuffer      ( eSliceType, iQp, (UChar*)INIT_SAO_MERGE_LEFT_FLAG );
  m_cSaoMergeUpSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_SAO_MERGE_UP_FLAG );
  m_cSaoTypeIdxSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_SAO_TYPE_IDX );
  // new structure
  m_uiLastQp = iQp;
  
  m_pcBinIf->start();
  
  return;
}

/** The function does the following: 
 * If current slice type is P/B then it determines the distance of initialisation type 1 and 2 from the current CABAC states and 
 * stores the index of the closest table.  This index is used for the next P/B slice when cabac_init_present_flag is true.
 */
Void TEncSbac::determineCabacInitIdx()
{
  Int  qp              = m_pcSlice->getSliceQp();

  if (!m_pcSlice->isIntra())
  {
    SliceType aSliceTypeChoices[] = {B_SLICE, P_SLICE};

    UInt bestCost             = MAX_UINT;
    SliceType bestSliceType   = aSliceTypeChoices[0];
    for (UInt idx=0; idx<2; idx++)
    {
      UInt curCost          = 0;
      SliceType curSliceType  = aSliceTypeChoices[idx];

      curCost  = m_cCUSplitFlagSCModel.calcCost       ( curSliceType, qp, (UChar*)INIT_SPLIT_FLAG );
      curCost += m_cCUSkipFlagSCModel.calcCost        ( curSliceType, qp, (UChar*)INIT_SKIP_FLAG );
      curCost += m_cCUAlfCtrlFlagSCModel.calcCost     ( curSliceType, qp, (UChar*)INIT_ALF_CTRL_FLAG );
      curCost += m_cCUMergeFlagExtSCModel.calcCost    ( curSliceType, qp, (UChar*)INIT_MERGE_FLAG_EXT);
      curCost += m_cCUMergeIdxExtSCModel.calcCost     ( curSliceType, qp, (UChar*)INIT_MERGE_IDX_EXT);
      curCost += m_cCUPartSizeSCModel.calcCost        ( curSliceType, qp, (UChar*)INIT_PART_SIZE );
      curCost += m_cCUAMPSCModel.calcCost             ( curSliceType, qp, (UChar*)INIT_CU_AMP_POS );
      curCost += m_cCUPredModeSCModel.calcCost        ( curSliceType, qp, (UChar*)INIT_PRED_MODE );
      curCost += m_cCUIntraPredSCModel.calcCost       ( curSliceType, qp, (UChar*)INIT_INTRA_PRED_MODE );
      curCost += m_cCUChromaPredSCModel.calcCost      ( curSliceType, qp, (UChar*)INIT_CHROMA_PRED_MODE );
      curCost += m_cCUInterDirSCModel.calcCost        ( curSliceType, qp, (UChar*)INIT_INTER_DIR );
      curCost += m_cCUMvdSCModel.calcCost             ( curSliceType, qp, (UChar*)INIT_MVD );
      curCost += m_cCURefPicSCModel.calcCost          ( curSliceType, qp, (UChar*)INIT_REF_PIC );
      curCost += m_cCUDeltaQpSCModel.calcCost         ( curSliceType, qp, (UChar*)INIT_DQP );
      curCost += m_cCUQtCbfSCModel.calcCost           ( curSliceType, qp, (UChar*)INIT_QT_CBF );
      curCost += m_cCUQtRootCbfSCModel.calcCost       ( curSliceType, qp, (UChar*)INIT_QT_ROOT_CBF );
      curCost += m_cCUSigCoeffGroupSCModel.calcCost   ( curSliceType, qp, (UChar*)INIT_SIG_CG_FLAG );
      curCost += m_cCUSigSCModel.calcCost             ( curSliceType, qp, (UChar*)INIT_SIG_FLAG );
      curCost += m_cCuCtxLastX.calcCost               ( curSliceType, qp, (UChar*)INIT_LAST );
      curCost += m_cCuCtxLastY.calcCost               ( curSliceType, qp, (UChar*)INIT_LAST );
      curCost += m_cCUOneSCModel.calcCost             ( curSliceType, qp, (UChar*)INIT_ONE_FLAG );
      curCost += m_cCUAbsSCModel.calcCost             ( curSliceType, qp, (UChar*)INIT_ABS_FLAG );
      curCost += m_cMVPIdxSCModel.calcCost            ( curSliceType, qp, (UChar*)INIT_MVP_IDX );
      curCost += m_cALFFlagSCModel.calcCost           ( curSliceType, qp, (UChar*)INIT_ALF_FLAG );
      curCost += m_cALFUvlcSCModel.calcCost           ( curSliceType, qp, (UChar*)INIT_ALF_UVLC );
      curCost += m_cALFSvlcSCModel.calcCost           ( curSliceType, qp, (UChar*)INIT_ALF_SVLC );
      curCost += m_cCUTransSubdivFlagSCModel.calcCost ( curSliceType, qp, (UChar*)INIT_TRANS_SUBDIV_FLAG );
      curCost += m_cSaoFlagSCModel.calcCost           ( curSliceType, qp, (UChar*)INIT_SAO_FLAG );
      curCost += m_cSaoUvlcSCModel.calcCost           ( curSliceType, qp, (UChar*)INIT_SAO_UVLC );
      curCost += m_cSaoSvlcSCModel.calcCost           ( curSliceType, qp, (UChar*)INIT_SAO_SVLC );
      curCost += m_cSaoMergeLeftSCModel.calcCost      ( curSliceType, qp, (UChar*)INIT_SAO_MERGE_LEFT_FLAG );
      curCost += m_cSaoMergeUpSCModel.calcCost        ( curSliceType, qp, (UChar*)INIT_SAO_MERGE_UP_FLAG );
      curCost += m_cSaoTypeIdxSCModel.calcCost        ( curSliceType, qp, (UChar*)INIT_SAO_TYPE_IDX );

      if (curCost < bestCost)
      {
        bestSliceType = curSliceType;
        bestCost      = curCost;
      }
    }
    m_pcSlice->getPPS()->setEncCABACTableIdx( bestSliceType );
  }
  else
  {
    m_pcSlice->getPPS()->setEncCABACTableIdx( I_SLICE );
  }  
}


/** The function does the followng: Write out terminate bit. Flush CABAC. Intialize CABAC states. Start CABAC.
 */
Void TEncSbac::updateContextTables( SliceType eSliceType, Int iQp, Bool bExecuteFinish )
{
  m_pcBinIf->encodeBinTrm(1);
  if (bExecuteFinish) m_pcBinIf->finish();
  m_cCUSplitFlagSCModel.initBuffer       ( eSliceType, iQp, (UChar*)INIT_SPLIT_FLAG );
  
  m_cCUSkipFlagSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_SKIP_FLAG );
  m_cCUAlfCtrlFlagSCModel.initBuffer     ( eSliceType, iQp, (UChar*)INIT_ALF_CTRL_FLAG );
  m_cCUMergeFlagExtSCModel.initBuffer    ( eSliceType, iQp, (UChar*)INIT_MERGE_FLAG_EXT);
  m_cCUMergeIdxExtSCModel.initBuffer     ( eSliceType, iQp, (UChar*)INIT_MERGE_IDX_EXT);
  m_cCUPartSizeSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_PART_SIZE );
  m_cCUAMPSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_CU_AMP_POS );
  m_cCUPredModeSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_PRED_MODE );
  m_cCUIntraPredSCModel.initBuffer       ( eSliceType, iQp, (UChar*)INIT_INTRA_PRED_MODE );
  m_cCUChromaPredSCModel.initBuffer      ( eSliceType, iQp, (UChar*)INIT_CHROMA_PRED_MODE );
  m_cCUInterDirSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_INTER_DIR );
  m_cCUMvdSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_MVD );
  m_cCURefPicSCModel.initBuffer          ( eSliceType, iQp, (UChar*)INIT_REF_PIC );
  m_cCUDeltaQpSCModel.initBuffer         ( eSliceType, iQp, (UChar*)INIT_DQP );
  m_cCUQtCbfSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_QT_CBF );
  m_cCUQtRootCbfSCModel.initBuffer       ( eSliceType, iQp, (UChar*)INIT_QT_ROOT_CBF );
  m_cCUSigCoeffGroupSCModel.initBuffer   ( eSliceType, iQp, (UChar*)INIT_SIG_CG_FLAG );
  m_cCUSigSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_SIG_FLAG );
  m_cCuCtxLastX.initBuffer               ( eSliceType, iQp, (UChar*)INIT_LAST );
  m_cCuCtxLastY.initBuffer               ( eSliceType, iQp, (UChar*)INIT_LAST );
  m_cCUOneSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_ONE_FLAG );
  m_cCUAbsSCModel.initBuffer             ( eSliceType, iQp, (UChar*)INIT_ABS_FLAG );
  m_cMVPIdxSCModel.initBuffer            ( eSliceType, iQp, (UChar*)INIT_MVP_IDX );
  m_cALFFlagSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_ALF_FLAG );
  m_cALFUvlcSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_ALF_UVLC );
  m_cALFSvlcSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_ALF_SVLC );
  m_cCUTransSubdivFlagSCModel.initBuffer ( eSliceType, iQp, (UChar*)INIT_TRANS_SUBDIV_FLAG );
  m_cSaoFlagSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_SAO_FLAG );
  m_cSaoUvlcSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_SAO_UVLC );
  m_cSaoSvlcSCModel.initBuffer           ( eSliceType, iQp, (UChar*)INIT_SAO_SVLC );
  m_cSaoMergeLeftSCModel.initBuffer      ( eSliceType, iQp, (UChar*)INIT_SAO_MERGE_LEFT_FLAG );
  m_cSaoMergeUpSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_SAO_MERGE_UP_FLAG );
  m_cSaoTypeIdxSCModel.initBuffer        ( eSliceType, iQp, (UChar*)INIT_SAO_TYPE_IDX );
  m_pcBinIf->start();
}

Void TEncSbac::writeTileMarker( UInt uiTileIdx, UInt uiBitsUsed )
{
  for (Int iShift=uiBitsUsed-1; iShift>=0; iShift--)
  {
    m_pcBinIf->encodeBinEP ( (uiTileIdx & (1 << iShift)) >> iShift );
  }
}

void TEncSbac::codeSEI(const SEI&)
{
  assert(0);
}

Void TEncSbac::codeSPS( TComSPS* pcSPS )
{
  assert (0);
  return;
}

Void TEncSbac::codePPS( TComPPS* pcPPS )
{
  assert (0);
  return;
}

Void TEncSbac::codeSliceHeader( TComSlice* pcSlice )
{
  assert (0);
  return;
}

Void TEncSbac::codeTilesWPPEntryPoint( TComSlice* pSlice )
{
  assert (0);
  return;
}

Void TEncSbac::codeTerminatingBit( UInt uilsLast )
{
  m_pcBinIf->encodeBinTrm( uilsLast );
}

Void TEncSbac::codeSliceFinish()
{
  m_pcBinIf->finish();
}

Void TEncSbac::codeFlush()
{
  m_pcBinIf->flush();
}

Void TEncSbac::encodeStart()
{
  m_pcBinIf->start();
}

Void TEncSbac::xWriteUnarySymbol( UInt uiSymbol, ContextModel* pcSCModel, Int iOffset )
{
  m_pcBinIf->encodeBin( uiSymbol ? 1 : 0, pcSCModel[0] );
  
  if( 0 == uiSymbol)
  {
    return;
  }
  
  while( uiSymbol-- )
  {
    m_pcBinIf->encodeBin( uiSymbol ? 1 : 0, pcSCModel[ iOffset ] );
  }
  
  return;
}

Void TEncSbac::xWriteUnaryMaxSymbol( UInt uiSymbol, ContextModel* pcSCModel, Int iOffset, UInt uiMaxSymbol )
{
  if (uiMaxSymbol == 0)
  {
    return;
  }
  
  m_pcBinIf->encodeBin( uiSymbol ? 1 : 0, pcSCModel[ 0 ] );
  
  if ( uiSymbol == 0 )
  {
    return;
  }
  
  Bool bCodeLast = ( uiMaxSymbol > uiSymbol );
  
  while( --uiSymbol )
  {
    m_pcBinIf->encodeBin( 1, pcSCModel[ iOffset ] );
  }
  if( bCodeLast )
  {
    m_pcBinIf->encodeBin( 0, pcSCModel[ iOffset ] );
  }
  
  return;
}

Void TEncSbac::xWriteEpExGolomb( UInt uiSymbol, UInt uiCount )
{
  UInt bins = 0;
  Int numBins = 0;
  
  while( uiSymbol >= (UInt)(1<<uiCount) )
  {
    bins = 2 * bins + 1;
    numBins++;
    uiSymbol -= 1 << uiCount;
    uiCount  ++;
  }
  bins = 2 * bins + 0;
  numBins++;
  
  bins = (bins << uiCount) | uiSymbol;
  numBins += uiCount;
  
  assert( numBins <= 32 );
  m_pcBinIf->encodeBinsEP( bins, numBins );
}

/** Coding of coeff_abs_level_minus3
 * \param uiSymbol value of coeff_abs_level_minus3
 * \param ruiGoRiceParam reference to Rice parameter
 * \returns Void
 */
Void TEncSbac::xWriteGoRiceExGolomb( UInt uiSymbol, UInt &ruiGoRiceParam )
{
  UInt uiMaxVlc     = g_auiGoRiceRange[ ruiGoRiceParam ];
  Bool bExGolomb    = ( uiSymbol > uiMaxVlc );
  UInt uiCodeWord   = min<UInt>( uiSymbol, ( uiMaxVlc + 1 ) );
  UInt uiQuotient   = uiCodeWord >> ruiGoRiceParam;
  UInt uiMaxPreLen  = g_auiGoRicePrefixLen[ ruiGoRiceParam ];
  
  UInt binValues;
  Int numBins;
  
  if ( uiQuotient >= uiMaxPreLen )
  {
    numBins = uiMaxPreLen;
    binValues = ( 1 << numBins ) - 1;
  }
  else
  {
    numBins = uiQuotient + 1;
    binValues = ( 1 << numBins ) - 2;
  }
  
  m_pcBinIf->encodeBinsEP( ( binValues << ruiGoRiceParam ) + uiCodeWord - ( uiQuotient << ruiGoRiceParam ), numBins + ruiGoRiceParam );
#if !SIMPLE_PARAM_UPDATE  
  ruiGoRiceParam = g_aauiGoRiceUpdate[ ruiGoRiceParam ][ min<UInt>( uiSymbol, 23 ) ];
#endif
  
  if( bExGolomb )
  {
    uiSymbol -= uiMaxVlc + 1;
    xWriteEpExGolomb( uiSymbol, 0 );
  }
}

// SBAC RD
Void  TEncSbac::load ( TEncSbac* pSrc)
{
  this->xCopyFrom(pSrc);
}

Void  TEncSbac::loadIntraDirModeLuma( TEncSbac* pSrc)
{
  m_pcBinIf->copyState( pSrc->m_pcBinIf );
  
  this->m_cCUIntraPredSCModel      .copyFrom( &pSrc->m_cCUIntraPredSCModel       );
}


Void  TEncSbac::store( TEncSbac* pDest)
{
  pDest->xCopyFrom( this );
}


Void TEncSbac::xCopyFrom( TEncSbac* pSrc )
{
  m_pcBinIf->copyState( pSrc->m_pcBinIf );
  
  this->m_uiCoeffCost = pSrc->m_uiCoeffCost;
  this->m_uiLastQp    = pSrc->m_uiLastQp;
  
  memcpy( m_contextModels, pSrc->m_contextModels, m_numContextModels * sizeof( ContextModel ) );
}

Void TEncSbac::codeMVPIdx ( TComDataCU* pcCU, UInt uiAbsPartIdx, RefPicList eRefList )
{
  Int iSymbol = pcCU->getMVPIdx(eRefList, uiAbsPartIdx);
  Int iNum = AMVP_MAX_NUM_CANDS;

  xWriteUnaryMaxSymbol(iSymbol, m_cMVPIdxSCModel.get(0), 1, iNum-1);
}

Void TEncSbac::codePartSize( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
  PartSize eSize         = pcCU->getPartitionSize( uiAbsPartIdx );
  if ( pcCU->isIntra( uiAbsPartIdx ) )
  {
    if( uiDepth == g_uiMaxCUDepth - g_uiAddCUDepth )
    {
      m_pcBinIf->encodeBin( eSize == SIZE_2Nx2N? 1 : 0, m_cCUPartSizeSCModel.get( 0, 0, 0 ) );
    }
    return;
  }
  
  switch(eSize)
  {
    case SIZE_2Nx2N:
    {
      m_pcBinIf->encodeBin( 1, m_cCUPartSizeSCModel.get( 0, 0, 0) );
      break;
    }
    case SIZE_2NxN:
    case SIZE_2NxnU:
    case SIZE_2NxnD:
    {
      m_pcBinIf->encodeBin( 0, m_cCUPartSizeSCModel.get( 0, 0, 0) );
      m_pcBinIf->encodeBin( 1, m_cCUPartSizeSCModel.get( 0, 0, 1) );
      if ( pcCU->getSlice()->getSPS()->getAMPAcc( uiDepth ) )
      {
        if (eSize == SIZE_2NxN)
        {
          m_pcBinIf->encodeBin(1, m_cCUAMPSCModel.get( 0, 0, 0 ));
        }
        else
        {
          m_pcBinIf->encodeBin(0, m_cCUAMPSCModel.get( 0, 0, 0 ));          
          m_pcBinIf->encodeBinEP((eSize == SIZE_2NxnU? 0: 1));
        }
      }
      break;
    }
    case SIZE_Nx2N:
    case SIZE_nLx2N:
    case SIZE_nRx2N:
    {
      m_pcBinIf->encodeBin( 0, m_cCUPartSizeSCModel.get( 0, 0, 0) );
      m_pcBinIf->encodeBin( 0, m_cCUPartSizeSCModel.get( 0, 0, 1) );
      if( uiDepth == g_uiMaxCUDepth - g_uiAddCUDepth && !( pcCU->getSlice()->getSPS()->getDisInter4x4() && pcCU->getWidth(uiAbsPartIdx) == 8 && pcCU->getHeight(uiAbsPartIdx) == 8 ) )
      {
        m_pcBinIf->encodeBin( 1, m_cCUPartSizeSCModel.get( 0, 0, 2) );
      }
      if ( pcCU->getSlice()->getSPS()->getAMPAcc( uiDepth ) )
      {
        if (eSize == SIZE_Nx2N)
        {
          m_pcBinIf->encodeBin(1, m_cCUAMPSCModel.get( 0, 0, 0 ));
        }
        else
        {
          m_pcBinIf->encodeBin(0, m_cCUAMPSCModel.get( 0, 0, 0 ));
          m_pcBinIf->encodeBinEP((eSize == SIZE_nLx2N? 0: 1));
        }
      }
      break;
    }
    case SIZE_NxN:
    {
      if( uiDepth == g_uiMaxCUDepth - g_uiAddCUDepth && !( pcCU->getSlice()->getSPS()->getDisInter4x4() && pcCU->getWidth(uiAbsPartIdx) == 8 && pcCU->getHeight(uiAbsPartIdx) == 8 ) )
      {
        m_pcBinIf->encodeBin( 0, m_cCUPartSizeSCModel.get( 0, 0, 0) );
        m_pcBinIf->encodeBin( 0, m_cCUPartSizeSCModel.get( 0, 0, 1) );
        m_pcBinIf->encodeBin( 0, m_cCUPartSizeSCModel.get( 0, 0, 2) );
      }
      break;
    }
    default:
    {
      assert(0);
    }
  }
}

/** code prediction mode
 * \param pcCU
 * \param uiAbsPartIdx  
 * \returns Void
 */
Void TEncSbac::codePredMode( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  // get context function is here
  Int iPredMode = pcCU->getPredictionMode( uiAbsPartIdx );
  m_pcBinIf->encodeBin( iPredMode == MODE_INTER ? 0 : 1, m_cCUPredModeSCModel.get( 0, 0, 0 ) );
}

#if !AHG6_ALF_OPTION2
Void TEncSbac::codeAlfCtrlFlag( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  if (!m_bAlfCtrl)
    return;
  
  if( pcCU->getDepth(uiAbsPartIdx) > m_uiMaxAlfCtrlDepth && !pcCU->isFirstAbsZorderIdxInDepth(uiAbsPartIdx, m_uiMaxAlfCtrlDepth))
  {
    return;
  }
  
  const UInt uiSymbol = pcCU->getAlfCtrlFlag( uiAbsPartIdx ) ? 1 : 0;
  m_pcBinIf->encodeBin( uiSymbol, *m_cCUAlfCtrlFlagSCModel.get( 0 ) );
}

Void TEncSbac::codeAlfCtrlDepth()
{
  if (!m_bAlfCtrl)
    return;
  
  UInt uiDepth = m_uiMaxAlfCtrlDepth;
  xWriteUnaryMaxSymbol(uiDepth, m_cALFUvlcSCModel.get(0), 1, g_uiMaxCUDepth-1);
}
#endif
/** code skip flag
 * \param pcCU
 * \param uiAbsPartIdx 
 * \returns Void
 */
Void TEncSbac::codeSkipFlag( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  // get context function is here
  UInt uiSymbol = pcCU->isSkipped( uiAbsPartIdx ) ? 1 : 0;
  UInt uiCtxSkip = pcCU->getCtxSkipFlag( uiAbsPartIdx ) ;
  m_pcBinIf->encodeBin( uiSymbol, m_cCUSkipFlagSCModel.get( 0, 0, uiCtxSkip ) );
  DTRACE_CABAC_VL( g_nSymbolCounter++ );
  DTRACE_CABAC_T( "\tSkipFlag" );
  DTRACE_CABAC_T( "\tuiCtxSkip: ");
  DTRACE_CABAC_V( uiCtxSkip );
  DTRACE_CABAC_T( "\tuiSymbol: ");
  DTRACE_CABAC_V( uiSymbol );
  DTRACE_CABAC_T( "\n");
}

/** code merge flag
 * \param pcCU
 * \param uiAbsPartIdx 
 * \returns Void
 */
Void TEncSbac::codeMergeFlag( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  const UInt uiSymbol = pcCU->getMergeFlag( uiAbsPartIdx ) ? 1 : 0;
  m_pcBinIf->encodeBin( uiSymbol, *m_cCUMergeFlagExtSCModel.get( 0 ) );

  DTRACE_CABAC_VL( g_nSymbolCounter++ );
  DTRACE_CABAC_T( "\tMergeFlag: " );
  DTRACE_CABAC_V( uiSymbol );
  DTRACE_CABAC_T( "\tAddress: " );
  DTRACE_CABAC_V( pcCU->getAddr() );
  DTRACE_CABAC_T( "\tuiAbsPartIdx: " );
  DTRACE_CABAC_V( uiAbsPartIdx );
  DTRACE_CABAC_T( "\n" );
}

/** code merge index
 * \param pcCU
 * \param uiAbsPartIdx 
 * \returns Void
 */
Void TEncSbac::codeMergeIndex( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  UInt uiNumCand = MRG_MAX_NUM_CANDS;
  UInt uiUnaryIdx = pcCU->getMergeIndex( uiAbsPartIdx );
  uiNumCand = pcCU->getSlice()->getMaxNumMergeCand();
  if ( uiNumCand > 1 )
  {
    for( UInt ui = 0; ui < uiNumCand - 1; ++ui )
    {
      const UInt uiSymbol = ui == uiUnaryIdx ? 0 : 1;
      if ( ui==0 )
      {
        m_pcBinIf->encodeBin( uiSymbol, m_cCUMergeIdxExtSCModel.get( 0, 0, 0 ) );
      }
      else
      {
        m_pcBinIf->encodeBinEP( uiSymbol );
      }
      if( uiSymbol == 0 )
      {
        break;
      }
    }
  }
  DTRACE_CABAC_VL( g_nSymbolCounter++ );
  DTRACE_CABAC_T( "\tparseMergeIndex()" );
  DTRACE_CABAC_T( "\tuiMRGIdx= " );
  DTRACE_CABAC_V( pcCU->getMergeIndex( uiAbsPartIdx ) );
  DTRACE_CABAC_T( "\n" );
}

Void TEncSbac::codeSplitFlag   ( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
  if( uiDepth == g_uiMaxCUDepth - g_uiAddCUDepth )
    return;
  
  UInt uiCtx           = pcCU->getCtxSplitFlag( uiAbsPartIdx, uiDepth );
  UInt uiCurrSplitFlag = ( pcCU->getDepth( uiAbsPartIdx ) > uiDepth ) ? 1 : 0;
  
  assert( uiCtx < 3 );
  m_pcBinIf->encodeBin( uiCurrSplitFlag, m_cCUSplitFlagSCModel.get( 0, 0, uiCtx ) );
  DTRACE_CABAC_VL( g_nSymbolCounter++ )
  DTRACE_CABAC_T( "\tSplitFlag\n" )
  return;
}

Void TEncSbac::codeTransformSubdivFlag( UInt uiSymbol, UInt uiCtx )
{
  m_pcBinIf->encodeBin( uiSymbol, m_cCUTransSubdivFlagSCModel.get( 0, 0, uiCtx ) );
  DTRACE_CABAC_VL( g_nSymbolCounter++ )
  DTRACE_CABAC_T( "\tparseTransformSubdivFlag()" )
  DTRACE_CABAC_T( "\tsymbol=" )
  DTRACE_CABAC_V( uiSymbol )
  DTRACE_CABAC_T( "\tctx=" )
  DTRACE_CABAC_V( uiCtx )
  DTRACE_CABAC_T( "\n" )
}
Void TEncSbac::codeIntraDirLumaAng( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  UInt uiDir         = pcCU->getLumaIntraDir( uiAbsPartIdx );
  
  Int uiPreds[3] = {-1, -1, -1};
  Int uiPredNum = pcCU->getIntraDirLumaPredictor(uiAbsPartIdx, uiPreds);  

  Int uiPredIdx = -1;

  for(UInt i = 0; i < uiPredNum; i++)
  {
    if(uiDir == uiPreds[i])
    {
      uiPredIdx = i;
    }
  }
 
  if(uiPredIdx != -1)
  {
    m_pcBinIf->encodeBin( 1, m_cCUIntraPredSCModel.get( 0, 0, 0 ) );
    m_pcBinIf->encodeBinEP( uiPredIdx ? 1 : 0 );
    if (uiPredIdx)
    {
      m_pcBinIf->encodeBinEP( uiPredIdx-1 );
    }
  }
  else
  {
    m_pcBinIf->encodeBin( 0, m_cCUIntraPredSCModel.get( 0, 0, 0 ) );
  
    if (uiPreds[0] > uiPreds[1])
    { 
      std::swap(uiPreds[0], uiPreds[1]); 
    }
    if (uiPreds[0] > uiPreds[2])
    {
      std::swap(uiPreds[0], uiPreds[2]);
    }
    if (uiPreds[1] > uiPreds[2])
    {
      std::swap(uiPreds[1], uiPreds[2]);
    }

    for(Int i = (uiPredNum - 1); i >= 0; i--)
    {
      uiDir = uiDir > uiPreds[i] ? uiDir - 1 : uiDir;
    }

    m_pcBinIf->encodeBinsEP( uiDir, 5 );
   }
  return;
}
Void TEncSbac::codeIntraDirChroma( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  UInt uiIntraDirChroma = pcCU->getChromaIntraDir( uiAbsPartIdx );

  if( uiIntraDirChroma == DM_CHROMA_IDX ) 
  {
    m_pcBinIf->encodeBin( 0, m_cCUChromaPredSCModel.get( 0, 0, 0 ) );
  } 
  else if( uiIntraDirChroma == LM_CHROMA_IDX )
  {
    m_pcBinIf->encodeBin( 1, m_cCUChromaPredSCModel.get( 0, 0, 0 ) );
    m_pcBinIf->encodeBin( 0, m_cCUChromaPredSCModel.get( 0, 0, 1 ) );
  }
  else
  { 
    UInt uiAllowedChromaDir[ NUM_CHROMA_MODE ];
    pcCU->getAllowedChromaDir( uiAbsPartIdx, uiAllowedChromaDir );

    for( Int i = 0; i < NUM_CHROMA_MODE - 2; i++ )
    {
      if( uiIntraDirChroma == uiAllowedChromaDir[i] )
      {
        uiIntraDirChroma = i;
        break;
      }
    }
    m_pcBinIf->encodeBin( 1, m_cCUChromaPredSCModel.get( 0, 0, 0 ) );

    if (pcCU->getSlice()->getSPS()->getUseLMChroma())
    {
      m_pcBinIf->encodeBin( 1, m_cCUChromaPredSCModel.get( 0, 0, 1 ));
    }
    m_pcBinIf->encodeBinsEP( uiIntraDirChroma, 2 );
  }
  return;
}

Void TEncSbac::codeInterDir( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  const UInt uiInterDir = pcCU->getInterDir( uiAbsPartIdx ) - 1;
  const UInt uiCtx      = pcCU->getCtxInterDir( uiAbsPartIdx );
  ContextModel *pCtx    = m_cCUInterDirSCModel.get( 0 );
  m_pcBinIf->encodeBin( uiInterDir == 2 ? 1 : 0, *( pCtx + uiCtx ) );

  return;
}

Void TEncSbac::codeRefFrmIdx( TComDataCU* pcCU, UInt uiAbsPartIdx, RefPicList eRefList )
{
  if ( pcCU->getSlice()->getNumRefIdx(REF_PIC_LIST_C) > 0 && pcCU->getInterDir( uiAbsPartIdx ) != 3)
  {
    Int iRefFrame = pcCU->getSlice()->getRefIdxOfLC(eRefList, pcCU->getCUMvField( eRefList )->getRefIdx( uiAbsPartIdx ));

    ContextModel *pCtx = m_cCURefPicSCModel.get( 0 );
    m_pcBinIf->encodeBin( ( iRefFrame == 0 ? 0 : 1 ), *pCtx );

    if( iRefFrame > 0 )
    {
      xWriteUnaryMaxSymbol( iRefFrame - 1, pCtx + 1, 1, pcCU->getSlice()->getNumRefIdx( REF_PIC_LIST_C )-2 );
    }
  }
  else
  {
    Int iRefFrame = pcCU->getCUMvField( eRefList )->getRefIdx( uiAbsPartIdx );
    ContextModel *pCtx = m_cCURefPicSCModel.get( 0 );
    m_pcBinIf->encodeBin( ( iRefFrame == 0 ? 0 : 1 ), *pCtx );
    
    if( iRefFrame > 0 )
    {
      xWriteUnaryMaxSymbol( iRefFrame - 1, pCtx + 1, 1, pcCU->getSlice()->getNumRefIdx( eRefList )-2 );
    }
  }
  return;
}

Void TEncSbac::codeMvd( TComDataCU* pcCU, UInt uiAbsPartIdx, RefPicList eRefList )
{
  if(pcCU->getSlice()->getMvdL1ZeroFlag() && eRefList == REF_PIC_LIST_1 && pcCU->getInterDir(uiAbsPartIdx)==3)
  {
    return;
  }

  const TComCUMvField* pcCUMvField = pcCU->getCUMvField( eRefList );
  const Int iHor = pcCUMvField->getMvd( uiAbsPartIdx ).getHor();
  const Int iVer = pcCUMvField->getMvd( uiAbsPartIdx ).getVer();
  ContextModel* pCtx = m_cCUMvdSCModel.get( 0 );

  m_pcBinIf->encodeBin( iHor != 0 ? 1 : 0, *pCtx );
  m_pcBinIf->encodeBin( iVer != 0 ? 1 : 0, *pCtx );

  const Bool bHorAbsGr0 = iHor != 0;
  const Bool bVerAbsGr0 = iVer != 0;
  const UInt uiHorAbs   = 0 > iHor ? -iHor : iHor;
  const UInt uiVerAbs   = 0 > iVer ? -iVer : iVer;
  pCtx++;

  if( bHorAbsGr0 )
  {
    m_pcBinIf->encodeBin( uiHorAbs > 1 ? 1 : 0, *pCtx );
  }

  if( bVerAbsGr0 )
  {
    m_pcBinIf->encodeBin( uiVerAbs > 1 ? 1 : 0, *pCtx );
  }

  if( bHorAbsGr0 )
  {
    if( uiHorAbs > 1 )
    {
      xWriteEpExGolomb( uiHorAbs-2, 1 );
    }

    m_pcBinIf->encodeBinEP( 0 > iHor ? 1 : 0 );
  }

  if( bVerAbsGr0 )
  {
    if( uiVerAbs > 1 )
    {
      xWriteEpExGolomb( uiVerAbs-2, 1 );
    }

    m_pcBinIf->encodeBinEP( 0 > iVer ? 1 : 0 );
  }
  
  return;
}

Void TEncSbac::codeDeltaQP( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  Int iDQp  = pcCU->getQP( uiAbsPartIdx ) - pcCU->getRefQP( uiAbsPartIdx );
  
  Int qpBdOffsetY =  pcCU->getSlice()->getSPS()->getQpBDOffsetY();
  iDQp = (iDQp + 78 + qpBdOffsetY + (qpBdOffsetY/2)) % (52 + qpBdOffsetY) - 26 - (qpBdOffsetY/2);

  if ( iDQp == 0 )
  {
    m_pcBinIf->encodeBin( 0, m_cCUDeltaQpSCModel.get( 0, 0, 0 ) );
  }
  else
  {
    m_pcBinIf->encodeBin( 1, m_cCUDeltaQpSCModel.get( 0, 0, 0 ) );
   
    UInt uiSign = (iDQp > 0 ? 0 : 1);

    m_pcBinIf->encodeBinEP(uiSign);

    assert(iDQp >= -(26+(qpBdOffsetY/2)));
    assert(iDQp <=  (25+(qpBdOffsetY/2)));

    UInt uiMaxAbsDQpMinus1 = 24 + (qpBdOffsetY/2) + (uiSign);
    UInt uiAbsDQpMinus1 = (UInt)((iDQp > 0)? iDQp  : (-iDQp)) - 1;
    xWriteUnaryMaxSymbol( uiAbsDQpMinus1, &m_cCUDeltaQpSCModel.get( 0, 0, 1 ), 1, uiMaxAbsDQpMinus1);
  }
  
  return;
}

Void TEncSbac::codeQtCbf( TComDataCU* pcCU, UInt uiAbsPartIdx, TextType eType, UInt uiTrDepth )
{
  UInt uiCbf = pcCU->getCbf     ( uiAbsPartIdx, eType, uiTrDepth );
  UInt uiCtx = pcCU->getCtxQtCbf( uiAbsPartIdx, eType, uiTrDepth );
  m_pcBinIf->encodeBin( uiCbf , m_cCUQtCbfSCModel.get( 0, eType ? TEXT_CHROMA : eType, uiCtx ) );
  DTRACE_CABAC_VL( g_nSymbolCounter++ )
  DTRACE_CABAC_T( "\tparseQtCbf()" )
  DTRACE_CABAC_T( "\tsymbol=" )
  DTRACE_CABAC_V( uiCbf )
  DTRACE_CABAC_T( "\tctx=" )
  DTRACE_CABAC_V( uiCtx )
  DTRACE_CABAC_T( "\tetype=" )
  DTRACE_CABAC_V( eType )
  DTRACE_CABAC_T( "\tuiAbsPartIdx=" )
  DTRACE_CABAC_V( uiAbsPartIdx )
  DTRACE_CABAC_T( "\n" )
}

/** Code I_PCM information. 
 * \param pcCU pointer to CU
 * \param uiAbsPartIdx CU index
 * \param numIPCM the number of succesive IPCM blocks with the same size 
 * \param firstIPCMFlag 
 * \returns Void
 */
Void TEncSbac::codeIPCMInfo( TComDataCU* pcCU, UInt uiAbsPartIdx, Int numIPCM, Bool firstIPCMFlag)
{
  UInt uiIPCM = (pcCU->getIPCMFlag(uiAbsPartIdx) == true)? 1 : 0;

  Bool writePCMSampleFlag = pcCU->getIPCMFlag(uiAbsPartIdx);

  if( uiIPCM == 0 || firstIPCMFlag)
  {
    m_pcBinIf->encodeBinTrm (uiIPCM);

    if ( firstIPCMFlag )
    {
      m_pcBinIf->encodeNumSubseqIPCM( numIPCM - 1 );
      m_pcBinIf->encodePCMAlignBits();
    }
  }

  if (writePCMSampleFlag)
  {
    UInt uiMinCoeffSize = pcCU->getPic()->getMinCUWidth()*pcCU->getPic()->getMinCUHeight();
    UInt uiLumaOffset   = uiMinCoeffSize*uiAbsPartIdx;
    UInt uiChromaOffset = uiLumaOffset>>2;
    Pel* piPCMSample;
    UInt uiWidth;
    UInt uiHeight;
    UInt uiSampleBits;
    UInt uiX, uiY;

    piPCMSample = pcCU->getPCMSampleY() + uiLumaOffset;
    uiWidth = pcCU->getWidth(uiAbsPartIdx);
    uiHeight = pcCU->getHeight(uiAbsPartIdx);
    uiSampleBits = pcCU->getSlice()->getSPS()->getPCMBitDepthLuma();

    for(uiY = 0; uiY < uiHeight; uiY++)
    {
      for(uiX = 0; uiX < uiWidth; uiX++)
      {
        UInt uiSample = piPCMSample[uiX];

        m_pcBinIf->xWritePCMCode(uiSample, uiSampleBits);
      }
      piPCMSample += uiWidth;
    }

    piPCMSample = pcCU->getPCMSampleCb() + uiChromaOffset;
    uiWidth = pcCU->getWidth(uiAbsPartIdx)/2;
    uiHeight = pcCU->getHeight(uiAbsPartIdx)/2;
    uiSampleBits = pcCU->getSlice()->getSPS()->getPCMBitDepthChroma();

    for(uiY = 0; uiY < uiHeight; uiY++)
    {
      for(uiX = 0; uiX < uiWidth; uiX++)
      {
        UInt uiSample = piPCMSample[uiX];

        m_pcBinIf->xWritePCMCode(uiSample, uiSampleBits);
      }
      piPCMSample += uiWidth;
    }

    piPCMSample = pcCU->getPCMSampleCr() + uiChromaOffset;
    uiWidth = pcCU->getWidth(uiAbsPartIdx)/2;
    uiHeight = pcCU->getHeight(uiAbsPartIdx)/2;
    uiSampleBits = pcCU->getSlice()->getSPS()->getPCMBitDepthChroma();

    for(uiY = 0; uiY < uiHeight; uiY++)
    {
      for(uiX = 0; uiX < uiWidth; uiX++)
      {
        UInt uiSample = piPCMSample[uiX];

        m_pcBinIf->xWritePCMCode(uiSample, uiSampleBits);
      }
      piPCMSample += uiWidth;
    }
    numIPCM--;
    if(numIPCM == 0)
    {
      m_pcBinIf->resetBac();
    }
  }
}

Void TEncSbac::codeQtRootCbf( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  UInt uiCbf = pcCU->getQtRootCbf( uiAbsPartIdx );
  UInt uiCtx = 0;
  m_pcBinIf->encodeBin( uiCbf , m_cCUQtRootCbfSCModel.get( 0, 0, uiCtx ) );
  DTRACE_CABAC_VL( g_nSymbolCounter++ )
  DTRACE_CABAC_T( "\tparseQtRootCbf()" )
  DTRACE_CABAC_T( "\tsymbol=" )
  DTRACE_CABAC_V( uiCbf )
  DTRACE_CABAC_T( "\tctx=" )
  DTRACE_CABAC_V( uiCtx )
  DTRACE_CABAC_T( "\tuiAbsPartIdx=" )
  DTRACE_CABAC_V( uiAbsPartIdx )
  DTRACE_CABAC_T( "\n" )
}

/** Encode (X,Y) position of the last significant coefficient
 * \param uiPosX X component of last coefficient
 * \param uiPosY Y component of last coefficient
 * \param width  Block width
 * \param height Block height
 * \param eTType plane type / luminance or chrominance
 * \param uiScanIdx scan type (zig-zag, hor, ver)
 * This method encodes the X and Y component within a block of the last significant coefficient.
 */
Void TEncSbac::codeLastSignificantXY( UInt uiPosX, UInt uiPosY, Int width, Int height, TextType eTType, UInt uiScanIdx )
{  
  // swap
  if( uiScanIdx == SCAN_VER )
  {
    swap( uiPosX, uiPosY );
  }

  UInt uiCtxLast;
  ContextModel *pCtxX = m_cCuCtxLastX.get( 0, eTType );
  ContextModel *pCtxY = m_cCuCtxLastY.get( 0, eTType );
  UInt uiGroupIdxX    = g_uiGroupIdx[ uiPosX ];
  UInt uiGroupIdxY    = g_uiGroupIdx[ uiPosY ];

  // posX
  Int widthCtx = eTType? 4: width;
  const UInt *puiCtxIdxX = g_uiLastCtx + ( g_aucConvertToBit[ widthCtx ] * ( g_aucConvertToBit[ widthCtx ] + 3 ) );
  for( uiCtxLast = 0; uiCtxLast < uiGroupIdxX; uiCtxLast++ )
  {
    if (eTType)
    {
      m_pcBinIf->encodeBin( 1, *( pCtxX + (uiCtxLast>>g_aucConvertToBit[ width ]) ) );
    }
    else
    {
      m_pcBinIf->encodeBin( 1, *( pCtxX + puiCtxIdxX[ uiCtxLast ] ) );
    }
  }
  if( uiGroupIdxX < g_uiGroupIdx[ width - 1 ])
  {
    if ( eTType )
    {
      m_pcBinIf->encodeBin( 0, *( pCtxX + (uiCtxLast>>g_aucConvertToBit[ width ]) ) );
    }
    else
    {
      m_pcBinIf->encodeBin( 0, *( pCtxX + puiCtxIdxX[ uiCtxLast ] ) );
    }
  }

  // posY
  Int heightCtx = eTType? 4: height;
  const UInt *puiCtxIdxY = g_uiLastCtx + ( g_aucConvertToBit[ heightCtx ] * ( g_aucConvertToBit[ heightCtx ] + 3 ) );
  for( uiCtxLast = 0; uiCtxLast < uiGroupIdxY; uiCtxLast++ )
  {
    if (eTType)
    {
      m_pcBinIf->encodeBin( 1, *( pCtxY + (uiCtxLast>>g_aucConvertToBit[ height ])));
    }
    else
    {
      m_pcBinIf->encodeBin( 1, *( pCtxY + puiCtxIdxY[ uiCtxLast ] ) );
    }
  }
  if( uiGroupIdxY < g_uiGroupIdx[ height - 1 ])
  {
    if (eTType)
    {
      m_pcBinIf->encodeBin( 0, *( pCtxY + (uiCtxLast>>g_aucConvertToBit[ height ]) ) );
    }
    else
    {
      m_pcBinIf->encodeBin( 0, *( pCtxY + puiCtxIdxY[ uiCtxLast ] ) );
    }
    }
  if ( uiGroupIdxX > 3 )
  {      
    UInt uiCount = ( uiGroupIdxX - 2 ) >> 1;
    uiPosX       = uiPosX - g_uiMinInGroup[ uiGroupIdxX ];
    for (Int i = uiCount - 1 ; i >= 0; i-- )
    {
      m_pcBinIf->encodeBinEP( ( uiPosX >> i ) & 1 );
    }
  }
  if ( uiGroupIdxY > 3 )
  {      
    UInt uiCount = ( uiGroupIdxY - 2 ) >> 1;
    uiPosY       = uiPosY - g_uiMinInGroup[ uiGroupIdxY ];
    for ( Int i = uiCount - 1 ; i >= 0; i-- )
    {
      m_pcBinIf->encodeBinEP( ( uiPosY >> i ) & 1 );
    }
  }
}

Void TEncSbac::codeCoeffNxN( TComDataCU* pcCU, TCoeff* pcCoef, UInt uiAbsPartIdx, UInt uiWidth, UInt uiHeight, UInt uiDepth, TextType eTType )
{
  DTRACE_CABAC_VL( g_nSymbolCounter++ )
  DTRACE_CABAC_T( "\tparseCoeffNxN()\teType=" )
  DTRACE_CABAC_V( eTType )
  DTRACE_CABAC_T( "\twidth=" )
  DTRACE_CABAC_V( uiWidth )
  DTRACE_CABAC_T( "\theight=" )
  DTRACE_CABAC_V( uiHeight )
  DTRACE_CABAC_T( "\tdepth=" )
  DTRACE_CABAC_V( uiDepth )
  DTRACE_CABAC_T( "\tabspartidx=" )
  DTRACE_CABAC_V( uiAbsPartIdx )
  DTRACE_CABAC_T( "\ttoCU-X=" )
  DTRACE_CABAC_V( pcCU->getCUPelX() )
  DTRACE_CABAC_T( "\ttoCU-Y=" )
  DTRACE_CABAC_V( pcCU->getCUPelY() )
  DTRACE_CABAC_T( "\tCU-addr=" )
  DTRACE_CABAC_V(  pcCU->getAddr() )
  DTRACE_CABAC_T( "\tinCU-X=" )
  DTRACE_CABAC_V( g_auiRasterToPelX[ g_auiZscanToRaster[uiAbsPartIdx] ] )
  DTRACE_CABAC_T( "\tinCU-Y=" )
  DTRACE_CABAC_V( g_auiRasterToPelY[ g_auiZscanToRaster[uiAbsPartIdx] ] )
  DTRACE_CABAC_T( "\tpredmode=" )
  DTRACE_CABAC_V(  pcCU->getPredictionMode( uiAbsPartIdx ) )
  DTRACE_CABAC_T( "\n" )

  if( uiWidth > m_pcSlice->getSPS()->getMaxTrSize() )
  {
    uiWidth  = m_pcSlice->getSPS()->getMaxTrSize();
    uiHeight = m_pcSlice->getSPS()->getMaxTrSize();
  }
  
  UInt uiNumSig = 0;
  
  // compute number of significant coefficients
  uiNumSig = TEncEntropy::countNonZeroCoeffs(pcCoef, uiWidth * uiHeight);
  
  if ( uiNumSig == 0 )
    return;
  
  eTType = eTType == TEXT_LUMA ? TEXT_LUMA : ( eTType == TEXT_NONE ? TEXT_NONE : TEXT_CHROMA );
  
  //----- encode significance map -----
  const UInt   uiLog2BlockSize   = g_aucConvertToBit[ uiWidth ] + 2;
  UInt uiScanIdx = pcCU->getCoefScanIdx(uiAbsPartIdx, uiWidth, eTType==TEXT_LUMA, pcCU->isIntra(uiAbsPartIdx));
  if (uiScanIdx == SCAN_ZIGZAG)
  {
    // Map zigzag to diagonal scan
    uiScanIdx = SCAN_DIAG;
  }
  Int blockType = uiLog2BlockSize;
  if (uiWidth != uiHeight)
  {
    uiScanIdx = SCAN_DIAG;
    blockType = 4;
  }
  
  const UInt * scan;
  if (uiWidth == uiHeight)
  {
    scan = g_auiSigLastScan[ uiScanIdx ][ uiLog2BlockSize - 1 ];
  }
  else
  {
    scan = g_sigScanNSQT[ uiLog2BlockSize - 2 ];
  }
  
#if !FIXED_SBH_THRESHOLD
  UInt const tsig = pcCU->getSlice()->getPPS()->getTSIG();
#endif
#if LOSSLESS_CODING
  Bool beValid; 
  if (pcCU->isLosslessCoded(uiAbsPartIdx))
  {
    beValid = false;
  }
  else 
  {
    beValid = pcCU->getSlice()->getPPS()->getSignHideFlag() > 0;
  }
#else
  Bool beValid = pcCU->getSlice()->getPPS()->getSignHideFlag() > 0;
#endif

  // Find position of last coefficient
  Int scanPosLast = -1;
  Int posLast;

  const UInt * scanCG;
  if (uiWidth == uiHeight)
  {
    scanCG = g_auiSigLastScan[ uiScanIdx ][ uiLog2BlockSize > 3 ? uiLog2BlockSize-2-1 : 0 ];
    if( uiLog2BlockSize == 3 )
    {
      scanCG = g_sigLastScan8x8[ uiScanIdx ];
    }
    else if( uiLog2BlockSize == 5 )
    {
      scanCG = g_sigLastScanCG32x32;
    }
  }
  else
  {
    scanCG = g_sigCGScanNSQT[ uiLog2BlockSize - 2 ];
  }
  UInt uiSigCoeffGroupFlag[ MLS_GRP_NUM ];
  static const UInt uiShift = MLS_CG_SIZE >> 1;
  const UInt uiNumBlkSide = uiWidth >> uiShift;

    ::memset( uiSigCoeffGroupFlag, 0, sizeof(UInt) * MLS_GRP_NUM );

    do
    {
      posLast = scan[ ++scanPosLast ];

      // get L1 sig map
      UInt uiPosY    = posLast >> uiLog2BlockSize;
      UInt uiPosX    = posLast - ( uiPosY << uiLog2BlockSize );
      UInt uiBlkIdx  = uiNumBlkSide * (uiPosY >> uiShift) + (uiPosX >> uiShift);
      if( uiWidth == 8 && uiHeight == 8 && (uiScanIdx == SCAN_HOR || uiScanIdx == SCAN_VER) )
      {
        if( uiScanIdx == SCAN_HOR )
        {
          uiBlkIdx = uiPosY >> 1;
        }
        else if( uiScanIdx == SCAN_VER )
        {
          uiBlkIdx = uiPosX >> 1;
        }
      }
      if( pcCoef[ posLast ] )
      {
        uiSigCoeffGroupFlag[ uiBlkIdx ] = 1;
      }

      uiNumSig -= ( pcCoef[ posLast ] != 0 );
    }
    while ( uiNumSig > 0 );

  // Code position of last coefficient
  Int posLastY = posLast >> uiLog2BlockSize;
  Int posLastX = posLast - ( posLastY << uiLog2BlockSize );
  codeLastSignificantXY(posLastX, posLastY, uiWidth, uiHeight, eTType, uiScanIdx);
  
  //===== code significance flag =====
  ContextModel * const baseCoeffGroupCtx = m_cCUSigCoeffGroupSCModel.get( 0, eTType );
  ContextModel * const baseCtx = (eTType==TEXT_LUMA) ? m_cCUSigSCModel.get( 0, 0 ) : m_cCUSigSCModel.get( 0, 0 ) + NUM_SIG_FLAG_CTX_LUMA;


  const Int  iLastScanSet      = scanPosLast >> LOG2_SCAN_SET_SIZE;
  UInt uiNumOne                = 0;
  UInt uiGoRiceParam           = 0;
  Int  iScanPosSig             = scanPosLast;

  for( Int iSubSet = iLastScanSet; iSubSet >= 0; iSubSet-- )
  {
    Int numNonZero = 0;
    Int  iSubPos     = iSubSet << LOG2_SCAN_SET_SIZE;
    uiGoRiceParam    = 0;
    Int absCoeff[16];
    UInt coeffSigns = 0;

    Int lastNZPosInCG = -1, firstNZPosInCG = SCAN_SET_SIZE;

    if( iScanPosSig == scanPosLast )
    {
      absCoeff[ 0 ] = abs( pcCoef[ posLast ] );
      coeffSigns    = ( pcCoef[ posLast ] < 0 );
      numNonZero    = 1;
      lastNZPosInCG  = iScanPosSig;
      firstNZPosInCG = iScanPosSig;
      iScanPosSig--;
    }

      // encode significant_coeffgroup_flag
      Int iCGBlkPos = scanCG[ iSubSet ];
      Int iCGPosY   = iCGBlkPos / uiNumBlkSide;
      Int iCGPosX   = iCGBlkPos - (iCGPosY * uiNumBlkSide);
      if( uiWidth == 8 && uiHeight == 8 && (uiScanIdx == SCAN_HOR || uiScanIdx == SCAN_VER) )
      {
        iCGPosY = (uiScanIdx == SCAN_HOR ? iCGBlkPos : 0);
        iCGPosX = (uiScanIdx == SCAN_VER ? iCGBlkPos : 0);
      }
      if( iSubSet == iLastScanSet || iSubSet == 0)
      {
        uiSigCoeffGroupFlag[ iCGBlkPos ] = 1;
      }
      else
      {
          UInt uiSigCoeffGroup   = (uiSigCoeffGroupFlag[ iCGBlkPos ] != 0);
          UInt uiCtxSig  = TComTrQuant::getSigCoeffGroupCtxInc( uiSigCoeffGroupFlag, iCGPosX, iCGPosY, uiScanIdx, uiWidth, uiHeight );
          m_pcBinIf->encodeBin( uiSigCoeffGroup, baseCoeffGroupCtx[ uiCtxSig ] );
      }
      
      // encode significant_coeff_flag
      if( uiSigCoeffGroupFlag[ iCGBlkPos ] )
      {
#if POS_BASED_SIG_COEFF_CTX
        Int patternSigCtx = TComTrQuant::calcPatternSigCtx( uiSigCoeffGroupFlag, iCGPosX, iCGPosY, uiWidth, uiHeight );
#endif
        UInt uiBlkPos, uiPosY, uiPosX, uiSig, uiCtxSig;
        for( ; iScanPosSig >= iSubPos; iScanPosSig-- )
        {
          uiBlkPos  = scan[ iScanPosSig ]; 
          uiPosY    = uiBlkPos >> uiLog2BlockSize;
          uiPosX    = uiBlkPos - ( uiPosY << uiLog2BlockSize );
          uiSig     = (pcCoef[ uiBlkPos ] != 0);
          if( iScanPosSig > iSubPos || iSubSet == 0 || numNonZero )
          {
#if POS_BASED_SIG_COEFF_CTX
            uiCtxSig  = TComTrQuant::getSigCtxInc( patternSigCtx, uiPosX, uiPosY, blockType, uiWidth, uiHeight, eTType );
#else
            uiCtxSig  = TComTrQuant::getSigCtxInc( pcCoef, uiPosX, uiPosY, blockType, uiWidth, uiHeight, eTType );
#endif
            m_pcBinIf->encodeBin( uiSig, baseCtx[ uiCtxSig ] );
          }
          if( uiSig )
          {
            absCoeff[ numNonZero ] = abs( pcCoef[ uiBlkPos ] );
            coeffSigns = 2 * coeffSigns + ( pcCoef[ uiBlkPos ] < 0 );
            numNonZero++;
            if( lastNZPosInCG == -1 )
            {
              lastNZPosInCG = iScanPosSig;
            }
            firstNZPosInCG = iScanPosSig;
          }
        }
      }
      else
      {
        iScanPosSig = iSubPos - 1;
      }

    if( numNonZero > 0 )
    {
#if FIXED_SBH_THRESHOLD
      Bool signHidden = ( lastNZPosInCG - firstNZPosInCG >= SBH_THRESHOLD );
#else
      Bool signHidden = ( lastNZPosInCG - firstNZPosInCG >= (Int)tsig );
#endif

      UInt c1 = 1;
      UInt uiCtxSet = (iSubSet > 0 && eTType==TEXT_LUMA) ? 2 : 0;
      
      if( uiNumOne > 0 )
      {
        uiCtxSet++;
      }
      
      uiNumOne       >>= 1;
      ContextModel *baseCtxMod = ( eTType==TEXT_LUMA ) ? m_cCUOneSCModel.get( 0, 0 ) + 4 * uiCtxSet : m_cCUOneSCModel.get( 0, 0 ) + NUM_ONE_FLAG_CTX_LUMA + 4 * uiCtxSet;
      
      Int numC1Flag = min(numNonZero, C1FLAG_NUMBER);
      Int firstC2FlagIdx = -1;
      for( Int idx = 0; idx < numC1Flag; idx++ )
      {
        UInt uiSymbol = absCoeff[ idx ] > 1;
        m_pcBinIf->encodeBin( uiSymbol, baseCtxMod[c1] );
        if( uiSymbol )
        {
          c1 = 0;

          if (firstC2FlagIdx == -1)
          {
            firstC2FlagIdx = idx;
          }
        }
        else if( (c1 < 3) && (c1 > 0) )
        {
          c1++;
        }
      }
      
      if (c1 == 0)
      {

        baseCtxMod = ( eTType==TEXT_LUMA ) ? m_cCUAbsSCModel.get( 0, 0 ) + uiCtxSet : m_cCUAbsSCModel.get( 0, 0 ) + NUM_ABS_FLAG_CTX_LUMA + uiCtxSet;
        if ( firstC2FlagIdx != -1)
        {
          UInt symbol = absCoeff[ firstC2FlagIdx ] > 2;
          m_pcBinIf->encodeBin( symbol, baseCtxMod[0] );
        }
      }
      
      if( beValid && signHidden )
      {
        m_pcBinIf->encodeBinsEP( (coeffSigns >> 1), numNonZero-1 );
      }
      else
      {
        m_pcBinIf->encodeBinsEP( coeffSigns, numNonZero );
      }
      
      Int iFirstCoeff2 = 1;    
      if (c1 == 0 || numNonZero > C1FLAG_NUMBER)
      {
        for ( Int idx = 0; idx < numNonZero; idx++ )
        {
          UInt baseLevel  = (idx < C1FLAG_NUMBER)? (2 + iFirstCoeff2 ) : 1;

          if( absCoeff[ idx ] >= baseLevel)
          {
            xWriteGoRiceExGolomb( absCoeff[ idx ] - baseLevel, uiGoRiceParam ); 
#if SIMPLE_PARAM_UPDATE
            if(absCoeff[idx] > 3*(1<<uiGoRiceParam))
               uiGoRiceParam = min<UInt>(uiGoRiceParam+ 1, 4);
#endif
          }
          if(absCoeff[ idx ] >= 2)  
          {
            iFirstCoeff2 = 0;
            uiNumOne++;
          }
        }        
      }
    }
    else
    {
      uiNumOne >>= 1;
    }
  }

  return;
}
#if AHG6_ALF_OPTION2
Void TEncSbac::codeAlfCtrlFlag( Int compIdx, UInt code )
{
  m_pcBinIf->encodeBin( code, m_cCUAlfCtrlFlagSCModel.get( 0, 0, 0) );

  DTRACE_CABAC_VL( g_nSymbolCounter++ )
  DTRACE_CABAC_T( "\codeAlfCtrlFlag()" )
  DTRACE_CABAC_T( "\tsymbol=" )
  DTRACE_CABAC_V( code )
  DTRACE_CABAC_T( "\tcompIdx=" )
  DTRACE_CABAC_V( compIdx )
  DTRACE_CABAC_T( "\n" )
}
#else
Void TEncSbac::codeAlfFlag       ( UInt uiCode )
{
  UInt uiSymbol = ( ( uiCode == 0 ) ? 0 : 1 );
  m_pcBinIf->encodeBin( uiSymbol, m_cALFFlagSCModel.get( 0, 0, 0 ) );
}

Void TEncSbac::codeAlfCtrlFlag( UInt uiSymbol )
{
  m_pcBinIf->encodeBin( uiSymbol, m_cCUAlfCtrlFlagSCModel.get( 0, 0, 0) );
}

Void TEncSbac::codeAlfUvlc       ( UInt uiCode )
{
  Int i;
  
  if ( uiCode == 0 )
  {
    m_pcBinIf->encodeBin( 0, m_cALFUvlcSCModel.get( 0, 0, 0 ) );
  }
  else
  {
    m_pcBinIf->encodeBin( 1, m_cALFUvlcSCModel.get( 0, 0, 0 ) );
    for ( i=0; i<uiCode-1; i++ )
    {
      m_pcBinIf->encodeBin( 1, m_cALFUvlcSCModel.get( 0, 0, 1 ) );
    }
    m_pcBinIf->encodeBin( 0, m_cALFUvlcSCModel.get( 0, 0, 1 ) );
  }
}

Void TEncSbac::codeAlfSvlc       ( Int iCode )
{
  Int i;
  
  if ( iCode == 0 )
  {
    m_pcBinIf->encodeBin( 0, m_cALFSvlcSCModel.get( 0, 0, 0 ) );
  }
  else
  {
    m_pcBinIf->encodeBin( 1, m_cALFSvlcSCModel.get( 0, 0, 0 ) );
    
    // write sign
    if ( iCode > 0 )
    {
      m_pcBinIf->encodeBin( 0, m_cALFSvlcSCModel.get( 0, 0, 1 ) );
    }
    else
    {
      m_pcBinIf->encodeBin( 1, m_cALFSvlcSCModel.get( 0, 0, 1 ) );
      iCode = -iCode;
    }
    
    // write magnitude
    for ( i=0; i<iCode-1; i++ )
    {
      m_pcBinIf->encodeBin( 1, m_cALFSvlcSCModel.get( 0, 0, 2 ) );
    }
    m_pcBinIf->encodeBin( 0, m_cALFSvlcSCModel.get( 0, 0, 2 ) );
  }
}
#endif
Void TEncSbac::codeSaoFlag       ( UInt uiCode )
{
  UInt uiSymbol = ( ( uiCode == 0 ) ? 0 : 1 );
  m_pcBinIf->encodeBin( uiSymbol, m_cSaoFlagSCModel.get( 0, 0, 0 ) );
}

Void TEncSbac::codeSaoUvlc       ( UInt uiCode )
{
  Int i;

  if ( uiCode == 0 )
  {
    m_pcBinIf->encodeBin( 0, m_cSaoUvlcSCModel.get( 0, 0, 0 ) );
  }
  else
  {
    m_pcBinIf->encodeBin( 1, m_cSaoUvlcSCModel.get( 0, 0, 0 ) );
    for ( i=0; i<uiCode-1; i++ )
    {
      m_pcBinIf->encodeBin( 1, m_cSaoUvlcSCModel.get( 0, 0, 1 ) );
    }
    m_pcBinIf->encodeBin( 0, m_cSaoUvlcSCModel.get( 0, 0, 1 ) );
  }
}

Void TEncSbac::codeSaoSvlc       ( Int iCode )
{
  Int i;

  if ( iCode == 0 )
  {
    m_pcBinIf->encodeBin( 0, m_cSaoSvlcSCModel.get( 0, 0, 0 ) );
  }
  else
  {
    m_pcBinIf->encodeBin( 1, m_cSaoSvlcSCModel.get( 0, 0, 0 ) );

    // write sign
    if ( iCode > 0 )
    {
      m_pcBinIf->encodeBin( 0, m_cSaoSvlcSCModel.get( 0, 0, 1 ) );
    }
    else
    {
      m_pcBinIf->encodeBin( 1, m_cSaoSvlcSCModel.get( 0, 0, 1 ) );
      iCode = -iCode;
    }

    // write magnitude
    for ( i=0; i<iCode-1; i++ )
    {
      m_pcBinIf->encodeBin( 1, m_cSaoSvlcSCModel.get( 0, 0, 2 ) );
    }
    m_pcBinIf->encodeBin( 0, m_cSaoSvlcSCModel.get( 0, 0, 2 ) );
  }
}
/** Code SAO band position 
 * \param uiCode
 */
Void TEncSbac::codeSaoUflc       ( UInt uiCode )
{
  for (Int i=0;i<5;i++)
  {
    m_pcBinIf->encodeBinEP ( (uiCode>>i) &0x01 );
  }
}
/** Code SAO merge left flag 
 * \param uiCode
 * \param uiCompIdx
 */
Void TEncSbac::codeSaoMergeLeft       ( UInt uiCode, UInt uiCompIdx )
{
  if (uiCode == 0)
  {
    m_pcBinIf->encodeBin(0,  m_cSaoMergeLeftSCModel.get( 0, 0, uiCompIdx ));
  }
  else
  {
    m_pcBinIf->encodeBin(1,  m_cSaoMergeLeftSCModel.get( 0, 0, uiCompIdx ));
  }
}
/** Code SAO merge up flag 
 * \param uiCode
 */
Void TEncSbac::codeSaoMergeUp       ( UInt uiCode)
{
  if (uiCode == 0)
  {
    m_pcBinIf->encodeBin(0,  m_cSaoMergeUpSCModel.get( 0, 0, 0 ));
  }
  else
  {
    m_pcBinIf->encodeBin(1,  m_cSaoMergeUpSCModel.get( 0, 0, 0 ));
  }
}
/** Code SAO type index 
 * \param uiCode
 */
Void TEncSbac::codeSaoTypeIdx       ( UInt uiCode)
{
  Int i;
  if ( uiCode == 0 )
  {
    m_pcBinIf->encodeBin( 0, m_cSaoTypeIdxSCModel.get( 0, 0, 0 ) );
  }
  else
  {
    m_pcBinIf->encodeBin( 1, m_cSaoTypeIdxSCModel.get( 0, 0, 0 ) );
    for ( i=0; i<uiCode-1; i++ )
    {
      m_pcBinIf->encodeBin( 1, m_cSaoTypeIdxSCModel.get( 0, 0, 1 ) );
    }
    m_pcBinIf->encodeBin( 0, m_cSaoTypeIdxSCModel.get( 0, 0, 1 ) );
  }
}
/*!
 ****************************************************************************
 * \brief
 *   estimate bit cost for CBP, significant map and significant coefficients
 ****************************************************************************
 */
Void TEncSbac::estBit( estBitsSbacStruct* pcEstBitsSbac, Int width, Int height, TextType eTType )
{
  estCBFBit( pcEstBitsSbac, 0, eTType );

  estSignificantCoeffGroupMapBit( pcEstBitsSbac, 0, eTType );
  
  // encode significance map
  estSignificantMapBit( pcEstBitsSbac, width, height, eTType );
  
  // encode significant coefficients
  estSignificantCoefficientsBit( pcEstBitsSbac, 0, eTType );
}

/*!
 ****************************************************************************
 * \brief
 *    estimate bit cost for each CBP bit
 ****************************************************************************
 */
Void TEncSbac::estCBFBit( estBitsSbacStruct* pcEstBitsSbac, UInt uiCTXIdx, TextType eTType )
{
  ContextModel *pCtx = m_cCUQtCbfSCModel.get( 0 );

  for( UInt uiCtxInc = 0; uiCtxInc < 3*NUM_QT_CBF_CTX; uiCtxInc++ )
  {
    pcEstBitsSbac->blockCbpBits[ uiCtxInc ][ 0 ] = pCtx[ uiCtxInc ].getEntropyBits( 0 );
    pcEstBitsSbac->blockCbpBits[ uiCtxInc ][ 1 ] = pCtx[ uiCtxInc ].getEntropyBits( 1 );
  }

  pCtx = m_cCUQtRootCbfSCModel.get( 0 );
  
  for( UInt uiCtxInc = 0; uiCtxInc < 4; uiCtxInc++ )
  {
    pcEstBitsSbac->blockRootCbpBits[ uiCtxInc ][ 0 ] = pCtx[ uiCtxInc ].getEntropyBits( 0 );
    pcEstBitsSbac->blockRootCbpBits[ uiCtxInc ][ 1 ] = pCtx[ uiCtxInc ].getEntropyBits( 1 );
  }
}


/*!
 ****************************************************************************
 * \brief
 *    estimate SAMBAC bit cost for significant coefficient group map
 ****************************************************************************
 */
Void TEncSbac::estSignificantCoeffGroupMapBit( estBitsSbacStruct* pcEstBitsSbac, UInt uiCTXIdx, TextType eTType )
{
  Int firstCtx = 0, numCtx = NUM_SIG_CG_FLAG_CTX;

  for ( Int ctxIdx = firstCtx; ctxIdx < firstCtx + numCtx; ctxIdx++ )
  {
    for( UInt uiBin = 0; uiBin < 2; uiBin++ )
    {
      pcEstBitsSbac->significantCoeffGroupBits[ ctxIdx ][ uiBin ] = m_cCUSigCoeffGroupSCModel.get(  0, eTType, ctxIdx ).getEntropyBits( uiBin );
    }
  }
}


/*!
 ****************************************************************************
 * \brief
 *    estimate SAMBAC bit cost for significant coefficient map
 ****************************************************************************
 */
Void TEncSbac::estSignificantMapBit( estBitsSbacStruct* pcEstBitsSbac, Int width, Int height, TextType eTType )
{
#if UNIFIED_POS_SIG_CTX
  Int firstCtx = 1, numCtx = 8;
  if (std::max(width, height) >= 16)
  {
    firstCtx = 18;
    numCtx = (eTType == TEXT_LUMA) ? 6 : 3;
  }
  else if (width == 8)
  {
    firstCtx = 9;
    numCtx = 9;
  }
#else
  Int firstCtx = 0, numCtx = (eTType == TEXT_LUMA) ? 9 : 6;
  if (std::max(width, height) >= 16)
  {
    firstCtx = (eTType == TEXT_LUMA) ? 20 : 17;
    numCtx = (eTType == TEXT_LUMA) ? 7 : 4;    
  }
  else if (width == 8)
  {
    firstCtx = (eTType == TEXT_LUMA) ? 9 : 6;
    numCtx = 11;
  }
#endif
  
  if (eTType == TEXT_LUMA )
  {
#if UNIFIED_POS_SIG_CTX
    for( UInt bin = 0; bin < 2; bin++ )
    {
      pcEstBitsSbac->significantBits[ 0 ][ bin ] = m_cCUSigSCModel.get(  0, 0, 0 ).getEntropyBits( bin );
    }
#endif

    for ( Int ctxIdx = firstCtx; ctxIdx < firstCtx + numCtx; ctxIdx++ )
    {
      for( UInt uiBin = 0; uiBin < 2; uiBin++ )
      {
        pcEstBitsSbac->significantBits[ ctxIdx ][ uiBin ] = m_cCUSigSCModel.get(  0, 0, ctxIdx ).getEntropyBits( uiBin );
      }
    }
  }
  else
  {
#if UNIFIED_POS_SIG_CTX
    for( UInt bin = 0; bin < 2; bin++ )
    {
      pcEstBitsSbac->significantBits[ 0 ][ bin ] = m_cCUSigSCModel.get(  0, 0, NUM_SIG_FLAG_CTX_LUMA + 0 ).getEntropyBits( bin );
    }
#endif
    for ( Int ctxIdx = firstCtx; ctxIdx < firstCtx + numCtx; ctxIdx++ )
    {
      for( UInt uiBin = 0; uiBin < 2; uiBin++ )
      {
        pcEstBitsSbac->significantBits[ ctxIdx ][ uiBin ] = m_cCUSigSCModel.get(  0, 0, NUM_SIG_FLAG_CTX_LUMA + ctxIdx ).getEntropyBits( uiBin );
      }
    }
  }
  Int iBitsX = 0, iBitsY = 0;
  const UInt *puiCtxIdx;
  Int ctx;
  Int widthCtx = eTType? 4 : width;
  puiCtxIdx = g_uiLastCtx + (g_aucConvertToBit[ widthCtx ]*(g_aucConvertToBit[ widthCtx ]+3));
  ContextModel *pCtxX      = m_cCuCtxLastX.get( 0, eTType );
  for (ctx = 0; ctx < g_uiGroupIdx[ width - 1 ]; ctx++)
  {
    Int ctxOffset = puiCtxIdx[ ctx ];
    if (eTType)
    {
      Int ctxOffsetC =  ctx>>g_aucConvertToBit[ width ];
      pcEstBitsSbac->lastXBits[ ctx ] = iBitsX + pCtxX[ ctxOffsetC ].getEntropyBits( 0 );
      iBitsX += pCtxX[ ctxOffsetC].getEntropyBits( 1 );
    }
    else
    {
      pcEstBitsSbac->lastXBits[ ctx ] = iBitsX + pCtxX[ ctxOffset ].getEntropyBits( 0 );
      iBitsX += pCtxX[ ctxOffset ].getEntropyBits( 1 );
    }
    }
  pcEstBitsSbac->lastXBits[ctx] = iBitsX;

  Int heightCtx = eTType? 4 : height;
  puiCtxIdx = g_uiLastCtx + (g_aucConvertToBit[ heightCtx ]*(g_aucConvertToBit[ heightCtx ]+3));
  ContextModel *pCtxY      = m_cCuCtxLastY.get( 0, eTType );
  for (ctx = 0; ctx < g_uiGroupIdx[ height - 1 ]; ctx++)
  {
    Int ctxOffset = puiCtxIdx[ ctx ];
    if (eTType)
    {
      Int ctxOffsetC =  ctx>>g_aucConvertToBit[ height ];
      pcEstBitsSbac->lastYBits[ ctx ] = iBitsY + pCtxY[ ctxOffsetC ].getEntropyBits( 0 );
      iBitsY += pCtxY[ctxOffsetC].getEntropyBits( 1 );
    }
    else
    {
      pcEstBitsSbac->lastYBits[ ctx ] = iBitsY + pCtxY[ ctxOffset ].getEntropyBits( 0 );
      iBitsY += pCtxY[ ctxOffset ].getEntropyBits( 1 );
    }
    }
  pcEstBitsSbac->lastYBits[ctx] = iBitsY;
}

/*!
 ****************************************************************************
 * \brief
 *    estimate bit cost of significant coefficient
 ****************************************************************************
 */
Void TEncSbac::estSignificantCoefficientsBit( estBitsSbacStruct* pcEstBitsSbac, UInt uiCTXIdx, TextType eTType )
{
  if (eTType==TEXT_LUMA)
  {
    ContextModel *ctxOne = m_cCUOneSCModel.get(0, 0);
    ContextModel *ctxAbs = m_cCUAbsSCModel.get(0, 0);

    for (Int ctxIdx = 0; ctxIdx < NUM_ONE_FLAG_CTX_LUMA; ctxIdx++)
    {
      pcEstBitsSbac->m_greaterOneBits[ ctxIdx ][ 0 ] = ctxOne[ ctxIdx ].getEntropyBits( 0 );
      pcEstBitsSbac->m_greaterOneBits[ ctxIdx ][ 1 ] = ctxOne[ ctxIdx ].getEntropyBits( 1 );    
    }

    for (Int ctxIdx = 0; ctxIdx < NUM_ABS_FLAG_CTX_LUMA; ctxIdx++)
    {
      pcEstBitsSbac->m_levelAbsBits[ ctxIdx ][ 0 ] = ctxAbs[ ctxIdx ].getEntropyBits( 0 );
      pcEstBitsSbac->m_levelAbsBits[ ctxIdx ][ 1 ] = ctxAbs[ ctxIdx ].getEntropyBits( 1 );    
    }
  }
  else
  {
    ContextModel *ctxOne = m_cCUOneSCModel.get(0, 0) + NUM_ONE_FLAG_CTX_LUMA;
    ContextModel *ctxAbs = m_cCUAbsSCModel.get(0, 0) + NUM_ABS_FLAG_CTX_LUMA;

    for (Int ctxIdx = 0; ctxIdx < NUM_ONE_FLAG_CTX_CHROMA; ctxIdx++)
    {
      pcEstBitsSbac->m_greaterOneBits[ ctxIdx ][ 0 ] = ctxOne[ ctxIdx ].getEntropyBits( 0 );
      pcEstBitsSbac->m_greaterOneBits[ ctxIdx ][ 1 ] = ctxOne[ ctxIdx ].getEntropyBits( 1 );    
    }

    for (Int ctxIdx = 0; ctxIdx < NUM_ABS_FLAG_CTX_CHROMA; ctxIdx++)
    {
      pcEstBitsSbac->m_levelAbsBits[ ctxIdx ][ 0 ] = ctxAbs[ ctxIdx ].getEntropyBits( 0 );
      pcEstBitsSbac->m_levelAbsBits[ ctxIdx ][ 1 ] = ctxAbs[ ctxIdx ].getEntropyBits( 1 );    
    }
  }
}

/**
 - Initialize our context information from the nominated source.
 .
 \param pSrc From where to copy context information.
 */
Void TEncSbac::xCopyContextsFrom( TEncSbac* pSrc )
{  
  memcpy(m_contextModels, pSrc->m_contextModels, m_numContextModels*sizeof(m_contextModels[0]));
}

Void  TEncSbac::loadContexts ( TEncSbac* pScr)
{
  this->xCopyContextsFrom(pScr);
}
//! \}
