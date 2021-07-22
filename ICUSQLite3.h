/*
 Copyright (c) 2010 Bryan Ashby

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#ifndef __ICU_SQLITE3_H__
#define __ICU_SQLITE3_H__

//	:TODO: (see list)
//	-	 Update all int's to int32_t /etc.
//	-	Const correctness needed all over!!
//	-	See some of the tricks here: https://src.chromium.org/svn/trunk/src/sql/connection.cc

#pragma once

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "ICUSQLite3.h"
#endif	//	defined(__GNUG__) && !defined(__APPLE__)

//	ICU
#include <unicode/utypes.h>
#include <unicode/unistr.h>

//	STL
#include <set>
#include <vector>
#include <memory>

#include "ICUSQLite3Def.h"
#include "ICUSQLite3Utility.h"

const int ICUSQLITE_COLUMN_IDX_INVALID	= (-1);

//	:TODO: make all of these singular:
enum EIcuSqlite3ColumnTypes {
	ICUSQLITE_COLUMN_TYPE_INVALID	= 0,
	ICUSQLITE_COLUMN_TYPE_INTEGER	= 1,
	ICUSQLITE_COLUMN_TYPE_FLOAT		= 2,
	ICUSQLITE_COLUMN_TYPE_TEXT		= 3,
	ICUSQLITE_COLUMN_TYPE_BLOB		= 4,
	ICUSQLITE_COLUMN_TYPE_NULL		= 5,
};

enum EIcuSqlite3TransTypes {
	ICUSQLITE_TRANSACTION_DEFAULT,
	ICUSQLITE_TRANSACTION_DEFERRED,
	ICUSQLITE_TRANSACTION_IMMEDIATE,
	ICUSQLITE_TRANSACTION_EXCLUSIVE,
};

enum EIcuSqlite3OpenFlags {
	ICUSQLITE_OPEN_NONE			= 0x00000000,
	ICUSQLITE_OPEN_READONLY		= 0x00000001,
	ICUSQLITE_OPEN_READWRITE	= 0x00000002,
	ICUSQLITE_OPEN_CREATE		= 0x00000004,
	ICUSQLITE_OPEN_NOMUTEX		= 0x00008000,
	ICUSQLITE_OPEN_FULLMUTEX	= 0x00010000,
	ICUSQLITE_OPEN_SHAREDCACHE	= 0x00020000,
	ICUSQLITE_OPEN_PRIVATECACHE	= 0x00040000,
};

enum EIcuSqlite3ExtOpenFlags {
	ICUSQLITE_EXT_OPEN_NONE		= 0x00000000,
	ICUSQLITE_EXT_OPEN_UTF16	= 0x00000001,
	ICUSQLITE_EXT_OPEN_ICUEXT	= 0x00000002,	//	ICU extensions (e.g.: LOWER(s, locale), etc.)
	ICUSQLITE_EXT_OPEN_FK		= 0x00000004,	//	enable foreign keys at open
	ICUSQLITE_EXT_OPEN_WAL		= 0x00000008,	//	Utilize WAL mode

	ICUSQLITE_EXT_OPEN_DEFAULT = (ICUSQLITE_EXT_OPEN_ICUEXT | ICUSQLITE_EXT_OPEN_FK),
};

enum EIcuSqlite3SupportFlags {
	ICUSQLITE_SUPPORT_NONE			= 0x00000000,
	ICUSQLITE_SUPPORT_ENCRYPTION	= 0x00000001,
	ICUSQLITE_SUPPORT_METADATA		= 0x00000002,
	ICUSQLITE_SUPPORT_LOAD_EXT		= 0x00000004,
	ICUSQLITE_SUPPORT_INC_BLOB		= 0x00000008,
	ICUSQLITE_SUPPORT_SAVEPOINT		= 0x00000010,
	ICUSQLITE_SUPPORT_BACKUP		= 0x00000020,
};

enum EIcuSqlite3RestoreResults {
	ICUSQLITE_RESTORE_FAILED	= (-2),
	ICUSQLITE_RESTORE_BUSY		= (-1),
	ICUSQLITE_RESTORE_SUCCESS	= 1,
};

enum EIcuSqlite3Config {
	ICUSQLITE_CONFIG_SINGLETHREAD  		= 1,  /* nil */
	ICUSQLITE_CONFIG_MULTITHREAD   		= 2,  /* nil */
	ICUSQLITE_CONFIG_SERIALIZED			= 3,  /* nil */
	ICUSQLITE_CONFIG_MALLOC        		= 4,  /* sqlite3_mem_methods* */
	ICUSQLITE_CONFIG_GETMALLOC     		= 5,  /* sqlite3_mem_methods* */
	ICUSQLITE_CONFIG_SCRATCH       		= 6,  /* void*, int sz, int N */
	ICUSQLITE_CONFIG_PAGECACHE     		= 7,  /* void*, int sz, int N */
	ICUSQLITE_CONFIG_HEAP          		= 8,  /* void*, int nByte, int min */
	ICUSQLITE_CONFIG_MEMSTATUS			= 9,  /* boolean */
	ICUSQLITE_CONFIG_MUTEX        		= 10,  /* sqlite3_mutex_methods* */
	ICUSQLITE_CONFIG_GETMUTEX     		= 11,  /* sqlite3_mutex_methods* */
	ICUSQLITE_CONFIG_LOOKASIDE    		= 13,  /* int int */
	ICUSQLITE_CONFIG_PCACHE       		= 14,  /* sqlite3_pcache_methods* */
	ICUSQLITE_CONFIG_GETPCACHE    		= 15,  /* sqlite3_pcache_methods* */
	ICUSQLITE_CONFIG_LOG          		= 16,  /* xFunc, void* */
};

enum EIcuSqlite3Limit {
  ICUSQLITE_LIMIT_LENGTH              = 0,
  ICUSQLITE_LIMIT_SQL_LENGTH          = 1,
  ICUSQLITE_LIMIT_COLUMN              = 2,
  ICUSQLITE_LIMIT_EXPR_DEPTH          = 3,
  ICUSQLITE_LIMIT_COMPOUND_SELECT     = 4,
  ICUSQLITE_LIMIT_VDBE_OP             = 5,
  ICUSQLITE_LIMIT_FUNCTION_ARG        = 6,
  ICUSQLITE_LIMIT_ATTACHED            = 7,
  ICUSQLITE_LIMIT_LIKE_PATTERN_LENGTH = 8,
  ICUSQLITE_LIMIT_VARIABLE_NUMBER     = 9,
  ICUSQLITE_LIMIT_TRIGGER_DEPTH       = 10
};

enum EIcuSqlite3WALCheckpoint {
	ICUSQLITE_WAL_CHECKPOINT_PASSIVE	= 0,
	ICUSQLITE_WAL_CHECKPOINT_FULL		= 1,
	ICUSQLITE_WAL_CHECKPOINT_RESTART	= 2,
};

class ICUSQLITE_DLLIMPEXP IcuSqlite3StatementBuffer
{
public:
	IcuSqlite3StatementBuffer();
	//	:TODO: ctor with (format, ...) -- is that possible?
	
	~IcuSqlite3StatementBuffer();
	
	const char* Format(const char* format, ...);
	//	:TODO: UnicodeString Format(const char* format, ...);
	
	operator const char*() const { return m_buffer; }
	void Clear();
private:
	char*	m_buffer;
};

class ICUSQLITE_DLLIMPEXP IcuSqlite3FunctionContext
{
public:
	//IcuSqlite3FunctionContext(sqlite3_context* pContext, int argCount, sqlite3_value** ppArgs);
	IcuSqlite3FunctionContext(void* ctxt, int argCount, void** args);
	int GetArgCount() const;
	const unsigned char* GetAsBlob(const unsigned int n, int& len);
	UnicodeString GetArgAsUnicodeString(const unsigned int n);

	int32_t GetArgAsInt(const unsigned int n);
	int64_t GetArgAsInt64(const unsigned int n);
	double GetArgAsDouble(const unsigned int n);

	void SetResult(const UnicodeString& str);
	void SetResult(const char* str, int length = -1);
	void SetResult(int64_t i);
	void SetResult(double d);
	void SetNullResult();
private:
	void*			m_ctxt;
	int				m_argCount;
	void**			m_args;
};

class IcuSqlite3ScalarFunction
{
public:
	virtual ~IcuSqlite3ScalarFunction() {}

	virtual void Scalar(IcuSqlite3FunctionContext* pContext) = 0;
};

class IcuSqlite3AggregateFunction
{
public:
	virtual ~IcuSqlite3AggregateFunction() {}

	virtual void Step(IcuSqlite3FunctionContext* pContext) = 0;
	virtual void Finalize(IcuSqlite3FunctionContext* pContext) = 0;
};

//	:TODO: IcuSqlite3Authorizer
//	:TODO: IcuSqlite3Hook
//	:TODO: IcuSqlite3Collation

class ICUSQLITE_DLLIMPEXP IcuSqlite3ResultSet
{
public:
	IcuSqlite3ResultSet();
	IcuSqlite3ResultSet(const IcuSqlite3ResultSet& resultSet);
	
	IcuSqlite3ResultSet(void* db, void* stmt, bool eof, bool first = true,
		bool ownStmt = true);
		
	IcuSqlite3ResultSet& operator=(const IcuSqlite3ResultSet& resultSet);
	
	~IcuSqlite3ResultSet();
	
	int GetColumnCount() const { return (nullptr != m_stmt) ? m_cols : 0; }

	int FindColumnIndex(const UnicodeString& colName) const;
	UnicodeString GetColumnName(const int colIdx) const;
	UnicodeString GetDeclaredColumnType(const int colIdx) const;
	EIcuSqlite3ColumnTypes GetColumnType(const int colIdx) const;

	UnicodeString GetDatabaseName(const int colIdx) const;
	UnicodeString GetTableName(const int colIdx) const;
	UnicodeString GetOriginName(const int colIdx) const;
	
	int32_t GetInt(const int colIdx, const int32_t defVal = 0) const;
	int32_t GetInt(const UnicodeString& colName, const int32_t defVal = 0) const;

	int64_t GetInt64(const int colIdx, const int64_t defVal = 0) const;
	int64_t GetInt64(const UnicodeString& colName, const int64_t defVal = 0) const;
	
	double GetDouble(const int colIdx, const double defVal = 0.0) const;
	double GetDouble(const UnicodeString& colName, const double defVal = 0.0) const;
	
	UnicodeString GetString(const int colIdx, const UnicodeString& defVal = "") const;
	UnicodeString GetString(const UnicodeString& colName, const UnicodeString& defVal = "") const;

	std::string GetStringUTF8(const int colIdx, const std::string& defVal = "") const;
	std::string GetStringUTF8(const UnicodeString& colName, const std::string& defVal = "") const;

	UDate GetDateTime(const int colIdx, const UDate defVal = 0.0);
	UDate GetDateTime(const UnicodeString& colName, const UDate defVal = 0.0);

	bool GetBool(const int colIdx, const bool defVal = false) const;
	bool GetBool(const UnicodeString& colName, const bool defVal = false) const;
		
	const unsigned char* GetBlob(const int colIdx, int& len) const;
	const unsigned char* GetBlob(const UnicodeString& colName, int& len) const;
	//	:TODO: ICU has a memory buffer, make a GetBlob() for it
	
	
	bool IsNull(const int colIdx) const;
	bool IsNull(const UnicodeString& colName) const;
	
	bool Eof() const { return (nullptr != m_stmt) ? m_eof : true; }
	bool NextRow();
	void Finalize();
	
	const char* GetRawSQL() const;
	UnicodeString GetSQL() const;
	
	bool IsOK() const { return nullptr != m_db && nullptr != m_stmt; }
private:
	void*				m_db;
	void*				m_stmt;
	bool				m_eof;
	bool				m_first;
	int					m_cols;
	bool				m_ownStmt;
	ICUSQLite3Utility*	m_util;
};

class ICUSQLITE_DLLIMPEXP IcuSqlite3Table
{
public:
	IcuSqlite3Table();
	IcuSqlite3Table(const IcuSqlite3Table& table);
	IcuSqlite3Table(char** results, int rows, int cols);
	
	virtual ~IcuSqlite3Table();
	
	IcuSqlite3Table& operator=(const IcuSqlite3Table& table);
	
	int GetColumnCount() const { return (nullptr != m_results) ? m_cols : 0; }
	int GetRowCount() const { return (nullptr != m_results) ? m_rows : 0; }
	
	int FindColumnIndex(const char* utf8ColName) const;
	int FindColumnIndex(const UnicodeString& colName) const;
	
	UnicodeString GetColumnName(const int colIdx) const;
	
	int GetInt(const int colIdx, const int defVal = 0) const;
	int GetInt(const UnicodeString& colName, const int defVal = 0) const;
	
	//	:TODO: pass references for int64_t, double, UDate, etc.
	
	int64_t GetInt64(const int colIdx, const int64_t defVal = 0) const;
	int64_t GetInt64(const UnicodeString& colName, const int64_t defVal = 0) const;
	
	double GetDouble(const int colIdx, const double defVal = 0.0) const;
	double GetDouble(const UnicodeString& colName, const double defVal = 0.0) const;
	
	UnicodeString GetString(const int colIdx, const UnicodeString& defVal = "") const;
	UnicodeString GetString(const UnicodeString& colName, const UnicodeString& defVal = "") const;

	std::string GetStringUTF8(const int colIdx, const std::string& defVal = "") const;
	std::string GetStringUTF8(const UnicodeString& colName, const std::string& defVal = "") const;

	//
	//	Get Date/Time as ICU UTC
	//
	int64_t GetDateTime64(const int colIdx, const int64_t& defVal = 0,
		const EIcuSqlite3DTStorageTypes storedAs = ICUSQLITE_DATETIME_ISO8601);

	int64_t GetDateTime64(const UnicodeString& colName, const int64_t& defVal = 0,
		const EIcuSqlite3DTStorageTypes storedAs = ICUSQLITE_DATETIME_ISO8601);
	
	//
	//	Get Date/Time as ICU UDate
	//
	UDate GetDateTimeUDate(const int colIdx, const UDate& defVal = 0.0,
		const EIcuSqlite3DTStorageTypes storedAs = ICUSQLITE_DATETIME_ISO8601);

	UDate GetDateTimeUDate(const UnicodeString& colName, const UDate& defVal = 0.0,
		const EIcuSqlite3DTStorageTypes storedAs = ICUSQLITE_DATETIME_ISO8601);
	
	bool GetBool(const int colIdx) const;
	bool GetBool(const UnicodeString& colName) const;
	
	bool IsNull(const int colIdx) const;
	bool IsNull(const UnicodeString& colName) const;
	
	void SetRow(const int rowIdx)
	{
		if(nullptr == m_results || rowIdx < 0 || rowIdx > m_rows - 1) {
			return;
		}
		m_currentRow = rowIdx;
	}

	void Finalize();
	
	bool IsOK() const { return (nullptr != m_results); }
private:
	int					m_cols;
	int					m_rows;
	int					m_currentRow;
	char**				m_results;
	ICUSQLite3Utility*	m_util;
	
	char* GetValue(const int colIdx) const
	{
		if(nullptr == m_results || colIdx < 0 || colIdx > m_cols - 1) {
			return nullptr;
		}
		return m_results[(m_currentRow * m_cols) + m_cols + colIdx];
	}
};

//	:TODO: IcuSqlite3AutoTable with Next() for while(tbl.Next()) { ... }

class ICUSQLITE_DLLIMPEXP IcuSqlite3Statement
{
public:
	IcuSqlite3Statement();
	IcuSqlite3Statement(const IcuSqlite3Statement& stmt);
	IcuSqlite3Statement& operator=(const IcuSqlite3Statement& stmt);
	IcuSqlite3Statement(void* db, void* stmt);
	
	virtual ~IcuSqlite3Statement();
	
	int ExecuteUpdate();
	
	IcuSqlite3ResultSet ExecuteQuery();

	bool ExecuteScalar(UnicodeString& result);
	bool ExecuteScalar(std::string& result);
	bool ExecuteScalar(int32_t& result);
	bool ExecuteScalar(int64_t& result);
	bool ExecuteScalar(double& result);
	bool ExecuteScalar(bool& result);

	int GetParamCount();
	int GetParamIndex(const UnicodeString& paramName);
	UnicodeString GetParamName(const int paramIdx);
	
	bool Bind(const int paramIdx, const UnicodeString& paramValue);
	bool Bind(const int paramIdx, const int paramValue);
	bool Bind(const int paramIdx, const int64_t& paramValue);
	bool Bind(const int paramIdx, const uint64_t& paramValue);
	bool Bind(const int paramIdx, const double& paramValue);
	bool Bind(const int paramIdx, const char* paramValue);
	bool Bind(const int paramIdx, const std::string& paramValue);
	bool Bind(const int paramIdx, const unsigned char* blobValue,
		const int blobLen);
	//	:TODO: Bind() with ICU's memory buffer - ByteSink ?
	
	bool BindDateTime64(const int paramIdx, const int64_t& paramValue,
		const EIcuSqlite3DTStorageTypes storeAs = ICUSQLITE_DATETIME_ISO8601);
	bool BindDateTimeUDate(const int paramIdx, const UDate& paramValue,
		const EIcuSqlite3DTStorageTypes storeAs = ICUSQLITE_DATETIME_ISO8601);
	
	bool BindBool(const int paramIdx, const bool value);
	bool BindNull(const int paramIdx);
	bool BindZeroBlob(const int paramIdx, const int blobSize);
	
	void ClearBindings();
	
	UnicodeString GetSQL() const;
	
	void Reset();
	void Finalize();
	bool IsOk() const { return (nullptr != m_db && nullptr != m_stmt); }
private:
	void*				m_db;
	void*				m_stmt;
	ICUSQLite3Utility*	m_util;
};

//	:TODO: IcuSqlite3Blob

class ICUSQLITE_DLLIMPEXP IcuSqlite3Database
{
public:
	IcuSqlite3Database();
	virtual ~IcuSqlite3Database();
	
	bool Open(const UnicodeString& filename, const UnicodeString& key,
		const int flags = ICUSQLITE_OPEN_READWRITE | ICUSQLITE_OPEN_CREATE,
		const int extFlags = ICUSQLITE_EXT_OPEN_DEFAULT);
	bool Open(const UnicodeString& filename, 
		const int flags = ICUSQLITE_OPEN_READWRITE | ICUSQLITE_OPEN_CREATE,
		const int extFlags = ICUSQLITE_EXT_OPEN_DEFAULT,
		const unsigned char* key = nullptr,
		const int keyLen = 0);
	
	bool IsOpen() const { return (nullptr != m_db); }

	void Close();
	
	bool Backup(const UnicodeString& targetFilename, 
		const UnicodeString& targetKey,
		const UnicodeString& sourceDatabase = "main");

	bool Backup(const UnicodeString& targetFilename, 
		const unsigned char* targetKey = nullptr, const int keyLen = 0,
		const UnicodeString& sourceDatabase = "main");
		
	EIcuSqlite3RestoreResults Restore(const UnicodeString& sourceFilename, 
		const UnicodeString& sourceKey,
		const UnicodeString& targetDatabase = "main");

	EIcuSqlite3RestoreResults Restore(const UnicodeString& sourceFilename,
		const unsigned char* sourceKey = nullptr, const int keyLen = 0,
		const UnicodeString& targetDatabase = "main");
		
	bool Begin(const EIcuSqlite3TransTypes type = ICUSQLITE_TRANSACTION_DEFAULT);
	bool Commit();
	bool Rollback(const UnicodeString& savepointName = "");
	
	bool IsAutoCommitMode() const;
	
	bool Savepoint(const UnicodeString& savepointName);
	bool ReleaseSavepoint(const UnicodeString& savepointName);
	
	bool TableExists(const UnicodeString& tableName, const UnicodeString& dbName = "") const;
	bool TableExists(const char* tableName, const char* dbName = nullptr) const;
	
	//	:TODO: TableExists(name, std::set<UnicodeString>& dbNames)
	
	void GetDatabaseNames(std::set<std::string>& dbNames,
		std::set<std::string>& dbFiles) const;

	bool Attach(const UnicodeString& dbPath, const std::string& asName);

	bool IsAttached(const std::string& alias) const;
	int GetAttachedCount() const;
		
	bool EnableForeignKeySupport(const bool enable);
	bool IsForeignKeySupportEnabled() const;
	
	bool CheckSyntax(const UnicodeString& sql);
	bool CheckSyntax(const char* sql);
	//	:TODO: bool CheckSyntax(const IcuSqlite3StatementBuffer& sql);
	
	int ExecuteUpdate(const UnicodeString& sql);
	int ExecuteUpdate(const char* sql);
	int ExecuteUpdate(const IcuSqlite3StatementBuffer& sql);
	
	IcuSqlite3ResultSet ExecuteQuery(const UnicodeString& sql) const;
	IcuSqlite3ResultSet ExecuteQuery(const char* sql) const;
	IcuSqlite3ResultSet ExecuteQuery(const IcuSqlite3StatementBuffer& sql) const;

	bool ExecuteScalar(const UnicodeString& sql, UnicodeString& result) const;
	bool ExecuteScalar(const char* sql, UnicodeString& result) const;
	bool ExecuteScalar(const UnicodeString& sql, int32_t& result) const;
	bool ExecuteScalar(const char* sql, int32_t& result) const;
	bool ExecuteScalar(const UnicodeString& sql, int64_t& result) const;
	bool ExecuteScalar(const char* sql, int64_t& result) const;
	bool ExecuteScalar(const UnicodeString& sql, double& result) const;
	bool ExecuteScalar(const char* sql, double& result) const;
	bool ExecuteScalar(const UnicodeString& sql, bool& result) const;
	bool ExecuteScalar(const char* sql, bool& result) const;
	bool ExecuteScalar(const UnicodeString& sql, std::string& result) const;
	bool ExecuteScalar(const char* sql, std::string& result) const;
	bool ExecuteScalarDateTime(const UnicodeString& sql, UDate& result);
	bool ExecuteScalarDateTime(const char* sql, UDate& result);
	
	IcuSqlite3Table GetTable(const UnicodeString& sql) const;
	IcuSqlite3Table GetTable(const char* sql) const;
	IcuSqlite3Table GetTable(const IcuSqlite3StatementBuffer& sql) const;
	
	IcuSqlite3Statement PrepareStatement(const UnicodeString& sql) const;
	IcuSqlite3Statement PrepareStatement(const char* sql) const;
	IcuSqlite3Statement PrepareStatement(const IcuSqlite3StatementBuffer& sql) const;
	
	int64_t GetLastRowId() const;
	int64_t GetChanges() const;
	//	:TODO: GetReadOnlyBlob()
	//	:TODO: GetWritableBlob();
	//	:TODO: GetBlob();
	
	void Interrupt();
	bool SetBusyTimeout(const int ms);
	
	bool CreateScalarFunction(const char* funcName, const int args, 
		IcuSqlite3ScalarFunction* func);
	bool CreateAggregateFunction(const char* funcName, const int args, 
		IcuSqlite3AggregateFunction* func);

	//	:TODO: SetAuthorizer()
	//	:TODO: SetHook(hookFunc, type)
	//	:TODO: SetCollation()
	
	void GetMetaData(const UnicodeString& dbName, 
		const UnicodeString& tableName, const UnicodeString& colName,
		UnicodeString* declaredDataType = nullptr, 
		UnicodeString* collation = nullptr, int* flags = nullptr);
	
	bool LoadExtension(const UnicodeString& filename, 
		const char* entryPoint = "sqlite3_extension_init");
		
	bool EnableLoadExtension(bool enable);
	
	bool ReKey(const char* key);
	bool ReKey(const unsigned char* keyBuf, const int keyLen);
	
	bool IsEncrypted() const { return m_encrypted; }
	
	//	:TODO: static GetLimitName(id)
	UnicodeString GetDatabaseFileName(const UnicodeString& dbName) const;
	int GetLimit(const EIcuSqlite3Limit limit) const;
	int SetLimit(const EIcuSqlite3Limit limit, const int value);
	
	//
	//	Error access
	//
	int GetLastErrorCode(const bool extended = false) const;
	UnicodeString GetLastErrorMessage() const;
	int GetLastSystemErrorCode() const;
	
	//
	//	static methods
	//
	static bool InitializeSQLite();
	static bool ShutdownSQLite();
	static bool Randomness(unsigned char* randBuf, const int bufLen);
	static bool EnableSharedCache(const bool enable);
	
	static bool IsSharedCacheEnabled()
	{ 
#if !defined(SQLITE_OMIT_SHARED_CACHE)
		return ms_sharedCacheEnabled;
#else
		return false;
#endif
	}
	
	static UnicodeString GetVersion();
	static UnicodeString GetSourceId();
	static bool HasSupport(const EIcuSqlite3SupportFlags supportFor);
	
//	static bool Config(const EIcuSqlite3Config configOpt, ...);
	
#if defined(ICUSQLITE3_ANDROID) || defined(ICUSQLITE3_IOS)
	static int ReleaseMemory();
#endif	//	defined(ICUSQLITE3_ANDROID)

	int RecoverMemory();

	bool WALCheckpoint(
		const char* dbName = nullptr, const EIcuSqlite3WALCheckpoint checkpointType = ICUSQLITE_WAL_CHECKPOINT_PASSIVE) const;
protected:
	void* GetDatabaseHandle() const { return m_db; }

	//	:TODO: collation stuffs
	
	
private:
	void*			m_db;
	int				m_busyTimeout;
	bool			m_encrypted;

#if !defined(SQLITE_OMIT_SHARED_CACHE)
	static bool		ms_sharedCacheEnabled;
#endif	//	!defined(SQLITE_OMIT_SHARED_CACHE)

	static int		ms_supportFlags;

	IcuSqlite3Database(const IcuSqlite3Database& db);
	IcuSqlite3Database& operator=(const IcuSqlite3Database& db);
	
	//	:TODO: give these better, more descriptive names:
	static void xFunc(void* ctxt, int argCount, void** args);
	static void xStep(void* ctxt, int argCount, void** args);
	static void xFinalize(void* ctxt);
	static void xDestroyScalar(void* userData);
	static void xDestroyAggregate(void* userData);

	void* Prepare(const UChar* sql, const int32_t sqlLen = -1) const;
};

class ICUSQLITE_DLLIMPEXP IcuSqlite3Transaction
{
public:
	explicit IcuSqlite3Transaction(IcuSqlite3Database* db,
		const bool commitOnDestroy);
	explicit IcuSqlite3Transaction(IcuSqlite3Database* db,
		const EIcuSqlite3TransTypes type = ICUSQLITE_TRANSACTION_DEFAULT,
		const bool commitOnDestroy = true);
	~IcuSqlite3Transaction();

	bool Open(const EIcuSqlite3TransTypes type = ICUSQLITE_TRANSACTION_DEFAULT);
	bool Flush(const bool reOpen = false);
	bool Rollback(const bool reOpen = false);
	
	bool IsOk() const { return m_transOk; }
	bool IsOpen() const { return m_transOpen; }

	bool Execute(const UnicodeString& sql);
	bool Execute(const char* sql);
	bool Execute(const IcuSqlite3StatementBuffer& sql);
	bool Execute(IcuSqlite3Statement& stmt);
	
	int64_t GetLastRowId() const;
private:
	IcuSqlite3Database*	m_db;
	bool				m_transOk;
	bool				m_transOpen;
	const bool			m_commitOnDestroy;

	static void* operator new(size_t size);	//	this obj must be created on the stack
	static void operator delete(void* p);
	IcuSqlite3Transaction(const IcuSqlite3Transaction& t);	//	 prevent copy
	IcuSqlite3Transaction& operator=(const IcuSqlite3Transaction& t);	//	prevent assign
};


#endif	//	!__ICU_SQLITE3_H__
