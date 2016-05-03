#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <iomanip>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define BK	"\x1B[40m"	// black
#define BR	"\x1B[41m"	// red
#define BG	"\x1B[42m"	// green
#define BY	"\x1B[43m"	// yellow (brown)
#define BB	"\x1B[44m"	// blue
#define BM	"\x1B[45m"	// magenta
#define BC	"\x1B[46m"	// cyan
#define BW	"\x1B[47m"	// white (L.gray)
#define CLEAR "\x1B[0m"

#define K0 0x5a827999
#define K1 0x6ed9eba1
#define K2 0x8f1bbcdc
#define K3 0xca62c1d6


inline uint32_t rotate_left(uint32_t x, uint32_t r)
{
	return ((x << r) | (x >> (32 - r)));
}

inline uint32_t sha1_f1(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x & y) | (~x & z));
}

inline uint32_t sha1_f2(uint32_t x, uint32_t y, uint32_t z)
{
	return (x ^ y ^ z);
}

inline uint32_t sha1_f3(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x & y) | (x & z) | (y & z));
}

inline uint32_t sha1_f4(uint32_t x, uint32_t y, uint32_t z)
{
	return (x ^ y ^ z);
}

inline void sha1_step_round1(int t, uint32_t Q[85], uint32_t m[80])
{
	Q[4+t+1] = rotate_left(Q[4+t],5) + rotate_left(Q[4+t-4],30) + sha1_f1(Q[4+t-1],rotate_left(Q[4+t-2],30),rotate_left(Q[4+t-3],30)) + K0 + m[t];
}
inline void sha1_step_round2(int t, uint32_t Q[85], uint32_t m[80])
{
	Q[4+t+1] = rotate_left(Q[4+t],5) + rotate_left(Q[4+t-4],30) + sha1_f2(Q[4+t-1],rotate_left(Q[4+t-2],30),rotate_left(Q[4+t-3],30)) + K1 + m[t];
}
inline void sha1_step_round3(int t, uint32_t Q[85], uint32_t m[80])
{
	Q[4+t+1] = rotate_left(Q[4+t],5) + rotate_left(Q[4+t-4],30) + sha1_f3(Q[4+t-1],rotate_left(Q[4+t-2],30),rotate_left(Q[4+t-3],30)) + K2 + m[t];
}
inline void sha1_step_round4(int t, uint32_t Q[85], uint32_t m[80])
{
	Q[4+t+1] = rotate_left(Q[4+t],5) + rotate_left(Q[4+t-4],30) + sha1_f4(Q[4+t-1],rotate_left(Q[4+t-2],30),rotate_left(Q[4+t-3],30)) + K3 + m[t];
}

void sha1_me(uint32_t M[80])
{
	for (int i = 16; i < 80; ++i)
	{
		M[i] = rotate_left(M[i - 3] ^ M[i - 8] ^ M[i - 14] ^ M[i - 16], 1);
	}
}

void sha1_80(uint32_t out_cv[5], uint32_t Q[85], const uint32_t in_cv[5], const uint32_t m[16])
{
	uint32_t W[80];
	for (unsigned i = 0; i < 16; ++i)
		W[i] = m[i];
	sha1_me(W);
	
	Q[4] = in_cv[0];
	Q[3] = in_cv[1];
	Q[2] = rotate_left(in_cv[2],2);
	Q[1] = rotate_left(in_cv[3],2);
	Q[0] = rotate_left(in_cv[4],2);
	
	for (int t =  0; t < 20; ++t)
		sha1_step_round1(t, Q, W);
	for (int t = 20; t < 40; ++t)
		sha1_step_round2(t, Q, W);
	for (int t = 40; t < 60; ++t)
		sha1_step_round3(t, Q, W);
	for (int t = 60; t < 80; ++t)
		sha1_step_round4(t, Q, W);
		
	out_cv[0] = in_cv[0] + Q[4+80];
	out_cv[1] = in_cv[1] + Q[4+79];
	out_cv[2] = in_cv[2] + rotate_left(Q[4+78],30);
	out_cv[3] = in_cv[3] + rotate_left(Q[4+77],30);
	out_cv[4] = in_cv[4] + rotate_left(Q[4+76],30);
}

void test()
{
	uint32_t iv1[5] = { 0x506b0178, 0xff6d1890, 0x202291fd, 0x3ade3871, 0xb2c665ea };
	uint32_t iv2[5] = { 0x506b0178, 0xff6d1891, 0xa02291fd, 0x3ade3871, 0xb2c665ea };

	uint32_t m1[16] = { 0x9d443828, 0xa5ea3df0, 0x86eaa0fa, 0x7783a736, 0x3324484d, 0xaf702aaa, 0xa3dab679, 0xd8a69e2d, 0x543820ed, 0xa7fffb52, 0xd3ff493f, 0xc3ff551e, 0xfbffd97f, 0x55feeef2, 0x085af312, 0x088688a9 };
	uint32_t m2[16] = { 0x3f443838, 0x81ea3dec, 0xa0eaa0ee, 0x5183a72c, 0x3324485d, 0xab702ab6, 0x6fdab66d, 0xd4a69e2f, 0x943820fd, 0x13fffb4e, 0xefff493b, 0x7fff5504, 0xdbffd96f, 0x71feeeee, 0xe45af306, 0x048688ab };

	uint32_t cv1[5], cv2[5];
	uint32_t Q1[85], Q2[85];
        
	sha1_80(cv1, Q1, iv1, m1);
	sha1_80(cv2, Q2, iv2, m2);

	for (int t = -4; t <= 80; ++t)
	{
	    std::cout << std::setw(2) << t << " ";
	    uint32_t q1 = Q1[4+t], q2 = Q2[4+t];
	    for (int b = 31; b >= 0; --b)
            {
                uint32_t q1b = (q1>>b)&1, q2b = (q2>>b)&1;
                if (q1b == q2b) {
                    std::cout << q1b;
                } else if (q1b == 0) {
                    std::cout << BR << "+" << KNRM;
                } else if (q1b == 1) {
                    std::cout << BR << "-" << KNRM;
                }
            }
            std::cout << std::endl;
            if (t == 0 || t == 75 || t == 80)
                std::cout << std::endl;
	}
	for (unsigned i = 0; i < 5; ++i)
	{
	    std::cout << std::setw(2) << i << " ";
	    uint32_t q1 = cv1[i], q2 = cv2[i];
	    for (int b = 31; b >= 0; --b)
            {
                uint32_t q1b = (q1>>b)&1, q2b = (q2>>b)&1;
                if (q1b == q2b) {
                    std::cout << q1b;
                } else if (q1b == 0) {
                    std::cout << BR << "+" << KNRM;
                } else if (q1b == 1) {
                    std::cout << BR << "-" << KNRM;
                }
            }
            std::cout << std::endl;
	}

	std::cout << std::endl;
	for (unsigned i = 0; i < 5; ++i)
	{
		std::cout << std::setw(2) << i << " " << std::hex << cv1[i] << " " << std::hex << cv2[i] << std::dec << std::endl;
	}
}

int main(int argc, char **argv)
{
	test();
	return 0;
}
