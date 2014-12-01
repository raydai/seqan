// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2013, Knut Reinert, FU Berlin
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
// Tests for bio alphabet functions and metafunctions.
// ==========================================================================

#ifndef SEQAN_TESTS_BASIC_TEST_BASIC_ALPHABET_BIO_H_
#define SEQAN_TESTS_BASIC_TEST_BASIC_ALPHABET_BIO_H_

// We only test that the default implementations forward to *Impl.

struct MyBioType_
{
    int value;

    MyBioType_() : value(0)
    {}

    explicit
    MyBioType_(int v) : value(v)
    {}
};

inline MyBioType_ gapValueImpl(MyBioType_ * /*tag*/)
{
    return MyBioType_(-1);
}

inline MyBioType_ unknownValueImpl(MyBioType_ * /*tag*/)
{
    return MyBioType_(-2);
}

SEQAN_DEFINE_TEST(test_basic_alphabet_bio_gap_value_function)
{
    SEQAN_ASSERT_EQ(seqan::gapValue<MyBioType_>().value, -1);
}

SEQAN_DEFINE_TEST(test_basic_alphabet_bio_unknown_value_function)
{
    SEQAN_ASSERT_EQ(seqan::unknownValue<MyBioType_>().value, -2);
}

#endif  // #ifndef SEQAN_TESTS_BASIC_TEST_BASIC_ALPHABET_BIO_H_
