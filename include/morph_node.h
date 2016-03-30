/*
 * Split.h
 *
 *  Created on: Mar 29, 2016
 *      Author: derek
 */

#ifndef INCLUDE_MORPH_NODE_H_
#define INCLUDE_MORPH_NODE_H_

#include <string>
#include <unordered_set>

#include "morph.h"
#include "types.h"

namespace morfessor
{

class Morph;
class Corpus;
class Model;

// Represents a possible split or a word or morph into two smaller morphs.
class MorphNode
{
public:
  explicit MorphNode(std::string , size_t count) noexcept;
  explicit MorphNode(const Morph& morph) noexcept;
  bool has_children() const noexcept {
    return left_child_ != nullptr && right_child_ != nullptr;
  }
  std::string morph() const noexcept { return morph_; }
  size_t count() const noexcept { return count_; }
  void set_count(size_t value) noexcept { count_ = value; }
  MorphNode* left_child() const noexcept { return left_child_; }
  MorphNode* right_child() const noexcept { return right_child_; }
  bool operator==(const MorphNode& other) const noexcept {
    return morph_ == other.morph_;
  }

private:
  // Stores the string/substring this node refers to
  std::string morph_;
  // The number of times it appears in the model
  size_t count_;
	// Left and right children in the binary splitting tree.
	// Will either both be null or both point to a child.
	MorphNode* left_child_;
	MorphNode* right_child_;
};

void resplitnode(MorphNode* node, Model* model, const Corpus& corpus);

} // namespace morfessor

namespace std
{

template <>
struct hash<morfessor::MorphNode>
{
  size_t operator()(const morfessor::MorphNode& k) const noexcept
  {
    return hash<string>()(k.morph());
  }
};

} // namespace std;

namespace morfessor
{

// Stores recursive segmentations of a set of words.
// Example:
//     SegmentationTree segmentations{};
//     segmentations.emplace{"reopen", 1};
//     segmentations.split("reopen", 2);
//     assert(segmentations.contains("re"));
//     assert(segmentations.contains("open"));
//     assert(segmentations.contains("reopen"));
//     assert(segmentations["open"].count() == 1);
//     assert(segmentations["re"].count() == 1);
//     assert(segmentations["reopen"].count() == 1);
class SegmentationTree
{
 public:
  explicit SegmentationTree() noexcept;
  template <typename InputIterator>
  explicit SegmentationTree(InputIterator first, InputIterator last);
  void split(std::string morph, size_t split_index);
 private:
  std::unordered_set<MorphNode> nodes_;
};

template <typename InputIterator>
SegmentationTree::SegmentationTree(InputIterator first, InputIterator last)
{
  while (first != last)
  {
    nodes_.emplace(*first);
  }
}

} // namespace morfessor

#endif /* INCLUDE_MORPH_NODE_H_ */
