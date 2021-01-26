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

int AdditiveFactorial(const int value) // Given a number, returns the sum of all values which come before it, as opposed to the product.
{
	if (value == 1) return 1;
	else return value + AdditiveFactorial(value - 1);
}

int AdditiveFactorialConst(const int n) // Given a number, returns the sum of all values which come before it, as opposed to the product.
{
	if (n % 2) return (n * ((n + 1) / 2)); // n is odd
	else return AdditiveFactorialConst(n - 1) + n; // n is even
}

#include <chrono>
#include <vector>

int main() {
	std::vector<int> inputs;
	long long outA = 0;
	long long outB = 0;
	for (int i = 0; i < 10000; ++i) inputs.push_back(rand() % 1024);
	auto startTime = std::chrono::high_resolution_clock::now();
	for (int n : inputs)
	{
		outA += AdditiveFactorial(n);
	}
	auto timeA = std::chrono::high_resolution_clock::now() - startTime;
	startTime = std::chrono::high_resolution_clock::now();
	for (int n : inputs)
	{
		outB += AdditiveFactorialConst(n);
	}
	auto timeB = std::chrono::high_resolution_clock::now() - startTime;
	printf("Form A took %i nanoseconds, or %f/op.\nForm B took %i nanoseconds, or %f/op.", (int)timeA.count(), (double)timeA.count() / double(inputs.size()), (int)timeB.count(), (double)timeB.count() / double(inputs.size()));

	printf("sum A %i\nsum B %i\n\n", (int)outA, (int)outB);

	return 0;



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
		const int bytesPerPixel = 3; // red, green, & blue
		const int fileHeaderSize = 14;
		const int infoHeaderSize = 40;

		int widthInBytes = width * bytesPerPixel;

		int paddingSize = (4 - (widthInBytes) % 4) % 4;

		int stride = (widthInBytes)+paddingSize;

		int fileSize = fileHeaderSize + infoHeaderSize + (stride * height);
		int headerSize = fileHeaderSize + infoHeaderSize;
		unsigned char bitsPerPix = bytesPerPixel * 8;

		outfile
			.write("BM", 2)
			.write((char*)(&fileSize), sizeof(int))
			.write((char*)(&headerSize), sizeof(int))
			.write("\0", (fileHeaderSize - (2 + sizeof(int) + sizeof(int)))); // Padding
		outfile
			.write((char*)(&infoHeaderSize), sizeof(int))
			.write((char*)(&width), sizeof(int))
			.write((char*)(&height), sizeof(int))
			.write("\1", 1)
			.write((char*)(&bitsPerPix), 1)
			.write("\0", (infoHeaderSize - (sizeof(int) + sizeof(int) + sizeof(int) + 1 + 1))); // Padding

		if (paddingSize > 0) { // Width or height is not a clean power of 2
			for (int i = 0; i < height; ++i) {
				Color* writeMapRow = &(writeMap[i * widthInBytes]);
				outfile.write((char*)writeMapRow, widthInBytes); // Row
				outfile.write("\0", paddingSize); // Padding
			}
		}
		else { // Clean powers of 2 for both width & height
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
