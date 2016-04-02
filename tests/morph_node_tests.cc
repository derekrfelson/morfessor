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

#include <vector>

#include <gtest/gtest.h>

#include "morph.h"

using Morph = morfessor::Morph;
using SegmentationTree = morfessor::SegmentationTree;

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
