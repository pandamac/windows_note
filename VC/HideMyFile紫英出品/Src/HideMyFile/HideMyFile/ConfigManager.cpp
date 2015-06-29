#include "StdAfx.h"
#include "ConfigManager.h"


CConfigManager::CConfigManager(void)
{

}


CConfigManager::~CConfigManager(void)
{
}

BOOL CConfigManager::InitializeConfigFile()
{
    //得到配置文件路径
    m_ConfigFilePath = GetSystemDir();
    m_ConfigFilePath +=  L"\\ArtemisConfig.ini";

    //文件不存在则创建
    if (!PathFileExists(m_ConfigFilePath))
    {
 char szConfigFile[] = "[HideConfig]\r\nAutoStart=0\r\nShowTrayOnStart=1\r\nHideProcess=0\r\nCloseState=0\r\nConfigPwd=000000\r\n";
        HANDLE hFile = CreateFile(m_ConfigFilePath, GENERIC_WRITE,  FILE_SHARE_READ, NULL, CREATE_ALWAYS, 
                                            FILE_ATTRIBUTE_NORMAL,NULL );

        if (INVALID_HANDLE_VALUE == hFile)
        {
            return FALSE;
        }

        //写入默认配置
        DWORD dwWrite;
        WriteFile(hFile, szConfigFile, strlen(szConfigFile), &dwWrite, NULL);

        CloseHandle(hFile);
    }

    //MessageBox(NULL, m_ConfigFilePath, NULL, MB_OK);
    return TRUE;
}

BOOL CConfigManager::LoadConfigFile()
{
    m_bAutoStart = GetConfigValueInt(CCM_TYPE_AUTOSTART);
    m_bShowTrayOnStart = GetConfigValueInt(CCM_TYPE_SHOWTRAY);
    m_bHideProcess = GetConfigValueInt(CCM_TYPE_HIDEPROCESS);
    m_bCloseState = GetConfigValueInt(CCM_TYPE_CLOSESTATE);
    //m_ConfigPwd = GetConfigFileString(L"HideConfig", L"ConfigPwd");

    return TRUE;
}

CString CConfigManager::GetSystemDir()
{
    CString strSystemPath;
    GetWindowsDirectory(strSystemPath.GetBuffer(MAX_PATH+1), MAX_PATH);
    strSystemPath.ReleaseBuffer();

    return strSystemPath;
}

CString CConfigManager::XORPassword( CString &strSrc )
{
    TCHAR *szXORBuf = NULL;
    CString strTargetString;
    int nLength = strSrc.GetLength();
    szXORBuf = new TCHAR[nLength+1];

    strSrc.GetBuffer();
    wcscpy_s(szXORBuf, nLength+1, strSrc); //拷贝一份

    for (int i = 0; i< nLength; ++i)
    {
        //szXORBuf[i] ^= 0x6D;   //异或加密
    }

    strSrc.ReleaseBuffer();

    strTargetString = szXORBuf;

    delete []szXORBuf;


    return strTargetString;

}

UINT CConfigManager::GetConfigValueInt( CCMConfigType nCCMConfigType )
{
    UINT nValue = 0;

    switch(nCCMConfigType)
    {
    case CCM_TYPE_AUTOSTART:
        {
            nValue = GetPrivateProfileInt(CCM_APP_NAME, CCM_KEY_AUTOSTART, 0, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_SHOWTRAY:
        {
            nValue = GetPrivateProfileInt(CCM_APP_NAME, CCM_KEY_SHOWTRAY, 0, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_HIDEPROCESS:
        {
            nValue = GetPrivateProfileInt(CCM_APP_NAME, CCM_KEY_HIDEPROCESS, 0, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_CLOSESTATE:
        {
            nValue = GetPrivateProfileInt(CCM_APP_NAME, CCM_KEY_CLOSTSTATE, 0, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_CONFIGPWD:
        {
            nValue = -1;
            break;
        }
    default:
        break;
    }

    return nValue;
}

CString CConfigManager::GetConfigValueString( CCMConfigType nCCMConfigType )
{
    CString strValue;

    switch(nCCMConfigType)
    {
    case CCM_TYPE_AUTOSTART:
        {
            GetPrivateProfileString(CCM_APP_NAME, CCM_KEY_AUTOSTART, NULL, 
                                           strValue.GetBuffer(MAX_PATH), MAX_PATH, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_SHOWTRAY:
        {
            GetPrivateProfileString(CCM_APP_NAME, CCM_KEY_SHOWTRAY, NULL, 
                                           strValue.GetBuffer(MAX_PATH), MAX_PATH, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_HIDEPROCESS:
        {
            GetPrivateProfileString(CCM_APP_NAME, CCM_KEY_HIDEPROCESS, NULL, 
                                          strValue.GetBuffer(MAX_PATH), MAX_PATH, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_CLOSESTATE:
        {
            GetPrivateProfileString(CCM_APP_NAME, CCM_KEY_CLOSTSTATE, NULL, 
                                           strValue.GetBuffer(MAX_PATH), MAX_PATH, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_CONFIGPWD:
        {
            GetPrivateProfileString(CCM_APP_NAME, CCM_KEY_CONFIGPWD, NULL, 
                                          strValue.GetBuffer(MAX_PATH), MAX_PATH, m_ConfigFilePath);
            break;
        }
    default:
        break;
    }
    
    strValue.ReleaseBuffer();

    return strValue;
}

BOOL CConfigManager::SetConfigValueInt( CCMConfigType nCCMConfigType, int nValue )
{
    BOOL bRet = FALSE;
    TCHAR szBuffer[100];
    _itow_s(nValue, szBuffer, 10); //将数字转换成字符串

    switch(nCCMConfigType)
    {
    case CCM_TYPE_AUTOSTART:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_AUTOSTART, szBuffer, m_ConfigFilePath);
            m_bAutoStart = nValue;
            break;
        }
    case CCM_TYPE_SHOWTRAY:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_SHOWTRAY, szBuffer, m_ConfigFilePath);
            m_bShowTrayOnStart = nValue;
            break;
        }
    case CCM_TYPE_HIDEPROCESS:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_HIDEPROCESS, szBuffer, m_ConfigFilePath);
            m_bHideProcess = nValue;
            break;
        }
    case CCM_TYPE_CLOSESTATE:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_CLOSTSTATE, szBuffer, m_ConfigFilePath);
            m_bCloseState = nValue;
            break;
        }
    default:
        break;
    }

    return bRet;
}

BOOL CConfigManager::SetConfigValueString( CCMConfigType nCCMConfigType, CString strValue )
{
    BOOL bRet = FALSE;

    switch(nCCMConfigType)
    {
    case CCM_TYPE_AUTOSTART:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_AUTOSTART, strValue, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_SHOWTRAY:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_SHOWTRAY, strValue, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_HIDEPROCESS:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_HIDEPROCESS, strValue, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_CLOSESTATE:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_CLOSTSTATE, strValue, m_ConfigFilePath);
            break;
        }
    case CCM_TYPE_CONFIGPWD:
        {
            bRet = WritePrivateProfileString(CCM_APP_NAME, CCM_KEY_CONFIGPWD, strValue, m_ConfigFilePath);
            break;
        }
    default:
        break;
    }
    return bRet;
}

void CConfigManager::Initialize()
{
    if (InitializeConfigFile())
    {
        LoadConfigFile();
    }
    else
    {
        MessageBox(NULL,L"初始化配置文件失败！", L"错误", MB_OK|MB_ICONERROR);
    }
}
