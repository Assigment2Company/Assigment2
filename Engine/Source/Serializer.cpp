#pragma once

class Serializer {
private:

	static Serializer* serializer;

	Serializer() 
	{
	}

public:
	// deleting copy constructor
	Serializer(const Serializer& obj) = delete;

	static Serializer* getInstance()
	{
		if (serializer == nullptr)
		{
			serializer = new Serializer();
		}

		return serializer;
	}
};
