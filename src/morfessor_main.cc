/*
 * morfessor_main.cc
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#include <cassert>

#include "morph_node.h"

using SegmentationTree = morfessor::SegmentationTree;

int main(int argc, char** argv)
{
  SegmentationTree segmentations{};
  segmentations.emplace("reopen", 1);
  segmentations.emplace("redo", 2);
  assert(2 == segmentations.size());
  segmentations.Optimize();
  assert(3 == segmentations.size());
  assert(1 == segmentations.at("reopen").count);
  assert(2 == segmentations.at("redo").count);
  assert(3 == segmentations.at("re").count);

  return 0;
}
