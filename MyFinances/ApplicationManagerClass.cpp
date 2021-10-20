#pragma once
#include "stdafx.h"
//using namespace std;
//extern ApplicationManagerClass app_mgr;
#include <Windows.h>
#include <CommCtrl.h>
#include <vector>
#include <string>
#include <iomanip> 
#include <sstream>
#include "sys/stat.h"
#include "resource.h"
#include "MyFinancesDefinitions.h"
#include "Structs.h"
#include "BaseClass.h"
#include "ControlClass.h"
#include "WindowClass.h"
#include "ApplicationManagerClass.h"
#include "3rdpartylibs/sqlite3.h"
#include "3rdpartylibs/sqlitewrapper.h"



ApplicationManagerClass::ApplicationManagerClass()
{
	build_finished = false;
	control_font = (WPARAM)CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
	current_user = 0;
	heading_font = (WPARAM)CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
	heading_text_color = RGB(255, 255, 255);
	heading_background_color = RGB(89, 29, 123);
	label_background_color = RGB(240, 240, 240);
	date_from = "0000-00-00";
	date_to = "0000-00-00";


	transaction_map["BAC"] = true;
	transaction_map["C/L"] = false;
	transaction_map["DPC"] = false;
	transaction_map["EDC"] = false;
	transaction_map["POS"] = false;
	transaction_map["FOOD"] = false;
	transaction_map["BILL"] = false;
	transaction_map["GIFT-OUT"] = false;
	transaction_map["GIFT-IN"] = true;
	transaction_map["TRAVEL"] = false;
	
}






void ApplicationManagerClass::addMainMenu() {
	HMENU hMenu = CreateMenu();
	menu_file = CreateMenu();
	menu_account = CreateMenu();
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)menu_file, L"File");
	AppendMenu(menu_file, MF_STRING | MF_DISABLED, MNU_FILE_IMPORT_ID, L"Import");
	AppendMenu(menu_file, MF_STRING | MF_DISABLED, MNU_FILE_EXPORT_ID, L"Export");
	AppendMenu(menu_file, MF_SEPARATOR, MNU_SEPARATOR_ID, L"Separator");
	AppendMenu(menu_file, MF_STRING, MNU_FILE_EXIT_ID, L"Exit");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)menu_account, L"Account");
	AppendMenu(menu_account, MF_STRING | MF_DISABLED, MNU_ACCOUNT_UPDATE_ID, L"Update Account");
	AppendMenu(menu_account, MF_STRING | MF_DISABLED, MNU_ACCOUNT_LOGOUT_ID, L"Log Out");
	AppendMenu(menu_account, MF_SEPARATOR, MNU_SEPARATOR_ID, L"Separator");
	AppendMenu(menu_account, MF_STRING | MF_DISABLED, MNU_ACCOUNT_DELETE_ID, L"Delete Account");
	SetMenu(getAppHandle(), hMenu);
}






HWND ApplicationManagerClass::addWindow(WindowNames window_name, LPCWSTR window_class, HWND window_parent){
/*
Method used to add windows to the app
	window_type = definition value for window type(WIN_MAIN, WIN_LOGIN etc)
	window_class = WINDCLASS window class used by the window
	window_parent = HWND handle of the parent window
*/

	SizeStruct window_size = { APP_DEFAULT_XY, APP_DEFAULT_XY, APP_START_WIDTH, APP_START_HEIGHT }; //create SizeStruct based on default app start size
	HWND temp_handle = NULL;

	switch (window_name)
	{
		case WIN_MAIN:
		{
			temp_handle = CreateWindowW(window_class, APP_NAME, WS_OVERLAPPEDWINDOW | WS_VISIBLE, APP_DEFAULT_XY, APP_DEFAULT_XY, APP_START_WIDTH, APP_START_HEIGHT, window_parent, NULL, NULL, NULL);
			app_handle = temp_handle; // assign the HWND value to the class for use later
			windows.push_back({ temp_handle, window_size, window_name });  //push the window onto the windows vector
			centerApplication();
			addMainMenu(); // add the menu to the main window
		}
		break;
		case WIN_DIALOG:
		{
			temp_handle = CreateWindowW(window_class, APP_NAME, WS_OVERLAPPED | WS_POPUP | WS_EX_CONTROLPARENT | WS_BORDER, (APP_START_HEIGHT / 4) - APP_PADDING, APP_START_WIDTH / 4, (APP_START_WIDTH/2) + (APP_PADDING * 2), APP_START_HEIGHT/2, window_parent, NULL, NULL, NULL);
			windows.push_back({ temp_handle, window_size, window_name });  //push the window onto the windows vector
		}
		break;
		default:
		{
			temp_handle = CreateWindowW(window_class, APP_NAME, WS_CHILD | WS_CLIPCHILDREN | WS_EX_CONTROLPARENT, APP_DEFAULT_XY, APP_DEFAULT_XY, APP_START_WIDTH, APP_START_HEIGHT, window_parent, NULL, NULL, NULL);
			windows.push_back({ temp_handle, window_size, window_name });  //push the window onto the windows vector
		}
		break;
	}
	windows[window_name].buildWindow(); //build the controls on the window
	windows[window_name].positionControls(getAppSize()); //Position the controls based on the size of the app
	InvalidateRect(temp_handle, NULL, FALSE); //allow for window repaint (so no artifacts show onscreen)
	return temp_handle;
}



void ApplicationManagerClass::centerDialog() {
	/*
	Method used to center the application onscreen on start
	*/

	RECT rect_window;
	GetWindowRect(windows[WIN_DIALOG].getHandle(), &rect_window);  //load the window size into the RECT struct

	int win_width = rect_window.right - rect_window.left;  //calculate the width
	int win_height = rect_window.bottom - rect_window.top;  //calculate the height

	int new_left = (APP_SCREEN_X - win_width) / 2;   //calculate the new left
	int new_top = (APP_SCREEN_Y - win_height) / 2;   //calculate the top

	// make sure that the dialog box never moves outside of the screen
	if (new_left < 0) new_left = 0;
	if (new_top < 0) new_top = 0;
	if (new_left + win_width > APP_SCREEN_X) new_left = APP_SCREEN_X - win_width;
	if (new_top + win_height > APP_SCREEN_Y) new_top = APP_SCREEN_Y - win_height;

	MoveWindow(windows[WIN_DIALOG].getHandle(), new_left, new_top, win_width, win_height, TRUE); // apply the new sizes to the main window
}



void ApplicationManagerClass::centerApplication() {
	/*
	Method used to center the application onscreen on start
	*/

	RECT rect_window;
	GetWindowRect(getAppHandle(), &rect_window);  //load the window size into the RECT struct

	int win_width = rect_window.right - rect_window.left;  //calculate the width
	int win_height = rect_window.bottom - rect_window.top;  //calculate the height

	int new_left = (APP_SCREEN_X - win_width) / 2;   //calculate the new left
	int new_top = (APP_SCREEN_Y - win_height) / 2;   //calculate the top

	// make sure that the dialog box never moves outside of the screen
	if (new_left < 0) new_left = 0;
	if (new_top < 0) new_top = 0;
	if (new_left + win_width > APP_SCREEN_X) new_left = APP_SCREEN_X - win_width;
	if (new_top + win_height > APP_SCREEN_Y) new_top = APP_SCREEN_Y - win_height;

	MoveWindow(getAppHandle(), new_left, new_top, win_width, win_height, TRUE); // apply the new sizes to the main window
	updateApplicationSize({ new_top, new_left, win_width, win_height }); //update the class with the new values
}




string ApplicationManagerClass::convertStrings(const wstring&  ref_string) {
	/*
	method to convert a wide string to a string
	*/

	int oldstr_length = ref_string.length() + 1;  //get the length of the string (+1 for null terminating char)
	//if the final parameter in this function is 0, it returns the SIZE of the required string so we set up a variable wstring of that length
	int newstr_length = WideCharToMultiByte(CP_ACP, 0, ref_string.c_str(), oldstr_length, 0, 0, NULL, NULL);
	string return_str(newstr_length, L'\0');
	//do the conversion and place the result in return_str
	WideCharToMultiByte(CP_ACP, 0, ref_string.c_str(), oldstr_length, &return_str[0], newstr_length, NULL, NULL);
	return return_str;
}





wstring ApplicationManagerClass::convertStrings(const string& ref_string) {
	/*
	method to convert a string to a wide string
	*/

	int oldstr_length = ref_string.length() + 1;  //get the length of the string (+1 for null terminating char)
	//if the final parameter in this function is 0, it returns the SIZE of the required string so we set up a variable wstring of that length
	int newstr_length = MultiByteToWideChar(CP_ACP, 0, ref_string.c_str(), oldstr_length, 0, 0);
	wstring return_str(newstr_length, L'\0');
	//do the conversion and place the result in return_str
	MultiByteToWideChar(CP_ACP, 0, ref_string.c_str(), oldstr_length, &return_str[0], newstr_length);
	return return_str;
}





bool ApplicationManagerClass::fileExists(const string & filename)
{
	/*
	method to check if a file exists (used to check the presence of the database file)
	stat function checks the STATus of a file and returns -1 if there's an error or 0 if all OK
	*/
	struct stat buffer;// stat uses a pointer to a stat struct to return error codes as well as the function return value
	if (stat(filename.c_str(), &buffer) == 0)
	{
		return true;
	}
	return false;
}





HWND ApplicationManagerClass::getAppHandle() {
/*
get the handle of the main application window
*/

	return app_handle;
}





HINSTANCE ApplicationManagerClass::getAppInstance() {
/*
get the HINSTANCE value of the application
*/

	return app_instance;
}





SizeStruct ApplicationManagerClass::getAppSize() {
/*
Shortcut to get the HWND value of the main window without going through the vector
*/

	return windows[WIN_MAIN].getSize();
}





WPARAM ApplicationManagerClass::getControlFont() {
/*
return the chosen font for the application controls
*/
	return control_font;
}





int ApplicationManagerClass::getCurrentUser() {
/*
return the id of the current user
*/
	return current_user;
}





WPARAM ApplicationManagerClass::getHeadingFont() {
/*
return the chosen font for the application headers
*/
	return heading_font;
}





COLORREF ApplicationManagerClass::getHeadingTextColor() {
/*
return the chosen color for the application headers
*/
	return heading_text_color;
}





COLORREF ApplicationManagerClass::getHeadingBackgroundColor() {
/*
return the chosen color for the application headers
*/
	return heading_background_color;
}





COLORREF ApplicationManagerClass::getLabelBackgroundColor() {
/*
return the chosen color for the application headers
*/
	return label_background_color;
}





INT_PTR ApplicationManagerClass::handleLabelRepaint(HDC control_hdc, HWND control_handle) {
	/*
Method used to color header and standard labels using messages from the windows message loop

	control_hdc = Hardware Device Context of the control (ref to paintbrushes used on the control)
	control_hwnd = handle of the control
*/

	if (build_finished) {
		if (
			control_handle == windows[WIN_LOGIN].controls[LBL_LOGIN_HEADING].getHandle() ||
			control_handle == windows[WIN_ACCOUNT].controls[LBL_ACCOUNT_HEADING].getHandle() ||
			control_handle == windows[WIN_BUDGET].controls[LBL_BUDGET_HEADING].getHandle() ||
			control_handle == windows[WIN_BUDGET].controls[LBL_BUDGET_INCOMING].getHandle() ||
			control_handle == windows[WIN_BUDGET].controls[LBL_BUDGET_OUTGOING].getHandle() ||
			control_handle == windows[WIN_BUDGET].controls[LBL_BUDGET_SEARCH].getHandle() ||
			control_handle == windows[WIN_DIALOG].controls[LBL_DIALOG_HEADING].getHandle()
			)
		{
			SetTextColor(control_hdc, heading_text_color);
			SetBkColor(control_hdc, heading_background_color);
			return (INT_PTR)CreateSolidBrush(heading_background_color);;
		}
		if (
			control_handle == windows[WIN_BUDGET].controls[LBL_BUDGET_INCTOTAL].getHandle() ||
			control_handle == windows[WIN_BUDGET].controls[LBL_BUDGET_OUTTOTAL].getHandle()
			)
		{
			SetTextColor(control_hdc, heading_background_color);
			
		}
		SetBkMode(control_hdc, TRANSPARENT);
		return (INT_PTR)CreateSolidBrush(label_background_color);
	}
}





bool ApplicationManagerClass::initialise(HINSTANCE val_hinst, WNDPROC val_main_proc, WNDPROC val_child_proc) {
/*
Method used to as a one line way to initialise the various methods needed

	val_hinst = hInst value for the application (created by windows when the app starts)
	val_main_proc = pointer to the main window message loop function
	val_main_proc = pointer to the child window message loop function
*/

	if (!registerWindowClasses(val_hinst, val_main_proc, val_child_proc)) {
		return false;
	};

	//create the application windows
	addWindow(WIN_MAIN, L"mainWindowClass", NULL);  //main app window
	addWindow(WIN_LOGIN, L"childWindowClass", getAppHandle());  //login window
	addWindow(WIN_ACCOUNT, L"childWindowClass", getAppHandle());  //update account window
	addWindow(WIN_BUDGET, L"childWindowClass", getAppHandle());  //budgeting window
	addWindow(WIN_DIALOG, L"dialogWindowClass", getAppHandle());  //budgeting window
	build_finished = true;  //set to true to signal that it's safe to start processing positioning code
	SendMessage(app_mgr.windows[WIN_BUDGET].controls[LBL_BUDGET_INCTOTAL].getHandle(), WM_SETFONT, app_mgr.getHeadingFont(), TRUE); // set a font for a bold control
	SendMessage(app_mgr.windows[WIN_BUDGET].controls[LBL_BUDGET_OUTTOTAL].getHandle(), WM_SETFONT, app_mgr.getHeadingFont(), TRUE); // set a font for a bold control
	windows[WIN_LOGIN].clearControls(); //also sets the focus to the first control
	windows[WIN_LOGIN].visibility(true); //show the main login window
	windows[WIN_LOGIN].controls[TXT_LOGIN_USERNAME].setControlText("default");
	windows[WIN_LOGIN].controls[TXT_LOGIN_PASSWORD].setControlText("password");
	//event_attemptLogin(WIN_LOGIN, WIN_BUDGET);
	positionWindows();
	
	//initialise database and set up tables if they dont exist

	//SQLite has no IF NOT EXISTS and creating an existing table causes an error so need to check if the file exists, 
	//if not the fill will be created automatically, we just need to run the setup SQL to build it
	//the following code ONLY runs the setup SQL if the file wasn't found

	if (!fileExists(APP_DATABASE))
	{
		//because we're using text encryption all the fields need to be of type text, we can handle those in the application
		DatabaseClass database(APP_DATABASE);
		//Create the tblUsers Table
		database.exec("CREATE TABLE tblUsers("\
			" userID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"\
			" userFullName TEXT NOT NULL,"\
			" userUsername TEXT NOT NULL,"\
			" userPassword TEXT NOT NULL,"\
			" userEmail TEXT NOT NULL,"\
			" userSecretQuestion TEXT NOT NULL,"\
			" userSecretAnswer TEXT NOT NULL);");
		//Create the tblUsers Table
		database.exec("CREATE TABLE tblTransactions("\
			" transID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"\
			" userID INTEGER NOT NULL,"\
			" transDate TEXT NOT NULL,"\
			" transType TEXT NOT NULL,"\
			" transDescription TEXT NOT NULL,"\
			" transAmount TEXT NOT NULL);");

		//DEFAULT DATA FOR TESTING ONLY
		database.exec("INSERT INTO tblUsers VALUES (1,'GjcpSyg+J0AaLzYqUQ==','OjcpSyg+J2A=','LjM8WSo9IQRP','OjcpSyg+JyArOTU5JEYiZTYgR18=','bQ==','GjcpSyg+JwU9XA==');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (1,'bw==','2019-04-28','GRsJfnAbHQ==','GTspXn00IQ8ifBItP152ADQ7QjM9KzJP','a2JhGm1S');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (2,'bw==','2019-02-07','Dh0c','GiAmRDYhcxcmKDt4N1g/LjsrWV8=','b2ZhE2RS');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (3,'bw==','2019-01-14','HBMM','DSY6Tjg8J0ADMzI2IgoSIiYtXy0rKzEqMylP','b2p2GnNiY2A=');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (4,'bw==','2019-03-07','GB0Abg==','CTcqQTErczMnMyN4eWYfDxlmKg==','bGphH2hS');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (5,'bw==','2019-02-14','CgAOfBge','CiAuQzNyOw8iOXM+Plh2PDAqQTo2KnxnLzg7KFg9cV4=','b2BhE2VS');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (6,'bw==','2019-02-21','GB0Abg==','CTcqQTErczMnMyN4eWYfDxlmKg==','bWdhGW5S');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (7,'bw==','2019-02-28','GB0Abg==','CTcqQTErczMnMyN4eWYfDxlmKg==','bGFhHmRS');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (8,'bw==','2019-03-07','Dh0c','GiAmRDYhcxcmKDt4N1g/LjsrWV8=','b2ZhE2RS');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (9,'bw==','2019-03-14','Dh0c','CTcqQTErczMnMyN4eWYfDxlmKg==','amNhGGRS');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (10,'bw==','2019-03-21','Dh0c','EAYaCh49NQYqOVM=','bHx4H10=');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (11,'bw==','2019-03-28','Dh0c','CTcqQTErczMnMyN4eWYfDxlmKg==','bGVhH29S');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (12,'bw==','2019-04-07','Dh0c','CTcqQTErczMnMyN4eWYfDxlmKg==','b2FhH2VS');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (13,'bw==','2019-04-14','GRsJfnAdBjQ=','GjM9WDg8dBNvHjoqJUIyKixP','bGZhGWtS');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (14,'bw==','2019-04-21','Dh0c','DSYuWD8nMAs8fCQxJUJ2ASAjQzpY','Z3x2E10=');");
		database.exec("INSERT INTO tblTransactions (transID,userID,transDate,transType,transDescription,transAmount) VALUES (15,'bw==','2019-04-28','HBsDZg==','EAYaCg83PRRvDDIhPE84P1U=','Z2p/BGhmUw==');");
	}
}





bool ApplicationManagerClass::isIncome(const string &trans_type) {
	//check the map to find out of the referenced type is incoming or outgoing
	return transaction_map[trans_type];
}




SYSTEMTIME ApplicationManagerClass::makeSystemTime(const char *date)
{
	/*
creates a systemtime structure for use with DatePicker Boxes
*/
	SYSTEMTIME temp_holder;

	memset(&temp_holder, 0, sizeof(temp_holder));
	// Date string should be "yyyy-MM-dd"
	sscanf_s(date, "%d-%d-%d%d",
		&temp_holder.wYear,
		&temp_holder.wMonth,
		&temp_holder.wDay);
	return temp_holder;
}





UINT ApplicationManagerClass::msgbox(wstring message, MessageTypes icon){
/*
messagebox wrapper function for ease of use (wstring version)
*/
	HWND temp_handle = getAppHandle();
	LPCWSTR result = message.c_str();
	switch (icon)
	{
	case ICN_ERROR: 
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONERROR | MB_OK);
		break;
	case ICN_WARNING: 
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONWARNING | MB_OK);
		break;
	case ICN_QUESTION: 
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONWARNING | MB_OKCANCEL);
		break;
	default: 
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONINFORMATION | MB_OK);
		break;
	}
}





UINT ApplicationManagerClass::msgbox(string message, MessageTypes icon) {
	/*
	messagebox wrapper function for ease of use (wstring version)
	*/
	HWND temp_handle = getAppHandle();
	wstring new_message = convertStrings(message);
	LPCWSTR result = new_message.c_str();
	switch (icon)
	{
	case ICN_ERROR:
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONERROR | MB_OK);
		break;
	case ICN_WARNING:
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONWARNING | MB_OK);
		break;
	case ICN_QUESTION:
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONWARNING | MB_OKCANCEL);
		break;
	case ICN_YESNO:
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONWARNING | MB_YESNOCANCEL);
		break;
	default:
		return MessageBox(temp_handle, result, APP_NAME, MB_ICONINFORMATION | MB_OK);
		break;
	}
}





void ApplicationManagerClass::msgbox(int message)
{
	//messagebox wrapper function for ease of use in debugging ints
	wstring temp = to_wstring(message);
	LPCWSTR result = temp.c_str();
	MessageBox(getAppHandle(), result, L"MyFinances", MB_OK);
}





void ApplicationManagerClass::populateListview() {

	LV_ITEM lvw_item;      // CONTROL SPECIFIC - List view item struct
	// Check the validity of items.
	lvw_item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvw_item.mask = LVIF_TEXT;   // Style of the cell
	lvw_item.cchTextMax = MAX_PATH; //Max size of the contents of the cell

	//clear all listview items
	ListView_DeleteAllItems(windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle());
	//build SQL statement to run
	
	
	//set up the datepicker controls if the values haven't been set
	if (date_to == "0000-00-00" || date_from == "0000-00-00" ){
		SYSTEMTIME temp_date;  // systemtime struct to store a systemtime
		ostringstream ossDate; // formatting the date

		//get the date from the system
		GetSystemTime(&temp_date);
		//update the TO datepicker
		DateTime_SetSystemtime(app_mgr.windows[WIN_BUDGET].controls[DPK_BUDGET_TO].getHandle(), GDT_VALID, &temp_date);
		//convert to the correct format
		ossDate << temp_date.wYear << "-" << setw(2) << setfill('0') << temp_date.wMonth << "-" << setw(2) << setfill('0') << temp_date.wDay;
		//set the string in the app_mgr
		date_to = ossDate.str();

		//set the from datepicker to the first day of the previous month
		if (temp_date.wMonth == 1) {
			//if the month is january, set to December the year before
			temp_date.wDay = 1;
			temp_date.wMonth -= 1;
			temp_date.wYear -= 1;
		}
		else {
			temp_date.wDay = 1;
			temp_date.wMonth -= 1;
		}

		//reset stringstream
		ossDate.str("");
		DateTime_SetSystemtime(app_mgr.windows[WIN_BUDGET].controls[DPK_BUDGET_FROM].getHandle(), GDT_VALID, &temp_date);
		//convert to the correct format
		ossDate << temp_date.wYear << "-" << setw(2) << setfill('0') << temp_date.wMonth << "-" << setw(2) << setfill('0') << temp_date.wDay;
		//set the string in the app_mgr
		date_from = ossDate.str();
	}


	//build the string using the sort parameters
	string base_SQL = "SELECT transID, userID, transDate, transType, transDescription, transAmount FROM tblTransactions where userID = '" + encrypt(to_string(getCurrentUser())) + "' AND transDate BETWEEN '" + date_from + "' AND '" + date_to + "' " + app_mgr.current_sort.c_str() + ";";
	//create database object and statement object
	
	DatabaseClass database(APP_DATABASE);
	StatementClass statement(database, base_SQL.c_str());

	

	//store total values for incoming and outgoing transactions
	float income_total = 0;
	float outgoing_total = 0;
		
	lvw_item.iItem = 0; // initialise the item (row)
	wchar_t temp_holder[255];
	while (statement.step()) {

		lvw_item.iSubItem = 0; //subitem must be 0 to add a new row to the listview
		swprintf_s(temp_holder, app_mgr.convertStrings(statement.row().text(0)).c_str());  //copy the result from the database to a temp holder (string conversion)
		lvw_item.pszText = temp_holder; //set the contents of the cell
		//add the new listview item (row)
		ListView_InsertItem(windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), (LPARAM)&lvw_item);

		for (int loopcount = 1; loopcount < LVW_MAX_COLUMNS; loopcount++) {
			//loop to add the cells into the row (starts at 1 since the listview item always contains the first cell)
			lvw_item.iSubItem = loopcount;
			if (loopcount == 2) {
				swprintf_s(temp_holder, app_mgr.convertStrings(statement.row().text(loopcount)).c_str());
			}
			else {
				swprintf_s(temp_holder, app_mgr.convertStrings(decrypt(statement.row().text(loopcount))).c_str());
			};
			lvw_item.pszText = temp_holder;
			//add the subitem (cell) to the item
			ListView_SetItem(windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), (LPARAM)&lvw_item);
		}
		//next row
		lvw_item.iItem = lvw_item.iItem + 1;

		//check if this entry is incoming our outgoing and add it to the relevant total
		
		if (isIncome(decrypt(statement.row().text(3)))) {
			//make sure the strings conversion works!!
			try { income_total += stof(decrypt(statement.row().text(5))); }
			catch (...)
			{
				
				income_total += 0.00;
			}
		}
		else {
			//make sure the strings conversion works!!
			try { outgoing_total += stof(decrypt(statement.row().text(5))); }
			catch (...)
			{
				outgoing_total += 0.00;
			}
		}
	}
	stringstream stream;
	//print the totals on the correct labels using a stringstream to set 2 decimal places
	stream << fixed << setprecision(2) << income_total;
	windows[WIN_BUDGET].controls[LBL_BUDGET_INCTOTAL].setControlText(stream.str());

	//clear stream for reuse
	stream.str("");
	stream.clear();
	//print the totals on the correct labels using a stringstream to set 2 decimal places
	stream << fixed << setprecision(2) << outgoing_total;
	windows[WIN_BUDGET].controls[LBL_BUDGET_OUTTOTAL].setControlText(stream.str());
}






void ApplicationManagerClass::positionWindows() {
/*
Method used to position the windows in the app - generally happens when the user changes the size of the main window
*/ 

	if (build_finished) {  // check that all windows have actually been created
		SizeStruct app_size = updateApplicationSize();  // update and get the new application size


		if (app_size.width > APP_SCREEN_X) app_size.width = APP_SCREEN_X;
		if (app_size.height > APP_SCREEN_Y) app_size.height = APP_SCREEN_Y;

		for (vector<WindowClass>::iterator this_window = windows.begin(); this_window != windows.end(); ++this_window)  // iterate throught he windows vector
		{
			if (IsWindow(this_window->getHandle())) //child is initialised, safe to edit
			{
				switch (this_window->window_id) {
				case WIN_MAIN:
					this_window->positionControls(app_size);  //position the controls on the window to reflect the new size
					InvalidateRect(this_window->getHandle(), NULL, FALSE);  // repaint the window to stop artifacts
					break;
				case WIN_DIALOG:
					MoveWindow(this_window->getHandle(), (app_size.width - ((APP_START_WIDTH / 2) + (APP_PADDING * 2))) / 2, app_size.height / 4, (APP_START_WIDTH / 2) + (APP_PADDING * 2), APP_START_HEIGHT / 2, TRUE);  //position the window
					this_window->setSize({ app_size.width / 4, app_size.height / 4, app_size.width / 2, app_size.height / 2 }); // set the size in the class
					app_mgr.centerDialog();
					break;
				default:
					// skip WIN_MAIN since it's the main window and we only want to position child windows and skip WIN_DIALOG since it's smaller
					MoveWindow(this_window->getHandle(), app_size.left, app_size.top + APP_HEADER_HEIGHT, app_size.width, app_size.height - APP_HEADER_HEIGHT, TRUE);  //position the window
					this_window->setSize({ app_size.top + APP_HEADER_HEIGHT, app_size.left, app_size.width, app_size.height - APP_HEADER_HEIGHT }); // set the size in the class
				}	
				//update the controls on the window
				this_window->positionControls(app_size);
			};
		};
	}
}






bool ApplicationManagerClass::registerWindowClasses(HINSTANCE val_hinst, WNDPROC val_main_proc, WNDPROC val_child_proc) {
/*
Method used to register the Windows Classes for the windows we will use
	val_hinst = hInst value for the application (created by windows when the app starts)
	val_main_proc = pointer to the main window message loop function
	val_main_proc = pointer to the child window message loop function
*/

	HICON app_icon = LoadIcon(val_hinst, MAKEINTRESOURCE(IDI_ICON)); //load the application icon from resource file

	WNDCLASSW wcMain = { 0 }, wcChild = { 0 }, wcDialog = { 0 };
	//window class for the main app window
	wcMain.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));  //sets the window background to an RGB color value
	wcMain.hCursor = LoadCursor(NULL, IDC_ARROW); //sets the window mouse cursor to the standard arrow
	wcMain.hInstance = val_hinst; //sets the HINSTANCE value of the application
	wcMain.lpszClassName = L"mainWindowClass"; //sets the name of the class (so windows can use this)
	wcMain.hIcon = app_icon; //sets the application icon
	wcMain.lpfnWndProc = val_main_proc; //sets the name of the procedure which handles the message loop for the window 

	//window class for the child app window
	wcChild.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
	wcChild.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcChild.hInstance = val_hinst;
	wcChild.lpszClassName = L"childWindowClass";
	wcChild.lpfnWndProc = val_child_proc;

	//window class for the small dialog windows window
	wcDialog.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
	wcDialog.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcDialog.hInstance = val_hinst;
	wcDialog.lpszClassName = L"dialogWindowClass";
	wcDialog.lpfnWndProc = val_child_proc; //use the same event handler system as the child window
	
	app_instance = val_hinst; // assign the hinst value to the class for use later

	//register the above classes
	if (!RegisterClassW(&wcMain) || !RegisterClassW(&wcChild) || !RegisterClassW(&wcDialog))
	{
		msgbox(L"Unable to register the WINDCLASSW Window Classes");
		return false;
	}
	else { return true; }

}






void ApplicationManagerClass::setCurrentUser(int new_id) {
	/*
	return the id of the current user
	*/
	current_user = new_id;
	
}






SizeStruct ApplicationManagerClass::updateApplicationSize() {
/*
Update the application size without parameters causes the update to use a RECT based on the parent window
*/

	if (build_finished) {
		RECT app_size;
		GetClientRect(getAppHandle(), &app_size);  //load the window size into the RECT struct
		windows[WIN_MAIN].setSize({ app_size.top, app_size.left, app_size.right - app_size.left, app_size.bottom - app_size.top }); //update the class with the new values
		return(getAppSize());
	}
}






void ApplicationManagerClass::updateApplicationSize(SizeStruct app_size) {
/*
Update the application size (using supplied SizeStruct)
*/

	windows[WIN_MAIN].setSize({ app_size.top, app_size.left, app_size.width, app_size.height }); //update the class with the new values
}





void ApplicationManagerClass::updateMainMenu() {
	/*
	Method used to add the menu to the chosen window
		window_type = the name of the window to add to
	*/
	UINT toggle = MF_DISABLED;

	if (current_user != 0) {
		toggle = MF_ENABLED;
	}

	EnableMenuItem(menu_file, MNU_FILE_IMPORT_ID, toggle);
	EnableMenuItem(menu_file, MNU_FILE_EXPORT_ID, toggle);
	EnableMenuItem(menu_account, MNU_ACCOUNT_UPDATE_ID, toggle);
	EnableMenuItem(menu_account, MNU_ACCOUNT_LOGOUT_ID, toggle);
	EnableMenuItem(menu_account, MNU_ACCOUNT_DELETE_ID, toggle);
}