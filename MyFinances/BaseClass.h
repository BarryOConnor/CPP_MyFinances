#pragma once
#include "Structs.h"
#include <Windows.h>

class BaseClass {
	HWND handle;
	int height;
	int left;
	int top;
	int width;

public:
	BaseClass();
	BaseClass(HWND val_handle, SizeStruct val_size);
	HWND getHandle();
	SizeStruct getSize();
	void setHandle(HWND val_handle);
	void setSize(SizeStruct val_size);
};