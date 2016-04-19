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

#include <unistd.h>

#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>

#include <gflags/gflags.h>

#include "corpus.h"
#include "model.h"
#include "segmentation.h"

using Corpus = morfessor::Corpus;
using Segmentation = morfessor::Segmentation;
using AlgorithmModes = morfessor::AlgorithmModes;
using Model = morfessor::Model;

DEFINE_string(mode, "Baseline", "algorithm version to use "
    "(Baseline, Freq, Length, FreqLength)");
DEFINE_string(data, "", "word list to segment");
DEFINE_string(load, "", "pre-segmented word list to use as model");
DEFINE_double(hapax, 0.5, "prior probability for "
    "proportion of morphs that only appear once. Must be in range (0,1)");
DEFINE_double(finish, 0.005, "threshold for when to stop trying to improve"
    " the lexicon cost. Must be in range (0,1)");
DEFINE_double(most_common_length, 7, "most common morph length");
DEFINE_double(beta, 1.0, "beta value for morph length Gamma "
    "distribution");

static bool ValidateProportion(const char* flagname, double value) {
  return value > 0 && value < 1;
}

static bool ValidateLoad(const char* flagname, const std::string& path) {
  return path == "" || access(path.c_str(), F_OK) != -1;
}

static bool ValidateData(const char* flagname, const std::string& path) {
  return access(path.c_str(), F_OK) != -1;
}

static bool ValidateMode(const char* flagname, const std::string& mode) {
  return mode == "Baseline" || mode == "Freq" || mode == "Length" ||
      mode == "FreqLength";
}

static bool ValidateBeta(const char* flagname, double beta) {
  return beta > 0;
}

static bool ValidateLength(const char* flagname, double length) {
  return length > 0 && length < 24*FLAGS_beta;
}

int main(int argc, char** argv)
{
  gflags::RegisterFlagValidator(&FLAGS_hapax, &ValidateProportion);
  gflags::RegisterFlagValidator(&FLAGS_finish, &ValidateProportion);
  gflags::RegisterFlagValidator(&FLAGS_data, &ValidateData);
  gflags::RegisterFlagValidator(&FLAGS_load, &ValidateLoad);
  gflags::RegisterFlagValidator(&FLAGS_mode, &ValidateMode);
  gflags::RegisterFlagValidator(&FLAGS_most_common_length, &ValidateLength);
  gflags::RegisterFlagValidator(&FLAGS_beta, &ValidateBeta);

  google::ParseCommandLineFlags(&argc, &argv, true);

  std::shared_ptr<Corpus> corpus = nullptr;
  std::shared_ptr<Model> model = nullptr;

  if (FLAGS_load.empty()) {
    corpus = std::make_shared<Corpus>(FLAGS_data);
  } else {
    corpus = std::make_shared<Corpus>(FLAGS_load);
  }

  // Set algorithm parameters
  if (FLAGS_mode == "FreqLength") {
    model = std::make_shared<morfessor::BaselineModel>(*corpus);
  } else if (FLAGS_mode == "Freq") {
    model = std::make_shared<morfessor::BaselineFrequencyModel>(*corpus,
        FLAGS_hapax);
  } else if (FLAGS_mode == "Length") {
    model = std::make_shared<morfessor::BaselineLengthModel>(*corpus,
        FLAGS_most_common_length, FLAGS_beta);
  } else {
    model = std::make_shared<morfessor::BaselineFrequencyLengthModel>(*corpus,
        FLAGS_hapax, FLAGS_most_common_length, FLAGS_beta);
  }

  if (FLAGS_load.empty()) {
    Segmentation st(*corpus, model);
    st.Optimize();
    auto out = std::ofstream("output.dot");
    st.print_dot(out);
    std::cout << st;
  } else {
    Segmentation st(*corpus, model);
    Corpus test_corpus{FLAGS_data};
    auto segments = st.SegmentTestCorpus(test_corpus);
    for (auto word_splits : *segments) {
      std::cout << word_splits << std::endl;
    }
  }

  return 0;
}
