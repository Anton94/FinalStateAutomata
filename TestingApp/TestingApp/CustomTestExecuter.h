#pragma once

#include <iostream>
#include <string>

/* Reads the regex and the words for traversing from the passed file.
   Prints formated output with stats.

   The file should be in the following format:
   regular_expr
   number_of_words_for_traversing
   word1
   ...
   wordN

   For example the file should be in the following format:
	a:10 a:20 .
	4
	a
	aa

	aaa

   (note that there is an empty line for the empty word)
 */

void ExecuteCustomTestFromFile(std::string& fileName);