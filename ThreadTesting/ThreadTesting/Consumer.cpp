#include "Consumer.h"
#include <vector>

unsigned long PixelIndex(long x, long y) { return 4u * (unsigned)((y * (long)g_screenWidth) + x); }

Color GetPixelColor(long x, long y)
{
	unsigned long index = PixelIndex(x, y);

	Color output = {
		g_screenData[index + 2u],	// Red
		g_screenData[index + 1u],	// Green
		g_screenData[index] };		// Blue

	return output;
}

void Consume()
{
	std::vector<Color> buffer;

	for (long x = 0; x < 1920; x += 4)
	{
		for (long y = 0; y < 1080; y += 4)
		{
			buffer.push_back({255,0,255});
		}
	}

	while (loop)
	{
		//printf("    Consumer task started...\n");
		//long long start = GetTime();

		long long p = 0;
		for (long x = 0; x < 1920; x += 4)
		{
			for (long y = 0; y < 1080; y += 4)
			{
				buffer[p++] = GetPixelColor(x, y);
			}
		}
		//long long time = GetTime() - start;
		//int ms = int(time / 1000000);
		//printf("    Consumer task done. %dms\n", ms);

		p = 0;
		for (long x = 0; x < 1920; x += 4)
		{
			for (long y = 0; y < 1080; y += 4)
			{
				SetPixel(g_hConsoleDC, x / 4, y / 4, buffer[p++]);
			}
		}
		Sleep(2);
	}
}
