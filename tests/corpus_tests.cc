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

#include "corpus.h"

#include <gtest/gtest.h>

#include "morph.h"

using Corpus = morfessor::Corpus;

TEST(CorpusTests, EmptyCorpusSize)
{
	auto corpus = Corpus("../testdata/EmptyCorpus.txt");
	EXPECT_EQ(0, corpus.size());
}

TEST(CorpusTests, OneLineCorpusSize)
{
	auto corpus = Corpus("../testdata/SingleCorpusRow.txt");
	EXPECT_EQ(1, corpus.size());
}

TEST(CorpusTests, CorpusSize)
{
	auto corpus = Corpus("../testdata/CorpusTestData.txt");
	EXPECT_EQ(4, corpus.size());
}

TEST(CorpusTests, EmptyCorpusIterate)
{
	auto corpus = Corpus("../testdata/EmptyCorpus.txt");
	for (const auto& m : corpus)
	{
		EXPECT_FALSE(true);
	}
}

TEST(CorpusTests, OneLineCorpusIterate)
{
	auto corpus = Corpus("../testdata/SingleCorpusRow.txt");
	auto count = 0;
	for (const auto& m : corpus)
	{
		++count;
		EXPECT_EQ(548, m.frequency());
		EXPECT_EQ("abandon", m.letters());
	}
	EXPECT_EQ(1, count);
}

TEST(CorpusTests, CorpusIterate)
{
	auto corpus = Corpus("../testdata/CorpusTestData.txt");
	EXPECT_EQ(4, corpus.size());
	auto count = 0;
	for (const auto& m : corpus)
	{
		++count;
	}
	EXPECT_EQ(4, count);

	auto iter = corpus.cbegin();
	EXPECT_EQ(548, iter->frequency());
	EXPECT_EQ("abandon", iter->letters());
	++iter;
	EXPECT_EQ(779, iter->frequency());
	EXPECT_EQ("deck", iter->letters());
	++iter;
	EXPECT_EQ(8, iter->frequency());
	EXPECT_EQ("decker", iter->letters());
	++iter;
	EXPECT_EQ(195, iter->frequency());
	EXPECT_EQ("declining", iter->letters());
	++iter;
	EXPECT_EQ(corpus.cend(), iter);
}
