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

using Morph = morfessor::Morph;
using SegmentationTree = morfessor::SegmentationTree;
auto binomial = &boost::math::binomial_coefficient<double>;

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
  }

  virtual void TearDown() {
  }

  void split() {
    st.Split("reopen", 2);
    st.Split("redoing", 2);
    st.Split("doing", 2);
    st.Split("trying", 3);
  }

  SegmentationTree st;
};

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromImplicitFrequencies_WithUnsplitCorpus)
{
  // Morph tokens : unique morphs ratio = 7:3
  ASSERT_EQ(-std::log(binomial(6, 2)),
      st.ProbabilityFromImplicitFrequencies());
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromImplicitFrequencies_WithSplitCorpus)
{
  split();
  // Morph tokens : unique morphs ratio = 16:5
  ASSERT_EQ(-std::log(binomial(15, 4)),
      st.ProbabilityFromImplicitFrequencies());
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromImplicitFrequencies_AfterShallowRemoval)
{
  split();
  st.Remove("reopen");
  // Morph tokens : unique morphs ratio = 14:4
  ASSERT_EQ(-std::log(binomial(13, 3)),
      st.ProbabilityFromImplicitFrequencies());
}

TEST_F(SegmentationTreeProbabililtyTests,
    ProbabilityFromImplicitFrequencies_AfterDeepRemoval)
{
  split();
  st.Remove("redoing");
  // Morph tokens : unique morphs ratio = 10:4
  ASSERT_EQ(-std::log(binomial(9, 3)),
      st.ProbabilityFromImplicitFrequencies());
}
