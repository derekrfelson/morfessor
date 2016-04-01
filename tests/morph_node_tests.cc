/*
 * morph_node_tests.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: Derek Felson
 */

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

  segmentations.split("reopen", 2);

  EXPECT_TRUE(segmentations.contains("re"));
  EXPECT_TRUE(segmentations.contains("open"));
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_EQ(1, segmentations.at("open"));
  EXPECT_EQ(1, segmentations.at("re"));
  EXPECT_EQ(1, segmentations.at("reopen"));
}

TEST(SegmentationTree_Split, CountPreservedWithNoSharedElements)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 7);
  segmentations.emplace("counter", 10);

  segmentations.split("reopen", 2);
  segmentations.split("counter", 5);

  EXPECT_EQ(7, segmentations.at("re"));
  EXPECT_EQ(7, segmentations.at("reopen"));
  EXPECT_EQ(7, segmentations.at("open"));
  EXPECT_EQ(10, segmentations.at("counter"));
  EXPECT_EQ(10, segmentations.at("count"));
  EXPECT_EQ(10, segmentations.at("er"));
}

TEST(SegmentationTree_Split, CountCombinedWithSharedElements)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 7);
  segmentations.emplace("retry", 10);

  segmentations.split("reopen", 2);
  segmentations.split("retry", 2);

  EXPECT_EQ(7, segmentations.at("reopen"));
  EXPECT_EQ(7, segmentations.at("open"));
  EXPECT_EQ(10, segmentations.at("retry"));
  EXPECT_EQ(10, segmentations.at("try"));
  EXPECT_EQ(17, segmentations.at("re"));
}

TEST(SegmentationTree_Split, CountCombinedWithDeepSharedElements)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopening", 1);
  segmentations.emplace("retry", 2);
  segmentations.emplace("trying", 4);

  segmentations.split("reopening", 2);
  segmentations.split("opening", 4);
  segmentations.split("retry", 2);
  segmentations.split("trying", 3);

  EXPECT_EQ(3, segmentations.at("re"));
  EXPECT_EQ(5, segmentations.at("ing"));
  EXPECT_EQ(1, segmentations.at("open"));
  EXPECT_EQ(6, segmentations.at("try"));
}

TEST(SegmentationTree_Remove, CountDecreasedSimpleCase)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 1);
  segmentations.emplace("retry", 2);

  segmentations.split("reopen", 2);
  segmentations.split("retry", 2);

  EXPECT_EQ(3, segmentations.at("re"));

  segmentations.Remove("reopen");

  EXPECT_FALSE(segmentations.contains("reopen"));

  EXPECT_EQ(2, segmentations.at("re"));
}

TEST(SegmentationTree_Remove, CountDecreasedHarderCase)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopening", 1);
  segmentations.emplace("retry", 2);
  segmentations.emplace("trying", 4);

  segmentations.split("reopening", 2);
  segmentations.split("opening", 4);
  segmentations.split("retry", 2);
  segmentations.split("trying", 3);

  segmentations.Remove("trying");

  EXPECT_EQ(1, segmentations.at("ing"));
  EXPECT_EQ(2, segmentations.at("try"));
}

TEST(SegmentationTree_Remove, EmptyDescendantsRemoved)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopening", 1);
  segmentations.emplace("retry", 2);
  segmentations.emplace("trying", 4);

  segmentations.split("reopening", 2);
  segmentations.split("opening", 4);
  segmentations.split("retry", 2);
  segmentations.split("trying", 3);

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
  segmentations.split("reopen", 2);
  segmentations.emplace("reorder", 10);
  segmentations.split("reorder", 2);

  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_TRUE(segmentations.contains("reorder"));
  segmentations.Remove("reorder");
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_FALSE(segmentations.contains("reorder"));
  segmentations.Remove("reopen");
  EXPECT_FALSE(segmentations.contains("reopen"));
}
