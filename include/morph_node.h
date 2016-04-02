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

#ifndef INCLUDE_MORPH_NODE_H_
#define INCLUDE_MORPH_NODE_H_

#include <cmath>
#include <cassert>
#include <string>
#include <unordered_map>

#include "morph.h"
#include "types.h"

namespace morfessor
{

/// Represents a possible split or a word or morph into two smaller morphs.
struct MorphNode
{
 public:
  /// C'tor for an empty node with no children.
  MorphNode();

  /// C'tor for a morph with a given frequency and no children. The string
  /// the MorphNode corresponds to is stored in the corresponding data
  /// structure, and includes a pointer to this morph node.
  /// @param count The frequency of the corresponding morph.
  MorphNode(size_t count);

  /// Returns true if the node has a left and right child. Note that in any
  /// valid program state, there will either be both a left and right child
  /// or no children at all.
  bool has_children() const noexcept {
    return !(left_child.empty() && right_child.empty());
  }

  /// Stores the number of times this morph appears in the corpus.
  size_t count;

	/// Key for the left child in the data structure. Equal to the empty string
  /// if there is no left child.
	std::string left_child;

	/// Key for the right child in the data structure. Equal to the empty
	/// string if there is no right child.
	std::string right_child;
};

/// Stores recursive segmentations of a set of words.
/// Example:
///     SegmentationTree segmentations{};
///     segmentations.emplace("reopen", 1);
///     segmentations.split("reopen", 2);
class SegmentationTree
{
 public:
  /// C'tor that creates an empty segmentation tree. You probably want to use
  /// emplace to add morphs to it after.
  explicit SegmentationTree();

  /// C'tor that creates a segmentation tree an immediately populates it with
  /// morphs taken from some container. InputIterator must dereference to
  /// something of type Morph. Use this if you already have a set of
  /// (unique) morphs that you want to segment.
  /// @param first Beginning of morph container to copy from.
  /// @param last End of morph container to copy from.
  template <typename InputIterator>
  explicit SegmentationTree(InputIterator first, InputIterator last)
  {
    while (first != last)
    {
      total_morph_tokens_ += first->frequency();
      nodes_.emplace(first->letters(), first->frequency());
      ++first;
    }
  }

  /// Splits a morph in the segmentation tree into two parts. The morph must be
  /// in the segmentation tree already and must not have any children. The split
  /// position must be chosen so that the left and right strings have non-zero
  /// length.
  /// @param morph The word or morph in the data structure to split.
  /// @param left_length Length of left child after splitting.
  void Split(const std::string& morph, size_t left_length);

  /// Removes a morph from the data structure, properly decreases
  /// the frequency counts of its descendants, and cleans up any leaf
  /// nodes that no longer have anything referencing them.
  /// @param morph The morph or word to remove.
  void Remove(const std::string morph)
  {
    auto node = nodes_.find(morph);
    assert(node != end(nodes_));
    total_morph_tokens_ -= node->second.count;
    RemoveNode(node->second, node->first);
  }

  /// Updates the data structure by recursively finding the best split
  /// for each morph.
  void Optimize();

  /// Recursively finds the best split for a morph or word. Whereas regular
  /// Split will only split a morph once, and only where you tell it
  /// to, this will find the best way to split the morph, and it will
  /// recursively act on the resulting splits; this is what you want
  /// when when optimizing since in general you do not know how many
  /// morphs comprise a word or what the best split is.
  /// @param morph The word or morph to recursively split.
  void ResplitNode(const std::string& morph);

  /// Returns true if the given morph is in the data structure.
  /// @param morph The word or morph to look for.
  bool contains(const std::string& morph) const {
    return nodes_.find(morph) != nodes_.end();
  }

  /// Adds the given morph (with the given frequency) to the data structure.
  /// Requires that the given morph is not already in the data structure.
  void emplace(const std::string& morph, size_t frequency) {
    auto ret = nodes_.emplace(morph, frequency);
    //assert(ret.second);  // true iff element did not exist before
    total_morph_tokens_ += frequency;
  }

  /// Returns the morph node corresponding to the given morph.
  /// @param morph The given morph or word to look up.
  /// @throw out_of_range exception if the morph was not found.
  MorphNode& at(const std::string& morph) {
    return nodes_.at(morph);
  }

  /// Returns the morph node corresponding to the given morph.
  /// @param morph The given morph or word to look up.
  /// @throw out_of_range exception if the morph was not found.
  const MorphNode& at(const std::string& morph) const {
    return nodes_.at(morph);
  }

  /// Returns the number of unique morphs in the data structure.
  size_t size() const noexcept { return nodes_.size(); }

 private:
  /// Recursively removes a node rooted at a subtree. This is needed
  /// because a node's count adds to the count of all of its descendants,
  /// and when you remove a node you have to subtract its count from
  /// all of its descendants, plus clean up any descendants with
  /// no remaining morphs points to them. The most common usage for this
  /// is RemoveNode(node_to_remove, word_corresponding_to_node_to_remove).
  /// @param node_to_remove The node you want to completely remove.
  /// @param subtree_key The word or morph to remove the node from.
  void RemoveNode(const MorphNode& node_to_remove,
      const std::string& subtree_key);

  /// Calculates the probability of a morph.
  Probability pMorph(const Morph& morph)
  {
     return std::log(morph.frequency() / total_morph_tokens_);
  }

  /// Calculates the probability of the corpus given the model.
  Probability ProbabilityOfCorpusGivenModel() const;

  /// The data structure containing the morphs and their splits.
  std::unordered_map<std::string, MorphNode> nodes_;

  /// Placeholder probabilities.
  Probability pr_model_given_corpus_ = 0;
  Probability pr_corpus_given_model_ = 0;
  Probability pr_frequencies_ = 0;
  Probability pr_lengths_ = 0;

  /// Number of morph tokens in the data structure. Whereas size()
  /// returns the number of unique morphs, this number factors in
  /// the frequency of each morph in the corpus, as well as any
  /// changes due to splits or removals.
  size_t total_morph_tokens_ = 0;
};

} // namespace morfessor

#endif /* INCLUDE_MORPH_NODE_H_ */
