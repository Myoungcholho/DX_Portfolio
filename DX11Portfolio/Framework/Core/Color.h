#pragma once

#include <directxtk/SimpleMath.h>
#include <string>

using namespace DirectX::SimpleMath;

class FColor
{
public:
	FColor();
	FColor(float value);
	FColor(float r, float g, float b, float a);
	FColor(Vector4 value);
	FColor(Vector3 value, float alpha);
	FColor(UINT rgba);
	FColor(int rgba);
	FColor(float* values);

	operator float* ();
	operator const float* ();

	bool operator ==(const FColor& value2) const;
	bool operator !=(const FColor& value2) const;

	FColor operator +(const FColor& value2) const;
	FColor operator -(const FColor& value2) const;
	FColor operator *(const FColor& value2) const;
	FColor operator *(const float& scaleFactor) const;
	FColor operator /(const FColor& value2) const;
	FColor operator /(const float& divider) const;

	void operator +=(const FColor& value2);
	void operator -=(const FColor& value2);
	void operator *=(const FColor& value2);
	void operator *=(const float& scaleFactor);
	void operator /=(const FColor& value2);
	void operator /=(const float& divider);

	int ToRgba();
	int ToBgra();
	void ToBgra(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a);

	Vector3 ToVector3();
	Vector4 ToVector4();

	std::string ToString();

	static FColor Add(FColor left, FColor right);
	static FColor Subtract(FColor left, FColor right);
	static FColor Modulate(FColor left, FColor right);
	static FColor Scale(FColor value, float scale);
	static FColor Negate(FColor value);

	static FColor Clamp(FColor val, FColor min, FColor max);
	static FColor Lerp(FColor start, FColor end, float amount);
	static FColor SmoothStep(FColor start, FColor end, float amount);
	static FColor Max(FColor left, FColor right);
	static FColor Min(FColor left, FColor right);

	static FColor AdjustContrast(FColor value, float contrast);
	static FColor AdjustSaturation(FColor value, float saturation);
	static FColor Premultiply(FColor value);

	static FColor RandomColor();
	static FColor RandomColorWithAlpha();

public:
	static const FColor Aqua;
	static const FColor Beige;
	static const FColor Black;
	static const FColor Blue;
	static const FColor Brown;
	static const FColor Chocolate;
	static const FColor CornflowerBlue;
	static const FColor Cyan;
	static const FColor DarkBlue;
	static const FColor DarkCyan;
	static const FColor DarkGray;
	static const FColor DarkGreen;
	static const FColor DarkOrange;
	static const FColor DarkRed;
	static const FColor DarkSeaGreen;
	static const FColor DeepPink;
	static const FColor DeepSkyBlue;
	static const FColor GhostWhite;
	static const FColor Gold;
	static const FColor Gray;
	static const FColor Green;
	static const FColor HotPink;
	static const FColor Indigo;
	static const FColor Ivory;
	static const FColor Khaki;
	static const FColor Lavender;
	static const FColor LightBlue;
	static const FColor LightCoral;
	static const FColor LightCyan;
	static const FColor LightGray;
	static const FColor LightGreen;
	static const FColor LightPink;
	static const FColor LightSeaGreen;
	static const FColor LightSkyBlue;
	static const FColor LightYellow;
	static const FColor Lime;
	static const FColor Magenta;
	static const FColor MediumBlue;
	static const FColor MediumPurple;
	static const FColor MediumSeaGreen;
	static const FColor MidnightBlue;
	static const FColor Navy;
	static const FColor Olive;
	static const FColor Orange;
	static const FColor OrangeRed;
	static const FColor Pink;
	static const FColor Purple;
	static const FColor Red;
	static const FColor RoyalBlue;
	static const FColor Salmon;
	static const FColor SeaGreen;
	static const FColor Silver;
	static const FColor SkyBlue;
	static const FColor SpringGreen;
	static const FColor Tomato;
	static const FColor Transparent;
	static const FColor Violet;
	static const FColor Wheat;
	static const FColor White;
	static const FColor WhiteSmoke;
	static const FColor Yellow;
	static const FColor YellowGreen;

public:
	union
	{
		struct
		{
			float R, G, B, A;
		};

		float RGBA[4];
	};
};
