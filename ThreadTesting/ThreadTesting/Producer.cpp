#include "Producer.h"

void UpdateScreencap()
{
	BitBlt(g_hInternal, 0, 0, g_screenWidth, g_screenHeight, g_hDesktop, 0, 0, SRCCOPY);

	g_activity.producer.TrackOpPush("BitBlt", g_hInternalRecord, (char)rand());

	BITMAPINFOHEADER bmi = {0};
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = g_screenWidth;
	bmi.biHeight = -g_screenHeight;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0; // 3 * ScreenX * ScreenY; (position, not size)

	GetDIBits(g_hDesktop, g_hBitmap, 0, g_screenHeight, g_screenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

	g_activity.producer.TrackOpPush("GetDIBits", g_screenDataRecord, (char)rand());

	//BitBlt(g_hConsoleDC, 0, 0, g_screenWidth, g_screenHeight, g_hInternal, 0, 0, SRCCOPY);
}

void Produce()
{
	while (loop)
	{
		//printf("Producer task started...\n");
		//long long start = GetTime();
		UpdateScreencap();
		Sleep(2);
		//long long time = GetTime() - start;
		//int ms = int(time / 1000000);
		//printf("Producer task done. %dms\n", ms);

		//std::cout << time << " nanoseconds\n" << time / 1000000 << " milliseconds\n\n";
	}
}
