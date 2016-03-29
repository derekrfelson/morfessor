/*
 * Split.h
 *
 *  Created on: Mar 29, 2016
 *      Author: derek
 */

#ifndef INCLUDE_SPLIT_H_
#define INCLUDE_SPLIT_H_

#include <memory>
#include <string>

#include "types.h"

namespace morfessor
{

class Morph;
class Corpus;
class Model;

struct Node
{
	size_t split_index;
	size_t count;
	std::unique_ptr<Node> left_child;
	std::unique_ptr<Node> right_child;
};

void resplitnode(Node* node, Model* model, const Corpus& corpus);

} // namespace morfessor

#endif /* INCLUDE_SPLIT_H_ */
