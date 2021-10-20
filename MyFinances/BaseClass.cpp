#pragma once
#include "stdafx.h"
#include <Windows.h>
#include "Structs.h"
#include "BaseClass.h"


/*=========================================
				 BASE CLASS
===========================================*/
BaseClass::BaseClass()
//empty constructor
{
	setHandle(NULL);
	setSize({});
}


BaseClass::BaseClass(HWND val_handle, SizeStruct val_size)
//constructor with values
{
	setHandle(val_handle);
	setSize(val_size);
}


HWND BaseClass::getHandle()
{
//gets the HWND handle for the control/window
	return handle;
}


SizeStruct BaseClass::getSize()
{
//Returns the 4 size values of a window/control (top, left, width, height) using a SizeStruct
	return { top, left, width, height };
}


void BaseClass::setHandle(HWND val_handle)
{
//sets the HWND handle for the control/window
	handle = val_handle;
}


void BaseClass::setSize(SizeStruct val_size)
{
//Sets the 4 size values of a window/control (top, left, width, height) using a SizeStruct
	top = val_size.top;
	left = val_size.left;
	width = val_size.width;
	height = val_size.height;
}

