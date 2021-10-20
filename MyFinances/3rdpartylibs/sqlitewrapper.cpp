#pragma once
#include "stdafx.h"
#include "sqlitewrapper.h"


// SQLite wrapper
// Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all
// copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.



error::~error() throw ()
{
}

const char *error::what() const throw ()
{
    return "SQLite error";
}

void destroy_blob(void *blob)
{
    delete[] reinterpret_cast<uint8_t *>(blob);
}

void destroy_text(void *blob)
{
    delete[] reinterpret_cast<char *>(blob);
}

void destroy_text16(void *blob)
{
	delete[] reinterpret_cast<wchar_t *>(blob);
}

