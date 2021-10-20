#pragma once
//using namespace std;
#include <string>
#include <vector>
#include <Windows.h>

/*==================================
application definitions
==================================*/
#define APP_NAME L"MyFinances" //application title
#define APP_DATABASE "MyFinances.db"  // do not change
#define APP_ENCRYPTION_KEY "THE SHIVERING LAKE UNDRESSES INTO THE LONG SIGNATURE"  // CAPITALS work best
#define MAX_KEY_CHARS 53 //number of characters in the APP_ENCRYPTION_KEY
#define APP_SCREEN_X GetSystemMetrics(SM_CXSCREEN)
#define APP_SCREEN_Y GetSystemMetrics(SM_CYSCREEN)
#define APP_START_WIDTH 800  //minimum value for the screen width
#define APP_START_HEIGHT 600 //minimum value for the screen Height
#define APP_HEADER_HEIGHT 100 //height of the header image
#define APP_CONTROL_TOP 100 //vertical margin at which controls start to be positioned
#define APP_PADDING 15 //padding between controls
#define APP_CONTROL_HEIGHT 20 //default label and textbox height
#define APP_DEFAULT_XY 0  //default value for top and left positions


/*==================================
control Width SETTINGS
==================================*/
#define APP_HEADING_WIDTH 1920 //default label width
#define APP_HEADING_HEIGHT 30 //default label width
#define APP_LABEL_WIDTH 150 //default label width
#define APP_TEXT_WIDTH 100 //default textbox width
#define APP_TEXT_DOUBLE 200 //wider textbox
#define APP_BUTTON_WIDTH 150 //default button width
#define APP_LISTVIEW_WIDTH 550 //default listview width
#define APP_LISTVIEW_HEIGHT 350 //default listview width


/*==================================
Control "ID's" - The value of these is a calculation
it's got to be a number bigger than 100 but only applies to clickable controls like menus and buttons

====================================*/
enum ControlID {
	ID_NULL=0,
	//WIN_MAIN window has the following controls
	ID_HEADING_LABEL = 100,
	MNU_SEPARATOR_ID, //separator bar
	MNU_FILE_IMPORT_ID, //File->Import
	MNU_FILE_EXPORT_ID, //File->Export
	MNU_FILE_EXIT_ID, //File->Exit
	MNU_ACCOUNT_UPDATE_ID, //account->Update
	MNU_ACCOUNT_DELETE_ID, //account->Update
	MNU_ACCOUNT_LOGOUT_ID, //account->Logout
	//WIN_LOGIN window has the following controls
	BTN_LOGIN_CREATE_ACCOUNT_ID,
	BTN_LOGIN_LOGIN_ID,
	//WIN_ACCOUNT window has the following controls,
	BTN_ACCOUNT_RETURN_LOGIN_ID,
	BTN_ACCOUNT_NEW_ACCOUNT_ID,
	//WIN_BUDGET window has the following controls,
	LVW_BUDGET_TRANSACTIONS_ID,
	DPK_BUDGET_FROM_ID,
	DPK_BUDGET_TO_ID,
	BTN_BUDGET_ADDNEW_ID,
	BTN_BUDGET_EDIT_ID,
	BTN_BUDGET_DELETE_ID,
	//WIN_DIALOG window has the following controls,
	BTN_DIALOG_DATEPICKER_ID,
	BTN_DIALOG_CANCEL_ID,
	BTN_DIALOG_EDIT_ID,
	BTN_DIALOG_ADD_ID
};



/*==================================
Control "Namess" - The value of these is simply the element index of where they are in the corresponding controls vector
for each window. Since the vector starts at 0 and are associated with a window, these need to be numbered per window

This enum is a convenience - without it, the controls vector would have to be accessed as:
controls[MY_CONTROL_NAME % 100] which is just messy
====================================*/
enum ControlNames {
	//WIN_MAIN window has the following controls
	BMP_MAIN_HEADER = 0,
	//WIN_LOGIN window has the following controls
	LBL_LOGIN_HEADING = 0,
	LBL_LOGIN_USERNAME,
	TXT_LOGIN_USERNAME,
	LBL_LOGIN_PASSWORD,
	TXT_LOGIN_PASSWORD,
	BTN_LOGIN_CREATE_ACCOUNT,
	BTN_LOGIN_LOGIN,
	//WIN_ACCOUNT window has the following controls,
	LBL_ACCOUNT_HEADING = 0,
	LBL_ACCOUNT_FULLNAME,
	TXT_ACCOUNT_FULLNAME,
	LBL_ACCOUNT_USERNAME,
	TXT_ACCOUNT_USERNAME,
	LBL_ACCOUNT_PASSWORD,
	TXT_ACCOUNT_PASSWORD,
	LBL_ACCOUNT_EMAIL,
	TXT_ACCOUNT_EMAIL,
	LBL_ACCOUNT_SECRET_QUESTION,
	CBO_ACCOUNT_SECRET_QUESTION,
	LBL_ACCOUNT_SECRET_ANSWER,
	TXT_ACCOUNT_SECRET_ANSWER,
	BTN_ACCOUNT_RETURN_LOGIN,
	BTN_ACCOUNT_NEW_ACCOUNT,
	//WIN_BUDGET window has the following controls,
	LBL_BUDGET_HEADING = 0,
	LVW_BUDGET_TRANSACTIONS,
	LBL_BUDGET_INCOMING,
	LBL_BUDGET_INCTOTAL,
	LBL_BUDGET_OUTGOING,
	LBL_BUDGET_OUTTOTAL,
	LBL_BUDGET_SEARCH,
	LBL_BUDGET_FROM_DATE,
	DPK_BUDGET_FROM,
	LBL_BUDGET_TO_DATE,
	DPK_BUDGET_TO,
	BTN_BUDGET_ADDNEW,
	BTN_BUDGET_EDIT,
	BTN_BUDGET_DELETE,
	//WIN_DIALOG window has the following controls,
	LBL_DIALOG_HEADING = 0,
	LBL_DIALOG_DATE,
	DPK_DIALOG_DATE,
	LBL_DIALOG_TYPE,
	CBO_DIALOG_TYPE,
	LBL_DIALOG_DESCRIPTION,
	TXT_DIALOG_DESCRIPTION,
	LBL_DIALOG_AMOUNT,
	TXT_DIALOG_AMOUNT,
	LBL_DIALOG_TRANSACTION,
	BTN_DIALOG_CANCEL,
	BTN_DIALOG_EDIT,
	BTN_DIALOG_ADD
};

/*==================================
Listview Columns
==================================*/
enum ListviewColumns { transID, userID, transDate, transType, transDescription, transAmount };
#define LVW_MAX_COLUMNS 6 //default label width
#define LVW_COLUMN_1 L"Transaction" //default label width
#define LVW_COLUMN_2 L"User" //default label width
#define LVW_COLUMN_3 L"Date" //default label width
#define LVW_COLUMN_4 L"Type" //default label width
#define LVW_COLUMN_5 L"Description" //default label width
#define LVW_COLUMN_6 L"Amount" //default label width




/*==================================
enumerated types for alignment
==================================*/
enum AlignmentTypes { CONTROL_LEFT, CONTROL_RIGHT, CONTROL_CENTER, CONTROL_DIALOG_LEFT, CONTROL_DIALOG_RIGHT, CONTROL_DIALOG_CENTER };

/*==================================
enumerated types for validation
==================================*/
enum ValidationTypes { VAL_NOVALIDATE, VAL_NOTNULL, VAL_EMAIL, VAL_INTEGER, VAL_REAL, VAL_DATE };

/*==================================
enumerated types for messagebox
==================================*/
enum MessageTypes { ICN_INFO, ICN_ERROR, ICN_WARNING, ICN_QUESTION, ICN_YESNO };

/*==================================
control types
==================================*/
// CRL_IMAGE = control is an Image (STATIC)
// CRL_LABEL_HEADING = the heading label at the top of each page
// CRL_LABEL_SIDEBAR = the heading label at the right of the listview
// CRL_BUTTON_SIDEBAR = the buttons at the right of the listview
// CRL_LABEL = control is a label (STATIC)
// CRL_TEXTBOX = control is a Textbox (EDIT)
// CRL_PASSWORD = control is a Password (EDIT)
// CRL_BUTTON = control is a Button (EDIT)
// CRL_COMBO = control is a ComboBox (EDIT)
enum ControlTypes { CRL_NONE, CRL_LABEL_HEADING, CRL_LABEL_SIDEBAR, CRL_BUTTON_SIDEBAR, CRL_IMAGE, CRL_LABEL, CRL_TEXTBOX, CRL_TEXTBOX_SCROLL, CRL_PASSWORD, CRL_BUTTON, CRL_COMBO, CRL_LISTVIEW, CRL_DATEPICKER, CRL_DATEPICKER_SIDEBAR};


/*==================================
Window "Names" - these correspond to the vector element value of the window so it's an easy way to access the windows
the windows themselves must be created in the correct order to match these values
==================================*/
enum WindowNames { WIN_MAIN, WIN_LOGIN, WIN_ACCOUNT, WIN_BUDGET, WIN_DIALOG};


/*==================================
enumerated types for Listview sorting
==================================*/
enum SortTypes { LVW_DATE_ASC, LVW_DATE_DESC, LVW_TYPE_ASC, LVW_TYPE_DESC, LVW_DESCRIPTION_ASC, LVW_DESCRIPTION_DESC, LVW_AMOUNT_ASC, LVW_AMOUNT_DESC };

//Function definitions
LRESULT CALLBACK MainWinProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWinProc(HWND, UINT, WPARAM, LPARAM);
