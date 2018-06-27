#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "RegularFinalStateTransducerBuilder.h"
#include "Tests.h"

struct TestCaseInfo
{
	std::string regExpr;
	bool infinite;
	bool functional;
	typedef std::pair<std::string,  // Word for traversing and expected outputs.
					  std::vector<unsigned>> // a list of desired outputs (if empty then the word is not from the language)
		WordAndExpectedOutputs;
	std::vector<WordAndExpectedOutputs> wordsAndExpectedOutputs;
};

typedef std::vector<TestCaseInfo>
	FinalStateTransducerTestCases;

static FinalStateTransducerTestCases FSTTestcases;

static void PopulateWithTestCases()
{
	FSTTestcases.push_back({
		"a:5",
		false,
		true,
		{
			{ "", {} },
			{ "b", {} },
			{ "aa", {} },
			{ "a", { 5 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 *",
		false,
		true,
		{
			{ "", { 0 } },
			{ "b", {} },
			{ "a", { 5 } },
			{ "aa", { 10 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 a:100 | *",
		false,
		true,
		{
			{ "", { 0 } },
			{ "b", {} },
			{ "a", { 5, 100 } },
			{ "aa", { 10, 105, 200 } },
			{ "aaa", { 15, 110, 205, 300 } },
			{ "aaaa", { 20, 115, 210, 305, 400 } },
			{ "aaaaa", { 25, 120, 215, 310, 405, 500 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 a:100 | * c:40 * .",
		false,
		true,
		{
			{ "", { 0 } },
			{ "b", {} },
			{ "a", { 5, 100 } },
			{ "aa", { 10, 105, 200 } },
			{ "aaa", { 15, 110, 205, 300 } },
			{ "aaaa", { 20, 115, 210, 305, 400 } },
			{ "aaaaa", { 25, 120, 215, 310, 405, 500 } },
			{ "aaaaac", { 65, 160, 255, 350, 445, 540 } },
			{ "aaaac", { 60, 155, 250, 345, 440 } },
			{ "aaac", { 55, 150, 245, 340 } },
			{ "aac", { 50, 145, 240 } },
			{ "ac", { 45, 140 } },
			{ "c", { 40 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 |",
		false,
		true,
		{
			{ "", {} },
			{ "x", {} },
			{ "xasffsaq", {} },
			{ "a", { 5 } },
			{ "c", { 1 } },
			{ "b", { 100 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 .",
		false,
		true,
		{
			{ "", {} },
			{ "x", {} },
			{ "c", {} },
			{ "b", {} },
			{ "c", {} },
			{ "ab", {} },
			{ "xasffsaq", {} },
			{ "ac", { 6 } },
			{ "bc", { 101 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 . *",
		false,
		true,
		{
			{ "", { 0 } },
			{ "x", {} },
			{ "c", {} },
			{ "b", {} },
			{ "c", {} },
			{ "ab", {} },
			{ "bcc", {} },
			{ "xasffsaq", {} },
			{ "ac", { 6 } },
			{ "bc", { 101 } },
			{ "bcbc", { 202 } },
			{ "bcac", { 107 } },
			{ "acbc", { 107 } },
			{ "acac", { 12 } },
			{ "acacbc", { 113 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 . * d:3 |",
		false,
		true,
		{
			{ "", { 0 } },
			{ "x", {} },
			{ "c", {} },
			{ "b", {} },
			{ "c", {} },
			{ "ab", {} },
			{ "bcc", {} },
			{ "xasffsaq", {} },
			{ "acacbcd", {} },
			{ "dacacbc", {} },
			{ "d", { 3 } },
			{ "ac", { 6 } },
			{ "bc", { 101 } },
			{ "bcbc", { 202 } },
			{ "bcac", { 107 } },
			{ "acbc", { 107 } },
			{ "acac", { 12 } },
			{ "acacbc", { 113 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 . * d:3 | *",
		false,
		true,
		{
			{ "", { 0 } },
			{ "x", {} },
			{ "c", {} },
			{ "b", {} },
			{ "c", {} },
			{ "ab", {} },
			{ "bcc", {} },
			{ "xasffsaq", {} },
			{ "dacdadcd", {} },
			{ "d", { 3 } },
			{ "ac", { 6 } },
			{ "bc", { 101 } },
			{ "bcbc", { 202 } },
			{ "bcac", { 107 } },
			{ "acbc", { 107 } },
			{ "acac", { 12 } },
			{ "acacbc", { 113 } },
			{ "acacbcd", { 116 } },
			{ "dacacbc", { 116 } },
			{ "dacacbcd", { 119 } },
			{ "acacd", { 15 } },
			{ "dacacd", { 18 } },
			{ "dacdacd", { 21 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 . * d:3 | * x:7 .",
		false,
		true,
		{
			{ "", {} },
			{ "c", {} },
			{ "b", {} },
			{ "c", {} },
			{ "ab", {} },
			{ "bcc", {} },
			{ "xasffsaq", {} },
			{ "dacdadcd", {} },
			{ "d", {} },
			{ "ac", {} },
			{ "bc", {} },
			{ "bcbc", {} },
			{ "bcac", {} },
			{ "acbc", {} },
			{ "acac", {} },
			{ "acacbc", {} },
			{ "acacbcd", {} },
			{ "dacacbc", {} },
			{ "dacacbcd", {} },
			{ "acacd", {} },
			{ "dacacd", {} },
			{ "dacdacd", {} },
			{ "x", { 7 } },
			{ "dx", { 10 } },
			{ "acx", { 13 } },
			{ "bcx", { 108 } },
			{ "bcbcx", { 209 } },
			{ "bcacx", { 114 } },
			{ "acbcx", { 114 } },
			{ "acacx", { 19 } },
			{ "acacbcx", { 120 } },
			{ "acacbcdx", { 123 } },
			{ "dacacbcx", { 123 } },
			{ "dacacbcdx", { 126 } },
			{ "acacdx", { 22 } },
			{ "dacacdx", { 25 } },
			{ "dacdacdx", { 28 } },
		}
	});
	FSTTestcases.push_back({
		"a:1 b:2 c:3 d:4 e:5 . . . .",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "abcde", { 15 } },
		}
	});
	FSTTestcases.push_back({
		"a:1 b:2 c:3 d:4 e:5 . . . . +",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "abcdeabcd", {} },
			{ "abcdea", {} },
			{ "abcdeabcd", {} },
			{ "abcdea", {} },
			{ "abcde", { 15 } },
			{ "abcdeabcde", { 30 } },
			{ "abcdeabcdeabcde", { 45 } },
		}
	});
	FSTTestcases.push_back({
		"a:1 b:2 * c:3 d:4 e:5 . . . .",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "acde", { 13 } },
			{ "abcde", { 15 } },
			{ "abbcde", { 17 } },
			{ "abbbbbcde", { 23 } },
		}
	});
	FSTTestcases.push_back({
		"a:1 b:2 * c:3 d:4 e:5 . . . . g:6 |",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "ag", {} },
			{ "abg", {} },
			{ "abcg", {} },
			{ "abcdg", {} },
			{ "abcdeeg", {} },
			{ "bcdeg", {} },
			{ "bdfg", {} },
			{ "aeg", {} },
			{ "acdeg", {} },
			{ "abcdeg", {} },
			{ "abbcdeg", {} },
			{ "abbbbbcdeg", {} },
			{ "gg",{ } },
			{ "g", { 6 } },
			{ "acde", { 13 } },
			{ "abcde", { 15 } },
			{ "abbcde", { 17 } },
			{ "abbbbbcde", { 23 } },
		}
	});
	FSTTestcases.push_back({
		"abcde:15",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "abcde", { 15 } },
		}
	});
	FSTTestcases.push_back({
		"abcde:15 +",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "abcdeabcd", {} },
			{ "abcdea", {} },
			{ "abcdeabcd", {} },
			{ "abcdea", {} },
			{ "abcde", { 15 } },
			{ "abcdeabcde", { 30 } },
			{ "abcdeabcdeabcde", { 45 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 :3 .",
		false,
		false,
		{
			{ "", {} },
			{ "a", { 8 } },
			{ "ab", {} },
			{ "aa", {} },
			{ "aaa", {} },
			{ "b", {} },
			{ "c", {} },
		}
	});
	FSTTestcases.push_back({
		"a:5 :3 . *",
		false,
		false,
		{
			{ "ab", {} },
			{ "b", {} },
			{ "aba", {} },
			{ "abaa", {} },
			{ "c", {} },
			{ "", { 0 } },
			{ "a", { 8 } },
			{ "aa", { 16 } },
			{ "aaa", { 24 } },
		}
	});
	FSTTestcases.push_back({
		"tony:5 :3 . *",
		false,
		false,
		{
			{ "tonyb", {} },
			{ "b", {} },
			{ "tonybtony", {} },
			{ "tonybtonytony", {} },
			{ "c", {} },
			{ "", { 0 } },
			{ "tony", { 8 } },
			{ "tonytony", { 16 } },
			{ "tonytonytony", { 24 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 :3 |",
		false,
		false,
		{
			{ "ab", {} },
			{ "aa", {} },
			{ "b", {} },
			{ "aba", {} },
			{ "abaa", {} },
			{ "c", {} },
			{ "", { 3, 0 } },
			{ "a", { 5 } },
		}
	});
	FSTTestcases.push_back({
		":3 a:5 |",
		false,
		false,
		{
			{ "ab", {} },
			{ "aa", {} },
			{ "b", {} },
			{ "aba", {} },
			{ "abaa", {} },
			{ "c", {} },
			{ "", { 3, 0 } },
			{ "a", { 5 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 :3 | *",
		true,
		false,
		{
		}
	});
	FSTTestcases.push_back({
		":35 abc:15 . * ef:17 |",
		false,
		false,
		{
			{ "abcef", {} },
			{ "e", {} },
			{ "aba", {} },
			{ "abaa", {} },
			{ "c", {} },
			{ "abcabcef", {} },
			{ "", { 0 } },
			{ "abc", { 50 } },
			{ "ef", { 17 } },
			{ "abcabc", { 100 } },
			{ "abcabcabc", { 150 } },
		}
	});
	FSTTestcases.push_back({
		":35 abc:15 . + ef:17 |",
		false,
		false,
		{
			{ "abcef", {} },
			{ "e", {} },
			{ "aba", {} },
			{ "abaa", {} },
			{ "c", {} },
			{ "abcabcef", {} },
			{ "", {} },
			{ "abc", { 50 } },
			{ "ef", { 17 } },
			{ "abcabc", { 100 } },
			{ "abcabcabc", { 150 } },
		}
	});
	FSTTestcases.push_back({
		":35 :45 :55 | | abc:15 . + ef:17 |",
		false,
		false,
		{
			{ "abcef", {} },
			{ "e", {} },
			{ "aba", {} },
			{ "abaa", {} },
			{ "c", {} },
			{ "abcabcef", {} },
			{ "", {} },
			{ "abc", { 50, 60, 70 } },
			{ "ef", { 17 } },
			{ "abcabc", { 100, 110, 120, 130, 140 } },
			{ "abcabcabc", { 150, 160, 170, 180, 190, 200, 210 } },
		}
	});
	FSTTestcases.push_back({
		":100 axs:5 .",
		false,
		false,
		{
			{ "", {} },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s",{} },
			{ "a",{} },
			{ "axs", { 105 } },
		}
	});
	FSTTestcases.push_back({
		"axs:5 :100  .",
		false,
		false,
		{
			{ "", {} },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s", {} },
			{ "a", {} },
			{ "axs", { 105 } },
		}
	});
	FSTTestcases.push_back({
		":100 :200 axs:5 | |",
		false,
		false,
		{
			{ "", { 100, 200, 0 } },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s", {} },
			{ "a", {} },
			{ "axs", { 5 } },
		}
	});
	FSTTestcases.push_back({
		":100 :200 axs:5 | | :300 :400 | |",
		false,
		false,
		{
			{ "", { 100, 200, 300, 400, 0 } },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s", {} },
			{ "a", {} },
			{ "axs", { 5 } },
		}
	});
	FSTTestcases.push_back({
		":100 :200 axs:5 . . :300 :400 | |",
		false,
		false,
		{
			{ "", { 0, 300, 400 } },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s", {} },
			{ "a", {} },
			{ "axs", { 305 } },
		}
	});
	FSTTestcases.push_back({
		":100 :200 axs:5 . . :300 :400 . .",
		false,
		false,
		{
			{ "", {} },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s", {} },
			{ "a", {} },
			{ "axs", { 1005 } },
		}
	});
	FSTTestcases.push_back({
		":100 :200 axs:5 . . :300 :400 . . axs:666 |",
		false,
		false,
		{
			{ "", {} },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s", {} },
			{ "a", {} },
			{ "axs", { 1005, 666 } },
		}
	});
	FSTTestcases.push_back({
		":100 :200 axs:5 . . :300 :400 . . axs:666 | :4 .",
		false,
		false,
		{
			{ "", {} },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s", {} },
			{ "a", {} },
			{ "axs", { 1009, 670 } },
		}
	});
	FSTTestcases.push_back({
		":100 :200 axs:5 . . :300 :400 . . axs:666 | :4 . :0 .",
		false,
		false,
		{
			{ "", {} },
			{ "axss", {} },
			{ "aaxsa", {} },
			{ "aaxs", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "x", {} },
			{ "s", {} },
			{ "a", {} },
			{ "axs", { 1009, 670 } },
		}
	});
	FSTTestcases.push_back({
		":0",
		false,
		false,
		{
			{ "", { 0 } },
			{ "a", {} },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
		}
	});
	FSTTestcases.push_back({
		":10",
		false,
		false,
		{
			{ "", { 0, 10 } },
			{ "a",{} },
			{ "xx",{} },
			{ "aw",{} },
			{ "ax",{} },
			{ "xs",{} },
		}
	});
	FSTTestcases.push_back({
		":10 :20 |",
		false,
		false,
		{
			{ "", { 0, 10, 20 } },
			{ "a", {} },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
		}
	});
	FSTTestcases.push_back({
		":10 :20 .",
		false,
		false,
		{
			{ "", { 0, 30 } },
			{ "a", {} },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
		}
	});
	FSTTestcases.push_back({
		":10 :20 . a:6 .",
		false,
		false,
		{
			{ "", {} },
			{ "aa", {} },
			{ "aba", {} },
			{ "ax", {} },
			{ "xa", {} },
			{ "a", { 36 } },
		}
	});
	FSTTestcases.push_back({
		":10 :20 . words:6 .",
		false,
		false,
		{
			{ "", {} },
			{ "wordswords", {} },
			{ "word", {} },
			{ "ords", {} },
			{ "ord", {} },
			{ "words", { 36 } },
		}
	});
	FSTTestcases.push_back({
		":10 :20 words:6 . .",
		false,
		false,
		{
			{ "", {} },
			{ "wordswords", {} },
			{ "word", {} },
			{ "ords", {} },
			{ "ord", {} },
			{ "words", { 36 } },
		}
	});
	FSTTestcases.push_back({
		"words:6 :10 . :20 .",
		false,
		false,
		{
			{ "", {} },
			{ "wordswords", {} },
			{ "word", {} },
			{ "ords", {} },
			{ "ord", {} },
			{ "words", { 36 } },
		}
	});
	FSTTestcases.push_back({
		"words:6 :10 :20 . .",
		false,
		false,
		{
			{ "", {} },
			{ "wordswords", {} },
			{ "word", {} },
			{ "ords", {} },
			{ "ord", {} },
			{ "words", { 36 } },
		}
	});
	FSTTestcases.push_back({
		":10 :30 :50 . .",
		false,
		false,
		{
			{ "", { 0, 90 } },
			{ "a", {} },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
		}
	});
	FSTTestcases.push_back({
		":10 :30 :50 | |",
		false,
		false,
		{
			{ "", { 0, 10, 30, 50 } },
			{ "a", {} },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
		}
	});
	FSTTestcases.push_back({
		"a:5 :0 | *",
		false,
		false,
		{
			{ "", { 0 } },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "a", { 5 } },
			{ "aa", { 10 } },
			{ "aaa", { 15 } },
		}
	});
	FSTTestcases.push_back({
		":10 a:5 | *",
		true,
		false,
		{
			{ "", {} },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "a", {} },
			{ "aa", {} },
			{ "aaa", {} },
		}
	});
	FSTTestcases.push_back({
		":10 a:5 . *",
		false,
		false,
		{
			{ "", { 0 } },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "a", { 15 } },
			{ "aa", { 30 } },
			{ "aaa", { 45 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 :10 * . b:6 |",
		true,
		false,
		{
			{ "", {} },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "a", {} },
			{ "aa", {} },
			{ "aaa", {} },
			{ "bb", {} },
			{ "b", { 6 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 :10 * . b:6 .",
		true,
		false,
		{
			{ "", {} },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "a", {} },
			{ "aa", {} },
			{ "ab", {} },
			{ "aaa", {} },
			{ "bb", {} },
			{ "b", {} },
		}
	});
	FSTTestcases.push_back({
		"a:5 :10 * . b:6 * |",
		true,
		false,
		{
			{ "", { 0 } },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "a", {} },
			{ "aa", {} },
			{ "ab", {} },
			{ "aaa", {} },
			{ "abb", {} },
			{ "abba", {} },
			{ "bba", {} },
			{ "b", { 6 } },
			{ "bb", { 12 } },
			{ "bbb", { 18 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 :10 * .  abcd:6 * |",
		true,
		false,
		{
			{ "", { 0 } },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "a", {} },
			{ "aa", {} },
			{ "aabcd", {} },
			{ "aaa", {} },
			{ "abcdabcd", {} },
			{ "aabcdabcd", {} },
			{ "aabcdabcda", {} },
			{ "bba", {} },
			{ "abcd", {} },
			{ "abcdabcd", {} },
			{ "abcdabcdabcd", {} },
		}
	});
	FSTTestcases.push_back({
		"a:5 :10 * .  bcd:6 * |",
		true,
		false,
		{
			{ "", { 0 } },
			{ "xx", {} },
			{ "aw", {} },
			{ "ax", {} },
			{ "xs", {} },
			{ "a", {} },
			{ "aa", {} },
			{ "abcdd", {} },
			{ "aaa", {} },
			{ "abcdabcd", {} },
			{ "aabcdabcd", {} },
			{ "aabcdabcda", {} },
			{ "bba", {} },
			{ "bcd", { 6 } },
			{ "bcdbcd", { 12 } },
			{ "bcdbcdbcd", { 18 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 :10 * .  bcd:6 * | tony:11 .",
		true,
		false,
		{
			{ "", {} },
			{ "a", {} },
			{ "aa", {} },
			{ "abcdd",{} },
			{ "aaa", {} },
			{ "abcdabcd", {} },
			{ "aabcdabcd", {} },
			{ "aabcdabcda", {} },
			{ "bba", {} },
			{ "atony", {} },
			{ "aatony", {} },
			{ "abcddtony", {} },
			{ "aaatony", {} },
			{ "abcdabcdtony", {} },
			{ "aabcdabcdtony", {} },
			{ "aabcdabcdatony", {} },
			{ "bbatony", {} },
			{ "bcdtony", { 17 } },
			{ "bcdbcdtony", { 23 } },
			{ "bcdbcdbcdtony", { 29 } },
		}
	});
}


// TODO: test for e:5 word !!
void RunFinalStateTransducerTests()
{
	PopulateWithTestCases();

	size_t failedTests = 0;
	size_t testCases = 0;
	std::cout << "RUNNING TESTS WITH " << FSTTestcases.size() << " REG EXPRS:\n";
	for (const auto& testCase : FSTTestcases)
	{
		std::cout << "The regular expression is \"" << testCase.regExpr << "\". Running the test words & their outputs:\n";

		RegularFinalStateTransducerBuilder ts(testCase.regExpr.c_str());
		auto transducer = ts.GetBuildedTransducer();
		// If some test fails try to skip Real-time conversion.
		
		bool infinite;
		transducer->MakeRealTime(infinite);
		transducer->UpdateRecognizingEmptyWord();

		if (infinite != testCase.infinite)
		{
			std::cout << "FAILED: Expected the transducer to be "
				<< (testCase.infinite ? "infinite" : "non-infinite")
				<< " but it was not.\n";
			++failedTests;
		}

		size_t testNumber = 0;
		testCases += testCase.wordsAndExpectedOutputs.size();
		for (const auto& wordAndOutputs : testCase.wordsAndExpectedOutputs)
		{
			std::unordered_set<unsigned> outputs;
			const auto& testWord = wordAndOutputs.first;
			const auto& testOutputs = wordAndOutputs.second;

			transducer->TraverseWithWord(testWord.c_str(), outputs);

			std::cout << "\t" << testNumber << ": \"" << testWord << "\" ";
			for (const auto& expectedOutput : testOutputs)
			{
				std::cout << expectedOutput << " ";
			}

			bool passed = false;
			if (outputs.size() == testOutputs.size())
			{
				passed = true;
				for (const auto& expectedOutput : testOutputs)
				{
					if (outputs.find(expectedOutput) == outputs.end())
					{
						passed = false;
						break;
					}
				}
			}

			if (passed)
			{
				std::cout << " - passing.\n";
			}
			else
			{
				std::cout << "\n\t\tFAILED\n";
				std::cout << "\t\tGot the following output: ";
				for (const auto& output : outputs)
				{
					std::cout << output << " ";
				}
				std::cout << ".\n";
				++failedTests;
			}
			++testNumber;
		}
		std::cout << "\n";
	}

	if (failedTests > 0)
	{
		std::cout << "Passed " << testCases - failedTests << " tests.\n";
		std::cout << "Failed " << failedTests << " tests.\n";
	}
	else
	{
		std::cout << "Passed all " << testCases << " tests.\n";
	}
}