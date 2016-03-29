/*
 * Split.h
 *
 *  Created on: Mar 29, 2016
 *      Author: derek
 */

#ifndef INCLUDE_SPLIT_H_
#define INCLUDE_SPLIT_H_

#include "Types.h"
#include <memory>
#include <string>
class Morph;
class Corpus;
class Model;

class SplitTree
{
public:
	struct Node
	{
		size_t splitIndex;
		size_t count;
		std::unique_ptr<Node> leftChild;
		std::unique_ptr<Node> rightChild;
	};

	explicit SplitTree(std::string str, size_t count);
	size_t count() const;

private:
	std::string m_str;
	size_t m_count;
	std::unique_ptr<Node> m_root;
};

void splitWord(Morph& word, Model& model, const Corpus& corpus);
void resplitNode(SplitTree::Node& node, Model& model, const Corpus& corpus);

#endif /* INCLUDE_SPLIT_H_ */
