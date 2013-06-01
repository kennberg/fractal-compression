/*
 * Fractal Image Compression. Copyright 2004 Alex Kennberg.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;

#include "Image.h"

extern bool useYCbCr;

/////////////////////////////////////////////////////////////////////
// class ImageData

ImageData::ImageData()
{
	imagedata = NULL;
	imagedata2 = NULL;
	imagedata3 = NULL;
	width = height = 0;
	channels = 0;
}

ImageData::~ImageData()
{
	if (imagedata != NULL)
	{
		delete []imagedata;
		imagedata = NULL;
	}
	if (imagedata2 != NULL)
	{
		delete []imagedata2;
		imagedata2 = NULL;
	}
	if (imagedata3 != NULL)
	{
		delete []imagedata3;
		imagedata3 = NULL;
	}
}

/////////////////////////////////////////////////////////////////////
// class Image

Image::Image(string fileName)
{
	this->fileName = fileName;
	originalSize = 0;
}

Image::~Image()
{
}

void Image::Load()
{
	FILE *f = fopen(fileName.c_str(), "rb");
	unsigned char* chunk;
	int size;

	fseek(f, 0, SEEK_END);
	size = static_cast<int> (ftell(f));
	fseek(f, 0, SEEK_SET);

	size /= 3;
	img.channels = 3;

	for (img.width = 1; img.width*img.width < size; img.width++);
	img.height = img.width;
	size = img.width * img.height * img.channels;

	printf("Reading image (width=%d height=%d)\n", img.width, img.height);

	chunk = new unsigned char[size];
	originalSize = size;

	printf("Reading %s (size=%d)\n", fileName.c_str(), size);
	int rval = fread(chunk, 1, size, f);
	if (rval != size)
	{
		printf("Error: Failed to read image from disk (%d).\n", rval);
		exit(-1);
	}

	fclose(f);

	// Convert to image data type
	if (img.imagedata != NULL)
	{
		delete []img.imagedata;
		delete []img.imagedata2;
		delete []img.imagedata3;
	}

	size /= 3;
	img.imagedata = new PixelValue[size];
	img.imagedata2 = new PixelValue[size];
	img.imagedata3 = new PixelValue[size];
	for (int i = 0; i < size; i++)
	{
		PixelValue R = chunk[i * 3];
		PixelValue G = chunk[i * 3 + 1];
		PixelValue B = chunk[i * 3 + 2];

		ConvertToYCbCr(
			img.imagedata[i], img.imagedata2[i], img.imagedata3[i],
			R, G, B
		);
	}

	delete []chunk;
}

void Image::Save()
{
	FILE *f = fopen(fileName.c_str(), "wb");
	unsigned char* chunk;
	int size;

	printf("Writing image (width=%d height=%d channels=%d)\n",
		img.width, img.height, img.channels);

	size = img.width * img.height * img.channels;
	chunk = new unsigned char[size];
	originalSize = size;

	// Convert from image data type
	for (int i = 0; i < img.width * img.height; i++)
	{
		if (img.channels == 3)
		{
			PixelValue R, G, B;
			ConvertFromYCbCr(
				R, G, B,
				img.imagedata[i], img.imagedata2[i], img.imagedata3[i]
			);
			chunk[i * 3 + 0] = static_cast<unsigned char> (R);
			chunk[i * 3 + 1] = static_cast<unsigned char> (G);
			chunk[i * 3 + 2] = static_cast<unsigned char> (B);
		}
		else if (img.channels == 1)
		{
			chunk[i] = img.imagedata[i];
		}
	}

	printf("Writing %s (size=%d)\n", fileName.c_str(), size);
	int rval = fwrite(chunk, 1, size, f);
	if (rval != size)
	{
		printf("Error: Failed to write image to disk (%d).\n", rval);
		exit(-1);
	}

	fclose(f);
	delete []chunk;
}

void Image::GetChannelData(int channel, PixelValue* buffer, int size)
{
	if ((channel == 1 && img.imagedata == NULL) ||
		(channel == 2 && img.imagedata2 == NULL) ||
		(channel == 3 && img.imagedata3 == NULL))
	{
		printf("Error: Image data was not loaded yet.\n");
		exit(-1);
	}
	if (img.width * img.height != size)
	{
		printf("Error: Image data size mismatch.\n");
		exit(-1);
	}
	if (channel > img.channels || channel <= 0)
	{
		printf("Error: Image channel out of bounds.\n");
		exit(-1);
	}

	if (channel == 1)
		memcpy(buffer, img.imagedata, size);
	else if (channel == 2)
		memcpy(buffer, img.imagedata2, size);
	else if (channel == 3)
		memcpy(buffer, img.imagedata3, size);
}

void Image::SetChannelData(int channel, PixelValue* buffer, int size)
{
	PixelValue* imagedata = NULL;

	for (img.width = 1; img.width*img.width < size; img.width++);
	img.height = img.width;
	size = img.width * img.height;

	if (channel > img.channels)
		img.channels = channel;

	if (channel == 1)
	{
		imagedata = img.imagedata;
		img.imagedata = new PixelValue[size];
		memcpy(img.imagedata, buffer, size);
	}
	else if (channel == 2)
	{
		imagedata = img.imagedata2;
		img.imagedata2 = new PixelValue[size];
		memcpy(img.imagedata2, buffer, size);
	}
	else if (channel == 3)
	{
		imagedata = img.imagedata3;
		img.imagedata3 = new PixelValue[size];
		memcpy(img.imagedata3, buffer, size);
	}

	if (imagedata != NULL)
		delete []imagedata;
}

int Image::GetWidth()
{
	return img.width;
}

int Image::GetHeight()
{
	return img.height;
}

int Image::GetChannels()
{
	return img.channels;
}

int Image::GetOriginalSize()
{
	return originalSize;
}

void Image::ConvertToYCbCr(PixelValue& Y, PixelValue& Cb, PixelValue& Cr,
	PixelValue R, PixelValue G, PixelValue B)
{
	if (useYCbCr)
	{
		Y   = (PixelValue)(   0.299  *R + 0.587  *G + 0.114  *B       );
		Cb  = (PixelValue)( - 0.1687 *R - 0.3313 *G + 0.5    *B + 128 );
		Cr  = (PixelValue)(   0.5    *R - 0.4187 *G - 0.0813 *B + 128 );
	}
	else
	{
		Y = R;
		Cb = G;
		Cr = B;
	}
}

void Image::ConvertFromYCbCr(PixelValue& R, PixelValue& G, PixelValue& B,
	PixelValue Y, PixelValue Cb, PixelValue Cr)
{
	if (useYCbCr)
	{
		R = (PixelValue)( Y                     + 1.402   *(Cr-128) );
		G = (PixelValue)( Y - 0.34414 *(Cb-128) - 0.71414 *(Cr-128) );
		B = (PixelValue)( Y + 1.772   *(Cb-128)                     );
	}
	else
	{
		R = Y;
		G = Cb;
		B = Cr;
	}
}
