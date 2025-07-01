#include "Framework.h"
#include "Color.h"

FColor::FColor()
{
	A = R = G = B = 0.0f;
}

FColor::FColor(float value)
{
	A = R = G = B = value;
}

FColor::FColor(float r, float g, float b, float a)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

FColor::FColor(Vector4 value)
{
	R = value.x;
	G = value.y;
	B = value.z;
	A = value.w;
}

FColor::FColor(Vector3 value, float alpha)
{
	R = value.x;
	G = value.y;
	B = value.z;
	A = alpha;
}

FColor::FColor(UINT rgba)
{
	A = ((rgba >> 24) & 255) / 255.0f;
	B = ((rgba >> 16) & 255) / 255.0f;
	G = ((rgba >> 8) & 255) / 255.0f;
	R = (rgba & 255) / 255.0f;
}

FColor::FColor(int rgba)
{
	A = ((rgba >> 24) & 255) / 255.0f;
	B = ((rgba >> 16) & 255) / 255.0f;
	G = ((rgba >> 8) & 255) / 255.0f;
	R = (rgba & 255) / 255.0f;
}

FColor::FColor(float* values)
{
	R = values[0];
	G = values[1];
	B = values[2];
	A = values[3];
}

FColor::operator float* ()
{
	return RGBA;
}

FColor::operator const float* ()
{
	return RGBA;
}

bool FColor::operator==(const FColor& value2) const
{
	if (R == value2.R && G == value2.G && B == value2.B)
		return A == value2.A;
	else
		return false;
}

bool FColor::operator!=(const FColor& value2) const
{
	if (R == value2.R && G == value2.G && B == value2.B)
		return A != value2.A;
	else
		return true;
}

FColor FColor::operator+(const FColor& value2) const
{
	FColor color;
	color.R = R + value2.R;
	color.G = G + value2.G;
	color.B = B + value2.B;
	color.A = A + value2.A;

	return color;
}

FColor FColor::operator-(const FColor& value2) const
{
	FColor color;
	color.R = R - value2.R;
	color.G = G - value2.G;
	color.B = B - value2.B;
	color.A = A - value2.A;

	return color;
}

FColor FColor::operator*(const FColor& value2) const
{
	FColor color;
	color.R = R * value2.R;
	color.G = G * value2.G;
	color.B = B * value2.B;
	color.A = A * value2.A;

	return color;
}

FColor FColor::operator*(const float& scaleFactor) const
{
	FColor color;
	color.R = R * scaleFactor;
	color.G = G * scaleFactor;
	color.B = B * scaleFactor;
	color.A = A * scaleFactor;

	return color;
}

FColor FColor::operator/(const FColor& value2) const
{
	FColor color;
	color.R = R / value2.R;
	color.G = G / value2.G;
	color.B = B / value2.B;
	color.A = A / value2.A;

	return color;
}

FColor FColor::operator/(const float& divider) const
{
	float num = 1.0f / divider;

	FColor color;
	color.R = R * num;
	color.G = G * num;
	color.B = B * num;
	color.A = A * num;

	return color;
}

void FColor::operator+=(const FColor& value2)
{
	*this = *this + value2;
}

void FColor::operator-=(const FColor& value2)
{
	*this = *this - value2;
}

void FColor::operator*=(const FColor& value2)
{
	*this = *this * value2;
}

void FColor::operator*=(const float& scaleFactor)
{
	*this = *this * scaleFactor;
}

void FColor::operator/=(const FColor& value2)
{
	*this = *this / value2;
}

void FColor::operator/=(const float& divider)
{
	*this = *this / divider;
}

int FColor::ToRgba()
{
	UINT a = (UINT)(A * 255.0f) & 255;
	UINT r = (UINT)(R * 255.0f) & 255;
	UINT g = (UINT)(G * 255.0f) & 255;
	UINT b = (UINT)(B * 255.0f) & 255;

	UINT value = r;
	value |= g << 8;
	value |= b << 16;
	value |= a << 24;

	return (int)value;
}


int FColor::ToBgra()
{
	UINT a = (UINT)(A * 255.0f) & 255;
	UINT r = (UINT)(R * 255.0f) & 255;
	UINT g = (UINT)(G * 255.0f) & 255;
	UINT b = (UINT)(B * 255.0f) & 255;

	UINT value = b;
	value |= g << 8;
	value |= r << 16;
	value |= a << 24;

	return (int)value;
}

void FColor::ToBgra(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a)
{
	a = (unsigned char)(A * 255.0f);
	r = (unsigned char)(R * 255.0f);
	g = (unsigned char)(G * 255.0f);
	b = (unsigned char)(B * 255.0f);
}

Vector3 FColor::ToVector3()
{
	return Vector3(R, G, B);
}

Vector4 FColor::ToVector4()
{
	return Vector4(R, G, B, A);
}

std::string FColor::ToString()
{
	std::string temp = "";
	temp += "" + std::to_string(R);
	temp += "," + std::to_string(G);
	temp += "," + std::to_string(B);
	temp += "," + std::to_string(A);

	return temp;
}

FColor FColor::Add(FColor left, FColor right)
{
	return FColor(left.R + right.R, left.G + right.G, left.B + right.B, left.A + right.A);
}

FColor FColor::Subtract(FColor left, FColor right)
{
	return FColor(left.R - right.R, left.G - right.G, left.B - right.B, left.A - right.A);
}

FColor FColor::Modulate(FColor left, FColor right)
{
	return FColor(left.R * right.R, left.G * right.G, left.B * right.B, left.A * right.A);
}

FColor FColor::Scale(FColor value, float scale)
{
	return FColor(value.R * scale, value.G * scale, value.B * scale, value.A * scale);
}

FColor FColor::Negate(FColor value)
{
	return FColor(1.0f - value.R, 1.0f - value.G, 1.0f - value.B, 1.0f - value.A);
}

FColor FColor::Clamp(FColor val, FColor min, FColor max)
{
	float alpha = val.A;
	alpha = (alpha > max.A) ? max.A : alpha;
	alpha = (alpha < min.A) ? min.A : alpha;

	float red = val.R;
	red = (red > max.R) ? max.R : red;
	red = (red < min.R) ? min.R : red;

	float green = val.G;
	green = (green > max.G) ? max.G : green;
	green = (green < min.G) ? min.G : green;

	float blue = val.B;
	blue = (blue > max.B) ? max.B : blue;
	blue = (blue < min.B) ? min.B : blue;

	return FColor(red, green, blue, alpha);
}

FColor FColor::Lerp(FColor start, FColor end, float amount)
{
	Vector4 from(start.R, start.G, start.B, start.A);
	Vector4 to(end.R, end.G, end.B, end.A);
	Vector4 result = Vector4::Lerp(from, to, amount);
	return FColor(result);
}

FColor FColor::SmoothStep(FColor start, FColor end, float amount)
{
	amount = (amount <= 0) ? 0 : (amount >= 1) ? 1 : amount * amount * (3 - (2 * amount));

	return Lerp(start, end, amount);
}

FColor FColor::Max(FColor left, FColor right)
{
	FColor color;
	color.A = (left.A > right.A) ? left.A : right.A;
	color.R = (left.R > right.R) ? left.R : right.R;
	color.G = (left.G > right.G) ? left.G : right.G;
	color.B = (left.B > right.B) ? left.B : right.B;

	return color;
}

FColor FColor::Min(FColor left, FColor right)
{
	FColor color;
	color.A = (left.A < right.A) ? left.A : right.A;
	color.R = (left.R < right.R) ? left.R : right.R;
	color.G = (left.G < right.G) ? left.G : right.G;
	color.B = (left.B < right.B) ? left.B : right.B;

	return color;
}

FColor FColor::AdjustContrast(FColor value, float contrast)
{
	return FColor
	(
		0.5f + contrast * (value.R - 0.5f),
		0.5f + contrast * (value.G - 0.5f),
		0.5f + contrast * (value.B - 0.5f),
		value.A
	);
}

FColor FColor::AdjustSaturation(FColor value, float saturation)
{
	float grey = value.R * 0.2125f + value.G * 0.7154f + value.B * 0.0721f;

	return FColor
	(
		grey + saturation * (value.R - grey),
		grey + saturation * (value.G - grey),
		grey + saturation * (value.B - grey),
		value.A
	);
}

FColor FColor::Premultiply(FColor value)
{
	FColor color;
	color.A = value.A;
	color.R = value.R * value.A;
	color.G = value.G * value.A;
	color.B = value.B * value.A;

	return color;
}

FColor FColor::RandomColor()
{
	FColor color;
	color.R = FMath::Random(0.0f, 1.0f);
	color.G = FMath::Random(0.0f, 1.0f);
	color.B = FMath::Random(0.0f, 1.0f);
	color.A = 1.0f;

	return color;
}

FColor FColor::RandomColorWithAlpha()
{
	FColor color;
	color.R = FMath::Random(0.0f, 1.0f);
	color.G = FMath::Random(0.0f, 1.0f);
	color.B = FMath::Random(0.0f, 1.0f);
	color.A = FMath::Random(0.0f, 1.0f);

	return color;
}


const FColor FColor::Aqua = FColor(-256);
const FColor FColor::Beige = FColor(-2296331);
const FColor FColor::Black = FColor(-16777216);
const FColor FColor::Blue = FColor(-65536);
const FColor FColor::Brown = FColor(-14013787);
const FColor FColor::Chocolate = FColor(-14784046);
const FColor FColor::CornflowerBlue = FColor(-1206940);
const FColor FColor::Cyan = FColor(-256);
const FColor FColor::DarkBlue = FColor(-7667712);
const FColor FColor::DarkCyan = FColor(-7632128);
const FColor FColor::DarkGray = FColor(-5658199);
const FColor FColor::DarkGreen = FColor(-16751616);
const FColor FColor::DarkOrange = FColor(-16741121);
const FColor FColor::DarkRed = FColor(-16777077);
const FColor FColor::DarkSeaGreen = FColor(-7619441);
const FColor FColor::DeepPink = FColor(-7138049);
const FColor FColor::DeepSkyBlue = FColor(-16640);
const FColor FColor::GhostWhite = FColor(-1800);
const FColor FColor::Gold = FColor(-16721921);
const FColor FColor::Gray = FColor(-8355712);
const FColor FColor::Green = FColor(-16744448);
const FColor FColor::HotPink = FColor(-4953601);
const FColor FColor::Indigo = FColor(-8257461);
const FColor FColor::Ivory = FColor(-983041);
const FColor FColor::Khaki = FColor(-7543056);
const FColor FColor::Lavender = FColor(-334106);
const FColor FColor::LightBlue = FColor(-1648467);
const FColor FColor::LightCoral = FColor(-8355600);
const FColor FColor::LightCyan = FColor(-32);
const FColor FColor::LightGray = FColor(-2894893);
const FColor FColor::LightGreen = FColor(-7278960);
const FColor FColor::LightPink = FColor(-4081921);
const FColor FColor::LightSeaGreen = FColor(-5590496);
const FColor FColor::LightSkyBlue = FColor(-340345);
const FColor FColor::LightYellow = FColor(-2031617);
const FColor FColor::Lime = FColor(-16711936);
const FColor FColor::Magenta = FColor(-65281);
const FColor FColor::MediumBlue = FColor(-3342336);
const FColor FColor::MediumPurple = FColor(-2396013);
const FColor FColor::MediumSeaGreen = FColor(-9325764);
const FColor FColor::MidnightBlue = FColor(-9430759);
const FColor FColor::Navy = FColor(-8388608);
const FColor FColor::Olive = FColor(-16744320);
const FColor FColor::Orange = FColor(-16734721);
const FColor FColor::OrangeRed = FColor(-16759297);
const FColor FColor::Pink = FColor(-3424001);
const FColor FColor::Purple = FColor(-8388480);
const FColor FColor::Red = FColor(-16776961);
const FColor FColor::RoyalBlue = FColor(-2004671);
const FColor FColor::Salmon = FColor(-9273094);
const FColor FColor::SeaGreen = FColor(-11039954);
const FColor FColor::Silver = FColor(-4144960);
const FColor FColor::SkyBlue = FColor(-1323385);
const FColor FColor::SpringGreen = FColor(-8388864);
const FColor FColor::Tomato = FColor(-12098561);
const FColor FColor::Transparent = FColor(0);
const FColor FColor::Violet = FColor(-1146130);
const FColor FColor::Wheat = FColor(-4989195);
const FColor FColor::White = FColor(-1);
const FColor FColor::WhiteSmoke = FColor(-657931);
const FColor FColor::Yellow = FColor(-16711681);
const FColor FColor::YellowGreen = FColor(-13447782);