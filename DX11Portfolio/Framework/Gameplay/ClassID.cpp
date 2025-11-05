#include "Framework.h"
#include "ClassID.h"

void ClassID::Register(const string& name, FactoryFunc func)
{
	GetRegistry()[name] = func;
}

AActor* ClassID::Create(const string& name, UWorld* world)
{
	auto& registry = GetRegistry();
	auto it = registry.find(name);
	assert(it != registry.end() && "Class not registered!");
	return it->second(world);
}

bool ClassID::Exists(const string& name)
{
	return GetRegistry().count(name) > 0;
}
