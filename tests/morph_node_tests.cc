// The MIT License (MIT)
//
// Copyright (c) 2016 Derek Felson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "morph_node.h"

#include <cmath>
#include <vector>

#include <gtest/gtest.h>
#include <boost/math/special_functions/binomial.hpp>

#include "morph.h"
#include "corpus.h"

using Morph = morfessor::Morph;
using Corpus = morfessor::Corpus;
using SegmentationTree = morfessor::SegmentationTree;
auto binomial = &boost::math::binomial_coefficient<double>;
constexpr double threshold = 0.0001;

TEST(SegmentationTree_IteratorConstructor, Empty)
{
  std::vector<Morph> morphs;
  SegmentationTree segmentations{begin(morphs), end(morphs)};
  EXPECT_FALSE(segmentations.contains("anything"));
}

TEST(SegmentationTree_IteratorConstructor, OneElement)
{
  std::vector<Morph> morphs;
  morphs.emplace_back("reopen", 5);
  SegmentationTree segmentations{begin(morphs), end(morphs)};
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_FALSE(segmentations.contains("reorder"));
  EXPECT_FALSE(segmentations.contains("redo"));
}

TEST(SegmentationTree_IteratorConstructor, ManyElements)
{
  std::vector<Morph> morphs;
  morphs.emplace_back("reopen", 5);
  morphs.emplace_back("reorder", 6);
  SegmentationTree segmentations{begin(morphs), end(morphs)};
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_TRUE(segmentations.contains("reorder"));
  EXPECT_FALSE(segmentations.contains("redo"));
}

TEST(SegmentationTree_Split, OneNode)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 1);

  segmentations.Split("reopen", 2);

  EXPECT_TRUE(segmentations.contains("re"));
  EXPECT_TRUE(segmentations.contains("open"));
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_EQ(1, segmentations.at("open").count);
  EXPECT_EQ(1, segmentations.at("re").count);
  EXPECT_EQ(1, segmentations.at("reopen").count);
}

TEST(SegmentationTree_Split, CountPreservedWithNoSharedElements)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 7);
  segmentations.emplace("counter", 10);

  segmentations.Split("reopen", 2);
  segmentations.Split("counter", 5);

  EXPECT_EQ(7, segmentations.at("re").count);
  EXPECT_EQ(7, segmentations.at("reopen").count);
  EXPECT_EQ(7, segmentations.at("open").count);
  EXPECT_EQ(10, segmentations.at("counter").count);
  EXPECT_EQ(10, segmentations.at("count").count);
  EXPECT_EQ(10, segmentations.at("er").count);
}

TEST(SegmentationTree_Split, CountCombinedWithSharedElements)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 7);
  segmentations.emplace("retry", 10);

  segmentations.Split("reopen", 2);
  segmentations.Split("retry", 2);

  EXPECT_EQ(7, segmentations.at("reopen").count);
  EXPECT_EQ(7, segmentations.at("open").count);
  EXPECT_EQ(10, segmentations.at("retry").count);
  EXPECT_EQ(10, segmentations.at("try").count);
  EXPECT_EQ(17, segmentations.at("re").count);
}

TEST(SegmentationTree_Split, CountCombinedWithDeepSharedElements)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopening", 1);
  segmentations.emplace("retry", 2);
  segmentations.emplace("trying", 4);

  segmentations.Split("reopening", 2);
  segmentations.Split("opening", 4);
  segmentations.Split("retry", 2);
  segmentations.Split("trying", 3);

  EXPECT_EQ(3, segmentations.at("re").count);
  EXPECT_EQ(5, segmentations.at("ing").count);
  EXPECT_EQ(1, segmentations.at("open").count);
  EXPECT_EQ(6, segmentations.at("try").count);
}

TEST(SegmentationTree_Remove, CountDecreasedSimpleCase)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 1);
  segmentations.emplace("retry", 2);

  segmentations.Split("reopen", 2);
  segmentations.Split("retry", 2);

  EXPECT_EQ(3, segmentations.at("re").count);

  segmentations.Remove("reopen");

  EXPECT_FALSE(segmentations.contains("reopen"));

  EXPECT_EQ(2, segmentations.at("re").count);
}

TEST(SegmentationTree_Remove, CountDecreasedHarderCase)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopening", 1);
  segmentations.emplace("retry", 2);
  segmentations.emplace("trying", 4);

  segmentations.Split("reopening", 2);
  segmentations.Split("opening", 4);
  segmentations.Split("retry", 2);
  segmentations.Split("trying", 3);

  segmentations.Remove("trying");

  EXPECT_EQ(1, segmentations.at("ing").count);
  EXPECT_EQ(2, segmentations.at("try").count);
}

TEST(SegmentationTree_Remove, EmptyDescendantsRemoved)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopening", 1);
  segmentations.emplace("retry", 2);
  segmentations.emplace("trying", 4);

  segmentations.Split("reopening", 2);
  segmentations.Split("opening", 4);
  segmentations.Split("retry", 2);
  segmentations.Split("trying", 3);

  segmentations.Remove("trying");
  segmentations.Remove("retry");

  EXPECT_FALSE(segmentations.contains("try"));
}

TEST(SegmentationTree_Remove, NodeGoneFromOneElementTree)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 7);
  EXPECT_TRUE(segmentations.contains("reopen"));
  segmentations.Remove("reopen");
  EXPECT_FALSE(segmentations.contains("reopen"));
}

TEST(SegmentationTree_Remove, NodeGoneFromTwoElementTree)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 7);
  segmentations.emplace("reorder", 10);
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_TRUE(segmentations.contains("reorder"));
  segmentations.Remove("reorder");
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_FALSE(segmentations.contains("reorder"));
  segmentations.Remove("reopen");
  EXPECT_FALSE(segmentations.contains("reopen"));
}

TEST(SegmentationTree_Remove, CountDecreased)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 7);
  segmentations.Split("reopen", 2);
  segmentations.emplace("reorder", 10);
  segmentations.Split("reorder", 2);

  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_TRUE(segmentations.contains("reorder"));
  segmentations.Remove("reorder");
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_FALSE(segmentations.contains("reorder"));
  segmentations.Remove("reopen");
  EXPECT_FALSE(segmentations.contains("reopen"));
}

TEST(SegmentationTree_Optimize, NoWords)
{
  SegmentationTree segmentations{};
  EXPECT_EQ(0, segmentations.size());
  segmentations.Optimize();
  EXPECT_EQ(0, segmentations.size());
}

TEST(SegmentationTree_Optimize, OneWord)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 7);
  EXPECT_EQ(1, segmentations.size());
  segmentations.Optimize();
  EXPECT_EQ(7, segmentations.at("reopen").count);
  EXPECT_EQ(1, segmentations.size());
}

TEST(SegmentationTree_Optimize, TwoWords)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 1);
  segmentations.emplace("redo", 2);
  ASSERT_EQ(2, segmentations.size());

  segmentations.Optimize();
  EXPECT_EQ(3, segmentations.size());
  EXPECT_EQ(1, segmentations.at("reopen").count);
  EXPECT_EQ(2, segmentations.at("redo").count);
  ASSERT_TRUE(segmentations.contains("re"));
  EXPECT_EQ(3, segmentations.at("re").count);
}

TEST(SegmentationTree_ProbabilityOfMorph, NoSplits)
{
  SegmentationTree st{};

  st.emplace("reopen", 1);
  EXPECT_EQ(std::log(1.0), st.ProbabilityOfMorph("reopen"));

  st.emplace("redo", 2);
  EXPECT_EQ(std::log(1.0/3.0), st.ProbabilityOfMorph("reopen"));
  EXPECT_EQ(std::log(2.0/3.0), st.ProbabilityOfMorph("redo"));

  st.emplace("trying", 4);
  EXPECT_EQ(std::log(1.0/7.0), st.ProbabilityOfMorph("reopen"));
  EXPECT_EQ(std::log(2.0/7.0), st.ProbabilityOfMorph("redo"));
  EXPECT_EQ(std::log(4.0/7.0), st.ProbabilityOfMorph("trying"));
}

TEST(SegmentationTree_ProbabilityOfMorph, Splits)
{
  SegmentationTree st{};
  st.emplace("reopen", 1);
  st.emplace("redo", 2);
  st.emplace("trying", 4);
  ASSERT_EQ(std::log(1.0/7.0), st.ProbabilityOfMorph("reopen"));
  ASSERT_EQ(std::log(2.0/7.0), st.ProbabilityOfMorph("redo"));
  ASSERT_EQ(std::log(4.0/7.0), st.ProbabilityOfMorph("trying"));

  st.Split("reopen", 2);
  EXPECT_EQ(std::log(1.0/8.0), st.ProbabilityOfMorph("re"));
  EXPECT_EQ(std::log(1.0/8.0), st.ProbabilityOfMorph("open"));
  EXPECT_EQ(std::log(2.0/8.0), st.ProbabilityOfMorph("redo"));
  EXPECT_EQ(std::log(4.0/8.0), st.ProbabilityOfMorph("trying"));

  st.Split("redo", 2);
  EXPECT_EQ(std::log(3.0/10.0), st.ProbabilityOfMorph("re"));
  EXPECT_EQ(std::log(1.0/10.0), st.ProbabilityOfMorph("open"));
  EXPECT_EQ(std::log(2.0/10.0), st.ProbabilityOfMorph("do"));
  EXPECT_EQ(std::log(4.0/10.0), st.ProbabilityOfMorph("trying"));

  st.Split("trying", 3);
  EXPECT_EQ(std::log(3.0/14.0), st.ProbabilityOfMorph("re"));
  EXPECT_EQ(std::log(1.0/14.0), st.ProbabilityOfMorph("open"));
  EXPECT_EQ(std::log(2.0/14.0), st.ProbabilityOfMorph("do"));
  EXPECT_EQ(std::log(4.0/14.0), st.ProbabilityOfMorph("try"));
  EXPECT_EQ(std::log(4.0/14.0), st.ProbabilityOfMorph("ing"));
}

TEST(SegmentationTree_ProbabilityOfMorph, RemoveUnshared)
{
  SegmentationTree st{};
  st.emplace("reopen", 1);
  st.emplace("doing", 2);
  st.emplace("trying", 4);
  st.Split("reopen", 2);
  st.Split("doing", 2);
  st.Split("trying", 3);
  ASSERT_EQ(std::log(1.0/14.0), st.ProbabilityOfMorph("re"));
  ASSERT_EQ(std::log(1.0/14.0), st.ProbabilityOfMorph("open"));
  ASSERT_EQ(std::log(2.0/14.0), st.ProbabilityOfMorph("do"));
  ASSERT_EQ(std::log(6.0/14.0), st.ProbabilityOfMorph("ing"));
  ASSERT_EQ(std::log(4.0/14.0), st.ProbabilityOfMorph("try"));

  st.Remove("reopen");
  ASSERT_FALSE(st.contains("reopen"));
  ASSERT_FALSE(st.contains("re"));
  ASSERT_FALSE(st.contains("open"));
  EXPECT_EQ(std::log(2.0/12.0), st.ProbabilityOfMorph("do"));
  EXPECT_EQ(std::log(6.0/12.0), st.ProbabilityOfMorph("ing"));
  EXPECT_EQ(std::log(4.0/12.0), st.ProbabilityOfMorph("try"));
}

TEST(SegmentationTree_ProbabilityOfMorph, RemoveShallowShared)
{
  SegmentationTree st{};
  st.emplace("reopen", 1);
  st.emplace("doing", 2);
  st.emplace("trying", 4);
  st.Split("reopen", 2);
  st.Split("doing", 2);
  st.Split("trying", 3);
  ASSERT_EQ(std::log(1.0/14.0), st.ProbabilityOfMorph("re"));
  ASSERT_EQ(std::log(1.0/14.0), st.ProbabilityOfMorph("open"));
  ASSERT_EQ(std::log(2.0/14.0), st.ProbabilityOfMorph("do"));
  ASSERT_EQ(std::log(6.0/14.0), st.ProbabilityOfMorph("ing"));
  ASSERT_EQ(std::log(4.0/14.0), st.ProbabilityOfMorph("try"));

  st.Remove("doing");
  ASSERT_FALSE(st.contains("doing"));
  ASSERT_FALSE(st.contains("do"));
  EXPECT_EQ(std::log(1.0/10.0), st.ProbabilityOfMorph("re"));
  EXPECT_EQ(std::log(1.0/10.0), st.ProbabilityOfMorph("open"));
  EXPECT_EQ(std::log(4.0/10.0), st.ProbabilityOfMorph("ing"));
  EXPECT_EQ(std::log(4.0/10.0), st.ProbabilityOfMorph("try"));
}

TEST(SegmentationTree_ProbabilityOfMorph, RemoveDeepShared)
{
  SegmentationTree st{};
  st.emplace("reopen", 1);
  st.emplace("redoing", 2);
  st.emplace("trying", 4);
  st.Split("reopen", 2);
  st.Split("redoing", 2);
  st.Split("doing", 2);
  st.Split("trying", 3);
  ASSERT_EQ(std::log(3.0/16.0), st.ProbabilityOfMorph("re"));
  ASSERT_EQ(std::log(1.0/16.0), st.ProbabilityOfMorph("open"));
  ASSERT_EQ(std::log(2.0/16.0), st.ProbabilityOfMorph("do"));
  ASSERT_EQ(std::log(4.0/16.0), st.ProbabilityOfMorph("try"));
  ASSERT_EQ(std::log(6.0/16.0), st.ProbabilityOfMorph("ing"));

  st.Remove("redoing");
  ASSERT_FALSE(st.contains("redoing"));
  ASSERT_FALSE(st.contains("doing"));
  ASSERT_FALSE(st.contains("do"));
  EXPECT_EQ(std::log(1.0/10.0), st.ProbabilityOfMorph("re"));
  EXPECT_EQ(std::log(1.0/10.0), st.ProbabilityOfMorph("open"));
  EXPECT_EQ(std::log(4.0/10.0), st.ProbabilityOfMorph("try"));
  EXPECT_EQ(std::log(4.0/10.0), st.ProbabilityOfMorph("ing"));
}

TEST(SegmentationTree_ProbabilityOfCorpusGivenModel, ThreeMorphs)
{
  SegmentationTree st{};
  st.emplace("reopen", 1);
  st.emplace("redoing", 2);
  st.emplace("trying", 4);
  st.Split("reopen", 2);
  st.Split("redoing", 2);
  st.Split("doing", 2);
  st.Split("trying", 3);
  ASSERT_EQ(std::log(3.0/16.0), st.ProbabilityOfMorph("re"));
  ASSERT_EQ(std::log(1.0/16.0), st.ProbabilityOfMorph("open"));
  ASSERT_EQ(std::log(2.0/16.0), st.ProbabilityOfMorph("do"));
  ASSERT_EQ(std::log(4.0/16.0), st.ProbabilityOfMorph("try"));
  ASSERT_EQ(std::log(6.0/16.0), st.ProbabilityOfMorph("ing"));

  auto correct_sum = std::log(3.0/16.0) + std::log(1.0/16.0)
      + std::log(2.0/16.0) + std::log(4.0/16.0) + std::log(6.0/16.0);
  EXPECT_EQ(correct_sum, st.ProbabilityOfCorpusGivenModel());
}

class SegmentationTreeProbabililtyTests : public ::testing::Test {
 protected:
  virtual void SetUp() {
    st = SegmentationTree{};
    st.emplace("reopen", 1);
    st.emplace("redoing", 2);
    st.emplace("trying", 4);
    set_hapax_legomena_prior(0.5);
  }

  virtual void TearDown() {
  }

  void split() {
    st.Split("reopen", 2);
    st.Split("redoing", 2);
    st.Split("doing", 2);
    st.Split("trying", 3);
  }

  void set_hapax_legomena_prior(double value)
  {
    st.set_hapax_legomena_prior(value);
    hapax_legomena_exponent_ = std::log2(1 - value);
  }

  double explicit_frequency_probabilities(
      std::initializer_list<size_t> frequencies) const
  {
    double sum = 0;
    for (auto frequency : frequencies)
    {
      sum -= std::log2(std::pow(frequency, hapax_legomena_exponent_)
             - std::pow(frequency + 1, hapax_legomena_exponent_));
    }
    return sum;
  }

  double hapax_legomena_exponent_ = -1;
  SegmentationTree st;
};

// Test implicit frequencies

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromImplicitFrequencies_WithUnsplitCorpus)
{
  // Morph tokens : unique morphs ratio = 7:3
  ASSERT_NEAR(std::log2(binomial(6, 2)),
      st.ProbabilityFromImplicitFrequencies(), threshold);
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromImplicitFrequencies_WithSplitCorpus)
{
  split();
  // Morph tokens : unique morphs ratio = 16:5
  ASSERT_NEAR(std::log2(binomial(15, 4)),
      st.ProbabilityFromImplicitFrequencies(), threshold);
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromImplicitFrequencies_AfterShallowRemoval)
{
  split();
  st.Remove("reopen");
  // Morph tokens : unique morphs ratio = 14:4
  ASSERT_NEAR(std::log2(binomial(13, 3)),
      st.ProbabilityFromImplicitFrequencies(), threshold);
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromImplicitFrequencies_AfterDeepRemoval)
{
  split();
  st.Remove("redoing");
  // Morph tokens : unique morphs ratio = 10:4
  ASSERT_NEAR(std::log2(binomial(9, 3)),
      st.ProbabilityFromImplicitFrequencies(), threshold);
}

// Test explicit frequencies

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromExplicitFrequencies_WithUnsplitCorpus)
{
  EXPECT_NEAR(explicit_frequency_probabilities({1,2,4}),
      st.ProbabilityFromExplicitFrequencies(), threshold);
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromExplicitFrequencies_WithSplitCorpus)
{
  split();
  EXPECT_NEAR(explicit_frequency_probabilities({3,1,2,4,6}),
      st.ProbabilityFromExplicitFrequencies(), threshold);
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromExplicitFrequencies_AfterShallowRemoval)
{
  split();
  st.Remove("reopen");
  EXPECT_NEAR(explicit_frequency_probabilities({2,2,4,6}),
      st.ProbabilityFromExplicitFrequencies(), threshold);
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromExplicitFrequencies_AfterDeepRemoval)
{
  split();
  st.Remove("redoing");
  EXPECT_NEAR(explicit_frequency_probabilities({1,1,4,4}),
        st.ProbabilityFromExplicitFrequencies(), threshold);
}

// Test letter probabilities

TEST_F(SegmentationTreeProbabililtyTests,
    LetterProbabilities)
{
  auto lp = st.LetterProbabilities();

  // Calculated by Morfessor Baseline reference implementation
  EXPECT_NEAR(2.86507, lp['#'], threshold);
  EXPECT_NEAR(4.67243, lp['d'], threshold);
  EXPECT_NEAR(3.67243, lp['e'], threshold);
  EXPECT_NEAR(3.08746, lp['g'], threshold);
  EXPECT_NEAR(3.08746, lp['i'], threshold);
  EXPECT_NEAR(2.86507, lp['n'], threshold);
  EXPECT_NEAR(4.08746, lp['o'], threshold);
  EXPECT_NEAR(5.67243, lp['p'], threshold);
  EXPECT_NEAR(2.86507, lp['r'], threshold);
  EXPECT_NEAR(3.67243, lp['t'], threshold);
  EXPECT_NEAR(3.67243, lp['y'], threshold);
}

// Test frequency distribution

TEST_F(SegmentationTreeProbabililtyTests, FrequencyReferenceTest)
{
  // Calculated by Morfessor Baseline reference implementation
  EXPECT_EQ(7, st.total_morph_tokens());
  EXPECT_EQ(3, st.unique_morph_types());

  // Reference implementation for implicit does not work for small data sets
  //EXPECT_NEAR(7.59172, st.ProbabilityFromImplicitFrequencies(), threshold);

  EXPECT_NEAR(7.90689, st.ProbabilityFromExplicitFrequencies(), threshold);

  SegmentationTree st2;
  st2.emplace("going", 1);
  st2.emplace("walking", 2);
  st2.emplace("deciding", 4);
  st2.emplace("relief", 8);

  EXPECT_EQ(15, st2.total_morph_tokens());
  EXPECT_EQ(4, st2.unique_morph_types());

  // Reference implementation for implicit does not work for small data sets
  //EXPECT_NEAR(12.26495, st2.ProbabilityFromImplicitFrequencies(), threshold);

  EXPECT_NEAR(14.07682, st2.ProbabilityFromExplicitFrequencies(), threshold);

  morfessor::Corpus c{"../testdata/largetest.txt"};
  SegmentationTree st3{c.begin(), c.end()};
  st3.set_hapax_legomena_prior(0.5);
  EXPECT_EQ(64772, st3.total_morph_tokens());
  EXPECT_EQ(492, st3.unique_morph_types());
  EXPECT_NEAR(4165.46499, st3.ProbabilityFromImplicitFrequencies(), threshold);
  EXPECT_NEAR(2341.87284, st3.ProbabilityFromExplicitFrequencies(), threshold);
}

// Explicit frequency tests

TEST(SegmentationTree, ExplicitFrequencyCost_ReferenceTest1)
{
  morfessor::Corpus c{"../testdata/test1.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(7.90689, st.ProbabilityFromExplicitFrequencies(), threshold);
}

TEST(SegmentationTree, ExplicitFrequencyCost_ReferenceTest2)
{
  morfessor::Corpus c{"../testdata/test2.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(14.07682, st.ProbabilityFromExplicitFrequencies(), threshold);
}

TEST(SegmentationTree, ExplicitFrequencyCost_ReferenceTest3)
{
  morfessor::Corpus c{"../testdata/test3.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(2341.87284, st.ProbabilityFromExplicitFrequencies(), threshold);
}

TEST(SegmentationTree, ExplicitFrequencyCost_ReferenceTestFullEnglishCorpus)
{
  morfessor::Corpus c{"../testdata/test-full-english-corpus.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(822962.11904, st.ProbabilityFromExplicitFrequencies(),
      threshold);
}

// Implicit frequency tests
// Implicit frequency can only be compared to the reference implementation
// for large datasets.

TEST(SegmentationTree, ImplicitFrequencyCost_ReferenceTest3)
{
  morfessor::Corpus c{"../testdata/test3.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(4165.46499, st.ProbabilityFromImplicitFrequencies(), threshold);
}

TEST(SegmentationTree, ImplicitFrequencyCost_ReferenceTestFullEnglishCorpus)
{
  morfessor::Corpus c{"../testdata/test-full-english-corpus.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(1444149.68019, st.ProbabilityFromImplicitFrequencies(),
      threshold);
}

// Test morph string costs

TEST(SegmentationTree, MorphStringCost_ReferenceTest1)
{
  morfessor::Corpus c{"../testdata/test1.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(66.42218, st.ProbabilityFromLetters(), threshold);
}

TEST(SegmentationTree, MorphStringCost_ReferenceTest2)
{
  morfessor::Corpus c{"../testdata/test2.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(99.38380, st.ProbabilityFromLetters(), threshold);
}

TEST(SegmentationTree, MorphStringCost_ReferenceTest3)
{
  morfessor::Corpus c{"../testdata/test3.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(17957.46139, st.ProbabilityFromLetters(), threshold);
}

TEST(SegmentationTree, MorphStringCost_ReferenceTestFullEnglishCorpus)
{
  morfessor::Corpus c{"../testdata/test-full-english-corpus.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(6198711.87999, st.ProbabilityFromLetters(), threshold);
}

// Test morph length costs (implicit calculation method)

TEST(SegmentationTree, ImplicitMorphLengthCost_ReferenceTest1)
{
  morfessor::Corpus c{"../testdata/test1.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(8.59521, st.ProbabilityFromImplicitLengths(), threshold);
}

TEST(SegmentationTree, ImplicitMorphLengthCost_ReferenceTest2)
{
  morfessor::Corpus c{"../testdata/test2.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(11.70400, st.ProbabilityFromImplicitLengths(), threshold);
}

TEST(SegmentationTree, ImplicitMorphLengthCost_ReferenceTest3)
{
  morfessor::Corpus c{"../testdata/test3.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(1323.25343, st.ProbabilityFromImplicitLengths(), threshold);
}

TEST(SegmentationTree, ImplicitMorphLengthCost_ReferenceTestFullEnglishCorpus)
{
  morfessor::Corpus c{"../testdata/test-full-english-corpus.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(410817.33961, st.ProbabilityFromImplicitLengths(), threshold);
}

// Test morph length costs (explicit calculation method)

TEST(SegmentationTree, ExplicitMorphLengthCost_ReferenceTest1)
{
  morfessor::Corpus c{"../testdata/test1.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(8.46787, st.ProbabilityFromExplicitLengths(), threshold);
}

TEST(SegmentationTree, ExplicitMorphLengthCost_ReferenceTest2)
{
  morfessor::Corpus c{"../testdata/test2.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(11.70718, st.ProbabilityFromExplicitLengths(), threshold);
}

TEST(SegmentationTree, ExplicitMorphLengthCost_ReferenceTest3)
{
  morfessor::Corpus c{"../testdata/test3.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(1623.19011, st.ProbabilityFromExplicitLengths(), threshold);
}

TEST(SegmentationTree, ExplicitMorphLengthCost_ReferenceTestFullEnglishCorpus)
{
  morfessor::Corpus c{"../testdata/test-full-english-corpus.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(564175.94227, st.ProbabilityFromExplicitLengths(), threshold);
}

// Test probability adjustment based on morph orders in the lexicon

TEST(SegmentationTree, MorphOrderAdjustment_ReferenceTest1)
{
  morfessor::Corpus c{"../testdata/test1.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(-0.42680, st.ProbabilityAdjustmentFromLexiconOrdering(),
      threshold);
}

TEST(SegmentationTree, MorphOrderAdjustment_ReferenceTest2)
{
  morfessor::Corpus c{"../testdata/test2.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(-2.22922, st.ProbabilityAdjustmentFromLexiconOrdering(),
      threshold);
}

TEST(SegmentationTree, MorphOrderAdjustment_ReferenceTest3)
{
  morfessor::Corpus c{"../testdata/test3.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(-3689.91118, st.ProbabilityAdjustmentFromLexiconOrdering(),
      threshold);
}

TEST(SegmentationTree, MorphOrderAdjustment_ReferenceTestFullEnglishCorpus)
{
  morfessor::Corpus c{"../testdata/test-full-english-corpus.txt"};
  SegmentationTree st{c.begin(), c.end()};
  // Calculated from Morfessor Baseline reference implementation
  EXPECT_NEAR(-2662975.89120, st.ProbabilityAdjustmentFromLexiconOrdering(),
      threshold);
}

