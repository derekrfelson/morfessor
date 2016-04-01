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

  if (right_child == end(nodes_))
  {
    nodes_.emplace(new_right_node, node->second);
  }
  else
  {
    right_child->second += node->second;
  }
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

void resplitnode(MorphNode* node, Model* model, const Corpus& corpus)
{
  std::vector<Morph> morphs;
  morphs.emplace_back("reopen", 1);
  SegmentationTree segmentations{morphs.begin(), morphs.end()};

	// Node must correspond to an entire word or substring of a word
/*


	// Remove the current representation of the node
	if (model.contains(node))
	{
		for (auto& m : node.descendants())
		{
			m.count -= node.count;
			if (!m.hasChildren())
			{
				// Decrease L(corpus|M) and L(frequencies) accordingly
			}
			if (m.count == 0)
			{
				model.remove(m);
				// Subtract contribution of m from L(lengths) if m is leaf node
			}
		}
	}

	// First, try the node as a morph of its own
	if (!model.contains(node))
	{
		model.insert(node);
	}
	// increase L(corpus|M) and L(frequencies) accordingly
	// add contribution of node to L(lengths)
	// bestSolution = [L(M | corpus), node]

	// Then try every split of the node into two substrings
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
