#pragma once

/// Utility struct: A 4-component color
struct Color4f
{
	float r;
	float g;
	float b;
	float a;

	Color4f(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
	Color4f() : r(0), g(0), b(0), a(1) {}
};