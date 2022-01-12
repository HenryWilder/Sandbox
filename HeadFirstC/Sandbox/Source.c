#include <stdio.h>

inline void OutOfWorkingDirectoryFileIO(); // Successful
inline void BitmapFileModification();

int main()
{
	//OutOfWorkingDirectoryFileIO();
	BitmapFileModification();

	return 0;
}

inline void OutOfWorkingDirectoryFileIO()
{
	FILE* file;
	fopen_s(&file, "D:\\Repo\\Sandbox\\OutsideProjectFileIO.txt", "r");
	if (file)
	{
		char str[16];
		fgets(str, sizeof(str), file);
		puts(str);
		fclose(file);
	}
	else
	{
		puts("ERROR: File could not be found!");
	}
}

typedef struct BitmapHeader {
	char identifier[2];
	unsigned int sizeInBytes;
	char reserved[4];
	unsigned int offset;
} BitmapHeader;

typedef struct DIBHeader {
	unsigned int headerSize;
	unsigned int width;
	unsigned int height;
	unsigned short colorPlaneCount;
	unsigned short bitsPerPixel;
} DIBHeader;

#ifndef _countof
#define _countof(x) sizeof((x)) / sizeof((x)[0])
#endif // !_countof


inline void BitmapFileModification()
{

	FILE* image;
	fpos_t dataPos;
	fopen_s(&image, "Image.bmp", "r+");
	if (image)
	{
		fpos_t start;
		fgetpos(image, &start);

		BitmapHeader filehead = { { 0,0 }, 0, { 0,0,0,0 }, 0 };
		fread_s(filehead.identifier,		sizeof(filehead.identifier),		sizeof(filehead.identifier[0]),  _countof(filehead.identifier), image);
		fread_s(&filehead.sizeInBytes,		sizeof(filehead.sizeInBytes),		sizeof(filehead.sizeInBytes),    1u,							image);
		fread_s(filehead.reserved,			sizeof(filehead.reserved),			sizeof(filehead.reserved[0]),    _countof(filehead.reserved),   image);
		fread_s(&filehead.offset,			sizeof(filehead.offset),			sizeof(filehead.offset),		 1u,							image);

		DIBHeader DIBhead = { 0, 0, 0, 0, 0 };
		fread_s(&DIBhead.headerSize,		sizeof(DIBhead.headerSize),			sizeof(DIBhead.headerSize),      1u,							image);
		fread_s(&DIBhead.width,				sizeof(DIBhead.width),				sizeof(DIBhead.width),           1u,							image);
		fread_s(&DIBhead.height,			sizeof(DIBhead.height),				sizeof(DIBhead.height),			 1u,							image);
		fread_s(&DIBhead.colorPlaneCount,	sizeof(DIBhead.colorPlaneCount),	sizeof(DIBhead.colorPlaneCount), 1u,							image);
		fread_s(&DIBhead.bitsPerPixel,		sizeof(DIBhead.bitsPerPixel),		sizeof(DIBhead.bitsPerPixel),    1u,							image);

		printf("Identifier:     %2s\nSize (bytes):   %i\nData offset:    %i\nHeader size:    %i\nWidth:          %i\nHeight:         %i\nColor planes:   %i\nBits per pixel: %i\n\nData:\n",
			   filehead.identifier,
			   filehead.sizeInBytes,
			   filehead.offset,
			   DIBhead.headerSize,
			   DIBhead.width,
			   DIBhead.height,
			   DIBhead.colorPlaneCount,
			   DIBhead.bitsPerPixel);
		
		dataPos = start + filehead.offset;
		fsetpos(image, &dataPos);

		size_t i = 0;
		for (unsigned short y = 0; (y < DIBhead.height) && (i < filehead.sizeInBytes - filehead.offset); ++y)
		{
			for (unsigned short x = 0; (x < DIBhead.width) && (i < filehead.sizeInBytes - filehead.offset); ++x)
			{
				putchar('{');
				putchar(' ');
				for (unsigned short c = 0; (c < 3) && (i < filehead.sizeInBytes - filehead.offset); ++c)
				{
					unsigned char value;
					fread_s(&value, sizeof(value), sizeof(value), 1u, image);
					printf("%3i ", value);
					++i;
				}
				putchar('}');
			}
			puts("");
		}

		fsetpos(image, &dataPos);
		puts("\nEditing...");

		unsigned char color[3] = { (unsigned char)255u, (unsigned char)0u, (unsigned char)255u };

		i = 0;
		for (unsigned short y = 0; (y < DIBhead.height) && (i < filehead.sizeInBytes - filehead.offset); ++y)
		{
			for (unsigned short x = 0; (x < DIBhead.width) && (i < filehead.sizeInBytes - filehead.offset); ++x)
			{
				putchar('{');
				putchar(' ');
				for (unsigned short c = 0; (c < 3) && (i < filehead.sizeInBytes - filehead.offset); ++c)
				{
					unsigned char value = color[c];
					fwrite(&value, sizeof(value), 1u, image);
					printf("%3i ", value);
					++i;
				}
				putchar('}');
			}
			puts("");
		}

		fclose(image);
	}
	else
	{
		puts("ERROR: File could not be found!");
	}
}

