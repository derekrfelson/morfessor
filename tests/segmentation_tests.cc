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

#include "segmentation.h"

#include <memory>
#include <sstream>
#include <iostream>

#include <gtest/gtest.h>

#include "corpus.h"
#include "model.h"
#include "corpus_loader.h"

using Model = morfessor::Model;
using BaselineFrequencyModel = morfessor::BaselineFrequencyModel;
using BaselineModel = morfessor::BaselineModel;
using BaselineLengthModel = morfessor::BaselineLengthModel;
using BaselineFrequencyLengthModel = morfessor::BaselineFrequencyLengthModel;
using Corpus = morfessor::Corpus;
using Segmentation = morfessor::Segmentation;
static auto corpus_loader = &morfessor::tests::corpus_loader;

constexpr double threshold = 0.0001;

template <class T>
static void test_against_reference(
    std::shared_ptr<T> calculated_model,
    const Segmentation& segmentation) {

  std::stringstream results;
  segmentation.print_as_corpus(results);
  //std::cout << std::endl << "sstream:" << std::endl;
  //segmentation.print_as_corpus(std::cout);
  //std::cout << "end of sstream" << std::endl << std::endl;
  Corpus results_corpus{results};
  T results_model(results_corpus);

  EXPECT_NEAR(results_model.overall_cost(),
      calculated_model->overall_cost(), threshold);
  EXPECT_NEAR(results_model.lexicon_cost(),
      calculated_model->lexicon_cost(), threshold);
  EXPECT_NEAR(results_model.corpus_cost(),
      calculated_model->corpus_cost(), threshold);
  EXPECT_NEAR(results_model.frequency_cost(),
      calculated_model->frequency_cost(), threshold);
  EXPECT_NEAR(results_model.length_cost(),
      calculated_model->length_cost(), threshold);
  EXPECT_NEAR(results_model.morph_string_cost(),
      calculated_model->morph_string_cost(), threshold);
  EXPECT_NEAR(results_model.lexicon_order_cost(),
      calculated_model->lexicon_order_cost(), threshold);
  EXPECT_EQ(results_model.total_morph_tokens(),
      calculated_model->total_morph_tokens());
  EXPECT_EQ(results_model.unique_morph_types(),
      calculated_model->unique_morph_types());
}

template <class T>
static void test_optimization(const Corpus& corpus) {
  auto model = std::make_shared<T>(corpus);
  Segmentation s1(corpus, model);
  s1.Optimize();
  test_against_reference(model, s1);
}

TEST(SegmentationTests, OptimizeBaselineLengthCorpus1) {
  test_optimization<BaselineLengthModel>(corpus_loader().corpus1);
}

TEST(SegmentationTests, OptimizeBaselineLengthCorpus2) {
  test_optimization<BaselineLengthModel>(corpus_loader().corpus2);
}

TEST(SegmentationTests, OptimizeBaselineLengthCorpus3) {
  test_optimization<BaselineLengthModel>(corpus_loader().corpus3);
}

TEST(SegmentationTests, OptimizeBaselineLengthCorpus4) {
  test_optimization<BaselineLengthModel>(corpus_loader().corpus4);
}

TEST(SegmentationTests, OptimizeBaselineFreqLengthCorpus1) {
  test_optimization<BaselineFrequencyLengthModel>(corpus_loader().corpus1);
}

TEST(SegmentationTests, OptimizeBaselineFreqLengthCorpus2) {
  test_optimization<BaselineFrequencyLengthModel>(corpus_loader().corpus2);
}

TEST(SegmentationTests, OptimizeBaselineFreqLengthCorpus3) {
  test_optimization<BaselineFrequencyLengthModel>(corpus_loader().corpus3);
}

TEST(SegmentationTests, OptimizeBaselineFreqLengthCorpus4) {
  test_optimization<BaselineFrequencyLengthModel>(corpus_loader().corpus4);
}

TEST(SegmentationTests, AdjustMorphCountCanRemoveNodes) {
  auto model1 = std::make_shared<BaselineFrequencyModel>(
      corpus_loader().corpus1);
  Segmentation s1(corpus_loader().corpus1, model1);

  s1.AdjustMorphCount("redoing", -2);

  std::stringstream expected_results;
  expected_results << "4 trying" << std::endl << "1 reopen" << std::endl;
  std::stringstream results;
  s1.print_as_corpus(results);
  EXPECT_EQ(expected_results.str(), results.str());
}

TEST(SegmentationTests, BaselineLengthSaneAfterSplitting) {
  auto model1 = std::make_shared<BaselineLengthModel>(
      corpus_loader().corpus1);
  Segmentation s1(corpus_loader().corpus1, model1);

  s1.AdjustMorphCount("redoing", -2);
  s1.AdjustMorphCount("re", 2);
  s1.AdjustMorphCount("doing", 2);

  test_against_reference(model1, s1);
}

TEST(SegmentationTests, BaselineFreqLengthSaneAfterSplitting) {
  auto model1 = std::make_shared<BaselineFrequencyLengthModel>(
      corpus_loader().corpus1);
  Segmentation s1(corpus_loader().corpus1, model1);

  s1.AdjustMorphCount("redoing", -2);
  s1.AdjustMorphCount("re", 2);
  s1.AdjustMorphCount("doing", 2);

  test_against_reference(model1, s1);
}
