#pragma once
#include <wtypes.h>

struct Texture2D
{
	Texture2D() = default;
	Texture2D(UINT w, UINT h, void* d): width(w), height(h), data(d) {}
	Texture2D(Texture2D& other) = delete;

	UINT width;
	UINT height;
	void* data = nullptr;
};