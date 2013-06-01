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

#include <string>
#include <vector>
using namespace std;

#include "Image.h"
#include "IFSTransform.h"
#include "Decoder.h"

Decoder::Decoder(int width, int height)
{
	img.width = width;
	img.height = height;
	img.channels = 3;
	img.imagedata = new PixelValue[width * height];
	img.imagedata2 = new PixelValue[width * height];
	img.imagedata3 = new PixelValue[width * height];

	// Initialize to grey image
	for (int i = 0; i < img.width * img.height; i++)
	{
		img.imagedata[i] = 127;
		img.imagedata2[i] = 127;
		img.imagedata3[i] = 127;
	}
}

Decoder::~Decoder()
{
}

void Decoder::Decode(Transforms* transforms)
{
	Transform::iterator iter;

	img.channels = transforms->channels;

	for (int channel = 1; channel <= img.channels; channel++)
	{
		PixelValue* origImage = img.imagedata;
		if (channel == 2)
			origImage = img.imagedata2;
		else if (channel == 3)
			origImage = img.imagedata3;

		// Apple each transform at a time to this channel
		iter = transforms->ch[channel-1].begin();
		for (; iter != transforms->ch[channel-1].end(); iter++)
			iter[0]->Execute(origImage, img.width, origImage, img.width, false);
	}
}

Image* Decoder::GetNewImage(string fileName, int channel)
{
	Image* temp = new Image(fileName);
	int size = img.width * img.height;

	// Get according to channel number or all channels if number is zero
	if (img.channels >= 1 && (!channel || channel == 1))
		temp->SetChannelData(1, img.imagedata, size);
	if (img.channels >= 2 && (!channel || channel == 2))
		temp->SetChannelData((!channel ? 2 : 1), img.imagedata2, size);
	if (img.channels >= 3 && (!channel || channel == 3))
		temp->SetChannelData((!channel ? 3 : 1), img.imagedata3, size);

	return temp;
}
