#pragma once
//using namespace std;
#include <Windows.h>
#include <vector>
#include <string>
#include "MyFinancesDefinitions.h"
#include "Structs.h"
#include "BaseClass.h"
#include "ControlClass.h"

/*=========================================
WINDOW CLASS
Inherits from BASE CLASS

SizeStruct available in BaseCLass
===========================================*/
class WindowClass : public BaseClass {
	friend class ApplicationManagerClass;

	int window_id;

	HWND addControl(ControlTypes control_type, ControlID control_id, LPCWSTR control_text, SizeStruct control_size, AlignmentTypes control_alignment, int control_row, ValidationTypes control_validation = VAL_NOVALIDATE, wstring val_message = L"", int control_text_size = 0);
	void buildWindow();
	void setupListview(HWND listview_handle);

public:
	vector<ControlClass> controls;

	WindowClass();
	WindowClass(HWND val_handle, SizeStruct val_size, int val_id);

	void clearControls();
	bool validateForm();
	void positionControls(SizeStruct app_size);
	void visibility(bool show_window);
};