// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2016, Knut Reinert, FU Berlin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================
// Author: Manuel Holtgrewe <manuel.holtgrewe@fu-berlin.de>
// ==========================================================================
// Global alignment interface for the banded Needleman-Wunsch and Gotoh
// algorithms.
//
// We define the interface functions pretty explicitely (versus just TAlign,
// TFragments etc.) so the candidates the compiler gives when resolution to
// the globalFunction() fails is actually meaningful.
// ==========================================================================

#ifndef SEQAN_INCLUDE_SEQAN_ALIGN_GLOBAL_ALIGNMENT_BANDED_H_
#define SEQAN_INCLUDE_SEQAN_ALIGN_GLOBAL_ALIGNMENT_BANDED_H_

namespace seqan {

// ============================================================================
// Forwards
// ============================================================================

template <typename TScoreValue, typename TSpec>
class Score;
template <typename TSpec>
class Graph;
template <typename TStringSet, typename TCargo, typename TGraphSpec>
struct Alignment;
template <typename TSize, typename TFragmentSpec>
class Fragment;

// ============================================================================
// Tags, Classes, Enums
// ============================================================================

// ============================================================================
// Metafunctions
// ============================================================================

// ============================================================================
// Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function globalAlignment()                                   [banded, Align]
// ----------------------------------------------------------------------------

template <typename TSequence, typename TAlignSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec,
          typename TAlgoTag>
TScoreValue globalAlignment(Align<TSequence, TAlignSpec> & align,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & /*alignConfig*/,
                            int lowerDiag,
                            int upperDiag,
                            TAlgoTag const & /*algoTag*/)
{
    typedef Align<TSequence, TAlignSpec> TAlign;
    typedef typename Size<TAlign>::Type  TSize;
    typedef typename Position<TAlign>::Type TPosition;
    typedef TraceSegment_<TPosition, TSize> TTraceSegment;
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;

    String<TTraceSegment> trace;

    DPScoutState_<Default> dpScoutState;
    TScoreValue res = _setUpAndRunAlignment(trace, dpScoutState, source(row(align, 0)), source(row(align, 1)),
                                            scoringScheme, TAlignConfig2(lowerDiag, upperDiag), TGapModel());

    _adaptTraceSegmentsTo(row(align, 0), row(align, 1), trace);
    return res;
}

// Interface without AlignConfig<>.
template <typename TSequence, typename TAlignSpec,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
TScoreValue globalAlignment(Align<TSequence, TAlignSpec> & align,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            int lowerDiag,
                            int upperDiag,
                            TAlgoTag const & algoTag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(align, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TSequence, typename TAlignSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
TScoreValue globalAlignment(Align<TSequence, TAlignSpec> & align,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                            int lowerDiag,
                            int upperDiag)
{
    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignment(align, scoringScheme, alignConfig, lowerDiag, upperDiag, LinearGaps());
    else
        return globalAlignment(align, scoringScheme, alignConfig, lowerDiag, upperDiag, AffineGaps());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TSequence, typename TAlignSpec,
          typename TScoreValue, typename TScoreSpec>
TScoreValue globalAlignment(Align<TSequence, TAlignSpec> & align,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            int lowerDiag,
                            int upperDiag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(align, scoringScheme, alignConfig, lowerDiag, upperDiag);
}

// ----------------------------------------------------------------------------
// Function globalAlignment()                                    [banded, Gaps]
// ----------------------------------------------------------------------------

template <typename TSequenceH, typename TGapsSpecH,
          typename TSequenceV, typename TGapsSpecV,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec,
          typename TAlgoTag>
TScoreValue globalAlignment(Gaps<TSequenceH, TGapsSpecH> & gapsH,
                            Gaps<TSequenceV, TGapsSpecV> & gapsV,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & /*alignConfig*/,
                            int lowerDiag,
                            int upperDiag,
                            TAlgoTag const & /*algoTag*/)
{
    typedef typename Size<TSequenceH>::Type TSize;
    typedef typename Position<TSequenceH>::Type TPosition;
    typedef TraceSegment_<TPosition, TSize> TTraceSegment;
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;

    String<TTraceSegment> trace;

    DPScoutState_<Default> dpScoutState;
    TScoreValue res = _setUpAndRunAlignment(trace, dpScoutState, source(gapsH), source(gapsV), scoringScheme,
                                            TAlignConfig2(lowerDiag, upperDiag), TGapModel());
    _adaptTraceSegmentsTo(gapsH, gapsV, trace);
    return res;
}

// Interface without AlignConfig<>.
template <typename TSequenceH, typename TGapsSpecH,
          typename TSequenceV, typename TGapsSpecV,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
TScoreValue globalAlignment(Gaps<TSequenceH, TGapsSpecH> & gapsH,
                            Gaps<TSequenceV, TGapsSpecV> & gapsV,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            int lowerDiag,
                            int upperDiag,
                            TAlgoTag const & algoTag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(gapsH, gapsV, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TSequenceH, typename TGapsSpecH,
          typename TSequenceV, typename TGapsSpecV,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
TScoreValue globalAlignment(Gaps<TSequenceH, TGapsSpecH> & gapsH,
                            Gaps<TSequenceV, TGapsSpecV> & gapsV,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                            int lowerDiag,
                            int upperDiag)
{
    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignment(gapsH, gapsV, scoringScheme, alignConfig, lowerDiag, upperDiag, NeedlemanWunsch());
    else
        return globalAlignment(gapsH, gapsV, scoringScheme, alignConfig, lowerDiag, upperDiag, Gotoh());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TSequenceH, typename TGapsSpecH,
          typename TSequenceV, typename TGapsSpecV,
          typename TScoreValue, typename TScoreSpec>
TScoreValue globalAlignment(Gaps<TSequenceH, TGapsSpecH> & gapsH,
                            Gaps<TSequenceV, TGapsSpecV> & gapsV,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            int lowerDiag,
                            int upperDiag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(gapsH, gapsV, scoringScheme, alignConfig, lowerDiag, upperDiag);
}

// ----------------------------------------------------------------------------
// Function globalAlignment()                     [banded, Graph<Alignment<> >]
// ----------------------------------------------------------------------------

// Full interface.
template <typename TStringSet, typename TCargo, typename TGraphSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec,
          typename TAlgoTag>
TScoreValue globalAlignment(Graph<Alignment<TStringSet, TCargo, TGraphSpec> > & alignmentGraph,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & /*alignConfig*/,
                            int lowerDiag,
                            int upperDiag,
                            TAlgoTag const & /*algoTag*/)
{
    typedef Graph<Alignment<TStringSet, TCargo, TGraphSpec> > TGraph;
    typedef typename Position<TGraph>::Type TPosition;
    typedef typename Size<TGraph>::Type TSize;
    typedef TraceSegment_<TPosition, TSize> TTraceSegment;
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;

    String<TTraceSegment> trace;

    DPScoutState_<Default> dpScoutState;
    TScoreValue res = _setUpAndRunAlignment(trace, dpScoutState, value(stringSet(alignmentGraph), 0),
                                            value(stringSet(alignmentGraph), 1), scoringScheme,
                                            TAlignConfig2(lowerDiag, upperDiag), TGapModel());

    _adaptTraceSegmentsTo(alignmentGraph, positionToId(stringSet(alignmentGraph), 0),
                          positionToId(stringSet(alignmentGraph), 1), trace);
    return res;
}

// Interface without AlignConfig<>.
template <typename TStringSet, typename TCargo, typename TGraphSpec,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
TScoreValue globalAlignment(Graph<Alignment<TStringSet, TCargo, TGraphSpec> > & alignmentGraph,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            int lowerDiag,
                            int upperDiag,
                            TAlgoTag const & algoTag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(alignmentGraph, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TStringSet, typename TCargo, typename TGraphSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
TScoreValue globalAlignment(Graph<Alignment<TStringSet, TCargo, TGraphSpec> > & alignmentGraph,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                            int lowerDiag,
                            int upperDiag)
{
    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignment(alignmentGraph, scoringScheme, alignConfig, lowerDiag, upperDiag, NeedlemanWunsch());
    else
        return globalAlignment(alignmentGraph, scoringScheme, alignConfig, lowerDiag, upperDiag, Gotoh());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TStringSet, typename TCargo, typename TGraphSpec,
          typename TScoreValue, typename TScoreSpec>
TScoreValue globalAlignment(Graph<Alignment<TStringSet, TCargo, TGraphSpec> > & alignmentGraph,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            int lowerDiag,
                            int upperDiag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(alignmentGraph, scoringScheme, alignConfig, lowerDiag, upperDiag);
}

// ----------------------------------------------------------------------------
// Function globalAlignment()                     [banded, String<Fragment<> >]
// ----------------------------------------------------------------------------

// Full interface.
template <typename TSize, typename TFragmentSpec, typename TStringSpec,
          typename TSequence, typename TStringSetSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec,
          typename TAlgoTag>
TScoreValue globalAlignment(String<Fragment<TSize, TFragmentSpec>, TStringSpec> & fragmentString,
                            StringSet<TSequence, TStringSetSpec> const & strings,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & /*alignConfig*/,
                            int lowerDiag,
                            int upperDiag,
                            TAlgoTag const & /*algoTag*/)
{
    typedef String<Fragment<TSize, TFragmentSpec>, TStringSpec> TFragments;
    typedef typename Position<TFragments>::Type TPosition;
    typedef TraceSegment_<TPosition, TSize> TTraceSegment;
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;

    String<TTraceSegment> trace;

    DPScoutState_<Default> dpScoutState;
    TScoreValue res = _setUpAndRunAlignment(trace, dpScoutState, value(strings, 0), value(strings, 1), scoringScheme,
                                            TAlignConfig2(lowerDiag, upperDiag), TGapModel());

    _adaptTraceSegmentsTo(fragmentString, positionToId(strings, 0), positionToId(strings, 1), trace);
    return res;
}

// Interface without AlignConfig<>.
template <typename TSize, typename TFragmentSpec, typename TStringSpec,
          typename TSequence, typename TStringSetSpec,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
TScoreValue globalAlignment(String<Fragment<TSize, TFragmentSpec>, TStringSpec> & fragmentString,
                            StringSet<TSequence, TStringSetSpec> const & strings,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            int lowerDiag,
                            int upperDiag,
                            TAlgoTag const & algoTag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(fragmentString, strings, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TSize, typename TFragmentSpec, typename TStringSpec,
          typename TSequence, typename TStringSetSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
TScoreValue globalAlignment(String<Fragment<TSize, TFragmentSpec>, TStringSpec> & fragmentString,
                            StringSet<TSequence, TStringSetSpec> const & strings,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                            int lowerDiag,
                            int upperDiag)
{
    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignment(fragmentString, strings, scoringScheme, alignConfig, lowerDiag, upperDiag, NeedlemanWunsch());
    else
        return globalAlignment(fragmentString, strings, scoringScheme, alignConfig, lowerDiag, upperDiag, Gotoh());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TSize, typename TFragmentSpec, typename TStringSpec,
          typename TSequence, typename TStringSetSpec,
          typename TScoreValue, typename TScoreSpec>
TScoreValue globalAlignment(String<Fragment<TSize, TFragmentSpec>, TStringSpec> & fragmentString,
                            StringSet<TSequence, TStringSetSpec> const & strings,
                            Score<TScoreValue, TScoreSpec> const & scoringScheme,
                            int lowerDiag,
                            int upperDiag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(fragmentString, strings, scoringScheme, alignConfig, lowerDiag, upperDiag);
}

// ----------------------------------------------------------------------------
// Function globalAlignmentScore()                        [banded, 2 Strings]
// ----------------------------------------------------------------------------

template <typename TSequenceH,
          typename TSequenceV,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec,
          typename TAlgoTag>
TScoreValue globalAlignmentScore(TSequenceH const & seqH,
                                 TSequenceV const & seqV,
                                 Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                 AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & /*alignConfig*/,
                                 int lowerDiag,
                                 int upperDiag,
                                 TAlgoTag const & /*algoTag*/)
{
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps, TracebackOff> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;

    DPScoutState_<Default> dpScoutState;
    String<TraceSegment_<unsigned, unsigned> > traceSegments;  // Dummy segments.
    return _setUpAndRunAlignment(traceSegments, dpScoutState, seqH, seqV, scoringScheme,
                                 TAlignConfig2(lowerDiag, upperDiag), TGapModel());
}

// Interface without AlignConfig<>.
template <typename TSequenceH,
          typename TSequenceV,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
TScoreValue globalAlignmentScore(TSequenceH const & seqH,
                                 TSequenceV const & seqV,
                                 Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                 int lowerDiag,
                                 int upperDiag,
                                 TAlgoTag const & algoTag)
{
    AlignConfig<> alignConfig;
    return globalAlignmentScore(seqH, seqV, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TSequenceH,
          typename TSequenceV,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
TScoreValue globalAlignmentScore(TSequenceH const & seqH,
                                 TSequenceV const & seqV,
                                 Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                 AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                                 int lowerDiag,
                                 int upperDiag)
{
    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignmentScore(seqH, seqV, scoringScheme, alignConfig, lowerDiag, upperDiag, NeedlemanWunsch());
    else
        return globalAlignmentScore(seqH, seqV, scoringScheme, alignConfig, lowerDiag, upperDiag, Gotoh());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TSequenceH,
          typename TSequenceV,
          typename TScoreValue, typename TScoreSpec>
TScoreValue globalAlignmentScore(TSequenceH const & seqH,
                                 TSequenceV const & seqV,
                                 Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                 int lowerDiag,
                                 int upperDiag)
{
    AlignConfig<> alignConfig;
    return globalAlignmentScore(seqH, seqV, scoringScheme, alignConfig, lowerDiag, upperDiag);
}

// ----------------------------------------------------------------------------
// Function globalAlignmentScore()                        [banded, StringSet]
// ----------------------------------------------------------------------------

template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec,
          typename TAlgoTag>
TScoreValue globalAlignmentScore(StringSet<TString, TSpec> const & strings,
                                 Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                 AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & /*alignConfig*/,
                                 int lowerDiag,
                                 int upperDiag,
                                 TAlgoTag const & /*algoTag*/)
{
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps, TracebackOff> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;

    SEQAN_ASSERT_EQ(length(strings), 2u);

    DPScoutState_<Default> dpScoutState;
    String<TraceSegment_<unsigned, unsigned> > traceSegments;  // Dummy segments.
    return _setUpAndRunAlignment(traceSegments, dpScoutState, strings[0], strings[1], scoringScheme,
                                 TAlignConfig2(lowerDiag, upperDiag), TGapModel());
}

// Interface without AlignConfig<>.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
TScoreValue globalAlignmentScore(StringSet<TString, TSpec> const & strings,
                                 Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                 int lowerDiag,
                                 int upperDiag,
                                 TAlgoTag const & algoTag)
{
    SEQAN_ASSERT_EQ(length(strings), 2u);

    AlignConfig<> alignConfig;
    return globalAlignmentScore(strings[0], strings[1], scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
TScoreValue globalAlignmentScore(StringSet<TString, TSpec> const & strings,
                                 Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                 AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                                 int lowerDiag,
                                 int upperDiag)
{
    SEQAN_ASSERT_EQ(length(strings), 2u);

    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignmentScore(strings[0], strings[1], scoringScheme, alignConfig, lowerDiag, upperDiag, NeedlemanWunsch());
    else
        return globalAlignmentScore(strings[0], strings[1], scoringScheme, alignConfig, lowerDiag, upperDiag, Gotoh());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec>
TScoreValue globalAlignmentScore(StringSet<TString, TSpec> const & strings,
                                 Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                 int lowerDiag,
                                 int upperDiag)
{
    SEQAN_ASSERT_EQ(length(strings), 2u);

    AlignConfig<> alignConfig;
    return globalAlignmentScore(strings[0], strings[1], scoringScheme, alignConfig, lowerDiag, upperDiag);
}

#if SEQAN_SIMD_ENABLED
// ----------------------------------------------------------------------------
// Function globalAlignmentScore()         [banded, SIMD version, 2x StringSet]
// ----------------------------------------------------------------------------

template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec,
          typename TAlgoTag>
String<TScoreValue> globalAlignmentScore(StringSet<TString, TSpec> const & stringsH,
                                         StringSet<TString, TSpec> const & stringsV,
                                         Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                         AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                                         int lowerDiag,
                                         int upperDiag,
                                         TAlgoTag const & /*algoTag*/)
{
    SEQAN_ASSERT_EQ(length(stringsH), length(stringsV));
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps, TracebackOff> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;
    typedef typename SimdVector<int16_t>::Type TSimdAlign;

    auto const numAlignments = length(stringsV);
    unsigned const sizeBatch = LENGTH<TSimdAlign>::VALUE;

    String<TScoreValue> results;
    resize(results, numAlignments);

    StringSet<String<Nothing> > trace;  // We need to declare it, but it will not be used.

    // Create a SIMD scoring scheme.
    Score<TSimdAlign, ScoreSimdWrapper<Score<TScoreValue, TScoreSpec> > > simdScoringScheme(scoringScheme);

    for (auto pos = 0u; pos < numAlignments / sizeBatch; ++pos)
    {
        auto infSetH = infixWithLength(stringsH, pos * sizeBatch, sizeBatch);
        auto infSetV = infixWithLength(stringsV, pos * sizeBatch, sizeBatch);

        TSimdAlign resultsBatch;
        _prepareAndRunSimdAlignment(resultsBatch, trace, infSetH, infSetV, simdScoringScheme,
                                    TAlignConfig2(lowerDiag, upperDiag), TGapModel());

        // TODO(rrahn): Could be parallelized!
        for(auto x = pos * sizeBatch; x < (pos + 1) * sizeBatch; ++x)
            results[x] = resultsBatch[x - pos * sizeBatch];
    }

    //call the normal non-simd function for remaining alignments
    for(size_t pos = (numAlignments/sizeBatch)*sizeBatch; pos < numAlignments; ++pos)
        results[pos] = globalAlignmentScore(stringsH[pos], stringsV[pos], scoringScheme, alignConfig,
                                            lowerDiag, upperDiag, TAlgoTag());
    return results;


//    SEQAN_ASSERT_EQ(length(stringsH), length(stringsV));
//    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
//    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
//    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps, TracebackOff> TAlignConfig2;
//    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;
//
//    typedef typename SimdVector<int16_t>::Type TSimdAlign;
//
//    auto const numAlignments = length(stringsV);
//    auto const sizeBatch = LENGTH<TSimdAlign>::VALUE;
//
//    String<TScoreValue> results;
//    resize(results, numAlignments);
//
//    for (auto pos = 0u; pos < numAlignments / sizeBatch; ++pos)
//    {
//        auto infSetH = infixWithLength(stringsH.strings, pos * sizeBatch, sizeBatch);
//        auto infSetV = infixWithLength(stringsV.strings, pos * sizeBatch, sizeBatch);
//
//        TSimdAlign resultsBatch;
//        _prepareAndRunSimdAlignment(resultsBatch, infSetH, infSetV, scoringScheme,
//                                    TAlignConfig2(lowerDiag, upperDiag), TGapModel());
//
//        // TODO(rrahn): Could be parallelized!
//        for(auto x = pos * sizeBatch; x < (pos + 1) * sizeBatch; ++x)
//            results[x] = resultsBatch[x - pos * sizeBatch];
//    }
//
//    //call the normal non-simd function for remaining alignments
//    for(size_t pos = (numAlignments/sizeBatch)*sizeBatch; pos < numAlignments; ++pos)
//        results[pos] = globalAlignmentScore(stringsH[pos], stringsV[pos], scoringScheme, alignConfig, lowerDiag,
//                                            upperDiag, TAlgoTag());
//    return results;
}

// Interface without AlignConfig<>.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
String<TScoreValue> globalAlignmentScore(StringSet<TString, TSpec> const & stringsH,
                                         StringSet<TString, TSpec> const & stringsV,
                                         Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                         int lowerDiag,
                                         int upperDiag,
                                         TAlgoTag const & algoTag)
{
    AlignConfig<> alignConfig;
    return globalAlignmentScore(stringsH, stringsV, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
String<TScoreValue> globalAlignmentScore(StringSet<TString, TSpec> const & stringsH,
                                         StringSet<TString, TSpec> const & stringsV,
                                         Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                         AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                                         int lowerDiag,
                                         int upperDiag)
{
    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignmentScore(stringsH, stringsV, scoringScheme, alignConfig, lowerDiag, upperDiag, NeedlemanWunsch());
    else
        return globalAlignmentScore(stringsH, stringsV, scoringScheme, alignConfig, lowerDiag, upperDiag, Gotoh());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec>
String<TScoreValue> globalAlignmentScore(StringSet<TString, TSpec> const & stringsH,
                                         StringSet<TString, TSpec> const & stringsV,
                                         Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                         int lowerDiag,
                                         int upperDiag)
{
    AlignConfig<> alignConfig;
    return globalAlignmentScore(stringsH, stringsV, scoringScheme, alignConfig, lowerDiag, upperDiag);
}

// ----------------------------------------------------------------------------
// Function globalAlignmentScore()   [banded, SIMD version, String vs StringSet]
// ----------------------------------------------------------------------------

template <typename TStringH,
          typename TStringV, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec,
          typename TAlgoTag>
String<TScoreValue> globalAlignmentScore(TStringH const & stringH,
                                         StringSet<TStringV, TSpec> const & stringsV,
                                         Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                         AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                                         int lowerDiag,
                                         int upperDiag,
                                         TAlgoTag const & /*algoTag*/)
{
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps, TracebackOff> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;
    typedef typename SimdVector<int16_t>::Type TSimdAlign;

    auto const numAlignments = length(stringsV);
    unsigned const sizeBatch = LENGTH<TSimdAlign>::VALUE;

    String<TScoreValue> results;
    resize(results, numAlignments);

    StringSet<TStringH, Dependent<> > setH;
    for (auto i = 0u; i < sizeBatch; ++i)
        appendValue(setH, stringH);

    StringSet<String<Nothing> > trace;  // We need to declare it, but it will not be used.

    // Create a SIMD scoring scheme.
    Score<TSimdAlign, ScoreSimdWrapper<Score<TScoreValue, TScoreSpec> > > simdScoringScheme(scoringScheme);

    for (auto pos = 0u; pos < numAlignments / sizeBatch; ++pos)
    {
        auto infSetV = infixWithLength(stringsV, pos * sizeBatch, sizeBatch);

        TSimdAlign resultsBatch;
        _prepareAndRunSimdAlignment(resultsBatch, trace, setH, infSetV, simdScoringScheme,
                                    TAlignConfig2(lowerDiag, upperDiag), TGapModel());

        // TODO(rrahn): Could be parallelized!
        for(auto x = pos * sizeBatch; x < (pos + 1) * sizeBatch; ++x)
            results[x] = resultsBatch[x - pos * sizeBatch];
    }

    //call the normal non-simd function for remaining alignments
    for(size_t pos = (numAlignments / sizeBatch) * sizeBatch; pos < numAlignments; ++pos)
        results[pos] = globalAlignmentScore(stringH, stringsV[pos], scoringScheme, alignConfig,
                                            lowerDiag, upperDiag, TAlgoTag());
    return results;

//    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> TAlignConfig;
//    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type TFreeEndGaps;
//    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps, TracebackOff> TAlignConfig2;
//    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type TGapModel;
//    typedef typename SimdVector<int16_t>::Type TSimdAlign;
//
//    auto const numAlignments = length(stringsV);
//    auto const sizeBatch = LENGTH<TSimdAlign>::VALUE;
//
//    String<TScoreValue> results;
//    resize(results, numAlignments);
//
//    StringSet<TString1, Dependent<> > setH;
//    for (auto i = 0u; i < sizeBatch; ++i)
//        appendValue(setH, stringH);
//
//    for (auto pos = 0u; pos < numAlignments / sizeBatch; ++pos)
//    {
//        auto infSetV = infixWithLength(stringsV.strings, pos * sizeBatch, sizeBatch);
//
//        TSimdAlign resultsBatch;
//        _prepareAndRunSimdAlignment(resultsBatch, setH, infSetV, scoringScheme, TAlignConfig2(), TGapModel());
//
//        // TODO(rrahn): Could be parallelized!
//        for(auto x = pos * sizeBatch; x < (pos + 1) * sizeBatch; ++x)
//            results[x] = resultsBatch[x - pos * sizeBatch];
//    }
//
//    //call the normal non-simd function for remaining alignments
//    for(size_t pos = (numAlignments / sizeBatch) * sizeBatch; pos < numAlignments; ++pos)
//        results[pos] = globalAlignmentScore(stringH, stringsV[pos], scoringScheme, alignConfig, lowerDiag, upperDiag,
//                                            TAlgoTag());
//    return results;
}

// Interface without AlignConfig<>.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
String<TScoreValue> globalAlignmentScore(TString const & stringH,
                                         StringSet<TString, TSpec> const & stringsV,
                                         Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                         int lowerDiag,
                                         int upperDiag,
                                         TAlgoTag const & algoTag)
{
    AlignConfig<> alignConfig;
    return globalAlignmentScore(stringH, stringsV, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
String<TScoreValue> globalAlignmentScore(TString const & stringH,
                                         StringSet<TString, TSpec> const & stringsV,
                                         Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                         AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                                         int lowerDiag,
                                         int upperDiag)
{
    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignmentScore(stringH, stringsV, scoringScheme, alignConfig, lowerDiag, upperDiag, NeedlemanWunsch());
    else
        return globalAlignmentScore(stringH, stringsV, scoringScheme, alignConfig, lowerDiag, upperDiag, Gotoh());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TString, typename TSpec,
          typename TScoreValue, typename TScoreSpec>
String<TScoreValue> globalAlignmentScore(TString const & stringH,
                                         StringSet<TString, TSpec> const & stringsV,
                                         Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                         int lowerDiag,
                                         int upperDiag)
{
    AlignConfig<> alignConfig;
    return globalAlignmentScore(stringH, stringsV, scoringScheme, alignConfig, lowerDiag, upperDiag);
}

// ----------------------------------------------------------------------------
// Function globalAlignment()              [banded, SIMD version, GapsH, GapsV]
// ----------------------------------------------------------------------------

template <typename TGapSequenceH, typename TSetSpecH,
typename TGapSequenceV, typename TSetSpecV,
typename TScoreValue, typename TScoreSpec,
bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec, typename TAlgoTag>
inline auto
globalAlignment(StringSet<TGapSequenceH, TSetSpecH> & gapSeqSetH,
                StringSet<TGapSequenceV, TSetSpecV> & gapSeqSetV,
                Score<TScoreValue, TScoreSpec> const & scoringScheme,
                AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                int const lowerDiag,
                int const upperDiag,
                TAlgoTag const & algoTag)
{
    typedef AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec>              TAlignConfig;
    typedef typename SubstituteAlignConfig_<TAlignConfig>::Type         TFreeEndGaps;
    typedef AlignConfig2<DPGlobal, DPBandConfig<BandOn>, TFreeEndGaps> TAlignConfig2;
    typedef typename SubstituteAlgoTag_<TAlgoTag>::Type                 TGapModel;

    typedef typename Size<TGapSequenceH>::Type                          TSize;
    typedef typename Position<TGapSequenceH>::Type                      TPosition;
    typedef TraceSegment_<TPosition, TSize>                             TTraceSegment;

    typedef typename SimdVector<int16_t>::Type                          TSimdAlign;

    auto const numAlignments = length(gapSeqSetH);
    unsigned const sizeBatch = LENGTH<TSimdAlign>::VALUE;

    String<TScoreValue> results;
    resize(results, numAlignments);

    StringSet<String<TTraceSegment> > trace;
    resize(trace, sizeBatch);

    // Create a SIMD scoring scheme.
    Score<TSimdAlign, ScoreSimdWrapper<Score<TScoreValue, TScoreSpec> > > simdScoringScheme(scoringScheme);

    // Prepare string sets with sequences.
    StringSet<typename Source<TGapSequenceH>::Type, Dependent<> > depSetH;
    StringSet<typename Source<TGapSequenceV>::Type, Dependent<> > depSetV;
    reserve(depSetH, numAlignments);
    reserve(depSetV, numAlignments);
    auto zipCont = makeZipView(gapSeqSetH, gapSeqSetV);
    for (auto obj : zipCont)
    {
        appendValue(depSetH, source(std::get<0>(obj)));
        appendValue(depSetV, source(std::get<1>(obj)));
    }

    for (auto pos = 0u; pos < numAlignments / sizeBatch; ++pos)
    {
        auto infSetH = infixWithLength(depSetH, pos * sizeBatch, sizeBatch);
        auto infSetV = infixWithLength(depSetV, pos * sizeBatch, sizeBatch);

        TSimdAlign resultsBatch;
        _prepareAndRunSimdAlignment(resultsBatch, trace, infSetH, infSetV, simdScoringScheme,
                                    TAlignConfig2(lowerDiag, upperDiag), TGapModel());

        // copy results and finish traceback
        // TODO(rrahn): Could be parallelized!
        // to for_each call
        for(auto x = pos * sizeBatch; x < (pos + 1) * sizeBatch; ++x)
        {
            results[x] = resultsBatch[x - pos * sizeBatch];
            _adaptTraceSegmentsTo(gapSeqSetH[x], gapSeqSetV[x], trace[x - pos * sizeBatch]);
        }
    }
    //call the normal non-simd function for remaining alignments
    for(auto pos = (numAlignments / sizeBatch) * sizeBatch; pos < numAlignments; ++pos)
        results[pos] = globalAlignment(gapSeqSetH[pos], gapSeqSetV[pos], scoringScheme, alignConfig,
                                       lowerDiag, upperDiag, algoTag);
    
    return results;
}


// ----------------------------------------------------------------------------
// Function globalAlignment()          [banded, SIMD version, StringSet<Align>]
// ----------------------------------------------------------------------------

template <typename TSequence, typename TAlignSpec, typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec, typename TAlgoTag>
String<TScoreValue> globalAlignment(StringSet<Align<TSequence, TAlignSpec> > & alignSet,
                                    Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                    AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                                    int const lowerDiag,
                                    int const upperDiag,
                                    TAlgoTag const & algoTag)
{
    typedef Align<TSequence, TAlignSpec>    TAlign;
    typedef typename Row<TAlign>::Type      TGapSequence;

    StringSet<TGapSequence, Dependent<> > gapSetH;
    StringSet<TGapSequence, Dependent<> > gapSetV;
    reserve(gapSetH, length(alignSet));
    reserve(gapSetV, length(alignSet));

    for (auto & align : alignSet)
    {
        appendValue(gapSetH, row(align, 0));
        appendValue(gapSetV, row(align, 1));
    }

    return globalAlignment(gapSetH, gapSetV, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without AlignConfig<>.
template <typename TSequence, typename TAlignSpec,
          typename TScoreValue, typename TScoreSpec,
          typename TAlgoTag>
String<TScoreValue> globalAlignment(StringSet<Align<TSequence, TAlignSpec> > & align,
                                    Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                    int lowerDiag, int upperDiag,
                                    TAlgoTag const & algoTag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(align, scoringScheme, alignConfig, lowerDiag, upperDiag, algoTag);
}

// Interface without algorithm tag.
template <typename TSequence, typename TAlignSpec,
          typename TScoreValue, typename TScoreSpec,
          bool TOP, bool LEFT, bool RIGHT, bool BOTTOM, typename TACSpec>
String<TScoreValue> globalAlignment(StringSet<Align<TSequence, TAlignSpec> > & align,
                                    Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                    AlignConfig<TOP, LEFT, RIGHT, BOTTOM, TACSpec> const & alignConfig,
                                    int lowerDiag, int upperDiag)
{
    if (scoreGapOpen(scoringScheme) == scoreGapExtend(scoringScheme))
        return globalAlignment(align, scoringScheme, alignConfig, lowerDiag, upperDiag, NeedlemanWunsch());
    else
        return globalAlignment(align, scoringScheme, alignConfig, lowerDiag, upperDiag, Gotoh());
}

// Interface without AlignConfig<> and algorithm tag.
template <typename TSequence, typename TAlignSpec,
          typename TScoreValue, typename TScoreSpec>
String<TScoreValue> globalAlignment(StringSet<Align<TSequence, TAlignSpec> > & align,
                                    Score<TScoreValue, TScoreSpec> const & scoringScheme,
                                    int lowerDiag, int upperDiag)
{
    AlignConfig<> alignConfig;
    return globalAlignment(align, scoringScheme, alignConfig, lowerDiag, upperDiag);
}
#endif  // SEQAN_SIMD_ENABLED

}  // namespace seqan

#endif  // #ifndef SEQAN_INCLUDE_SEQAN_ALIGN_GLOBAL_ALIGNMENT_BANDED_H_
