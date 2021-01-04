// Copy a file
#include <iostream>
#include <fstream>
//#include <Windows.h>

int MakeLilEndian(int value)
{
	return (
		((value >> 000) % 0400) &
		((value >> 010) % 0400) &
		((value >> 020) % 0400) &
		((value >> 030) % 0400) );
}

struct Color
{
	unsigned char r, g, b;

#ifdef RGB
	operator COLORREF() const { return RGB(r, g, b); }
#endif
};

Color RandomColor() {
	return Color{
		(unsigned char)(rand() % 255),
		(unsigned char)(rand() % 255),
		(unsigned char)(rand() % 255)
	};
}

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);

#ifdef RGB
HWND window = GetConsoleWindow();
HDC hdc = GetDC(window);
#endif

int main() {
	const int width = 32;
	const int height = 32;

	// Produce data
	Color writeMap[width * height];

	size_t i = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			writeMap[i++] = RandomColor();
		}
	}

#ifdef RGB
	i = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			SetPixel(hdc, x, y + 16, writeMap[i++]);
		}
	}
#endif

	// Write
	std::ofstream outfile("new.txt", std::ofstream::binary);

	{
		const int BYTES_PER_PIXEL = 3; /// red, green, & blue
		const int FILE_HEADER_SIZE = 14;
		const int INFO_HEADER_SIZE = 40;

		int widthInBytes = width * BYTES_PER_PIXEL;

		unsigned char padding[3] = { 0, 0, 0 };
		int paddingSize = (4 - (widthInBytes) % 4) % 4;

		int stride = (widthInBytes)+paddingSize;

		int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

		unsigned char header[] = {
			(unsigned char)('B'),
			(unsigned char)('M'),
			(unsigned char)(fileSize >> 000),
			(unsigned char)(fileSize >> 010),
			(unsigned char)(fileSize >> 020),
			(unsigned char)(fileSize >> 030),
			(unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE),
			(unsigned char)(INFO_HEADER_SIZE),
			(unsigned char)(width >> 000),
			(unsigned char)(width >> 010),
			(unsigned char)(width >> 020),
			(unsigned char)(width >> 030),
			(unsigned char)(height >> 000),
			(unsigned char)(height >> 010),
			(unsigned char)(height >> 020),
			(unsigned char)(height >> 030),
			(unsigned char)(1),
			(unsigned char)(BYTES_PER_PIXEL * 8) // Bits in a pixel
		};

		outfile.write(
			reinterpret_cast<char*>(header), // Convert unsigned to signed without changing the bits
			sizeof(header));

		if (paddingSize > 0) {
			for (int i = 0; i < height; ++i) {
				outfile.write((char*)(writeMap + (i * widthInBytes)), widthInBytes);
				outfile.write("\0\0\0", paddingSize);
			}
		}
		else
		{
			outfile.write((char*)writeMap, sizeof(writeMap));
		}
	}

	outfile.close();

	// Read
	std::ifstream infile("new.txt", std::ofstream::binary);

	int readSize = 0;
	Color* readMap;
	infile.ignore(2).read((char*)(&readSize), sizeof(int)).ignore(8);
	size_t totalPixels = readSize / 3;
	readMap = new Color[totalPixels];
	infile.read((char*)(readMap), readSize);

#ifdef RGB
	i = 0;
	for (int y = 0; y < bitmapWidth; ++y) {
		for (int x = 0; x < bitmapWidth; ++x) {
			SetPixel(hdc, x, y + 16 + width + 5, readMap[i++]);
		}
	}
#endif

	delete[] readMap;
	infile.close();
	std::cin.ignore();

	return 0;
}
