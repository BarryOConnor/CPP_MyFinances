#pragma once
//using namespace std;
#include <windows.h>
#include "sqlite3.h"
#include "../MyFinancesDefinitions.h"
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



#include <exception>
#include <algorithm>
#include <cstring>
#include <string>
#include <cstdint>




// Exception thrown on error
class error : public std::exception {
public:
    error(int c) : _code(c) { }
    virtual ~error() throw ();

    const char *what() const throw ();

    int code() const
    {
	return _code;
    }

private:
    int _code;
};

// Check return code, throw error if not ok


static inline void check(int code)
{
	wstring message = L"An SQLite error occurred. Error Number: " + to_wstring(code);
	if (code != SQLITE_OK) MessageBoxW(NULL, message.c_str(),APP_NAME, MB_ICONERROR | MB_OK);

}

void destroy_blob(void *blob);
void destroy_text(void *blob);
void destroy_text16(void *blob);



// Database handle
class DatabaseClass {
public:
	DatabaseClass() : _db(nullptr) { }

	DatabaseClass(const std::string& filename)
    {
	check(sqlite3_open(filename.c_str(), &_db));
    }

	DatabaseClass(const std::wstring& filename)
	{
		check(sqlite3_open16(filename.c_str(), &_db));
	}

	DatabaseClass(const char *filename)
    {
	check(sqlite3_open(filename, &_db));
    }

	DatabaseClass(const wchar_t *filename)
	{
		check(sqlite3_open16(filename, &_db));
	}

    ~DatabaseClass()
    {
	if (_db)
	    sqlite3_close(_db);
    }

	DatabaseClass(const DatabaseClass&) = delete;
	DatabaseClass& operator=(const DatabaseClass&) = delete;

    void swap(DatabaseClass& r)
    {
	std::swap(_db, r._db);
    }

	DatabaseClass(DatabaseClass&& r) : _db(r._db)
    {
	r._db = nullptr;
    }

	DatabaseClass& operator=(DatabaseClass&& r)
    {
		DatabaseClass m(std::move(r));

	swap(m);
	return *this;
    }

    sqlite3 *get()
    {
	return _db;
    }

    const sqlite3 *get() const
    {
	return _db;
    }

    // Number of changes due to the most recent statement.
    unsigned int changes() const
    {
	return sqlite3_changes(_db);
    }

    // Execute a simple statement
    void exec(const std::string& text)
    {
	check(sqlite3_exec(_db, text.c_str(),
				   nullptr, nullptr, nullptr));
    }

    void exec(const char *text)
    {
	check(sqlite3_exec(_db, text,
				   nullptr, nullptr, nullptr));
    }

private:
    ::sqlite3 *_db;
};

// Statement
class StatementClass {
public:
	StatementClass() : _stmt(nullptr) { }

	StatementClass(DatabaseClass& db, const char *sql)
    {
	check(sqlite3_prepare_v2(db.get(), sql, -1, &_stmt, nullptr));
    }

	StatementClass(DatabaseClass& db, const wchar_t *sql)
	{
		check(sqlite3_prepare16_v2(db.get(), sql, -1, &_stmt, nullptr));
	}

    ~StatementClass()
    {
	if (_stmt)
	    sqlite3_finalize(_stmt);
    }

	StatementClass(const StatementClass&) = delete;
	StatementClass& operator=(const StatementClass&) = delete;

    void swap(StatementClass& r)
    {
	std::swap(_stmt, r._stmt);
    }


	StatementClass(StatementClass&& r) : _stmt(r._stmt)
    {
	r._stmt = nullptr;
    }

	StatementClass& operator=(StatementClass&& r)
    {
		StatementClass m(std::move(r));

	swap(m);
	return *this;
    }

	

    sqlite3_stmt *get()
    {
	return _stmt;
    }

    const sqlite3_stmt *get() const
    {
	return _stmt;
    }

    class binder {
    public:
	binder(StatementClass& state) : _stmt(state._stmt) { }

	binder& blob(unsigned int intval, const void *data, size_t len)
	{
	    uint8_t *copy = new uint8_t[len];

	    memcpy(copy, data, len);
	    check(sqlite3_bind_blob(_stmt, intval, copy, len,
					    destroy_blob));
	    return *this;
	}

	binder& blob_ref(unsigned int intval, const void *data, size_t len)
	{
	    check(sqlite3_bind_blob(_stmt, intval,
		data, len, nullptr));
	    return *this;
	}

	binder& real(unsigned int intval, double value)
	{
	    check(sqlite3_bind_double(_stmt, intval, value));
	    return *this;
	}

	binder& int32(unsigned int intval, int32_t value)
	{
	    check(sqlite3_bind_int(_stmt, intval, value));
	    return *this;
	}

	binder& int64(unsigned int intval, int64_t value)
	{
	    check(sqlite3_bind_int64(_stmt, intval, value));
	    return *this;
	}

	binder& null(unsigned int intval)
	{
	    check(sqlite3_bind_null(_stmt, intval));
	    return *this;
	}

	binder& text(unsigned int intval, const char *orig)
	{
	    const size_t len = ::strlen(orig);
	    char *copy = new char[len];

	    memcpy(copy, orig, len);
	    check(sqlite3_bind_text(_stmt, intval, copy, len,
			destroy_text));
	    return *this;
	}

	binder& text(unsigned int intval, const std::string& value)
	{
	    const char *orig = value.c_str();
	    const size_t len = value.size();
	    char *copy = new char[len];

	    memcpy(copy, orig, len);
	    check(::sqlite3_bind_text(_stmt, intval, copy, len,
			destroy_text));
	    return *this;
	}

	binder& text_ref(unsigned int intval, const std::string& value)
	{
	    check(sqlite3_bind_text(_stmt, intval,
			value.c_str(), value.size(),
			nullptr));
	    return *this;
	}

	binder& text_ref(unsigned int intval, const char *value)
	{
	    check(sqlite3_bind_text(_stmt, intval,
			value, -1, nullptr));
	    return *this;
	}

	binder& text16(unsigned int intval, const wchar_t *orig)
	{
		const size_t len = ::wcslen(orig);
		wchar_t *copy = new wchar_t[len];

		wmemcpy(copy, orig, len);
		check(sqlite3_bind_text16(_stmt, intval, copy, len,
			destroy_text16));
		return *this;
	}

	binder& text16(unsigned int intval, const wstring& value)
	{
		const wchar_t *orig = value.c_str();
		const size_t len = value.size();
		wchar_t *copy = new wchar_t[len];

		wmemcpy(copy, orig, len);
		check(sqlite3_bind_text16(_stmt, intval, copy, len,
			destroy_text16));
		return *this;
	}

	binder& text16_ref(unsigned int intval, const wstring& value)
	{
		check(sqlite3_bind_text16(_stmt, intval,
			value.c_str(), value.size(),
			nullptr));
		return *this;
	}

	binder& text_ref(unsigned int intval, const wchar_t *value)
	{
		check(sqlite3_bind_text16(_stmt, intval,
			value, -1, nullptr));
		return *this;
	}

	void clear()
	{
	    check(sqlite3_clear_bindings(_stmt));
	}

    private:
	sqlite3_stmt *_stmt;
    };

    binder bind()
    {
	return binder(*this);
    }

    bool step()
    {
	const int code = sqlite3_step(_stmt);

	if (code == SQLITE_ROW)
	    return true;

	if (code == SQLITE_DONE)
	    return false;

	throw error(code);
    }

    void exec()
    {
	while (step());
    }

    void reset()
    {
	check(sqlite3_reset(_stmt));
    }

    class reader {
    public:
	reader(StatementClass& sstment) : _stmt(sstment._stmt) { }

	const void *blob(unsigned int intval)
	{
	    return sqlite3_column_blob(_stmt, intval);
	}

	size_t size(unsigned int intval)
	{
	    return sqlite3_column_bytes(_stmt, intval);
	}

	size_t size16(unsigned int intval)
	{
		return sqlite3_column_bytes16(_stmt, intval);
	}

	double real(unsigned int intval)
	{
	    return sqlite3_column_double(_stmt, intval);
	}

	int32_t int32(unsigned int intval)
	{
	    return sqlite3_column_int(_stmt, intval);
	}

	int64_t int64(unsigned int intval)
	{
	    return sqlite3_column_int64(_stmt, intval);
	}

	const char *cstr(unsigned int intval)
	{
		return reinterpret_cast<const char *>
			(sqlite3_column_text(_stmt, intval));
	}

	string text(unsigned int intval)
	{
		return string(cstr(intval), size(intval));
	}

	const wchar_t *cstr16(unsigned int intval)
	{
	    return reinterpret_cast<const wchar_t *>
		    (sqlite3_column_text16(_stmt, intval));
	}

	wstring text16(unsigned int intval)
	{
	    return wstring(cstr16(intval), size16(intval));
	}

    private:
	sqlite3_stmt *_stmt;
    };

    reader row()
    {
	return reader(*this);
    }

private:
    sqlite3_stmt *_stmt;
};


