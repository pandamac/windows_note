#pragma once
#include <WinIoCtl.h>

typedef enum _SystemVer
{
    VER_WINDOWS_XP = 0,
    VER_WINDOWS_7 =1
}SystemVer;

#define CDM_SERVICE_START_TYPE SERVICE_DEMAND_START
#define CDM_SERVICE_NAME L"HideSys"
#define CDM_DISPLAY_NAME L"HideSys"

//添加新文件
#define IOCTL_ADD_HIDDEN_FILE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810,METHOD_BUFFERED, FILE_ANY_ACCESS)

//设置文件隐藏
#define IOCTL_SET_FILE_HIDDEN CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811,METHOD_BUFFERED, FILE_ANY_ACCESS)

//设置文件显示
#define IOCTL_SET_FILE_SHOWN CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812,METHOD_BUFFERED, FILE_ANY_ACCESS)

//清空文件计数
#define IOCTL_CLEAR_FILE_COUNT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813,METHOD_BUFFERED, FILE_ANY_ACCESS)

//隐藏进程
#define IOCTL_HIDE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815,METHOD_BUFFERED, FILE_ANY_ACCESS)

//显示进程
#define IOCTL_SHOW_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816,METHOD_BUFFERED, FILE_ANY_ACCESS)


class CDriverManager
{
public:
    CDriverManager(void);
    ~CDriverManager(void);

public:
    void Initialize();

    //启动驱动服务
    BOOL StartDriverService();

    //关闭驱动服务
    BOOL StopDriverService();

    //安装驱动
    BOOL InstallDriver();

    //卸载驱动
    BOOL UninstallDriver();

    //添加文件
    BOOL AddNewHiddenFile(LPCTSTR strFileName);

    //设置文件隐藏状态
    BOOL SetFileDriverHiddenState(LPCTSTR strFileName, BOOL bState);

    //设置进程隐藏状态
    BOOL SetProcessHiddenState(LPCTSTR strProcessName, BOOL bState);

    //清空文件计数
    BOOL ClearFileCount();

    //获取当前进程名称
    CString GetCurrentProcessName();

private:
    void InitializeDriver();

    SystemVer CheckWindowsVersion();

    BOOL DriverIoControl(DWORD dwControlCode, LPCTSTR InputBuffer, DWORD dwInputSize, LPDWORD dwRet);

    //释放驱动文件
    BOOL ExtractSysFile();

    //获取系统目录
    CString GetSystemDir();
private:
    CString m_strDriverPath;
};

