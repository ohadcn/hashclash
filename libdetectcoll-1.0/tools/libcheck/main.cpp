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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <iomanip>

#include <boost/nondet_random.hpp>
#include <boost/random.hpp>
#include <boost/timer.hpp>
#include <boost/progress.hpp>

extern "C"
{
#include "sha1.h"
#include "ubc_check.h"

	void ubc_check_verify(const uint32_t W[80], uint32_t dvmask[DVMASKSIZE]);

	void nc_callback(uint64_t byteoffset, const uint32_t ihvin1[5], const uint32_t ihvin2[5], const uint32_t m1[80], const uint32_t m2[80])
	{
		unsigned i;
		printf("Detected near-collision block:\n");
		printf("IHVin1  = { 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x };\n", ihvin1[0], ihvin1[1], ihvin1[2], ihvin1[3], ihvin1[4]);
		printf("IHVin2  = { 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x };\n", ihvin2[0], ihvin2[1], ihvin2[2], ihvin2[3], ihvin2[4]);
		printf("MSGBLK1 = { 0x%08x", m1[0]);
		for (i = 1; i < 16; ++i)
			printf(", 0x%08x", m1[i]);
		printf(" };\n");
		printf("MSGBLK2 = { 0x%08x", m2[0]);
		for (i = 1; i < 16; ++i)
			printf(", 0x%08x", m2[i]);
		printf(" };\n");
	}

} // extern "C"

using namespace std;

inline uint32_t rotate_left(const uint32_t x, const unsigned n)
{
	return (x << n) | (x >> (32 - n));
}

template<typename RNG>
void gen_W(RNG& rng, uint32_t W[80])
{
	for (unsigned i = 0; i < 16; ++i)
		W[i] = rng();
	for (unsigned i = 16; i < 80; ++i)
		W[i] = rotate_left(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
}

int main(int argc, char** argv)
{
	boost::random::random_device seeder;
	boost::random::mt19937 rng(seeder);

	SHA1_CTX ctx;
	unsigned char hash[20];

	vector<char> buffer;

	boost::timer timer;



	uint32_t dvmask[DVMASKSIZE], dvmask_test[DVMASKSIZE];

	cout << "Verifying ubc_check() against ubc_check_verify():" << endl;
	boost::progress_display pd(1 << 24);
	for (unsigned ll = 0; ll < (1 << 24); ++ll, ++pd)
	{
		uint32_t W[80];

		gen_W(rng, W);

		for (unsigned i = 0; i < DVMASKSIZE; ++i)
		{
			dvmask[i] = 0;
			dvmask_test[i] = ~uint32_t(0);
		}

		ubc_check(W, dvmask);
		ubc_check_verify(W, dvmask_test);

		for (unsigned i = 0; i < DVMASKSIZE; ++i)
			if (dvmask[i] != dvmask_test[i])
			{
				cerr << "Found error:" << endl
					<< "dvmask [" << i << "] = 0x" << hex << std::setw(8) << std::setfill('0') << dvmask[i] << dec << endl
					<< "dvmask2[" << i << "] = 0x" << hex << std::setw(8) << std::setfill('0') << dvmask_test[i] << dec << endl
					;
				return 1;
			}
	}
	cout << "Found no discrepancies between ubc_check() and ubc_check_verify()." << endl << endl;




	size_t testcount = 128;
	size_t mindata = 1 << 24;

	for (size_t i = 5; i < 30; ++i)
	{
		buffer.resize(1 << i);

		size_t minloop = mindata / buffer.size();
		if (minloop == 0)
			minloop = 1;
		
		double elap_noubc = 0;
		double elap_ubc = 0;

		for (size_t r = 0; r < testcount; ++r)
		{
			for (size_t j = 0; j < buffer.size(); ++j)
				buffer[j] = rng();

			timer.restart();
			for (size_t k = 0; k < minloop; ++k)
			{
				SHA1DCInit(&ctx);
				SHA1DCSetCallback(&ctx, nc_callback);
				SHA1DCSetUseUBC(&ctx, 0);
				SHA1DCUpdate(&ctx, &buffer[0], buffer.size());
				SHA1DCFinal(hash, &ctx);
			}
			elap_noubc += timer.elapsed();

			timer.restart();
			for (size_t k = 0; k < minloop; ++k)
			{
				SHA1DCInit(&ctx);
				SHA1DCSetCallback(&ctx, nc_callback);
//				SHA1DCSetUseUBC(&ctx, 1);
				SHA1DCUpdate(&ctx, &buffer[0], buffer.size());
				SHA1DCFinal(hash, &ctx);
			}
			elap_ubc += timer.elapsed();
		}

		elap_noubc /= double(minloop)*double(testcount);
		elap_ubc /= double(minloop)*double(testcount);

		cout << buffer.size() << "\t : " << elap_noubc << "s (w/o ubc), \t " << elap_ubc << "s (w/ ubc), " << elap_noubc/elap_ubc << " (noubc/ubc)" << endl;
	}


	cout << "Performing endurance test..." << endl;
	uint64_t total = 0;
	SHA1DCInit(&ctx);
	SHA1DCSetCallback(&ctx, nc_callback);
	buffer.resize(1 << 30);
	timer.restart();
	while (true)
	{
		for (size_t i = 0; i < buffer.size(); i += 4)
			(*reinterpret_cast<uint32_t*>(&buffer[i])) = rng();
		SHA1DCUpdate(&ctx, &buffer[0], buffer.size());
		total += buffer.size();
		if (timer.elapsed() > 60)
		{
			cout << "Hashed " << (total >> 30) << " GB..." << endl;
			timer.restart();
		}
	}


}
