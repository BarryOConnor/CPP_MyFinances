#pragma once
#include <string>

struct SizeStruct {
/*==================================
structure to handle window and control sizing
==================================*/
	int top;  //control's top value
	int left;  //control's left value
	int width;  //control's width value
	int height;  //control's height value

	SizeStruct();  //default constructor
	SizeStruct(int val_top, int val_left, int val_width, int val_height); //constructor with values
};


struct SortVarsStruct {
	/*==================================
	structure to handle sorting the database/listview by ASC/DESC
	==================================*/
	bool sort_desc_date;
	bool sort_desc_type;
	bool sort_desc_description;
	bool sort_desc_amount;

	SortVarsStruct();
};