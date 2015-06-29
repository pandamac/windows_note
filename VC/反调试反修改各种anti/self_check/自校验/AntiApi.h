#pragma once

class CAntiApi
{
public:
	CAntiApi(void);
	~CAntiApi(void);

public:
	void AntiCC( char * m_ModuleName,  char * m_ApiName);
	void AntiPage_Protect( char * m_ModuleName,  char * m_ApiName);
	void AntiHardware();
};
