#pragma once
#include "common.h"

namespace amy
{
	void MemCopy(void* dest, void* src, size_t bytesToCopy)
	{
		printf("Copying %zu bytes from %p to %p.\n", bytesToCopy, src, dest);
		char* csrc = (char*)src;
		char* cdest = (char*)dest;

		for (size_t i = 0; i < bytesToCopy; ++i)
		{
			printf("Byte %zu successfully copied\n", i);
			cdest[i] = csrc[i];
		}
	}
}
