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

#include <cassert>
#include <array>
#include <iostream>

#include <boost/math/distributions/gamma.hpp>
#include <boost/math/special_functions/binomial.hpp>

#include "corpus.h"
#include "morph.h"

namespace morfessor {

SegmentationTree::SegmentationTree()
    : nodes_{} {}

void SegmentationTree::Split(const std::string& morph, size_t left_length) {
  assert(morph.size() > 1);
  assert(left_length > 0 && left_length < morph.size() - 1);
  auto found_node = nodes_.find(morph);
  assert(found_node != end(nodes_));
  MorphNode* node = &found_node->second;
  assert(!node->has_children());

  node->left_child = morph.substr(0, left_length);
  node->right_child = morph.substr(left_length);
  nodes_[node->left_child].count += node->count;
  nodes_[node->right_child].count += node->count;

  // We only count leaf nodes, and we're losing one leaf node
  // and adding two every time we split.
  total_morph_tokens_ += node->count;

  // We lost one unique morph by splitting what we started with, but we
  // may have gained up to two new unique morphs, depending on whether
  // the results of the split were already morphs we knew about.
  unique_morph_types_ += -1
      + static_cast<int>(nodes_[node->left_child].count == node->count)
      + static_cast<int>(nodes_[node->right_child].count == node->count);
}

Probability SegmentationTree::ProbabilityOfCorpusGivenModel() const {
  Probability sum = 0;
  for (const auto& iter : nodes_) {
    if (!iter.second.has_children()) {
      sum -= ProbabilityOfMorph(iter.first) * iter.second.count;
    }
  }
  return sum / std::log(2);
}

Probability SegmentationTree::ProbabilityFromImplicitFrequencies() const {
  // Formula without approximation
  if (total_morph_tokens_ < 100) {
    return std::log2(boost::math::binomial_coefficient<Probability>(
        total_morph_tokens_ - 1, unique_morph_types_ - 1));
  } else {
    // Formula with logarithmic approximation to binomial coefficients
    // based on Stirling's approximation
    //
    // return (total_morph_tokens_ - 1) * std::log2(total_morph_tokens_ - 2)
    // - (unique_morph_types - 1) * std::log2(unique_morph_types_ - 2)
    // - (total_morph_tokens_ - unique_morph_types_)
    //  * std::log2(total_morph_tokens_ - unique_morph_types_ - 1);
    //
    // The above should be the correct forumula to use here for a fast
    // approximation, but the Morfessor reference implementation uses
    // a slightly different version, which is used below.
    //
    // Formula from reference implementation
    return (total_morph_tokens_ - 1) * std::log2(total_morph_tokens_ - 2)
          - (unique_morph_types_ - 1) * std::log2(unique_morph_types_ - 2)
          - (total_morph_tokens_ - unique_morph_types_)
            * std::log2(total_morph_tokens_ - unique_morph_types_ - 1);
  }
}

Probability SegmentationTree::ProbabilityFromExplicitFrequencies() const {
  auto exponent = std::log2(1 - hapax_legomena_prior_);
  Probability sum = 0;
  for (const auto& iter : nodes_) {
    if (!iter.second.has_children()) {
      sum -= std::log2(std::pow(iter.second.count, exponent)
                      - std::pow(iter.second.count + 1, exponent));
    }
  }
  return sum;
}

std::unordered_map<char, Probability>
SegmentationTree::LetterProbabilities(bool include_end_of_string) const
{
  // Calculate the probabilities of each letter in the corpus
  std::unordered_map<char, Probability> letter_probabilities;
  size_t total_letters = 0;
  size_t unique_morphs = 0;
  size_t total_morph_tokens = 0;
  Probability end_of_morph_string_probability = 0;

  // Get the frequency of all the letters first
  for (const auto& iter : nodes_) {
    if (!iter.second.has_children()) {
      auto& morph_string = iter.first;
      auto& node = iter.second;
      ++unique_morphs;
      total_morph_tokens += node.count;
      for (auto c : morph_string)
      {
        total_letters += node.count;
        // letter_probabiltieis actually contains count at this point
        letter_probabilities[c] += node.count;
      }
    }
  }

  // Sanity check
  assert(unique_morphs == unique_morph_types_);
  assert(total_morph_tokens == total_morph_tokens_);

  if (include_end_of_string) {
    // We count the "end of morph" character as a letter
    total_letters += total_morph_tokens;
  }

  // Calculate the actual letter probabilities using maximum likelihood
  auto log_total_letters = std::log2(total_letters);
  for (auto iter : letter_probabilities)
  {
    letter_probabilities[iter.first] =
        log_total_letters - std::log2(letter_probabilities[iter.first]);
  }

  if (include_end_of_string) {
    // The "end of morph string" character can be understood to appear
    // at the end of every string, i.e. total_morph_tokens number of times.
    letter_probabilities['#'] =
        log_total_letters - std::log2(total_morph_tokens);
  }

  return letter_probabilities;
}

Probability SegmentationTree::ProbabilityFromImplicitLengths() const {
  auto letter_probabilities = LetterProbabilities(true);

  Probability sum = 0;
  for (const auto& iter : nodes_) {
    if (!iter.second.has_children()) {
      auto& node = iter.second;
      sum += letter_probabilities['#'];
    }
  }

  return sum;
}

Probability SegmentationTree::ProbabilityFromExplicitLengths(
    double prior, double beta) const {
  auto alpha = prior / beta + 1;
  auto gd = boost::math::gamma_distribution<double>{alpha, beta};

  Probability sum = 0;
  for (const auto& iter : nodes_) {
    if (!iter.second.has_children()) {
      sum -= std::log2(boost::math::pdf(gd, iter.first.length()));
    }
  }

  return sum;
}

Probability SegmentationTree::MorphStringCost(bool use_implicit_length) const {
  auto letter_probabilities = LetterProbabilities(use_implicit_length);
  Probability sum = 0;
  auto p_end = letter_probabilities['#'];
  auto p_not_end = 1 - p_end;

  for (const auto& iter : nodes_) {
    if (!iter.second.has_children()) {
      auto& morph_string = iter.first;
      auto& node = iter.second;
      for (auto c : morph_string)
      {
        sum += letter_probabilities[c];
      }
    }
  }

  return sum;
}

Probability SegmentationTree::ProbabilityAdjustmentFromLexiconOrdering()
const {
  // Use the first term of Sterling's approximation
  // log n! ~ n * log(n - 1)
  return (unique_morph_types_ * (1 - std::log(unique_morph_types_)))
      / std::log(2);
}

void SegmentationTree::RemoveNode(const MorphNode& node_to_remove,
    const std::string& subtree_key) {
  MorphNode& subtree = nodes_.at(subtree_key);

  // Recursively remove the nodes childrens, if they exist
  if (!subtree.left_child.empty()) {
    RemoveNode(node_to_remove, subtree.left_child);
  }
  if (!subtree.right_child.empty()) {
    RemoveNode(node_to_remove, subtree.right_child);
  }

  // Decrease the node count at the subtree
  auto count_reduction = node_to_remove.count;
  subtree.count -= count_reduction;

  // Decrease probabilities if subtree is leaf node
  if (!subtree.has_children()) {
    total_morph_tokens_ -= count_reduction;
    pr_corpus_given_model_ -= 0;  // TODO: actual logprob
    pr_frequencies_ -= 0;  // TODO: actual logprob
  }
  // If nothing points to the subtree anymore, delete it
  if (subtree.count == 0) {
    if (!subtree.has_children()) {
      unique_morph_types_ -= 1;
      pr_lengths_ -= 0;  // TODO: actual logprob
    }
    nodes_.erase(nodes_.find(subtree_key));
  }
}

Probability SegmentationTree::LexiconCost(AlgorithmModes mode) const {\
  auto sum = ProbabilityAdjustmentFromLexiconOrdering();
  switch (mode) {
  case AlgorithmModes::kBaseline:
    sum += ProbabilityFromImplicitFrequencies();
    sum += ProbabilityFromImplicitLengths();
    sum += MorphStringCost(true);
    break;
  case AlgorithmModes::kBaselineFreq:
    sum += ProbabilityFromExplicitFrequencies();
    sum += ProbabilityFromImplicitLengths();
    sum += MorphStringCost(true);
    break;
  case AlgorithmModes::kBaselineFreqLength:
    sum += ProbabilityFromExplicitFrequencies();
    sum += ProbabilityFromExplicitLengths();
    sum += MorphStringCost(false);
    break;
  case AlgorithmModes::kBaselineLength:
    sum += ProbabilityFromImplicitFrequencies();
    sum += ProbabilityFromExplicitLengths();
    sum += MorphStringCost(false);
    break;
  }
  return sum;
}

Probability SegmentationTree::OverallCost(AlgorithmModes mode) const {
  return LexiconCost(mode) + ProbabilityOfCorpusGivenModel();
}

void SegmentationTree::Optimize() {
  std::vector<std::string> keys;
  for (const auto& node_pair : nodes_) {
    keys.push_back(node_pair.first);
  }

  for (const auto& key : keys) {
    ResplitNode(key);
  }
}

void SegmentationTree::ResplitNode(const std::string& morph) {
  auto frequency = nodes_.at(morph).count;

	// Remove the current representation of the node, if we have it
	if (nodes_.find(morph) != end(nodes_)) {
	  RemoveNode(nodes_.at(morph), morph);
	}

	// First, try the node as a morph of its own
	emplace(morph, frequency);
	pr_corpus_given_model_ += 0;  // TODO: actual lobprob
	pr_frequencies_ += 0;  // TODO: actual logprob
	pr_lengths_ += 0;  // TODO: actual logprob

	// Save a copy of this as our current best solution
	auto best_solution_probability = pr_model_given_corpus_;
	size_t best_solution_split_index = 0;

	// Then try every split of the node into two substrings
	pr_model_given_corpus_ -= 0;  // TODO: actual logprob
	auto current_pr_model_given_corpus = pr_model_given_corpus_;
	auto data_structure_backup = nodes_;
	for (auto split_index = 1; split_index < morph.size(); ++split_index) {
	  std::array<std::string, 2> subnode_keys = {
	      morph.substr(0, split_index), morph.substr(split_index)
	  };
	  for (auto& key : subnode_keys) {
	    if (contains(key)) {
	      MorphNode& subnode = nodes_.at(key);
	      subnode.count += frequency;
	      if (!subnode.has_children()) {
	        pr_corpus_given_model_ += 0;  // TODO: actual logprob
	        pr_frequencies_ += 0;  // TODO: actual logprob
	      }
	    } else {
	      emplace(morph, frequency);
	      pr_corpus_given_model_ += 0;  // TODO: actual logprob
	      pr_frequencies_ += 0;  // TODO: actual logprob
	      pr_lengths_ += 0;  // TODO: actual logprob
	    }
	  }
	  // TODO: check if we should use < or >
	  if (pr_model_given_corpus_ < best_solution_probability) {
	    best_solution_probability = pr_model_given_corpus_;
	    best_solution_split_index = split_index;
	  }

	  // Restore old data structure and probability
	  nodes_ = data_structure_backup;
	  pr_model_given_corpus_ = current_pr_model_given_corpus;
	}

	// Select the best split or no split
	pr_model_given_corpus_ = best_solution_probability;
	if (best_solution_split_index > 0) {
	  auto left_key = morph.substr(0, best_solution_split_index);
	  auto right_key = morph.substr(best_solution_split_index);
	  nodes_[left_key].count += frequency;
	  nodes_[right_key].count += frequency;
	  nodes_.at(morph).left_child = left_key;
	  nodes_.at(morph).right_child = right_key;

	  // Proceed by splitting recursively
	  ResplitNode(left_key);
	  ResplitNode(right_key);
	}
}

MorphNode::MorphNode()
    : MorphNode(0) {}

MorphNode::MorphNode(size_t count)
    : count{count}, left_child{}, right_child{} {}

} // namespace morfessor
