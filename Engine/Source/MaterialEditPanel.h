#pragma once
#include "Panel.h"
#define MATERIALEDITPANEL "MaterialEdit##"

class MaterialEditPanel : public Panel
{
public:
	MaterialEditPanel();
	~MaterialEditPanel();

	void Draw(int windowFlags) override;
};

