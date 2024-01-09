#ifndef _MODULE_INPUT_H_
#define _MODULE_INPUT_H_

#include "Module.h"
#include "Globals.h"


#include "SDL_scancode.h"
#include "Math/float2.h"

#define NUM_MOUSE_BUTTONS 5

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{

public:

	ModuleInput();

	// Destructor
	~ModuleInput();

	bool Init() override;
	update_status PreUpdate() override;
	bool CleanUp() override;

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const { return keyboard[id]; }

	KeyState GetMouseButtonDown(int id) const { return mouse_buttons[id - 1]; }

	// Check for window events last frame
	bool GetWindowEvent(EventWindow code) const;

	// Get mouse / axis position
	const float2& GetMouseMotion() const;
	const float2& GetMousePosition() const;

private:
	bool windowEvents[WE_COUNT];
	KeyState* keyboard;
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	float2 mouse_motion;
	float2 mouse;
};

#endif /* _MODULE_INPUT_H_ */