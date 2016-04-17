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

#include <cstddef>
#include <string>

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

inline bool MorphNode::has_children() const noexcept {
  return !(left_child.empty() || right_child.empty());
}

} // namespace morfessor

#endif /* INCLUDE_MORPH_NODE_H_ */
