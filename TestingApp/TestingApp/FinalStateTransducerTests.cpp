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
		false,
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
		false,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
		true,
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
	FSTTestcases.push_back({
		"a:1 a:2 a:3 a:4 a:5 a:6 a:7 a:8 a:9 a:10 . . . . . . . . .",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "aa", {} },
			{ "aaa", {} },
			{ "aaaa", {} },
			{ "aaaaa", {} },
			{ "aaaaaa", {} },
			{ "aaaaaaaa", {} },
			{ "aaaaaaaaa", {} },
			{ "aaaaaaaaaa", { 55 } },
		}
	});
	FSTTestcases.push_back({
		"word:1 word:2 word:3 word:4 word:5 word:6 word:7 word:8 word:9 word:10 . . . . . . . . .",
		false,
		true,
		{
			{ "", {} },
			{ "ord", {} },
			{ "word", {} },
			{ "wordwordwordwordword", {} },
			{ "ordwordwordwordwordwo", {} },
			{ "wordwordwordwordwordword", {} },
			{ "wordwordwordwordwordwordword", {} },
			{ "wordwordwordwordwordwordwordword", {} },
			{ "wordwordwordwordwordwordwordwordword", {} },
			{ "ordwordwordwordwordwordwordwordwordwor", {} },
			{ "ordwordwordwordwordwordwordwordwordword", {} },
			{ "wordwordwordwordwordwordwordwordwordword", { 55 } },
		}
	});
	FSTTestcases.push_back({
		"word:1 word:2 word:3 word:4 word:5 word:6 word:7 word:8 word:9 word:10 word:11 word:12 word:13 word:14 word:15 word:16 word:17 word:18 word:19 word:20 word:21 word:22 word:23 word:24 word:25 word:26 word:27 word:28 word:29 word:30 word:31 word:32 word:33 word:34 word:35 word:36 word:37 word:38 word:39 word:40 word:41 word:42 word:43 word:44 word:45 word:46 word:47 word:48 word:49 word:50 word:51 word:52 word:53 word:54 word:55 word:56 word:57 word:58 word:59 word:60 word:61 word:62 word:63 word:64 word:65 word:66 word:67 word:68 word:69 word:70 word:71 word:72 word:73 word:74 word:75 word:76 word:77 word:78 word:79 word:80 word:81 word:82 word:83 word:84 word:85 word:86 word:87 word:88 word:89 word:90 word:91 word:92 word:93 word:94 word:95 word:96 word:97 word:98 word:99 word:100 word:101 word:102 word:103 word:104 word:105 word:106 word:107 word:108 word:109 word:110 word:111 word:112 word:113 word:114 word:115 word:116 word:117 word:118 word:119 word:120 word:121 word:122 word:123 word:124 word:125 word:126 word:127 word:128 word:129 word:130 word:131 word:132 word:133 word:134 word:135 word:136 word:137 word:138 word:139 word:140 word:141 word:142 word:143 word:144 word:145 word:146 word:147 word:148 word:149 word:150 word:151 word:152 word:153 word:154 word:155 word:156 word:157 word:158 word:159 word:160 word:161 word:162 word:163 word:164 word:165 word:166 word:167 word:168 word:169 word:170 word:171 word:172 word:173 word:174 word:175 word:176 word:177 word:178 word:179 word:180 word:181 word:182 word:183 word:184 word:185 word:186 word:187 word:188 word:189 word:190 word:191 word:192 word:193 word:194 word:195 word:196 word:197 word:198 word:199 word:200 word:201 word:202 word:203 word:204 word:205 word:206 word:207 word:208 word:209 word:210 word:211 word:212 word:213 word:214 word:215 word:216 word:217 word:218 word:219 word:220 word:221 word:222 word:223 word:224 word:225 word:226 word:227 word:228 word:229 word:230 word:231 word:232 word:233 word:234 word:235 word:236 word:237 word:238 word:239 word:240 word:241 word:242 word:243 word:244 word:245 word:246 word:247 word:248 word:249 word:250 word:251 word:252 word:253 word:254 word:255 word:256 word:257 word:258 word:259 word:260 word:261 word:262 word:263 word:264 word:265 word:266 word:267 word:268 word:269 word:270 word:271 word:272 word:273 word:274 word:275 word:276 word:277 word:278 word:279 word:280 word:281 word:282 word:283 word:284 word:285 word:286 word:287 word:288 word:289 word:290 word:291 word:292 word:293 word:294 word:295 word:296 word:297 word:298 word:299 word:300 word:301 word:302 word:303 word:304 word:305 word:306 word:307 word:308 word:309 word:310 word:311 word:312 word:313 word:314 word:315 word:316 word:317 word:318 word:319 word:320 word:321 word:322 word:323 word:324 word:325 word:326 word:327 word:328 word:329 word:330 word:331 word:332 word:333 word:334 word:335 word:336 word:337 word:338 word:339 word:340 word:341 word:342 word:343 word:344 word:345 word:346 word:347 word:348 word:349 word:350 word:351 word:352 word:353 word:354 word:355 word:356 word:357 word:358 word:359 word:360 word:361 word:362 word:363 word:364 word:365 word:366 word:367 word:368 word:369 word:370 word:371 word:372 word:373 word:374 word:375 word:376 word:377 word:378 word:379 word:380 word:381 word:382 word:383 word:384 word:385 word:386 word:387 word:388 word:389 word:390 word:391 word:392 word:393 word:394 word:395 word:396 word:397 word:398 word:399 word:400 word:401 word:402 word:403 word:404 word:405 word:406 word:407 word:408 word:409 word:410 word:411 word:412 word:413 word:414 word:415 word:416 word:417 word:418 word:419 word:420 word:421 word:422 word:423 word:424 word:425 word:426 word:427 word:428 word:429 word:430 word:431 word:432 word:433 word:434 word:435 word:436 word:437 word:438 word:439 word:440 word:441 word:442 word:443 word:444 word:445 word:446 word:447 word:448 word:449 word:450 word:451 word:452 word:453 word:454 word:455 word:456 word:457 word:458 word:459 word:460 word:461 word:462 word:463 word:464 word:465 word:466 word:467 word:468 word:469 word:470 word:471 word:472 word:473 word:474 word:475 word:476 word:477 word:478 word:479 word:480 word:481 word:482 word:483 word:484 word:485 word:486 word:487 word:488 word:489 word:490 word:491 word:492 word:493 word:494 word:495 word:496 word:497 word:498 word:499 word:500 word:501 word:502 word:503 word:504 word:505 word:506 word:507 word:508 word:509 word:510 word:511 word:512 word:513 word:514 word:515 word:516 word:517 word:518 word:519 word:520 word:521 word:522 word:523 word:524 word:525 word:526 word:527 word:528 word:529 word:530 word:531 word:532 word:533 word:534 word:535 word:536 word:537 word:538 word:539 word:540 word:541 word:542 word:543 word:544 word:545 word:546 word:547 word:548 word:549 word:550 word:551 word:552 word:553 word:554 word:555 word:556 word:557 word:558 word:559 word:560 word:561 word:562 word:563 word:564 word:565 word:566 word:567 word:568 word:569 word:570 word:571 word:572 word:573 word:574 word:575 word:576 word:577 word:578 word:579 word:580 word:581 word:582 word:583 word:584 word:585 word:586 word:587 word:588 word:589 word:590 word:591 word:592 word:593 word:594 word:595 word:596 word:597 word:598 word:599 word:600 word:601 word:602 word:603 word:604 word:605 word:606 word:607 word:608 word:609 word:610 word:611 word:612 word:613 word:614 word:615 word:616 word:617 word:618 word:619 word:620 word:621 word:622 word:623 word:624 word:625 word:626 word:627 word:628 word:629 word:630 word:631 word:632 word:633 word:634 word:635 word:636 word:637 word:638 word:639 word:640 word:641 word:642 word:643 word:644 word:645 word:646 word:647 word:648 word:649 word:650 word:651 word:652 word:653 word:654 word:655 word:656 word:657 word:658 word:659 word:660 word:661 word:662 word:663 word:664 word:665 word:666 word:667 word:668 word:669 word:670 word:671 word:672 word:673 word:674 word:675 word:676 word:677 word:678 word:679 word:680 word:681 word:682 word:683 word:684 word:685 word:686 word:687 word:688 word:689 word:690 word:691 word:692 word:693 word:694 word:695 word:696 word:697 word:698 word:699 word:700 word:701 word:702 word:703 word:704 word:705 word:706 word:707 word:708 word:709 word:710 word:711 word:712 word:713 word:714 word:715 word:716 word:717 word:718 word:719 word:720 word:721 word:722 word:723 word:724 word:725 word:726 word:727 word:728 word:729 word:730 word:731 word:732 word:733 word:734 word:735 word:736 word:737 word:738 word:739 word:740 word:741 word:742 word:743 word:744 word:745 word:746 word:747 word:748 word:749 word:750 word:751 word:752 word:753 word:754 word:755 word:756 word:757 word:758 word:759 word:760 word:761 word:762 word:763 word:764 word:765 word:766 word:767 word:768 word:769 word:770 word:771 word:772 word:773 word:774 word:775 word:776 word:777 word:778 word:779 word:780 word:781 word:782 word:783 word:784 word:785 word:786 word:787 word:788 word:789 word:790 word:791 word:792 word:793 word:794 word:795 word:796 word:797 word:798 word:799 word:800 word:801 word:802 word:803 word:804 word:805 word:806 word:807 word:808 word:809 word:810 word:811 word:812 word:813 word:814 word:815 word:816 word:817 word:818 word:819 word:820 word:821 word:822 word:823 word:824 word:825 word:826 word:827 word:828 word:829 word:830 word:831 word:832 word:833 word:834 word:835 word:836 word:837 word:838 word:839 word:840 word:841 word:842 word:843 word:844 word:845 word:846 word:847 word:848 word:849 word:850 word:851 word:852 word:853 word:854 word:855 word:856 word:857 word:858 word:859 word:860 word:861 word:862 word:863 word:864 word:865 word:866 word:867 word:868 word:869 word:870 word:871 word:872 word:873 word:874 word:875 word:876 word:877 word:878 word:879 word:880 word:881 word:882 word:883 word:884 word:885 word:886 word:887 word:888 word:889 word:890 word:891 word:892 word:893 word:894 word:895 word:896 word:897 word:898 word:899 word:900 word:901 word:902 word:903 word:904 word:905 word:906 word:907 word:908 word:909 word:910 word:911 word:912 word:913 word:914 word:915 word:916 word:917 word:918 word:919 word:920 word:921 word:922 word:923 word:924 word:925 word:926 word:927 word:928 word:929 word:930 word:931 word:932 word:933 word:934 word:935 word:936 word:937 word:938 word:939 word:940 word:941 word:942 word:943 word:944 word:945 word:946 word:947 word:948 word:949 word:950 word:951 word:952 word:953 word:954 word:955 word:956 word:957 word:958 word:959 word:960 word:961 word:962 word:963 word:964 word:965 word:966 word:967 word:968 word:969 word:970 word:971 word:972 word:973 word:974 word:975 word:976 word:977 word:978 word:979 word:980 word:981 word:982 word:983 word:984 word:985 word:986 word:987 word:988 word:989 word:990 word:991 word:992 word:993 word:994 word:995 word:996 word:997 word:998 word:999 word:1000 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .",
		false,
		true,
		{
			{ "", {} },
			{ "ord", {} },
			{ "word", {} },
			{ "wordwordwordwordword", {} },
			{ "ordwordwordwordwordwo", {} },
			{ "wordwordwordwordwordword", {} },
			{ "wordwordwordwordwordwordword", {} },
			{ "wordwordwordwordwordwordwordword", {} },
			{ "wordwordwordwordwordwordwordwordword", {} },
			{ "ordwordwordwordwordwordwordwordwordwor", {} },
			{ "ordwordwordwordwordwordwordwordwordword", {} },
			{ "wordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordwordword", { 500500 } },
		}
	});
}

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
		
		bool infinite =	transducer->MakeRealTime();
		transducer->UpdateRecognizingEmptyWord();
		bool functional = transducer->TestForFunctionality();

		std::cout << "\tFST is " << (functional ? "functional" : "non-functional") << " and "
			<< (infinite ? "infinite" : "non-infinite") << ".\n";

		if (infinite != testCase.infinite)
		{
			std::cout << "FAILED: Expected the transducer to be "
				<< (testCase.infinite ? "infinite" : "non-infinite")
				<< " but it was not.\n";
			++failedTests;
		}

		if (functional != testCase.functional)
		{
			std::cout << "FAILED: Expected the transducer to be "
				<< (testCase.functional ? "functional" : "non-functional")
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