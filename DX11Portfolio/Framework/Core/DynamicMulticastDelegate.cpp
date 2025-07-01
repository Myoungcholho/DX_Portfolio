#include "Framework.h"
#include "DynamicMulticastDelegate.h"

void FDynamicMulticastDelegate::Broadcast()
{
	for (const auto& D : Delegates)
		D.Invoker();
}

void FDynamicMulticastDelegate::Clear()
{
	Delegates.clear();
}

bool FDynamicMulticastDelegate::IsBound() const
{
	return !Delegates.empty();
}
