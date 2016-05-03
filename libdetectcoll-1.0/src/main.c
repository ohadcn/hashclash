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

#include "sha1.h"

int main(int argc, char** argv) 
{
	FILE* fd;
	unsigned char hash2[20];
	char buffer[65536];
	size_t size;
	SHA1_CTX ctx2;
	int i,j;

	if (argc < 2) 
	{
		printf("Usage: test <file>\n");
		return 1;
	}

	for (i=1; i < argc; ++i) 
	{
		// initialize SHA-1 context
		SHA1DCInit(&ctx2);

		// if the program name includes the word 'partial' then also test for reduced-round SHA-1 collisions 
		if (NULL != strstr(argv[0], "partial"))
		{
			SHA1DCSetDetectReducedRoundCollision(&ctx2, 1);
		}

		// open file
		fd = fopen(argv[i], "rb");
		if (fd == NULL) 
		{
			printf("cannot open file: %s\n", argv[i]);
			return 1;
		}

		// feed file through SHA-1 update fuction
		while (1) 
		{
			size=fread(buffer,1,65536,fd);
			SHA1DCUpdate(&ctx2, buffer, (unsigned)(size));
			if (size != 65536)
				break;
		}
		if (ferror(fd)) 
		{
			printf("error while reading file: %s\n", argv[i]);
			return 1;
		}
		if (!feof(fd)) 
		{
			printf("not end of file?: %s\n",argv[i]);
			return 1;
		}

		// obtain SHA-1 and print it
		SHA1DCFinal(hash2,&ctx2);
		for (j = 0; j < 20; ++j)
		{ 
			sprintf(buffer+(j*2), "%02x", hash2[j]);
		}
		buffer[20*2] = 0;
		if (ctx2.found_collision) 
		{
			printf("%s *coll* %s\n", buffer, argv[i]);
		} 
		else 
		{
			printf("%s  %s\n", buffer, argv[i]);
		}

		fclose(fd);
	}
}
