/*
 * Split.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: Derek Felson
 */

#include "morph_node.h"

#include "corpus.h"
#include "model.h"
#include "morph.h"

namespace morfessor
{

SegmentationTree::SegmentationTree() noexcept
    : nodes_{}
{
}

void SegmentationTree::split(std::string morph, size_t split_index)
{

}

MorphNode::MorphNode(std::string morph, size_t count) noexcept
    : morph_{morph}, count_{count}, left_child_{nullptr},
      right_child_{nullptr}
{
}

MorphNode::MorphNode(const Morph& morph) noexcept
    : MorphNode(morph.letters(), morph.frequency())
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
