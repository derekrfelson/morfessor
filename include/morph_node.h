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

namespace morfessor {

/// Represents a possible split or a word or morph into two smaller morphs.
struct MorphNode {
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
  bool has_children() const noexcept;

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
class SegmentationTree {
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
  explicit SegmentationTree(InputIterator first, InputIterator last);

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
  void Remove(const std::string morph);

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

  /// Calculates the probability of a morph. The given word or morph must
  /// be in the data structure.
  /// @param morph The word or morph you want the probability of.
  Probability ProbabilityOfMorph(const std::string& morph) const;

  /// Calculates the probability of the corpus given the model.
  Probability ProbabilityOfCorpusGivenModel() const;

  /// Calculates the contribution of the frequencies in the corpus
  /// to the probability of the lexicon given the model. Uses the explicit
  /// version of the frequency calculation, which takes the individual morph
  /// frequencies into account, as well as the prior for the proportion of
  /// hapax legomena in the corpus. The result is a power law curve obtained
  /// from a derivation of the Zipf-Mandlebrot law.
  /// @see set_hapax_legomena_prior
  /// @return Code length (-log_2 probability)
  Probability ProbabilityFromExplicitFrequencies() const;

  /// Sets the prior belief for the proportion of morphs that only occur once
  /// in the corpus.
  /// @value The expected proportion. Must be in range (0,1)
  void set_hapax_legomena_prior(double value);

  /// Calculates the contribution of the frequencies in the corpus
  /// to the probability of the lexicon given the model. Uses the implicit
  /// version of the frequency calculation, which involves an uninformative
  /// prior that does not take individual morph frequencies into account.
  /// The result is an exponential distribution, where higher frequencies
  /// are exponentially less likely.
  /// @return Code length (-log_2 probability)
  Probability ProbabilityFromImplicitFrequencies() const;

  /// Calculates the cost of the morph lengths using a Gamma distribution
  /// with 2 parameters.
  /// @param prior The prior for the most common morph length, such that
  ///   0 < prior < 24*beta
  /// @param beta The beta value of the Gamma distribution, such that beta > 0.
  /// @return Code length (-log_2 probability)
  Probability ProbabilityFromExplicitLengths(double prior = 7.0,
      double beta = 1.0) const;

  /// Calculates cost of the morph lenghts using an exponential distribution
  /// with no parameters. The likelihood of a morph of a given length decreases
  /// exponentially with its length.
  /// @return Code length (-log_2 probability)
  Probability ProbabilityFromImplicitLengths() const;

  /// Calculates the adjustment to the probability of the lexicon based
  /// on the number of ways there are to order each morph in the lexicon.
  /// @return Code length (-log_2 probability)
  Probability ProbabilityAdjustmentFromLexiconOrdering() const;

  /// Calculates the contribution of the letters in each of the morphs
  /// to the probability of the lexicon given the model. Can also be
  /// understood as the morph string cost.
  /// @return Code length (-log_2 probability)
  Probability ProbabilityFromLetters() const;

  /// Calculates the probabilities of each letter in the corpus, and the
  /// end-of-morph marker.
  /// @return A map of letters to code lengths (-log_2 probability)
  std::unordered_map<char, Probability> LetterProbabilities() const;

  /// Returns true if the given morph is in the data structure.
  /// @param morph The word or morph to look for.
  bool contains(const std::string& morph) const;

  /// Adds the given morph (with the given frequency) to the data structure.
  /// Requires that the given morph is not already in the data structure.
  void emplace(const std::string& morph, size_t frequency);

  /// Returns the morph node corresponding to the given morph.
  /// @param morph The given morph or word to look up.
  /// @throw out_of_range exception if the morph was not found.
  MorphNode& at(const std::string& morph);

  /// Returns the morph node corresponding to the given morph.
  /// @param morph The given morph or word to look up.
  /// @throw out_of_range exception if the morph was not found.
  const MorphNode& at(const std::string& morph) const;

  /// Returns the number of nodes in the data structure. This does not form
  /// part of the probabalistic model.
  size_t size() const noexcept;

  /// Returns the number of morph tokens (unique morphs * their frequencies).
  size_t total_morph_tokens() const noexcept;

  /// Returns the number of unique morphs in the data structure.
  size_t unique_morph_types() const noexcept;

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

  /// The data structure containing the morphs and their splits.
  std::unordered_map<std::string, MorphNode> nodes_;

  /// Placeholder probabilities.
  Probability pr_model_given_corpus_ = 0;
  Probability pr_corpus_given_model_ = 0;
  Probability pr_frequencies_ = 0;
  Probability pr_lengths_ = 0;

  /// Number of morph tokens in the data structure. Whereas unique_morph_types_
  /// equals the number of unique morphs, this number factors in the frequency
  /// of each morph in the corpus.
  size_t total_morph_tokens_ = 0;

  // Number of unique morphs in the data structure. Unlike total_morph_tokens_,
  // this number ignores the frequency, only counting each unique morph once.
  size_t unique_morph_types_ = 0;

  /// The prior belief for the proportion of morphs that only occur once
  /// in the corpus. Typically this value is between 0.4 and 0.6 for English.
  /// It must be in the range (0,1).
  double hapax_legomena_prior_ = 0.5;

  /// Contains the probabilities of each letter in the corpus.
  /// The "end of morph" marker is '#'.
  std::unordered_map<char, Probability> letter_probabilities_;
};

inline bool MorphNode::has_children() const noexcept {
  return !(left_child.empty() && right_child.empty());
}

template <typename InputIterator>
SegmentationTree::SegmentationTree(InputIterator first, InputIterator last) {
  while (first != last) {
    total_morph_tokens_ += first->frequency();
    unique_morph_types_ += 1;
    nodes_.emplace(first->letters(), first->frequency());
    ++first;
  }
}

inline void SegmentationTree::Remove(const std::string morph) {
  auto node = nodes_.find(morph);
  assert(node != end(nodes_));
  RemoveNode(node->second, node->first);
}

inline Probability SegmentationTree::ProbabilityOfMorph(
    const std::string& morph) const {
  assert(contains(morph));
  return std::log(static_cast<Probability>(nodes_.at(morph).count)
      / total_morph_tokens_);
}

inline bool SegmentationTree::contains(const std::string& morph) const {
  return nodes_.find(morph) != nodes_.end();
}

inline void SegmentationTree::emplace(const std::string& morph,
    size_t frequency) {
  auto ret = nodes_.emplace(morph, frequency);
  //assert(ret.second);  // true iff element did not exist before
  total_morph_tokens_ += frequency;
  unique_morph_types_ += 1;
}

inline MorphNode& SegmentationTree::at(const std::string& morph) {
  return nodes_.at(morph);
}

inline const MorphNode& SegmentationTree::at(const std::string& morph) const {
  return nodes_.at(morph);
}

inline size_t SegmentationTree::size() const noexcept {
  return nodes_.size();
}

inline void SegmentationTree::set_hapax_legomena_prior(double value) {
  assert(value > 0 && value < 1);
  hapax_legomena_prior_ = value;
}


inline size_t SegmentationTree::total_morph_tokens() const noexcept {
  return total_morph_tokens_;
}

inline size_t SegmentationTree::unique_morph_types() const noexcept {
  return unique_morph_types_;
}

} // namespace morfessor

#endif /* INCLUDE_MORPH_NODE_H_ */
