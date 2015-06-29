#pragma once

//键值类型
enum CCMConfigType
{
    CCM_TYPE_AUTOSTART = 0,
    CCM_TYPE_SHOWTRAY = 1,
    CCM_TYPE_CLOSESTATE = 2,
    CCM_TYPE_HIDEPROCESS = 3,
    CCM_TYPE_CONFIGPWD = 4
};

//INI键值名
#define CCM_APP_NAME L"HideConfig"
#define CCM_KEY_AUTOSTART L"AutoStart "
#define CCM_KEY_SHOWTRAY L"ShowTrayOnStart"
#define CCM_KEY_CLOSTSTATE L"CloseState"
#define CCM_KEY_HIDEPROCESS L"HideProcess"
#define CCM_KEY_CONFIGPWD L"ConfigPwd"

class CConfigManager
{
public:
    CConfigManager(void);
    ~CConfigManager(void);

public:

    void Initialize();

     UINT GetConfigValueInt(CCMConfigType nCCMConfigType);

     CString GetConfigValueString(CCMConfigType nCCMConfigType);

     BOOL SetConfigValueInt(CCMConfigType nCCMConfigType, int nValue);

     BOOL SetConfigValueString(CCMConfigType nCCMConfigType, CString strValue);

     //XOR加密解密密码
     CString XORPassword(CString &strSrc);

     //获取系统文件夹路径 
     CString GetSystemDir();
private:
    //初始化配置文件
    BOOL InitializeConfigFile();

    //加载配置文件
    BOOL LoadConfigFile();

private:
    CString m_ConfigFilePath; //配置文件路径
    int m_bAutoStart;//开机启动
    int m_bShowTrayOnStart;//显示托盘图标
    int m_bHideProcess;//隐藏进程
    int m_bCloseState;//关闭按钮时操作
    CString m_ConfigPwd;
};

