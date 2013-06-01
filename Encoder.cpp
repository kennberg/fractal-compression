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
#include <vector>
#include <string>
using namespace std;

#include "Image.h"
#include "IFSTransform.h"
#include "Encoder.h"

double Encoder::GetScaleFactor(
	PixelValue* domainData, int domainWidth, int domainX, int domainY, int domainAvg,
	PixelValue* rangeData, int rangeWidth, int rangeX, int rangeY, int rangeAvg,
	int size)
{
	int top = 0;
	int bottom = 0;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			int domain = (domainData[(domainY + y) * domainWidth + (domainX + x)] - domainAvg);
			int range = (rangeData[(rangeY + y) * rangeWidth + (rangeX + x)] - rangeAvg);

			// According to the formula we want (R*D) / (D*D)
			top += range * domain;
			bottom += domain * domain;

			if (bottom < 0)
			{
				printf("Error: Overflow occured during scaling %d %d %d %d\n",
					y, domainWidth, bottom, top);
				exit(-1);
			}
		}
	}

	if (bottom == 0)
	{
		top = 0;
		bottom = 1;
	}

	return ((double)top) / ((double)bottom);
}

double Encoder::GetError(
	PixelValue* domainData, int domainWidth, int domainX, int domainY, int domainAvg,
	PixelValue* rangeData, int rangeWidth, int rangeX, int rangeY, int rangeAvg,
	int size, double scale)
{
	double top = 0;
	double bottom = (double)(size * size);

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			int domain = (domainData[(domainY + y) * domainWidth + (domainX + x)] - domainAvg);
			int range = (rangeData[(rangeY + y) * rangeWidth + (rangeX + x)] - rangeAvg);
			int diff = (int)(scale * (double)domain) - range;

			// According to the formula we want (DIFF*DIFF)/(SIZE*SIZE)
			top += (diff * diff);

			if (top < 0)
			{
				printf("Error: Overflow occured during error %lf\n", top);
				exit(-1);
			}
		}
	}

	return (top / bottom);
}

int Encoder::GetAveragePixel(PixelValue* domainData, int domainWidth,
	int domainX, int domainY, int size)
{
	int top = 0;
	int bottom = (size * size);

	// Simple average of all pixels.
	for (int y = domainY; y < domainY + size; y++)
	{
		for (int x = domainX; x < domainX + size; x++)
		{
			top += domainData[y * domainWidth + x];

			if (top < 0)
			{
				printf("Error: Accumulator rolled over averaging pixels.\n");
				exit(-1);
			}
		}
	}

	return (top / bottom);
}

