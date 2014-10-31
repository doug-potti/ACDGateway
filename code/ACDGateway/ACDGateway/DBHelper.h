#ifndef DB_ADO_HELPER_H
#define DB_ADO_HELPER_H




#import "C:\Program Files\Common Files\System\ADO\msado15.dll" \
	no_namespace rename("EOF", "EndOfFile")

#include <ole2.h>
#include <stdio.h>
#include <conio.h>
#include <string>

#pragma warning(disable : 4101)

class DBHelper
{
public:
	DBHelper();
	~DBHelper();

	bool GetDevFromDB();
	bool GetBusDialMap();
private:
	int  GetFieldType(int index);
	bool ExeSelectSQL(char* sqltext);
	bool MoveNextRecord();
	bool MoveFirstRecord();
	bool GetFieldVlaue(int index,_variant_t* value);
	int  GetFieldCount();
	int  GetRecordCount();
	bool ExecCommandSQL(char * sqltext);
	bool DisConnectDB();
	bool ConnectDB(const char* connstr);
	long GetState();
	bool Eof();

	bool								m_bConn;
	DWORD								m_errCode;
	std::string							m_errMsg;
private:
	char								m_dbConnStr[4096];
	_ConnectionPtr						m_pConnection;
	_CommandPtr							m_pCommand;
	_RecordsetPtr						m_pRecordSet;
};

extern DBHelper*                        gDBHelper;                         

#endif
