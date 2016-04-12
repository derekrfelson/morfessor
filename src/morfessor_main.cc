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

#include <gflags/gflags.h>
#include <boost/math/distributions/gamma.hpp>

#include "morph_node.h"

using SegmentationTree = morfessor::SegmentationTree;

DEFINE_bool(use_explicit_freq, false,
    "enables explicit frequency calculation");
DEFINE_string(data, "", "word list to segment");
DEFINE_string(load, "", "pre-segmented word list to use as model");
DEFINE_double(zipffreqdistr, 0.5, "prior probability for "
    "proportion of morphs that only appear once");

static bool ValidateProportion(const char* flagname, double value) {
  return value > 0 && value < 1;
}

static bool ValidatePath(const char* flagname, const std::string& path) {
  return path == "" || access(path.c_str(), F_OK) != -1;
}

int main(int argc, char** argv)
{
  gflags::RegisterFlagValidator(&FLAGS_zipffreqdistr, &ValidateProportion);
  gflags::RegisterFlagValidator(&FLAGS_data, &ValidatePath);
  gflags::RegisterFlagValidator(&FLAGS_load, &ValidatePath);
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_use_explicit_freq)
  {
    std::cout << "use explicit freq" << std::endl;
  }

  if (FLAGS_zipffreqdistr)
  {
    std::cout << "zipffreqdistr: " << FLAGS_zipffreqdistr << std::endl;
  }

  auto prior = 7.0;
  auto beta = 1.0;
  auto alpha = prior / beta + 1;
  auto gd = boost::math::gamma_distribution<double>{alpha, beta};
  for (auto i = 1; i < 25; ++i)
  {
    std::cout << "gd[" << i << "] = " << boost::math::pdf(gd, i) << std::endl;
  }

  return 0;
}
