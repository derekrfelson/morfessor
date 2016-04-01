/*
 * Split.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: Derek Felson
 */

#include "morph_node.h"

#include <cassert>

#include "corpus.h"
#include "model.h"
#include "morph.h"

namespace morfessor
{

SegmentationTree::SegmentationTree() noexcept
    : nodes_{}
{
}

void SegmentationTree::split(const std::string& morph, size_t left_length)
{
  assert(morph.size() > 1);
  assert(left_length > 0 && left_length < morph.size() - 1);
  auto node = nodes_.find(morph);
  assert(node != end(nodes_));
  assert(!node->first.has_children());

  auto new_left_node = MorphNode{morph.substr(0, left_length), node->second};
  auto new_right_node = MorphNode{morph.substr(left_length), node->second};
  auto left_child = nodes_.find(new_left_node);
  auto right_child = nodes_.find(new_right_node);

  if (left_child == end(nodes_))
  {
    nodes_.emplace(new_left_node, node->second);
  }
  else
  {
    left_child->second += node->second;
  }
  node->first.left_child_ =
      const_cast<MorphNode*>(&nodes_.find(new_left_node)->first);

  if (right_child == end(nodes_))
  {
    nodes_.emplace(new_right_node, node->second);
  }
  else
  {
    right_child->second += node->second;
  }
  node->first.right_child_ =
      const_cast<MorphNode*>(&nodes_.find(new_right_node)->first);
}

MorphNode::MorphNode(const std::string& morph, size_t count) noexcept
    : morph_{morph}, left_child_{nullptr}, right_child_{nullptr}
{
}

MorphNode::MorphNode(const Morph& morph) noexcept
    : MorphNode(morph.letters(), morph.frequency())
{
}

// Converts strings to morph nodes, enabling SegmentationTree.find(string)
MorphNode::MorphNode(const std::string& morph) noexcept
    : MorphNode(morph, 0)
{
}

// Recursively removes a node, rooted at a subtree, decreasing the node count
// of all its descendants and cleaning up any descendants that now have
// nothing pointing to them.
//
// node_to_remove: The node to remove
// subtree: The place to start recursively removing it from
void SegmentationTree::RemoveNode(const MorphNode& node_to_remove,
    const MorphNode& subtree)
{
  // Recursively remove the nodes childrens, if they exist
  if (subtree.left_child() != nullptr)
  {
    RemoveNode(node_to_remove, *subtree.left_child());
  }
  if (subtree.right_child() != nullptr)
  {
    RemoveNode(node_to_remove, *subtree.right_child());
  }

  // Decrease the node count at the subtree
  nodes_.at(subtree) -= nodes_.at(node_to_remove);
  // Decrease probabilities if subtree is leaf node
  if (!subtree.has_children())
  {
    pr_corpus_given_model_ -= 0;  // TODO: actual logprob
    pr_frequencies_ -= 0;  // TODO: actual logprob
  }
  // If nothing points to the subtree anymore, delete it
  if (nodes_.at(subtree) == 0)
  {
    if (!subtree.has_children())
    {
      pr_lengths_ -= 0;  // TODO: actual logprob
    }
    nodes_.erase(subtree);
  }
}

void SegmentationTree::ResplitNode(const MorphNode& node)
{
	// Node must correspond to an entire word or substring of a word

	// Remove the current representation of the node, if we have it
	if (nodes_.find(node) != end(nodes_))
	{
	  RemoveNode(node, node);
	}

	// First, try the node as a morph of its own
	//if (!model.contains(node))
	//{
	//	model.insert(node);
	//}
	// increase L(corpus|M) and L(frequencies) accordingly
	// add contribution of node to L(lengths)
	// bestSolution = [L(M | corpus), node]

	// Then try every split of the node into two substrings
	/*
	subtract contribution of node from L(M | corpus), but leave node in data structure

	store current L(M | corpus) and data structure
	for all substrings pre and suf such that pre ◦ suf = node do
	for subnode in [pre, suf] do
	if subnode is present in the data structure then
	for all nodes m in the subtree rooted at subnode do
	increase count(m) by count(node)
	increase L(corpus | M) and L(f μ 1 , . . . , f μ M ) if m is a leaf node
	else
	add subnode with count(node) into the data structure
	increase L(corpus | M) and L(f μ 1 , . . . , f μ M ) accordingly
	add contribution of subnode to L(s μ 1 , . . . , s μ M )
	if L(M | corpus) < code length stored in bestSolution then
	bestSolution ← [L(M | corpus), pre, suf]
	restore stored data structure and L(M | corpus)


	// Select the best split or no split

		// Proceed by splitting recursively
*/
}

} // namespace morfessor
