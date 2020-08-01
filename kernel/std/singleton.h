#pragma once
#include "move.h"
#include "new.h"

template <typename T>
class Singleton
{
public:
    template<typename... Args>
	inline static T* GetInstance(Args&&... args)
	{
		static auto instance = new T(forward<Args>(args)...);
		return instance;
	}
};