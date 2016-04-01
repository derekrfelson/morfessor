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

TEST(SegmentationTreeTests, IteratorConstructorEmpty)
{
  std::vector<Morph> morphs;
  SegmentationTree segmentations{begin(morphs), end(morphs)};
  EXPECT_FALSE(segmentations.contains("anything"));
}

TEST(SegmentationTreeTests, IteratorConstructorOneElement)
{
  std::vector<Morph> morphs;
  morphs.emplace_back("reopen", 5);
  SegmentationTree segmentations{begin(morphs), end(morphs)};
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_FALSE(segmentations.contains("reorder"));
  EXPECT_FALSE(segmentations.contains("redo"));
}

TEST(SegmentationTreeTests, IteratorConstructorManyElements)
{
  std::vector<Morph> morphs;
  morphs.emplace_back("reopen", 5);
  morphs.emplace_back("reorder", 6);
  SegmentationTree segmentations{begin(morphs), end(morphs)};
  EXPECT_TRUE(segmentations.contains("reopen"));
  EXPECT_TRUE(segmentations.contains("reorder"));
  EXPECT_FALSE(segmentations.contains("redo"));
}

TEST(SegmentationTreeTests, SingleMorphemeSplit)
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
