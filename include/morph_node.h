/*
 * Split.h
 *
 *  Created on: Mar 29, 2016
 *      Author: derek
 */

#ifndef INCLUDE_MORPH_NODE_H_
#define INCLUDE_MORPH_NODE_H_

#include <string>
#include <unordered_map>

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
  MorphNode(const std::string& morph) noexcept;
  explicit MorphNode(const std::string& morph, size_t count) noexcept;
  explicit MorphNode(const Morph& morph) noexcept;
  bool has_children() const noexcept {
    return left_child_ != nullptr && right_child_ != nullptr;
  }
  std::string morph() const noexcept { return morph_; }
  MorphNode* left_child() const noexcept { return left_child_; }
  MorphNode* right_child() const noexcept { return right_child_; }
  bool operator==(const MorphNode& other) const noexcept {
    return morph_ == other.morph_;
  }

private:
  // Stores the string/substring this node refers to
  std::string morph_;
	// Left and right children in the binary splitting tree.
	// Will either both be null or both point to a child.
	MorphNode* left_child_;
	MorphNode* right_child_;
	friend class SegmentationTree;
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
//     segmentations.emplace("reopen", 1);
//     segmentations.split("reopen", 2);
class SegmentationTree
{
 public:
  explicit SegmentationTree() noexcept;
  template <typename InputIterator>
  explicit SegmentationTree(InputIterator first, InputIterator last);
  void split(const std::string& morph, size_t split_index);
  bool contains(const std::string& morph) const {
    return nodes_.find(morph) != nodes_.end();
  }
  void emplace(const std::string& morph, size_t frequency) {
    nodes_.emplace(morph, frequency);
  }
  size_t& at(const std::string& morph) { return nodes_.at(morph); }
  const size_t& at(const std::string& morph) const { return nodes_.at(morph); }
 private:
  std::unordered_map<MorphNode, size_t> nodes_;
};

template <typename InputIterator>
SegmentationTree::SegmentationTree(InputIterator first, InputIterator last)
{
  while (first != last)
  {
    nodes_.emplace(first->letters(), first->frequency());
    ++first;
  }
}

} // namespace morfessor

#endif /* INCLUDE_MORPH_NODE_H_ */
