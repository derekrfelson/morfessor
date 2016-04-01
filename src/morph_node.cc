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
  auto node = &nodes_.find(morph)->second;
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
  auto& subtree = nodes_.at(subtree_key);

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

void SegmentationTree::ResplitNode(const std::string& morph)
{
	// Node must correspond to an entire word or substring of a word

	// Remove the current representation of the node, if we have it
	if (nodes_.find(morph) != end(nodes_))
	{
	  RemoveNode(nodes_.at(morph), morph);
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
