/**************************************************************************\
|
|    Copyright (C) 2012 Centrum Wiskunde & Informatica, The Netherlands
|
|    Contact:
|    Marc Stevens
|    Cryptology Group
|    Centrum Wiskunde & Informatica
|    P.O. Box 94079, 1090 GB Amsterdam, Netherlands
|    marc@marc-stevens.nl
|
|  Permission is hereby granted, free of charge, to any person obtaining a copy
|  of this software and associated documentation files (the "Software"), to deal
|  in the Software without restriction, including without limitation the rights
|  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
|  copies of the Software, and to permit persons to whom the Software is
|  furnished to do so, subject to the following conditions:
|
|  The above copyright notice and this permission notice shall be included in
|  all copies or substantial portions of the Software.
|
|  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
|  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
|  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
|  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
|  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
|  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
|  THE SOFTWARE.
|
|  Contributions were made by: Dan Shumow (Microsoft Research)
|
\**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include <boost/cstdint.hpp>

using namespace std;
using boost::uint32_t;

#include "ubc_check_test.h"

char*	usage_str = "%s usage:\n"
					"\t--all      - Run all tests.\n"
#ifdef INCLUDE_BASIC_TEST
					"\t--basic    - Run unavoidable bit condition check tests (default true.)\n"
#endif
					"-p,--nocheck - Supress correctness checks.\n"
					"-c,--noperf  - Supress performance tests.\n"
					"\t-h,--help  - Print this help message\n"
					"\n";

bool run_all_test = false;
#define TEST_ALL_ARG_STR	"--all"

#ifdef INCLUDE_BASIC_TEST
	int test_ubc_check();
	bool run_basic_test = true;
	#define TEST_BASIC_ARG_STR	"--basic"
#endif

bool run_correctness_checks = true;
bool run_perf_tests = true;

void usage(char* program_name)
{
	printf(usage_str, program_name);
}


int main(int argc, char** argv)
{
	for (size_t i = 1; i < argc; i++)
	{
		if (0 == strcmp(argv[i], TEST_ALL_ARG_STR))
		{
			run_all_test = true;
		}
#ifdef INCLUDE_BASIC_TEST
		else if (0 == strcmp(argv[i], TEST_BASIC_ARG_STR))
		{
			run_basic_test = true;
		}
#endif
		else if ((0 == strcmp(argv[i], "-p")) ||
			     (0 == strcmp(argv[i], "--nocheck")))
		{
			run_correctness_checks = false;
		}
		else if ((0 == strcmp(argv[i], "-c")) ||
			     (0 == strcmp(argv[i], "--noperf")))
		{
			run_perf_tests = false;
		}
		else if ((0 == strcmp(argv[i], "-h")) || 
				 (0 == strcmp(argv[i], "--help")))
		{
			usage(argv[0]);
			exit(0);
		}
		else
		{
			printf("Unknown argument: %s\n", argv[i]);
			usage(argv[0]);
			exit(-1);
		}
	}

#ifdef INCLUDE_BASIC_TEST
	if (run_all_test || run_basic_test)
	{
		cout << "=====================================================================" << endl;
		test_ubc_check();
		cout << "=====================================================================" << endl << endl << endl;
	}
#endif

}

