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

#ifndef INCLUDE_SEGMENTATION_H_
#define INCLUDE_SEGMENTATION_H_

#include <cmath>
#include <cassert>
#include <unordered_map>
#include <iosfwd>
#include <memory>
#include <vector>
#include <string>

#include "morph.h"
#include "model.h"
#include "types.h"
#include "morph_node.h"

namespace morfessor {

/// Stores recursive segmentations of a set of words.
class Segmentation {
 public:
  /// C'tor that initializes the segmentation with every word in the
  /// training corpus as its own morph.
  /// @param corpus The words in the corpus and their frequencies.
  /// @param model The cost model to use when optimizing the segmentation.
  explicit Segmentation(const Corpus& training_corpus,
      std::shared_ptr<Model> model);

  /// Returns the best splits for a test corpus given the current segmentation.
  std::shared_ptr<std::vector<std::string> >
  SegmentTestCorpus(const Corpus& test_corpus);

  /// Updates the data structure by recursively finding the best split
  /// for each morph.
  void Optimize();

  /// Recursively finds the best split for a morph or word. Whereas regular
  /// Split will only split a morph once, and only where you tell it
  /// to, this will find the best way to split the morph, and it will
  /// recursively act on the resulting splits; this is what you want
  /// when when optimizing since in general you do not know how many
  /// morphs comprise a word or what the best split is.
  /// @param morph The word or morph to recursively split. Cannot be empty
  ///   string.
  void ResplitNode(std::string morph);

  /// Recursively update the morph count for all nodes rooted at a given node.
  /// If the given node does not exist, creates it. The morph count after
  /// adjusting by delta must never be negative.
  /// @param morph The morph to adjust the count of. Cannot be empty string.
  /// @param delta The amount to adjust the count by.
  void AdjustMorphCount(std::string morph, int delta);

  /// Returns true if the given morph is in the data structure.
  /// @param morph The word or morph to look for.
  bool contains(const std::string& morph) const;

  /// Returns the morph node corresponding to the given morph.
  /// @param morph The given morph or word to look up.
  /// @throw out_of_range exception if the morph was not found.
  MorphNode& at(const std::string& morph);

  /// \overload
  const MorphNode& at(const std::string& morph) const;

  /// Prints the current state of the model.
  /// @param out An output stream.
  std::ostream& print(std::ostream& out) const;

  /// Prints the current state of the model, in the expected format for a
  // corpus.
  /// @param out An output stream.
  std::ostream& print_as_corpus(std::ostream& out) const;

  /// Prints the current state of the model as a graphviz dot file.
  /// @param out An output stream.
  std::ostream& print_dot(std::ostream& out) const;

  /// \overload
  std::ostream& print_dot_debug() const;

 private:
  /// The data structure containing the morphs and their splits.
  std::unordered_map<std::string, MorphNode> nodes_;

  /// The probabilistic model that guides the segmentation.
  std::shared_ptr<Model> model_;
};

inline bool Segmentation::contains(const std::string& morph) const {
  return nodes_.find(morph) != nodes_.end();
}

inline MorphNode& Segmentation::at(const std::string& morph) {
  return nodes_.at(morph);
}

inline const MorphNode& Segmentation::at(const std::string& morph) const {
  return nodes_.at(morph);
}

/// Outputs the segmentation tree.
inline std::ostream& operator<<(std::ostream& out,
    const Segmentation& st) {
  return st.print(out);
}

}  // namespace morfessor

#endif /* INCLUDE_SEGMENTATION_H_ */
