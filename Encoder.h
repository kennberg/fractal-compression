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

#ifndef E_H
#define E_H

class Encoder
{
public:

	virtual ~Encoder() {};

	virtual Transforms* Encode(Image* source) = 0;

	// These functions are helpers
	int GetAveragePixel(PixelValue* domainData, int domainWidth,
		int domainX, int domainY, int size);

	double GetScaleFactor(
		PixelValue* domainData, int domainWidth, int domainX, int domainY, int domainAvg,
		PixelValue* rangeData, int rangeWidth, int rangeX, int rangeY, int rangeAvg,
		int size);

	double GetError(
		PixelValue* domainData, int domainWidth, int domainX, int domainY, int domainAvg,
		PixelValue* rangeData, int rangeWidth, int rangeX, int rangeY, int rangeAvg,
		int size, double scale);

protected:
	ImageData img;
};

#endif // E_H
