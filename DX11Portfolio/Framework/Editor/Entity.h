#pragma once

class Entity
{
public:
	Entity();
	virtual ~Entity();

	void SetName(const std::string& name) { mName = name; }
	std::string& GetName() { return mName; }

private:
	std::string mName;
};