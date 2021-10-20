#pragma once
#include "stdafx.h"
//using namespace std;
//class ApplicationManagerClass;
//extern ApplicationManagerClass app_mgr;
#include <Windows.h>
#include <CommCtrl.h>
#include <vector>
#include <string>
#include <regex>
#include "resource.h"
#include "MyFinancesDefinitions.h"
#include "Structs.h"
#include "BaseClass.h"
#include "ControlClass.h"
#include "WindowClass.h"




/*=========================================
				WINDOW CLASS
===========================================*/


WindowClass::WindowClass()
{
	setHandle(NULL);
	setSize(SizeStruct());
	window_id = NULL;  // ID of the window 
}



WindowClass::WindowClass(HWND val_handle, SizeStruct val_size, int val_id)
{
	setHandle(val_handle);
	setSize(val_size);
	window_id = val_id; // ID of the window
}




HWND WindowClass::addControl(ControlTypes control_type, ControlID control_id, LPCWSTR control_text, SizeStruct control_size, AlignmentTypes control_alignment, int control_row, ValidationTypes control_validation, wstring val_message, int control_text_size) {
	/*
	Method used to add controls to a window
		control_type = type of control type(label, textbox etc)
		control_id = ID for the control in the WM_COMMAND message loop
		control_text = text on the control
		control_size = struct containing top, left, height and width values for the control
		control_alignment = value for the control alignment
		control_row = size of the "row" the control is on, used for positioning
		control_validation = type of validation to perform on the control text
		val_message = validation message to be displayed if the validation fails
		control_text_size = size of the control's text
	*/


	HWND temp_handle = NULL;
	switch (control_type)
	{
	case CRL_IMAGE: // create the header image
	{
		HBITMAP hLogoBitmap = LoadBitmap(app_mgr.getAppInstance(), MAKEINTRESOURCE(IDB_HEADER)); //load image from resource file
		temp_handle = CreateWindowW(L"Static", control_text, WS_VISIBLE | WS_CHILD | SS_BITMAP, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		SendMessageW(temp_handle, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogoBitmap);  //update the control with the loaded image
	}
	break;

	case CRL_LABEL_HEADING: // create a label
		temp_handle = CreateWindowW(L"static", control_text, WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE | SS_CENTER, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		break;

	case CRL_LABEL_SIDEBAR: // create a label
		temp_handle = CreateWindowW(L"static", control_text, WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE | SS_CENTER, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		break;

	case CRL_LABEL: // create a label
		temp_handle = CreateWindowW(L"static", control_text, WS_VISIBLE | WS_CHILD | SS_RIGHT, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		break;

	case CRL_TEXTBOX: // create a textbox
		temp_handle = CreateWindowW(L"edit", control_text, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		break;

	case CRL_TEXTBOX_SCROLL: // create a textbox
		temp_handle = CreateWindowW(L"edit", control_text, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		break;

	case CRL_PASSWORD: // create a password
		temp_handle = CreateWindowW(L"edit", control_text, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_PASSWORD, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		break;

	case CRL_BUTTON_SIDEBAR:
	case CRL_BUTTON: // create a button
		temp_handle = CreateWindowW(L"BUTTON", control_text, WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		break;

	case CRL_COMBO: // create a combobox
		temp_handle = CreateWindowW(WC_COMBOBOX, TEXT(""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWN |  CBS_DROPDOWNLIST, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		break;

	case CRL_DATEPICKER_SIDEBAR:
	case CRL_DATEPICKER: // create a combobox
		temp_handle = CreateWindowW(DATETIMEPICK_CLASS, TEXT("DateTime"), WS_BORDER | WS_CHILD | WS_VISIBLE | DTS_SHORTDATECENTURYFORMAT, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		DateTime_SetFormat(temp_handle, L"yyyy'-'MM'-'dd");
		break;


	case CRL_LISTVIEW: // creating a listview is more complicated so using a function to separate it
		temp_handle = CreateWindowW(WC_LISTVIEWW, L"", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_BORDER | WS_TABSTOP | LVS_SINGLESEL | LVS_SHOWSELALWAYS, control_size.top, control_size.left, control_size.width, control_size.height, getHandle(), (HMENU)control_id, NULL, NULL);
		setupListview(temp_handle);
		break;
	};
	controls.push_back({ temp_handle, control_size, control_alignment, control_type, control_row, control_text_size, control_validation, val_message });  //push the control onto the controls vector

	if (control_type != CRL_IMAGE) { //if it's not an image, set the font
		if (control_type == CRL_LABEL_HEADING) { //set heading font
			SendMessage(temp_handle, WM_SETFONT, app_mgr.getHeadingFont(), TRUE);
		}
		else {
			SendMessage(temp_handle, WM_SETFONT, app_mgr.getControlFont(), TRUE);
		}
	}
	return temp_handle;

}





void WindowClass::buildWindow() {
	/*
	Method used to create the required controls for each window. Takes the handle of the window
		parent = HWND handle to the parent window
	*/

	SizeStruct new_size = { NULL, NULL, NULL, NULL }; // struct to p
	int top_point = 0;
	int row_width = 0;

	switch (window_id)
	{
	case WIN_MAIN:  // The Main Window
	{
		//The Header Image
		new_size = { APP_DEFAULT_XY, APP_DEFAULT_XY, APP_SCREEN_X, APP_HEADER_HEIGHT };
		addControl(CRL_IMAGE, ID_NULL, NULL, new_size, CONTROL_CENTER, 1920);
	}
	break;
	case WIN_LOGIN:  // The Login Window
	{
		row_width = APP_HEADING_WIDTH;
		//label control (HEADING)
		new_size = { top_point, APP_DEFAULT_XY, APP_HEADING_WIDTH, APP_HEADING_HEIGHT };
		addControl(CRL_LABEL_HEADING, ID_HEADING_LABEL, L"Login", new_size, CONTROL_CENTER, row_width);


		//new row - set position values
		top_point = APP_CONTROL_TOP;  //starting point for controls
		row_width = APP_LABEL_WIDTH + APP_PADDING + APP_TEXT_DOUBLE;  //Row represents the width of all controls on a row plus padding
		//label control (Username)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT }; //updates the struct with the new control's size
		addControl(CRL_LABEL, ID_NULL, L"Username", new_size, CONTROL_LEFT, row_width);
		//text control (Username)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_TEXTBOX, ID_NULL, NULL, new_size, CONTROL_RIGHT, row_width, VAL_NOTNULL, L"Username cannot be empty", 100);

		//new row - set position values 
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (Password)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Password", new_size, CONTROL_LEFT, row_width);
		//text control (Password)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_PASSWORD, ID_NULL, NULL, new_size, CONTROL_RIGHT, row_width, VAL_NOTNULL, L"Password cannot be empty", 100);

		//new row - set position values
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		row_width = APP_BUTTON_WIDTH + APP_PADDING + APP_BUTTON_WIDTH;
		//button control (Create Account)
		new_size = { top_point, APP_DEFAULT_XY, APP_BUTTON_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON, BTN_LOGIN_CREATE_ACCOUNT_ID, L"Create Account", new_size, CONTROL_LEFT, row_width);
		//button control (Login)
		new_size = { top_point, APP_DEFAULT_XY, APP_BUTTON_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON, BTN_LOGIN_LOGIN_ID, L"Login", new_size, CONTROL_RIGHT, row_width);

		//set the default button for the window - this is the last control added on the page
		SendMessage(getHandle(), DM_SETDEFID, (WPARAM)controls.back().getHandle(), 0);
	}
	break;
	case WIN_ACCOUNT:
	{
		row_width = APP_HEADING_WIDTH;
		//label control (HEADING)
		new_size = { top_point, APP_DEFAULT_XY, APP_HEADING_WIDTH, APP_HEADING_HEIGHT };
		addControl(CRL_LABEL_HEADING, ID_HEADING_LABEL, L"Create New Account", new_size, CONTROL_CENTER, row_width);

		//new row - set position values
		top_point = APP_CONTROL_TOP;
		row_width = APP_LABEL_WIDTH + APP_PADDING + APP_TEXT_DOUBLE;
		//label control (Full Name)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Full Name", new_size, CONTROL_LEFT, row_width);
		//text control (Full Name)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_TEXTBOX, ID_NULL, NULL, new_size, CONTROL_RIGHT, row_width, VAL_NOTNULL, L"Full Name cannot be empty", 100);

		//new row - set position values
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (Username)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Username", new_size, CONTROL_LEFT, row_width);
		//text control (Username)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_TEXTBOX, ID_NULL, NULL, new_size, CONTROL_RIGHT, row_width, VAL_NOTNULL, L"Username cannot be empty", 100);

		//new row - set position values
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (password)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Password", new_size, CONTROL_LEFT, row_width);
		//text control (password)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_PASSWORD, ID_NULL, NULL, new_size, CONTROL_RIGHT, row_width, VAL_NOTNULL, L"Password cannot be empty", 100);

		//new row - set position values
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (Email)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Email", new_size, CONTROL_LEFT, row_width);
		//text control (Email)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_TEXTBOX, ID_NULL, NULL, new_size, CONTROL_RIGHT, row_width, VAL_EMAIL, L"Email cannot be empty and must be a valid email address", 100);

		//new row - set position values
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (Secret question)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Secret Question", new_size, CONTROL_LEFT, row_width);
		//combobox control (secret question)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_COMBO, ID_NULL, NULL, new_size, CONTROL_RIGHT, row_width, VAL_INTEGER, L"Please choose a Secret Question", 100);


		//populate the combobox with the following strings
		TCHAR Questions[4][14] = { TEXT("Mother's Name"), TEXT("First Pet"), TEXT("Town of Birth"), TEXT("First Job") };
		//temp store to pass the strings to the control
		TCHAR temp_store[16];
		//copy the string
		memset(&temp_store, 0, sizeof(temp_store));
		for (int loopcount = 0; loopcount <= 3; loopcount += 1)
		{
			//copy the string across
			wcscpy_s(temp_store, sizeof(temp_store) / sizeof(TCHAR), (TCHAR*)Questions[loopcount]);
			// Add string to combobox via a windows message
			SendMessage(controls[CBO_ACCOUNT_SECRET_QUESTION].getHandle(), (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)temp_store);
		}

		// Set the current selection in the combobox via windows message
		SendMessage(controls[CBO_ACCOUNT_SECRET_QUESTION].getHandle(), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);  

		//new row - set position values
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (Answer)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Answer", new_size, CONTROL_LEFT, row_width);
		//text control (answer)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_TEXTBOX, ID_NULL, NULL, new_size, CONTROL_RIGHT, row_width, VAL_NOTNULL, L"Answer cannot be empty", 100);

		//new row - set position values
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		row_width = APP_BUTTON_WIDTH + APP_PADDING + APP_BUTTON_WIDTH;
		//BUTTON control (Create Account)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON, BTN_ACCOUNT_RETURN_LOGIN_ID, L"Return to Login", new_size, CONTROL_LEFT, row_width);
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON, BTN_ACCOUNT_NEW_ACCOUNT_ID, L"Create Account", new_size, CONTROL_RIGHT, row_width);

		//set the default button for the window - this is the last control added on the page
		SendMessage(getHandle(), DM_SETDEFID, (WPARAM)controls.back().getHandle(), 0);
	}
	break;
	case WIN_BUDGET:  // The Budget Window
	{
		row_width = APP_HEADING_WIDTH;
		//label control (HEADING)
		new_size = { top_point, APP_DEFAULT_XY, APP_HEADING_WIDTH, APP_HEADING_HEIGHT };
		addControl(CRL_LABEL_HEADING, ID_HEADING_LABEL, L"Welcome", new_size, CONTROL_CENTER, row_width);
		
		//listview control
		top_point = APP_HEADING_HEIGHT + APP_PADDING;
		row_width = APP_START_WIDTH -50;
		new_size = { top_point, APP_DEFAULT_XY, APP_LISTVIEW_WIDTH, APP_LISTVIEW_HEIGHT };
		addControl(CRL_LISTVIEW, LVW_BUDGET_TRANSACTIONS_ID, L"", new_size, CONTROL_LEFT, row_width);

		//sidebar controls
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL_SIDEBAR, ID_NULL, L"Total Income", new_size, CONTROL_RIGHT, row_width);

		top_point += APP_CONTROL_HEIGHT;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL_SIDEBAR, ID_NULL, L"0", new_size, CONTROL_RIGHT, row_width);

		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL_SIDEBAR, ID_NULL, L"Total Spent", new_size, CONTROL_RIGHT, row_width);

		top_point += APP_CONTROL_HEIGHT;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL_SIDEBAR, ID_NULL, L"0", new_size, CONTROL_RIGHT, row_width);

		//search heading
		top_point += APP_CONTROL_HEIGHT * 3;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL_SIDEBAR, ID_NULL, L"Date Range", new_size, CONTROL_RIGHT, row_width);

		// Date Pickers
		top_point += APP_CONTROL_HEIGHT;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL_SIDEBAR, ID_NULL, L"Date From", new_size, CONTROL_RIGHT, row_width);

		top_point += APP_CONTROL_HEIGHT;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_DATEPICKER_SIDEBAR, DPK_BUDGET_FROM_ID, NULL, new_size, CONTROL_RIGHT, row_width, VAL_DATE, L"Select a date", 100);

		// Date Pickers
		top_point += APP_CONTROL_HEIGHT;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL_SIDEBAR, ID_NULL, L"Date To", new_size, CONTROL_RIGHT, row_width);

		top_point += APP_CONTROL_HEIGHT;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_DATEPICKER_SIDEBAR, DPK_BUDGET_TO_ID, NULL, new_size, CONTROL_RIGHT, row_width, VAL_DATE, L"Select a date", 100);

		//sidebar buttons
		//button control (add)
		top_point += APP_CONTROL_HEIGHT *3;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON_SIDEBAR, BTN_BUDGET_ADDNEW_ID, L"Add New Entry", new_size, CONTROL_RIGHT, row_width);

		//button control (edit)
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON_SIDEBAR, BTN_BUDGET_EDIT_ID, L"Edit Selected", new_size, CONTROL_RIGHT, row_width);

		//button control (delete)
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON_SIDEBAR, BTN_BUDGET_DELETE_ID, L"Delete Selected", new_size, CONTROL_RIGHT, row_width);
	}
	break;
	case WIN_DIALOG:  // The DIALOG Window
	{
		row_width = (APP_START_WIDTH / 2);
		//label control (Heading)
		new_size = { top_point, APP_DEFAULT_XY, (APP_START_WIDTH / 2) + (APP_PADDING * 2), APP_HEADING_HEIGHT };
		addControl(CRL_LABEL_HEADING, ID_HEADING_LABEL, L"Add a Transaction", new_size, CONTROL_DIALOG_CENTER, row_width);


		//new row - set position values
		top_point = APP_CONTROL_TOP - APP_PADDING;  //starting point for controls
		//label control (date)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT }; //updates the struct with the new control's size
		addControl(CRL_LABEL, ID_NULL, L"Transaction Date", new_size, CONTROL_DIALOG_LEFT, row_width);
		//text control (date)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_DATEPICKER, BTN_DIALOG_DATEPICKER_ID, NULL, new_size, CONTROL_DIALOG_RIGHT, row_width, VAL_DATE, L"Select a date", 100);

		//new row - set position values 
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (type)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Transaction Type", new_size, CONTROL_DIALOG_LEFT, row_width);
		//text control (type)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_COMBO, ID_NULL, NULL, new_size, CONTROL_DIALOG_RIGHT, row_width, VAL_INTEGER, L"Select a transaction type", 100);
		//populate the combobox with the following strings
		TCHAR Types[10][9] = { TEXT("BAC"), TEXT("C/L"), TEXT("DPC"), TEXT("EDC"), TEXT("POS"), TEXT("FOOD"), TEXT("BILL"), TEXT("GIFT-OUT"), TEXT("GIFT-IN"), TEXT("TRAVEL") };
		//temp store to pass the strings to the control
		TCHAR temp_store[20];
		//copy the string
		memset(&temp_store, 0, sizeof(temp_store));
		for (int loopcount = 0; loopcount <= 9; loopcount += 1)
		{
			//copy the string across
			wcscpy_s(temp_store, sizeof(temp_store) / sizeof(TCHAR), (TCHAR*)Types[loopcount]);
			// Add string to combobox via a windows message
			SendMessage(controls[CBO_DIALOG_TYPE].getHandle(), (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)temp_store);
		}

		// Set the current selection in the combobox via windows message
		SendMessage(controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		//new row - set position values 
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (description)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Transaction Description", new_size, CONTROL_DIALOG_LEFT, row_width);
		//text control (description)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_TEXTBOX_SCROLL, ID_NULL, NULL, new_size, CONTROL_DIALOG_RIGHT, row_width, VAL_NOTNULL, L"Description cannot be empty", 100);

		//new row - set position values 
		top_point += APP_CONTROL_HEIGHT + APP_PADDING;
		//label control (amount)
		new_size = { top_point, APP_DEFAULT_XY, APP_LABEL_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_LABEL, ID_NULL, L"Transaction Amount", new_size, CONTROL_DIALOG_LEFT, row_width);
		//text control (amount)
		new_size = { top_point, APP_DEFAULT_XY, APP_TEXT_DOUBLE, APP_CONTROL_HEIGHT };
		addControl(CRL_TEXTBOX, ID_NULL, NULL, new_size, CONTROL_DIALOG_RIGHT, row_width, VAL_REAL, L"Must be an amount", 100);

		//CREATE a hidden label to store the transaction ID for edits
		new_size = { 1000, 0, 0,0 };
		addControl(CRL_BUTTON, ID_NULL, L"Cancel", new_size, CONTROL_DIALOG_LEFT, row_width);


		//new row - set position values
		top_point += APP_CONTROL_HEIGHT * 2 + APP_PADDING;
		row_width = 1000;
		//button control (Cancel)
		new_size = { top_point, APP_DEFAULT_XY, APP_BUTTON_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON, BTN_DIALOG_CANCEL_ID, L"Cancel Edit", new_size, CONTROL_DIALOG_LEFT, row_width);
		//button control (EDIT)
		new_size = { top_point, APP_DEFAULT_XY, APP_BUTTON_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON, BTN_DIALOG_EDIT_ID, L"Edit Transaction", new_size, CONTROL_DIALOG_RIGHT, row_width);
		//button control (ADD)
		new_size = { top_point, APP_DEFAULT_XY, APP_BUTTON_WIDTH, APP_CONTROL_HEIGHT };
		addControl(CRL_BUTTON, BTN_DIALOG_ADD_ID, L"Add Transaction", new_size, CONTROL_DIALOG_RIGHT, row_width);

		


		//set the default button for the window - this is the last control added on the page
		SendMessage(getHandle(), DM_SETDEFID, (WPARAM)controls.back().getHandle(), 0);
	}
	break;

	};

}






void WindowClass::clearControls() {
	/*
	Method used to empty the controls on a form after use.
	*/

	bool first_input_control = true; //check for first editable control 
	for (vector<ControlClass>::iterator this_control = controls.begin(); this_control != controls.end(); ++this_control)  //iterate the vector
	{
		switch (this_control->type)
		{
		case CRL_TEXTBOX: // create a textbox
			this_control->setControlText("");
			if (first_input_control) {  //is the first editable control?
				SetFocus(this_control->getHandle()); //if so, set focus to it
				first_input_control = false;
			}
			break;
		case CRL_PASSWORD: // create a textbox
			this_control->setControlText("");
			if (first_input_control) {  //is the first editable control?
				SetFocus(this_control->getHandle()); //if so, set focus to it
				first_input_control = false;
			}
			break;
		case CRL_COMBO: // create a combobox
			SendMessage(this_control->getHandle(), CB_SETCURSEL, 0, NULL);
			if (first_input_control) {  //is the first editable control?
				SetFocus(this_control->getHandle()); //if so, set focus to it
				first_input_control = false;
			}
			break;
		};
	};
}




void WindowClass::positionControls(SizeStruct app_size) {
	/*
	Method used to position the controls on a window based upon a supplied SizeStruct
		app_size = struct containing top, left, height and width values for the app
	*/

	if (app_size.width > APP_SCREEN_X) app_size.width = APP_SCREEN_X; //max of screen width
	if (app_size.height > APP_SCREEN_Y) app_size.height = APP_SCREEN_Y; //max of screen height

	for (vector<ControlClass>::iterator this_control = controls.begin(); this_control != controls.end(); ++this_control)  //iterate the vector
	{
		SizeStruct control_size = this_control->getSize();

		if (IsWindow(this_control->getHandle())) //child is initialised, safe to edit
		{
			int left_side;

			//adjust values to position controls
			switch (this_control->alignment)
			{
			case CONTROL_CENTER:  // center the control
				left_side = (app_size.width - control_size.width) / 2;
				break;


			case CONTROL_LEFT:   // align the control left
				left_side = (app_size.width - this_control->row_width) / 2;
				if (this_control->type == CRL_LISTVIEW) {
					//special code for the listview so it expands and contracts
					control_size = app_mgr.getAppSize();
					left_side = APP_PADDING;
					control_size.width -= ((APP_PADDING * 4) + APP_LABEL_WIDTH);
					control_size.height -= ((APP_PADDING * 2) + APP_HEADER_HEIGHT + APP_HEADING_HEIGHT);
					control_size.top = (APP_PADDING + APP_HEADING_HEIGHT);
					//update the width of the listview columns
					ListView_SetColumnWidth(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), 2, control_size.width / 5);
					ListView_SetColumnWidth(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), 3, control_size.width / 5);
					ListView_SetColumnWidth(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), 4, (control_size.width / 5)*2);
					ListView_SetColumnWidth(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), 5, control_size.width / 5);
				}
				break;


			case CONTROL_RIGHT:   // align the control right
				left_side = (app_size.width / 2) - (control_size.width - (this_control->row_width / 2));
				if (this_control->type == CRL_LABEL_SIDEBAR || this_control->type == CRL_BUTTON_SIDEBAR || this_control->type == CRL_DATEPICKER_SIDEBAR) {
					//special code for the sidebar controls
					left_side = app_mgr.getAppSize().width - (APP_LABEL_WIDTH + APP_PADDING);
				}
				break;


			case CONTROL_DIALOG_LEFT:   // align the control left
				left_side = APP_PADDING * 2;
				break;

			case CONTROL_DIALOG_RIGHT:   // align the control right
				left_side = (APP_START_WIDTH /2) - control_size.width;
				break;

			default:
				left_side = 0;
				break;
			};

			MoveWindow(this_control->getHandle(), left_side, control_size.top, control_size.width, control_size.height, TRUE);//move the control to the new position
			control_size.left = left_side;  //set left equal to new value
			this_control->setSize(control_size); //update the control
			InvalidateRect(this_control->getHandle(), NULL, FALSE); //allow for control repaint (so no artifacts show onscreen)
		};
	};
}





void WindowClass::setupListview(HWND listview_handle) {
	/*
Method used to setup the listview with columns etc
*/



//make sure common controls are initialaised
	InitCommonControls();
	//add extended styles to the window so we can select a full row	
	ListView_SetExtendedListViewStyle(listview_handle, LVS_EX_FULLROWSELECT);

	LV_COLUMN lvw_column;  // CONTROL SPECIFIC - Column struct for the listview

	// the mask specifies that the .fmt, .ex, width, and .subitem members of the structure are valid
	lvw_column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	// alignment for the columns
	lvw_column.fmt = LVCFMT_CENTER;
	//column width
	lvw_column.cx = APP_LISTVIEW_WIDTH / (LVW_MAX_COLUMNS);  //width/number of columns


	//lvw_column.cx = 80;            
	//Set the text and size of the columns
	//we want to hide the first 2 columns as they are ID numbers so leave them until last

	lvw_column.fmt = LVCFMT_CENTER;// alignment for the column
	lvw_column.cx = APP_LISTVIEW_WIDTH / 5; //split the column width
	lvw_column.pszText = (LPWSTR)LVW_COLUMN_3;
	ListView_InsertColumn(listview_handle, 2, (LPARAM)&lvw_column);

	lvw_column.pszText = (LPWSTR)LVW_COLUMN_4;
	ListView_InsertColumn(listview_handle, 3, (LPARAM)&lvw_column);

	lvw_column.fmt = LVCFMT_LEFT;// alignment for the column
	lvw_column.cx = (APP_LISTVIEW_WIDTH / 5) * 2; //split the column width
	lvw_column.pszText = (LPWSTR)LVW_COLUMN_5;
	ListView_InsertColumn(listview_handle, 4, (LPARAM)&lvw_column);

	lvw_column.fmt = LVCFMT_CENTER;// alignment for the column
	lvw_column.cx = APP_LISTVIEW_WIDTH / 5; //split the column width
	lvw_column.pszText = (LPWSTR)LVW_COLUMN_6;
	ListView_InsertColumn(listview_handle, 5, (LPARAM)&lvw_column);

	lvw_column.cx = 0;  //set the column width to 0 for the ID number columns
	lvw_column.pszText = (LPWSTR)LVW_COLUMN_1;
	ListView_InsertColumn(listview_handle, 0, (LPARAM)&lvw_column);

	lvw_column.pszText = (LPWSTR)LVW_COLUMN_2;
	ListView_InsertColumn(listview_handle, 1, (LPARAM)&lvw_column);

	/*=======================================

	Contents would normally be added here but since this is
	control creation we will add the contents once someone logs in

	=========================================*/


	SendMessage(listview_handle, WM_SETFONT, app_mgr.getControlFont(), TRUE);
}





bool WindowClass::validateForm() {
	/*
	Method used to validate the contents of controls before submitting a form (window)
	*/

	bool returnvalue = true;
	bool error = false;
	int control_length = 0;
	wstring control_text;


	for (vector<ControlClass>::iterator this_control = controls.begin(); this_control != controls.end(); ++this_control)  //iterate the vector
	{
		control_text = wstring(this_control->getControlTextW());
		//adjust values to position controls
		switch (this_control->validation)
		{
		case VAL_NOTNULL:  // center the control
		{
			if (control_text.size() == 0 && !error)
			{
				app_mgr.msgbox(this_control->validation_message);
				returnvalue = false;
				error = true;
				SetFocus(this_control->getHandle());
			}
		}
		break;
		case VAL_EMAIL:   // align the control left
		{
			const wregex pattern(L"(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
			if (!regex_match(control_text, pattern) && !error)
			{
				app_mgr.msgbox(this_control->validation_message);
				returnvalue = false;
				error = true;
				SetFocus(this_control->getHandle());
			}
		}
		break;
		case VAL_INTEGER:   // align the control right
			control_length = GetWindowTextLengthW(this_control->getHandle());
			if (control_length == 0 && !error)
			{
				app_mgr.msgbox(this_control->validation_message);
				returnvalue = false;
				error = true;
				SetFocus(this_control->getHandle());
			}
			break;
		case VAL_REAL:   // align the control left
		{
			const wregex pattern(L"^[0-9]+(\.[0-9]{1,2})?$");
			if (!regex_match(control_text, pattern) && !error)
			{
				app_mgr.msgbox(this_control->validation_message);
				returnvalue = false;
				error = true;
				SetFocus(this_control->getHandle());
			}
		}
		break;
		default:
			break;
		};
	};

	return returnvalue;
}





void WindowClass::visibility(bool show_window) {
	/*
	Method used to show / hide a window.
	*/
	if (show_window) {
		clearControls();
		ShowWindow(getHandle(), SW_SHOW);
	}
	else {
		ShowWindow(getHandle(), SW_HIDE);
	}


}