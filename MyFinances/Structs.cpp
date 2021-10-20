#pragma once
#include "stdafx.h"
#include <string>
#include "Structs.h"

SizeStruct::SizeStruct() {
	//empty constructor - create a struct with default values
	top = 0;
	left = 0;
	width = 0;
	height = 0;
}

SizeStruct::SizeStruct(int val_top, int val_left, int val_width, int val_height) {
	//constructor with values - create a struct with supplied values
	top = val_top;
	left = val_left;
	width = val_width;
	height = val_height;
}





SortVarsStruct::SortVarsStruct() {
	sort_desc_date = true;
	sort_desc_type = true;
	sort_desc_description = true;
	sort_desc_amount = true;
}

