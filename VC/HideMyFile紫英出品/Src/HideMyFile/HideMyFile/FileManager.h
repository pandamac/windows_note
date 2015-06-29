#include "stdafx.h"


#define CFM_SECTION_PREFIX L"FSection"
#define CFM_KEY_ID L"nID"
#define CFM_KEY_FOLDER_NAME L"FileName"
#define CFM_KEY_FOLDER_PATH L"FilePath"
#define CFM_KEY_HIDDEN L"Hidden"

const int MAX_FOLDER_NUM  = 100;

class CFileItem
{
public:
    UINT m_nID;
    CString m_strFileName;
    CString m_strFilePath;
    UINT m_bHidden;

public:
    CFileItem()
    {
        m_nID = 0;
        m_strFileName = L"";
        m_strFilePath = L"";
        m_bHidden = 0;
    }
};


class CFileManager
{
public:
    CFileManager(void);
    ~CFileManager(void);

public:
    void Initialize();

    //添加一个文件项
    void AddFileItem(LPCTSTR strFilePath);

    //设置文件列表显示状态
    void SetFileListHiddenState(UINT nID, BOOL bHidden);

    //删除一个文件项目
    void DeleteFileItem(UINT nIndex);

    //获取文件名
    CString GetItemFileName(UINT nIndex);

    //获取文件路径
    CString GetItemFilePath(UINT nIndex);

    //获取ID
    UINT    GetItemFileId(UINT nIndex);

    //获取文件显示状态
    UINT    GetItemFileState(UINT nIndex);

    //获取文件列表数
    UINT GetFileItemCount();

private:
    
    void AddFileItem(CFileItem newItem);

    //读取文件列表
    void ReadFileList();

    //保存文件列表
    void SaveFileList();

    //刷新文件列表索引
    void RefreshFileItemIndex();

    //重新加载文件列表
    void ReloadFileList();

    //创建INI节点
    BOOL WriteFileListSection(LPCTSTR strSectionName);

    //写入String
    BOOL WriteFileListDataString(LPCTSTR strSectionName,LPCTSTR strKey, LPCTSTR strValue);

    //写入数值
    BOOL WriteFileListDataInt(LPCTSTR strSectionName, LPCTSTR strKey, UINT nValue);

    //获取String
    CString GetFileListDataString(LPCTSTR strSectionName, LPCTSTR strKey);

    //获取数值
    UINT GetFileListDataInt(LPCTSTR strSectionName, LPCTSTR strKey);

    //从文件路径获取文件名
    CString GetFileName(CString strFilePath);

    //获取系统目录
    CString GetSystemDir();


private:
    CArray<CFileItem, CFileItem> m_FileArray;
    CString m_strFileListFilePath;
};

