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

/** \file     Debug.cpp
    \brief    Defines types and objects for environment-variable-based debugging and feature control
*/

#include "Debug.h"
#include <algorithm>
#include <math.h>
#include "TComDataCU.h"
#include "TComPic.h"
#include "TComYuv.h"

static const UInt settingNameWidth  = 61;
static const UInt settingHelpWidth  = 84;
static const UInt settingValueWidth = 3;

// --------------------------------------------------------------------------------------------------------------------- //

//EnvVar definition

std::list<std::pair<std::string, std::string> > &EnvVar::getEnvVarList()
{
  static std::list<std::pair<std::string, std::string> > varInfoList;
  return varInfoList;
}

std::list<EnvVar*> &EnvVar::getEnvVarInUse()
{
  static std::list<EnvVar*> varInUseList;
  return varInUseList;
}

static inline Void printPair(const std::pair<std::string, std::string> &p)
{
  if (p.second=="")
  {
    std::cout << "\n" << std::setw(settingNameWidth) << p.first << "\n" << std::endl;
  }
  else
  {
    std::cout << std::setw(settingNameWidth) << p.first << ":   " << p.second << "\n" << std::endl;
  }
}

static inline Void printVal(const EnvVar* env)
{
  std::cout << std::setw(settingNameWidth) << env->getName() << " = " << std::setw(settingValueWidth) << env->getInt() << " (string = " << std::setw(15) << env->getString() << ")" << std::endl;
}

//static inline Bool sameEnvName( const std::pair<std::string, std::string> &a,
//                                const std::pair<std::string, std::string> &b )
//{
//  // only check env name
//  return (a.first==b.first);
//}

Void EnvVar::printEnvVar()
{
//  getEnvVarList().unique(sameEnvName);
  if (getEnvVarList().size()!=0)
  {
    std::cout << "--- Environment variables:\n" << std::endl;
    for_each(getEnvVarList().begin(), getEnvVarList().end(), printPair);
  }
  std::cout << std::endl;
}

Void EnvVar::printEnvVarInUse()
{
  if (getEnvVarInUse().size()!=0)
  {
    std::cout << "RExt Environment variables set as follows: \n" << std::endl;
    for_each(getEnvVarInUse().begin(), getEnvVarInUse().end(), printVal);
  }
  std::cout << std::endl;
}

EnvVar::EnvVar(const std::string &sName, const std::string &sDefault, const std::string &sHelp) :
                                                                m_sName(sName),
                                                                m_sHelp(sHelp),
                                                                m_sVal(),
                                                                m_dVal(0),
                                                                m_iVal(0),
                                                                m_bSet(false)
{
  if (getenv(m_sName.c_str()))
  {
    m_sVal = getenv(m_sName.c_str());
    m_bSet = true;
    getEnvVarInUse().push_back(this);
  }
  else m_sVal = sDefault;

  m_dVal = strtod(m_sVal.c_str(), 0);
  m_iVal = Int(m_dVal);

  getEnvVarList().push_back( std::pair<std::string, std::string>(m_sName, indentNewLines(lineWrap(splitOnSettings(m_sHelp), settingHelpWidth), (settingNameWidth + 4))) );
}


// --------------------------------------------------------------------------------------------------------------------- //

// Debug environment variables:

EnvVar Debug("-- Debugging","","");

EnvVar DebugOptionList::DebugSBAC           ("DEBUG_SBAC",              "0", "Output debug data from SBAC entropy coder (coefficient data etc.)");
EnvVar DebugOptionList::DebugRQT            ("DEBUG_RQT",               "0", "Output RQT debug data from entropy coder"                         );
EnvVar DebugOptionList::DebugPred           ("DEBUG_PRED",              "0", "Output prediction debug"                                          );
EnvVar DebugOptionList::ForceLumaMode       ("FORCE_LUMA_MODE",         "0", "Force a particular intra direction for Luma (0-34)"               );
EnvVar DebugOptionList::ForceChromaMode     ("FORCE_CHROMA_MODE",       "0", "Force a particular intra direction for chroma (0-5)"              );
EnvVar DebugOptionList::CopyLumaToChroma444 ("COPY_LUMA_TO_CHROMA_444", "0", "If using 444, copy luma channel to both chroma channels"          );
EnvVar DebugOptionList::SwapCbCrOnLoading   ("SWAP_CB_CR_ON_LOADING",   "0", "Swaps Cb and Cr channels on loading"                              );


// Tool setting environment variables:

EnvVar Tools("--     Tools","","");

EnvVar ToolOptionList::Chroma422IntraPlanarSingleStageCalculation       ("RExt__CHROMA_422_INTRA_PLANAR_SINGLE_STAGE_CALCULATION",         "0", "0 (default) = When generating planar intra prediction for a chroma 4:2:2 TU, use intermediate stages, 1 = combine all stages into a single calculation"                                                                                                                                                        );
EnvVar ToolOptionList::SetIntraChromaEdgeFilter422                      ("RExt__SET_INTRA_CHROMA_EDGE_FILTER_422",                         "0", "0 (default) = Disable intra edge filtering for chroma 4:2:2, 1 = Enable filtering in vertical direction only, 2 = Enable filtering in both horizontal and vertical directions"                                                                                                                                 );
EnvVar ToolOptionList::SetIntraChromaDCFilter422                        ("RExt__SET_INTRA_CHROMA_DC_FILTER_422",                           "0", "0 (default) = Disable intra DC filtering for chroma 4:2:2, 1 = Enable filtering in vertical direction only, 2 = Enable filtering in both horizontal and vertical directions"                                                                                                                                   );
EnvVar ToolOptionList::SetIntraChromaEdgeFilter444                      ("RExt__SET_INTRA_CHROMA_EDGE_FILTER_444",                         "0", "0 (default) = Disable intra edge filtering for chroma 4:4:4, 1 = Enable filtering in both horizontal and vertical directions"                                                                                                                                                                                  );
EnvVar ToolOptionList::SetIntraChromaDCFilter444                        ("RExt__SET_INTRA_CHROMA_DC_FILTER_444",                           "0", "0 (default) = Disable intra edge filtering for chroma 4:4:4, 1 = Enable filtering in both horizontal and vertical directions"                                                                                                                                                                                  );
EnvVar ToolOptionList::UseLumaFilterForChromaQuarterSampleInterpolation ("RExt__USE_LUMA_FILTER_FOR_CHROMA_QUARTER_SAMPLE_INTERPOLATION",  "0", "0 (default) = Use chroma filter for all chroma interpolation, 1 = Use luma filter wherever quarter-sample interpolation is required (4:2:2 vertical, 4:4:4 both directions)"                                                                                                                                   );
EnvVar ToolOptionList::EnableMDDTFor444Chroma                           ("RExt__ENABLE_MDDT_FOR_444_CHROMA",                               "0", "0 (default) = Use MDDT for luminance only in all formats, 1 = In 4:4:4, also allow MDDT for chrominance TUs"                                                                                                                                                                                                   );
EnvVar ToolOptionList::SingleTransformSkipFlagForAllChannels444         ("RExt__SINGLE_TRANSFORM_SKIP_FLAG_FOR_ALL_CHANNELS_444",          "0", "0 (default) = Always code a transform skip flag for each TU on each channel, 1 = In 4:4:4, code a transform skip flag only for luminance TUs, with corresponding chrominance TUs also using its value"                                                                                                         );
EnvVar ToolOptionList::Chroma422QuantiserAdjustment                     ("RExt__CHROMA_422_QUANTISER_ADJUSTMENT",                          "1", "0 = No quantiser modification for 4:2:2 TUs (shift in transform is rounded down), 1 (default) = Use rounded-down shift in transform and introduce an additional factor of sqrt(2) into the quantisation to normalise, 2 = Use rounded-up shift in transform and additional quantisation factor of 1/(sqrt(2))" );
EnvVar ToolOptionList::Chroma422QuantiserAdjustmentMethod               ("RExt__CHROMA_422_QUANTISER_ADJUSTMENT_METHOD",                   "1", "[NO EFFECT IF RExt__CHROMA_422_QUANTISER_ADJUSTMENT IS 0]  0 = Directly divide/multiply coefficients by sqrt(2), 1 (default) = Modify QP by +/- 3 to effect division/multiplication by sqrt(2), 2 = Modify QP_rem by +/- 3 and use extended 9-element quantisation coefficient tables"                         );
EnvVar ToolOptionList::AdditionalChromaQpMappingTables                  ("RExt__ADDITIONAL_CHROMA_QP_MAPPING_TABLES",                      "1", "0 = Use same g_aucChromaScale tables for mapping chroma QP as 4:2:0, 1 (default) = Use alternative tables for 4:2:2 and 4:4:4 that tend towards the behaviour of luma"                                                                                                                                         );
EnvVar ToolOptionList::UseTransformDepthFor444ChromaCBFContextSelection ("RExt__USE_TRANSFORM_DEPTH_FOR_444_CHROMA_CBF_CONTEXT_SELECTION", "1", "0 = 4:4:4 Chrominance CBFs use same method as luminance to select context variables, 1 (default) = 4:4:4 Chrominance CBFs use transform depth to select context variables (as in 4:2:0)"                                                                                                                       );
EnvVar ToolOptionList::Chroma422SignificanceMapContextGrid              ("RExt__CHROMA_422_SIGNIFICANCE_MAP_CONTEXT_GRID",                 "0", "[AFFECTS 4x8, 8x4, 8x16 and 16x8 TUs] 0 (default) = Use neighbourhood method for significance map context selection, 1 = Use position-repeated versions of the 4x4/8x8 context grids, 2 = As 1, but without re-using the DC context variable for 4x8/8x4"                                                      );
EnvVar ToolOptionList::PatternSigCtxMissingGroupsSameAsAvailableGroups  ("RExt__PATTERNSIGCTX_MISSING_GROUPS_SAME_AS_AVAILABLE_GROUPS",    "0", "0 (default) = When deriving patternSigCtx for significance map context selection, assume 0 for unavailable groups, 1 = If one neighbour group is available and the other is not, assume the same significance as the available group for both groups"                                                          );
EnvVar ToolOptionList::LumaMDCSMode                                     ("RExt__LUMA_MDCS_MODE",                                           "3", "0 = MDCS disabled for luminance, 1 = Horizontal scan only, 2 = Vertical scan only, 3 (default) = Full MDCS (horizontal and vertical scans enabled)"                                                                                                                                                            );
EnvVar ToolOptionList::LumaMDCSAngleLimit                               ("RExt__LUMA_MDCS_ANGLE_LIMIT",                                    "4", "(default 4) 0 = Horizontal/vertical only, 1 = Horizontal/vertical +/- 1, 2 = Horizontal/vertical +/- 2 etc..."                                                                                                                                                                                                 );
EnvVar ToolOptionList::LumaMDCSMaximumWidth                             ("RExt__LUMA_MDCS_MAXIMUM_WIDTH",                                  "8", "(default 8) Luminance TUs with width greater than this can only use diagonal scan"                                                                                                                                                                                                                             );
EnvVar ToolOptionList::LumaMDCSMaximumHeight                            ("RExt__LUMA_MDCS_MAXIMUM_HEIGHT",                                 "8", "(default 8) Luminance TUs with height greater than this can only use diagonal scan"                                                                                                                                                                                                                            );
EnvVar ToolOptionList::ChromaMDCSMode                                   ("RExt__CHROMA_MDCS_MODE",                                         "3", "0 = MDCS disabled for chrominance, 1 = Horizontal scan only, 2 = Vertical scan only, 3 (default) = Full MDCS (horizontal and vertical scans enabled)"                                                                                                                                                          );
EnvVar ToolOptionList::ChromaMDCSAngleLimit                             ("RExt__CHROMA_MDCS_ANGLE_LIMIT",                                  "4", "(default 4) 0 = Horizontal/vertical only, 1 = Horizontal/vertical +/- 1, 2 = Horizontal/vertical +/- 2 etc..."                                                                                                                                                                                                 );
EnvVar ToolOptionList::ChromaMDCSMaximumWidth                           ("RExt__CHROMA_MDCS_MAXIMUM_WIDTH",                                "4", "(default 4) Chrominance TUs with width greater than this can only use diagonal scan"                                                                                                                                                                                                                           );
EnvVar ToolOptionList::ChromaMDCSMaximumHeight                          ("RExt__CHROMA_MDCS_MAXIMUM_HEIGHT",                               "4", "(default 4) Chrominance TUs with height greater than this can only use diagonal scan"                                                                                                                                                                                                                          );
EnvVar ToolOptionList::NonSubsampledChromaUseLumaMDCSSizeLimits         ("RExt__NON_SUBSAMPLED_CHROMA_USE_LUMA_MDCS_SIZE_LIMITS",          "1", "0 = Always use chrominance size limits when determining if a chroma TU is too large to use MDCS, 1 (default) = Non-subsampled chrominance axes (vertical for 4:2:2, both for 4:4:4) use the luminance maximum width/height to determine if MDCS should be enabled"                                             );


// --------------------------------------------------------------------------------------------------------------------- //

//macro value printing function

Void printRExtMacroSettings()
{
  std::cout << "RExt Non-environment-variable-controlled macros set as follows: \n" << std::endl;

  //------------------------------------------------

  //setting macros

#if (RExt__ENVIRONMENT_VARIABLE_DEBUG_AND_TEST == 0)
  PRINT_CONSTANT(RExt__CHROMA_422_INTRA_PLANAR_SINGLE_STAGE_CALCULATION,         settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__SET_INTRA_CHROMA_EDGE_FILTER_422,                         settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__SET_INTRA_CHROMA_DC_FILTER_422,                           settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__SET_INTRA_CHROMA_EDGE_FILTER_444,                         settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__SET_INTRA_CHROMA_DC_FILTER_444,                           settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__USE_LUMA_FILTER_FOR_CHROMA_QUARTER_SAMPLE_INTERPOLATION,  settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__ENABLE_MDDT_FOR_444_CHROMA,                               settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__SINGLE_TRANSFORM_SKIP_FLAG_FOR_ALL_CHANNELS_444,          settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__CHROMA_422_QUANTISER_ADJUSTMENT,                          settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__CHROMA_422_QUANTISER_ADJUSTMENT_METHOD,                   settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__ADDITIONAL_CHROMA_QP_MAPPING_TABLES,                      settingNameWidth, settingValueWidth);
#endif
  PRINT_CONSTANT(RExt__INCREASE_NUMBER_OF_SCALING_LISTS_FOR_CHROMA,              settingNameWidth, settingValueWidth);
#if (RExt__ENVIRONMENT_VARIABLE_DEBUG_AND_TEST == 0)
  PRINT_CONSTANT(RExt__USE_TRANSFORM_DEPTH_FOR_444_CHROMA_CBF_CONTEXT_SELECTION, settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__CHROMA_422_SIGNIFICANCE_MAP_CONTEXT_GRID,                 settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__PATTERNSIGCTX_MISSING_GROUPS_SAME_AS_AVAILABLE_GROUPS,    settingNameWidth, settingValueWidth);
#endif
  //These settings cannot be defined using environment variables because they are used to set the size of static const arrays
  PRINT_CONSTANT(RExt__SIGNIFICANCE_MAP_CONTEXT_CHANNEL_SEPARATION,              settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__LAST_POSITION_CONTEXT_CHANNEL_SEPARATION,                 settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__C1_C2_CONTEXT_CHANNEL_SEPARATION,                         settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__CBF_CONTEXT_CHANNEL_SEPARATION,                           settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__EXTENDED_CHROMA_SIGNIFICANCE_MAP_CONTEXT,                 settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__EXTENDED_CHROMA_LAST_POSITION_CONTEXT,                    settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__EXTENDED_CHROMA_C1_C2_CONTEXT,                            settingNameWidth, settingValueWidth);
#if (RExt__ENVIRONMENT_VARIABLE_DEBUG_AND_TEST == 0)
  PRINT_CONSTANT(RExt__LUMA_MDCS_MODE,                                           settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__LUMA_MDCS_ANGLE_LIMIT,                                    settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__LUMA_MDCS_MAXIMUM_WIDTH,                                  settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__LUMA_MDCS_MAXIMUM_HEIGHT,                                 settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__CHROMA_MDCS_MODE,                                         settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__CHROMA_MDCS_ANGLE_LIMIT,                                  settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__CHROMA_MDCS_MAXIMUM_WIDTH,                                settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__CHROMA_MDCS_MAXIMUM_HEIGHT,                               settingNameWidth, settingValueWidth);
  PRINT_CONSTANT(RExt__NON_SUBSAMPLED_CHROMA_USE_LUMA_MDCS_SIZE_LIMITS,          settingNameWidth, settingValueWidth);
#endif

  std::cout << std::endl;

  //------------------------------------------------

  //derived macros

#ifdef RExt__EXTENDED_QP_TABLES
  std::cout << std::setw(settingNameWidth) << "RExt__EXTENDED_QP_TABLES" << " is defined" << std::endl;
#endif

#ifdef RExt__MULTIPLE_CHROMA_QP_MAPPING_TABLES
  std::cout << std::setw(settingNameWidth) << "RExt__MULTIPLE_CHROMA_QP_MAPPING_TABLES" << " is defined" << std::endl;
#endif

#ifdef RExt__CHROMA_SIGNIFICANCE_MAP_CONTEXT_SAME_AS_LUMA
  std::cout << std::setw(settingNameWidth) << "RExt__CHROMA_SIGNIFICANCE_MAP_CONTEXT_SAME_AS_LUMA" << " is defined" << std::endl;
#endif

#ifdef RExt__CHROMA_LAST_POSITION_CONTEXT_SAME_AS_LUMA
  std::cout << std::setw(settingNameWidth) << "RExt__CHROMA_LAST_POSITION_CONTEXT_SAME_AS_LUMA" << " is defined" << std::endl;
#endif

#ifdef RExt__CHROMA_C1_C2_CONTEXT_SAME_AS_LUMA
  std::cout << std::setw(settingNameWidth) << "RExt__CHROMA_C1_C2_CONTEXT_SAME_AS_LUMA" << " is defined" << std::endl;
#endif

  //------------------------------------------------

  std::cout << std::endl;
}


// --------------------------------------------------------------------------------------------------------------------- //

//Debugging

UInt  g_debugCounter  = 0;
Bool  g_printDebug    = false;
Void* g_debugAddr     = NULL;

Void printSBACCoeffData(  const UInt          lastX,
                          const UInt          lastY,
                          const UInt          width,
                          const UInt          height,
                          const UInt          chan,
                          const UInt          absPart,
                          const UInt          scanIdx,
                          const TCoeff *const pCoeff,
                          const Bool          finalEncode
                        )
{
  if (DebugOptionList::DebugSBAC.getInt()!=0 && finalEncode)
  {
    std::cout << "Size: " << width << "x" << height << ", Last X/Y: (" << lastX << ", " << lastY << "), absPartIdx: " << absPart << ", scanIdx: " << scanIdx << ", chan: " << chan << std::endl;
    for (Int i=0; i<width*height; i++)
    {
      std::cout << std::setw(3) << pCoeff[i];// + dcVal;
      if (i%width == width-1) std::cout << std::endl;
      else                    std::cout << ",";
    }
    std::cout << std::endl;
  }
}

Void printCbfArray( TComDataCU* pcCU  )
{
  const UInt CUSizeInParts = pcCU->getWidth(0)/4;
  const UInt numValidComp=pcCU->getPic()->getNumberValidComponents();
  for (UInt ch=0; ch<numValidComp; ch++)
  {
    const ComponentID compID=ComponentID(ch);
    printf("channel: %d\n", ch);
    for (Int y=0; y<CUSizeInParts; y++)
    {
      for (Int x=0; x<CUSizeInParts; x++)
      {
        printf(x+1==CUSizeInParts?"%3d\n":"%3d, ", pcCU->getCbf(compID)[g_auiRasterToZscan[y*CUSizeInParts + x]]);
      }
    }
  }
}

UInt getDecimalWidth(const Double value)
{
  return (value == 0) ? 1 : (UInt(floor(log10(fabs(value)))) + ((value < 0) ? 2 : 1));
                                                               //for the minus sign
}



// --------------------------------------------------------------------------------------------------------------------- //

//String manipulation functions for aligning and wrapping printed text


std::string splitOnSettings(const std::string &input)
{
  std::string result = input;

  std::string::size_type searchFromPosition = 0;

  while (searchFromPosition < result.length())
  {
    //find the " = " that is used to define each setting
    std::string::size_type equalsPosition = result.find(" = ", searchFromPosition);

    if (equalsPosition == std::string::npos) break;

    //then find the end of the numeric characters
    std::string::size_type splitPosition = result.find_last_of("1234567890", equalsPosition);

    //then find the last space before the first numeric character...
    if (splitPosition != std::string::npos) splitPosition = result.find_last_of(' ', splitPosition);

    //...and replace it with a new line
    if (splitPosition != std::string::npos) result.replace(splitPosition, 1, 1, '\n');

    //start the next search from the end of the " = " string
    searchFromPosition = (equalsPosition + 3);
  }

  return result;
}


std::string lineWrap(const std::string &input, const UInt maximumLineLength)
{
  if (maximumLineLength == 0) return input;
  std::string result = input;

  std::string::size_type lineStartPosition = result.find_first_not_of(' '); //don't wrap any leading spaces in the string

  while (lineStartPosition != std::string::npos)
  {
    //------------------------------------------------

    const std::string::size_type searchFromPosition = lineStartPosition + maximumLineLength;

    if (searchFromPosition >= result.length()) break;

    //------------------------------------------------

    //first check to see if there is another new line character before the maximum line length
    //we can't use find for this unfortunately because it doesn't take both a beginning and an end for its search range
    std::string::size_type nextLineStartPosition = std::string::npos;
    for (std::string::size_type currentPosition = lineStartPosition; currentPosition <= searchFromPosition; currentPosition++)
    {
      if (result[currentPosition] == '\n') { nextLineStartPosition = currentPosition + 1; break; }
    }

    //------------------------------------------------

    //if there ia another new line character before the maximum line length, we need to start this loop again from that position
    if (nextLineStartPosition != std::string::npos) lineStartPosition = nextLineStartPosition;
    else
    {
      std::string::size_type spacePosition = std::string::npos;
    
      //search backwards for the last space character (must use signed int because lineStartPosition can be 0)
      for (Int currentPosition = Int(searchFromPosition); currentPosition >= Int(lineStartPosition); currentPosition--)
      {
        if (result[currentPosition] == ' ') { spacePosition = currentPosition; break; }
      }

      //if we didn't find a space searching backwards, we must hyphenate
      if (spacePosition == std::string::npos)
      {
        result.insert(searchFromPosition, "-\n");
        lineStartPosition = searchFromPosition + 2; //make sure the next search ignores the hyphen
      }
      else //if we found a space to split on, replace it with a new line character
      {
        result.replace(spacePosition, 1, 1, '\n');
        lineStartPosition = spacePosition + 1;
      }
    }

    //------------------------------------------------
  }

  return result;
}


std::string indentNewLines(const std::string &input, const UInt indentBy)
{
  std::string result = input;

  const std::string indentString(indentBy, ' ');
  std::string::size_type offset = 0;

  while ((offset = result.find('\n', offset)) != std::string::npos)
  {
    if ((++offset) >= result.length()) break; //increment offset so we don't find the same \n again and do no indentation at the end
    result.insert(offset, indentString);
  }
        
  return result;
}


// --------------------------------------------------------------------------------------------------------------------- //


Void printBlockToStream( std::ostream &ss, const char *pLinePrefix, TComYuv &src, const UInt numSubBlocksAcross, const UInt numSubBlocksUp, const UInt defWidth )
{
  const UInt numValidComp=src.getNumberValidComponents();

  for (UInt ch=0; ch<numValidComp ; ch++)
  {
    const ComponentID compID = ComponentID(ch);
    const UInt width  = src.getWidth(compID);
    const UInt height = src.getHeight(compID);
    const UInt stride = src.getStride(compID);
    const Pel* blkSrc = src.getAddr(compID);
    const UInt subBlockWidth=width/numSubBlocksAcross;
    const UInt subBlockHeight=height/numSubBlocksUp;

    ss << pLinePrefix << " compID: " << compID << "\n";
    for (UInt y=0; y<height; y++)
    {
      if ((y%subBlockHeight)==0 && y!=0)
        ss << pLinePrefix << '\n';

      ss << pLinePrefix;
      for (UInt x=0; x<width; x++)
      {
        if ((x%subBlockWidth)==0 && x!=0)
          ss << std::setw(defWidth+2) << "";

        ss << std::setw(defWidth) << blkSrc[y*stride + x] << ' ';
      }
      ss << '\n';
    }
    ss << pLinePrefix << " --- \n";
  }
}
