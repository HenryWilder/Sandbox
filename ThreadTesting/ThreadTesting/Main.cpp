#include "Shared.h"
#include "Tracking.h"
#include "Consumer.h"
#include "Producer.h"

void Clear(Color color)
{
	for (long x = 0; x < g_screenWidth / 2; x += 4)
	{
		for (long y = 0; y < g_screenHeight; y += 4)
		{
			SetPixel(g_hConsoleDC,x/4,y/4,color);
		}
	}
}

int main()
{
	SelectObject(g_hInternal, g_hBitmap);

	std::thread producer(Produce);
	std::thread consumer(Consume);

	Sleep(10000);
	loop = false;

	producer.join();
	consumer.join();
	std::cout << "\nFinished.\n\n";

	g_activity.Print();

	std::cin.ignore();

	return 0;
}
