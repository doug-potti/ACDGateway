
#include "DBHelper.h"
#include "AGUtil.h"

DBHelper*                        gDBHelper;
DBHelper::DBHelper()
{
	m_bConn = false;
	if("MSSQL" == AGHelper::sAcdgwCfg.dbType)
	{
		m_conStr.append("Provider=SQLOLEDB.1;Password=");
		m_conStr.append(AGHelper::sAcdgwCfg.dbPasswnd);
		m_conStr.append(";Persist Security Info=False;User ID=");
		m_conStr.append(AGHelper::sAcdgwCfg.dbUser);
		m_conStr.append(";Initial Catalog=");
		m_conStr.append(AGHelper::sAcdgwCfg.dbName);
		m_conStr.append(";Data Source=");
		m_conStr.append(AGHelper::sAcdgwCfg.dbIp);
	}
}


DBHelper::~DBHelper()
{
}


bool DBHelper::ConnectDB(const char *connstr)
{
	HRESULT								hr;
	DWORD								dwConnEvt;
	IConnectionPointContainer *			pCPC = NULL;
	IConnectionPoint *					pCP = NULL;
	IUnknown *							pUnk = NULL;


	m_pCommand = NULL;
	m_pConnection = NULL;
	m_pRecordSet = NULL;
	memset(m_dbConnStr, 0, 4096);
	strcpy(m_dbConnStr, connstr);

	try
	{
		::CoInitialize(NULL);
		if ( FAILED(m_pConnection.CreateInstance(__uuidof(Connection))) )
			return false;


		if ( FAILED(m_pConnection->Open(m_dbConnStr, "", "", adConnectUnspecified)) )
			return false;
	}
	catch (_com_error &e)
	{
		m_errCode = GetLastError();
		m_errMsg = e.ErrorMessage();
		return false;
	}

	m_bConn = true;
	return true;
}




bool DBHelper::DisConnectDB()
{
	try
	{
		m_pCommand = NULL;
		m_pRecordSet->Close();
		m_pConnection->Close();
		m_pRecordSet = NULL;
		m_pConnection = NULL;
		m_bConn = false;
	}
	catch(_com_error &e)
	{
		m_pRecordSet = NULL;
		m_pConnection = NULL;
		m_bConn = false;
		return false;
	}

	return true;
}




bool DBHelper::ExecCommandSQL(char *sqltext)
{
	try
	{
		m_pRecordSet = NULL;
		m_pCommand = NULL;
		if ( FAILED(m_pCommand.CreateInstance(__uuidof(Command))) )
			return false;
		m_pCommand->ActiveConnection = m_pConnection;
		m_pCommand->CommandText = sqltext;
		m_pCommand->CommandType = adCmdText;
		m_pCommand->CommandTimeout = 15;
		_variant_t   vNull;   
		vNull.vt   =   VT_ERROR;   
		vNull.scode   =   DISP_E_PARAMNOTFOUND; 
		if ( FAILED(m_pRecordSet.CreateInstance(__uuidof(Recordset))) )
			return false;
		m_pRecordSet = m_pCommand->Execute(&vNull, &vNull, adCmdText);
	}
	catch(_com_error &e)
	{
		m_errCode = GetLastError();
		m_errMsg = e.ErrorMessage();
		return false;
	}

	return true;
}




int DBHelper::GetRecordCount()
{
	if ( m_pRecordSet != NULL )
	{
		return m_pRecordSet->RecordCount;
	}

	return -1;
}




int DBHelper::GetFieldCount()
{
	if ( m_pRecordSet != NULL )
	{
		return m_pRecordSet->Fields->Count;
	}

	return -1;
}




bool DBHelper::GetFieldVlaue(int index, _variant_t *value)
{
	_variant_t vtIndex;
	vtIndex.vt = VT_I2;
	vtIndex.iVal = index;


	_variant_t							tmp;


	try
	{
		if ( (m_pRecordSet != NULL) && (index <m_pRecordSet->Fields->Count) )
		{
			tmp = m_pRecordSet->GetCollect(vtIndex);
			if ( tmp.vt == VT_NULL )
			{
				*value = "";
			}
			else
			{
				*value = tmp;
			}
			return true;
		}
	}
	catch(_com_error &e)
	{
		m_errCode = GetLastError();
		m_errMsg = e.ErrorMessage();
		return false;
	}

	return false;
}




bool DBHelper::MoveFirstRecord()
{
	if ( ( m_pRecordSet != NULL ) && ( m_pRecordSet->RecordCount >= 1 ) )
	{
		m_pRecordSet->MoveFirst();
		return true;
	}

	return false;
}




bool DBHelper::MoveNextRecord()
{
	if ( ( m_pRecordSet != NULL ) && ( !m_pRecordSet->EndOfFile ) )
	{
		m_pRecordSet->MoveNext();
		return true;
	}

	return false;
}




bool DBHelper::ExeSelectSQL(char *sqltext)
{
	try
	{
		m_pRecordSet = NULL;
		if ( FAILED(m_pRecordSet.CreateInstance(__uuidof(Recordset))) )
			return false;
		if ( FAILED(m_pRecordSet->Open(sqltext,_variant_t((IDispatch *)m_pConnection),adOpenStatic,adLockReadOnly,adCmdText)) )
			return false;
	}
	catch(_com_error &e)
	{
		m_errCode = GetLastError();
		m_errMsg = e.ErrorMessage();
		return false;
	}

	return true;
}




int DBHelper::GetFieldType(int index)
{
	_variant_t vtIndex;
	vtIndex.vt = VT_I2;
	vtIndex.iVal = index;
	if ( ( m_pRecordSet != NULL ) && ( index <m_pRecordSet->Fields->Count ) )
	{
		return m_pRecordSet->Fields->GetItem(vtIndex)->Type;
	}
	return -1;
}




long DBHelper::GetState()
{
	if ( m_pConnection == NULL )
		return adStateClosed;
	long								ret;


	//m_pConnection->get_State(&ret);
	return m_pConnection->State;
}




bool DBHelper::Eof()
{
	if ( m_pRecordSet != NULL )
	{
		return m_pRecordSet->EndOfFile;
	}

	return false;
}

bool DBHelper::GetDevFromDB()
{
	std::string							tmpFiled;


	try
	{
		if ( !m_bConn )
		{
			ConnectDB(m_conStr.c_str());
		}

		if (m_bConn)
		{
			char								strSql[640];
			_variant_t							tmp;
			memset(strSql, 0, 640);
			strcpy(strSql, "select * from v_ACDGatewayDevList");
			bool ret = ExeSelectSQL(strSql);
			int recordcnt = GetRecordCount();
			if ( ret )
			{
				MoveFirstRecord();
			}

			if ("MSSQL" == AGHelper::sAcdgwCfg.dbType)
			{
				int fieldcnt = GetFieldCount();
				for ( int i = 0; i < recordcnt; ++i )
				{
					Dev_t * newDev = new Dev_t();
					ret = GetFieldVlaue(0, &tmp);
					tmpFiled = _bstr_t(tmp);
					newDev->devId = tmpFiled;
					ret = GetFieldVlaue(1, &tmp);
					tmpFiled = _bstr_t(tmp);
					if ("0" == tmpFiled)
					{
						newDev->devType = Device;
					}
					else if ("1" == tmpFiled)
					{
						newDev->devType = Vdn;
					}
					else
					{
						newDev->devType = Split;
					}
					AGHelper::sDevList.push_back(newDev);
					( i < recordcnt ) ? MoveNextRecord() : 0;
				}


			}
			return true;
		}
	}
	catch(...)
	{
		std::stringstream ssLog("");
		ssLog<<"DBHelper GetDevFromDB occur error errCode:"<<
			m_errCode<<" errMsg:"<<
			m_errMsg<<std::endl;

		OUTERRORLOG(ssLog.str());
		ssLog.str("");
		return false;
	}
	return false;
}

std::string DBHelper::GetDialNo(std::string mediaType, std::string busType, std::string custLvl)
{
	std::string							tmpFiled;
	try
	{
		if ( ! m_bConn )
		{
			ConnectDB(m_conStr.c_str());
		}
		if (m_bConn)
		{
			std::string                         sql;
			_variant_t							tmp;

			sql = "select * from v_ACDGatewayGetDialMap where MediaType ='";
			sql.append(mediaType);
			sql.append("' and BussinessType='");
			sql.append(busType);
			sql.append("' and CustomerLevel='");
			sql.append(custLvl);
			sql.append("'");
			bool ret = ExeSelectSQL(const_cast<char*>(sql.c_str()));
			int recordcnt = GetRecordCount();
			if ( ret )
			{
				MoveFirstRecord();
			}

			if ("MSSQL" == AGHelper::sAcdgwCfg.dbType)
			{
				int fieldcnt = GetFieldCount();
				for ( int i = 0; i < recordcnt; ++i )
				{
					ret = GetFieldVlaue(3, &tmp);
					tmpFiled = _bstr_t(tmp);
						
					return tmpFiled;
				}
			}
			return "";
		}
	}
	catch(...)
	{
		std::stringstream ssLog("");
		ssLog<<"DBHelper GetBusDialMap occur error errCode:"<<
			m_errCode<<" errMsg:"<<
			m_errMsg<<std::endl;

		OUTERRORLOG(ssLog.str());
		ssLog.str("");
		return "";
	}
	return "";
}
