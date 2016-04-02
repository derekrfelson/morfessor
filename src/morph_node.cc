/*
 * Split.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: Derek Felson
 */

#include "morph_node.h"

#include <cassert>
#include <array>

#include "corpus.h"
#include "model.h"
#include "morph.h"

namespace morfessor
{

SegmentationTree::SegmentationTree()
    : nodes_{}
{
}

void SegmentationTree::Split(const std::string& morph, size_t left_length)
{
  assert(morph.size() > 1);
  assert(left_length > 0 && left_length < morph.size() - 1);
  MorphNode* node = &nodes_.find(morph)->second;
  assert(!node->has_children());

  node->left_child = morph.substr(0, left_length);
  node->right_child = morph.substr(left_length);
  nodes_[node->left_child].count += node->count;
  nodes_[node->right_child].count += node->count;
}

MorphNode::MorphNode()
    : MorphNode(0)
{
}

MorphNode::MorphNode(size_t count)
    : count{count}, left_child{}, right_child{}
{
}

// Recursively removes a node, rooted at a subtree, decreasing the node count
// of all its descendants and cleaning up any descendants that now have
// nothing pointing to them.
//
// node_to_remove: The node to remove
// subtree: The place to start recursively removing it from
void SegmentationTree::RemoveNode(const MorphNode& node_to_remove,
    const std::string& subtree_key)
{
  MorphNode& subtree = nodes_.at(subtree_key);

  // Recursively remove the nodes childrens, if they exist
  if (!subtree.left_child.empty())
  {
    RemoveNode(node_to_remove, subtree.left_child);
  }
  if (!subtree.right_child.empty())
  {
    RemoveNode(node_to_remove, subtree.right_child);
  }

  // Decrease the node count at the subtree
  subtree.count -= node_to_remove.count;
  // Decrease probabilities if subtree is leaf node
  if (!subtree.has_children())
  {
    pr_corpus_given_model_ -= 0;  // TODO: actual logprob
    pr_frequencies_ -= 0;  // TODO: actual logprob
  }
  // If nothing points to the subtree anymore, delete it
  if (subtree.count == 0)
  {
    if (!subtree.has_children())
    {
      pr_lengths_ -= 0;  // TODO: actual logprob
    }
    nodes_.erase(nodes_.find(subtree_key));
  }
}

// Builds the best model given the morphs and frequencies currently known
void SegmentationTree::Optimize()
{
  std::vector<std::string> keys;
  for (const auto& node_pair : nodes_)
  {
    keys.push_back(node_pair.first);
  }

  for (const auto& key : keys)
  {
    ResplitNode(key);
  }
}

void SegmentationTree::ResplitNode(const std::string& morph)
{
  auto frequency = nodes_.at(morph).count;

	// Remove the current representation of the node, if we have it
	if (nodes_.find(morph) != end(nodes_))
	{
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
	for (auto split_index = 1; split_index < morph.size(); ++split_index)
	{
	  std::array<std::string, 2> subnode_keys = {
	      morph.substr(0, split_index), morph.substr(split_index)
	  };
	  for (auto& key : subnode_keys)
	  {
	    if (contains(key))
	    {
	      MorphNode& subnode = nodes_.at(key);
	      subnode.count += frequency;
	      if (!subnode.has_children())
	      {
	        pr_corpus_given_model_ += 0;  // TODO: actual logprob
	        pr_frequencies_ += 0;  // TODO: actual logprob
	      }
	    }
	    else
	    {
	      emplace(morph, frequency);
	      pr_corpus_given_model_ += 0;  // TODO: actual logprob
	      pr_frequencies_ += 0;  // TODO: actual logprob
	      pr_lengths_ += 0;  // TODO: actual logprob
	    }
	  }
	  // TODO: check if we should use < or >
	  if (pr_model_given_corpus_ < best_solution_probability)
	  {
	    best_solution_probability = pr_model_given_corpus_;
	    best_solution_split_index = split_index;
	  }

	  // Restore old data structure and probability
	  nodes_ = data_structure_backup;
	  pr_model_given_corpus_ = current_pr_model_given_corpus;
	}

	// Select the best split or no split
	pr_model_given_corpus_ = best_solution_probability;
	if (best_solution_split_index > 0)
	{
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

} // namespace morfessor
