/*
 * Split.h
 *
 *  Created on: Mar 29, 2016
 *      Author: derek
 */

#ifndef INCLUDE_MORPH_NODE_H_
#define INCLUDE_MORPH_NODE_H_

#include <cassert>
#include <string>
#include <unordered_map>
#include <stack>

#include "morph.h"
#include "types.h"

namespace morfessor
{

class Morph;
class Corpus;
class Model;

// Represents a possible split or a word or morph into two smaller morphs.
struct MorphNode
{
public:
  MorphNode();
  MorphNode(size_t count);
  bool has_children() const noexcept {
    return !(left_child.empty() && right_child.empty());
  }
  // Stores the number of times this morph appears in the corpus
  size_t count;
	// Keys to the left and right children in the binary splitting tree.
	// Will either both be null or both point to a child.
	std::string left_child;
	std::string right_child;
};

// Stores recursive segmentations of a set of words.
// Example:
//     SegmentationTree segmentations{};
//     segmentations.emplace("reopen", 1);
//     segmentations.split("reopen", 2);
class SegmentationTree
{
 public:
  explicit SegmentationTree();
  template <typename InputIterator>
  explicit SegmentationTree(InputIterator first, InputIterator last);
  void Split(const std::string& morph, size_t split_index);
  bool contains(const std::string& morph) const {
    return nodes_.find(morph) != nodes_.end();
  }
  void emplace(const std::string& morph, size_t frequency) {
    nodes_.emplace(morph, frequency);
  }
  MorphNode& at(const std::string& morph) {
    return nodes_.at(morph);
  }
  const MorphNode& at(const std::string& morph) const {
    return nodes_.at(morph);
  }
  size_t size() const noexcept { return nodes_.size(); }
  void Remove(const std::string morph);
  void Optimize();
 private:
  std::unordered_map<std::string, MorphNode> nodes_;
  Probability pr_model_given_corpus_ = 0;
  Probability pr_corpus_given_model_ = 0;
  Probability pr_frequencies_ = 0;
  Probability pr_lengths_ = 0;
  void ResplitNode(const std::string& morph);
  void RemoveNode(const MorphNode& node_to_remove,
      const std::string& subtree_key);
};

inline bool operator==(const MorphNode& lhs, const MorphNode& rhs) noexcept {
    return lhs.count == rhs.count;
}

inline void SegmentationTree::Remove(const std::string morph)
{
  auto node = nodes_.find(morph);
  assert(node != end(nodes_));
  RemoveNode(node->second, node->first);
}

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
