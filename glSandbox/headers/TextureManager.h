#pragma once
#include "ResourceManager.h"
#include "Texture.h"

class TextureManager : public ResourceManager<Texture>
{
public:
	static void initialize();
	static Texture* uvChecker1();
	static Texture* uvChecker2();
	static Texture* uvChecker3S();
	static Texture* uvChecker3M();
	static Texture* uvChecker3L();
	static Texture* uvChecker3XL();
};
