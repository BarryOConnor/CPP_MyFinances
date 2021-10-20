#pragma once
#include "stdafx.h"
#include <Windows.h>
#include <vector>
#include <string>
#include "MyFinancesDefinitions.h"
#include "Structs.h"
#include "BaseClass.h"
#include "ControlClass.h"
//using namespace std;
//class ApplicationManagerClass;
//extern ApplicationManagerClass app_mgr;

ControlClass::ControlClass()
//empty constructor
{
	setHandle(NULL);  
	setSize(SizeStruct());
	alignment = CONTROL_LEFT;  //display alignment of the control (DSP_CONTROL_LEFT, SP_CONTROL_RIGHT, DSP_CONTROL_CENTER)
	row_width = NULL;  //width of the row (multiple controls on one line) - used to position controls
	text_length = NULL; //length of the text in the control (only really applies to textboxes)
	type = CRL_NONE; //crontrol type
	validation = VAL_NOVALIDATE; //default is not to validate since only certain textboxes validate
	validation_message = L""; //empty wstring
}


ControlClass::ControlClass(HWND val_handle, SizeStruct val_size, AlignmentTypes val_alignment, ControlTypes val_type, int val_row, int val_text, ValidationTypes val_validation, wstring val_message)
//constructor with values
{
	setHandle(val_handle);
	setSize(val_size);
	alignment = val_alignment;
	row_width = val_row;
	text_length = val_text;
	type = val_type;
	validation = val_validation;
	validation_message = val_message;
}


string ControlClass::getControlText() {
	//Gets the text of a control and returns it as a wstring

	int len = GetWindowTextLengthW(getHandle()) + 1; //get the length of the control text and add 1 for the null terminator
	vector<wchar_t> wchar_buffer(len);  //create a vector of wchar_t characters of the same size as the text
	GetWindowText(getHandle(), &wchar_buffer[0], len); //get the window text and store it in the address of the buffer
	string new_text = app_mgr.convertStrings(&wchar_buffer[0]);
	return new_text; //return the buffer as a wstring
}


wstring ControlClass::getControlTextW() {
	//Gets the text of a control and returns it as a wstring

	int len = GetWindowTextLengthW(getHandle()) + 1; //get the length of the control text and add 1 for the null terminator
	vector<wchar_t> wchar_buffer(len);  //create a vector of wchar_t characters of the same size as the text
	GetWindowText(getHandle(), &wchar_buffer[0], len); //get the window text and store it in the address of the buffer
	return &wchar_buffer[0]; //return the buffer as a wstring
}


void ControlClass::setControlText(string text_value)
//Gets the text of a control and returns it as a wstring
{
	SetWindowTextA(getHandle(), text_value.c_str()); //get the window text and store it in the address of the buffer
}


void ControlClass::setControlTextW(wstring text_value)
//Gets the text of a control and returns it as a wstring
{
	SetWindowTextW(getHandle(), text_value.c_str());
}

void ControlClass::visibility(bool show_control) {
	/*
	Method used to show / hide a control.
	*/
	if (show_control) {
		ShowWindow(getHandle(), SW_SHOW);
	}
	else {
		ShowWindow(getHandle(), SW_HIDE);
	}


}
