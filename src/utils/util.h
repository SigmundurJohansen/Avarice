#pragma once
#include <random>
constexpr float PI = 3.14159265f;

struct Vector2I
{
	int x;
	int y;

	Vector2I operator*(const Vector2I& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}
};

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vector2i = Vector2<int32_t>;
using Vector2ui = Vector2<uint32_t>;
using Vector2us = Vector2<uint16_t>;

struct Vector2F
{
	float x;
	float y;

	float dot(const Vector2F& _vector)
	{
		return (this->x * _vector.x + this->y * _vector.y);	
	}

	float getAngle(Vector2F& _v)
	{
		float upper = this->x * _v.x + this->y * _v.y;
		float lower = this->length() + _v.length();
		float angle = 0;
		if(lower != 0)
		 angle = acos(upper/lower);
		return _v.y > 0.0f ? angle : -angle;
	}

	Vector2F GetVector(Vector2F& _v)
	{

	}

	float length()
	{
		return (this->x * this->x + this->y * this->y);
	}

	Vector2F* Div(float _mag)
	{
		this->x = x / _mag;
		this->y = y / _mag;
		return this;
	}


	Vector2F* Norm()
	{
		float mag = sqrt(this->x * this->x + this->y * this->y);
		if (mag > 0) {
			this->Div(mag);			
		}
		return this;
	}

	Vector2F operator/(const float other)
	{
		Vector2F myVector2F = {x = 0, y =0};
		if (other != 0) {
			myVector2F.x = this->x / other;
			myVector2F.y = this->y / other;
		}
		return myVector2F;
	}

	Vector2F operator*(const Vector2F& other)
	{
		Vector2F myVector2F;
		myVector2F.x = this->x * other.x;
		myVector2F.y = this->y * other.y;
		return myVector2F;
	}
	Vector2F operator*(const float& other)
	{
		Vector2F myVector2F;
		myVector2F.x = this->x * other;
		myVector2F.y = this->y * other;
		return myVector2F;
	}

	Vector2F operator+(const Vector2F& other)
	{
		Vector2F myVector2F;
		myVector2F.x = this->x + other.x;
		myVector2F.y = this->y + other.y;
		return myVector2F;
	}
	Vector2F operator-(const Vector2F& other)
	{
		Vector2F myVector2F;
		myVector2F.x = this->x - other.x;
		myVector2F.y = this->y - other.y;
		return myVector2F;
	}
	Vector2F operator+(const float& other)
	{
		Vector2F myVector2F;
		myVector2F.x = this->x + other;
		myVector2F.y = this->y + other;
		return myVector2F;
	}
	Vector2F operator+=(const Vector2F & other)
	{
		Vector2F myVector2F;
		myVector2F.x = this->x + other.x;
		myVector2F.y = this->y + other.y;
		return myVector2F;
	}
};

float getAVectorLength(const Vector2F& _v);

float getRandRange(float width);

float getRandUnder(float width);

//float getAngle(const Vector2F& _v)
//{
//	const float a = acos((_v.x() / (_v).norm()));
//	return _v.y() > 0.0f ? a : -a;
//}

float dot(const Vector2F& _v1, const Vector2F& _v2);

float sign(const float f);