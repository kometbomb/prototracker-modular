#pragma once

#include "SDL.h"

class Lockable
{
    mutable SDL_mutex *mMutex;
	mutable SDL_SpinLock mSpinlock;
	mutable int mLockCounter;

public:
    Lockable();
    ~Lockable();

    void lock() const;
	void unlock() const;
};
