#pragma once
#include "stdafx.h"
//using namespace std;
#include <Windows.h>
#include <string>
#include "MyFinancesDefinitions.h"
#include "Structs.h"
#include "BaseClass.h"


class ControlClass : public BaseClass {
	friend class WindowClass;

	AlignmentTypes alignment;
	int row_width;
	int text_length;
	ControlTypes type;
	ValidationTypes validation;
	wstring validation_message;

public:
	ControlClass();
	ControlClass(HWND val_handle, SizeStruct val_size, AlignmentTypes val_alignment, ControlTypes val_type, int val_row, int val_text, ValidationTypes val_validation, wstring val_message);
	string getControlText();
	wstring getControlTextW();
	void setControlText(string text_value);
	void setControlTextW(wstring text_value);
	void visibility(bool show_control);
};

