#ifndef _MODULE_WINDOW_H_
#define _MODULE_WINDOW_H_

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	~ModuleWindow();

	bool Init() override;
	bool CleanUp() override;


	int GetWidth() const;
	int GetHeight() const;
	float GetAspectRatio() const;

public:

	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr;
};

#endif /* _MODULE_WINDOW_H_ */