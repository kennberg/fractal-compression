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

#ifndef IFST_H
#define IFST_H

typedef vector<class IFSTransform*> Transform;

class Transforms
{
public:
	Transforms();

	~Transforms();

public:
	Transform ch[3];
	int channels;
};


class IFSTransform
{
public:

	enum SYM
	{
		SYM_NONE = 0,
		SYM_R90,
		SYM_R180,
		SYM_R270,
		SYM_HFLIP,
		SYM_VFLIP,
		SYM_FDFLIP,
		SYM_RDFLIP,
		SYM_MAX
	};

public:

	static PixelValue* DownSample(PixelValue* src, int srcWidth,
		int startX, int startY, int targetSize);

public:

	IFSTransform(int fromX, int fromY, int toX, int toY, int size,
		SYM symmetry, double scale, int offset);

	~IFSTransform();

	void Execute(PixelValue* src, int srcWidth,
		PixelValue* dest, int destWidth, bool downsampled);

private:

	bool isScanlineOrder();

	bool isPositiveX();

	bool isPositiveY();

private:

	// Spatial transformation
	int fromX;
	int fromY;
	int toX;
	int toY;
	int size;

	// Symmetry operation
	SYM symmetry;

	// Pixel intensity
	double scale;
	int offset;

};

#endif // IFST_H
