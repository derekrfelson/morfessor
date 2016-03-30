/*
 * Model.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#include "model.h"

#include <unordered_map>
#include <string>

#include "morph_node.h"

namespace morfessor
{

Model BuildModel(const Corpus& corpus)
{
  Probability pr_model_given_corpus = 0;
  Probability pr_corpus_given_model = 0;
  Probability pr_frequencies = 0;
  Probability pr_lengths = 0;
}

} // namespace morfessor

// Option 1: map from string->Node for all nodes

// contains       2 yes.
// descendants    2 yes. Need to implement an iterator on Node
// has_children   2 yes. Simple check on Node
// remove         2 yes.
// insert         2 yes
// select all leaf nodes 2 yes. Just use standard map iterator to go through. If it's a leaf node, then do something.
