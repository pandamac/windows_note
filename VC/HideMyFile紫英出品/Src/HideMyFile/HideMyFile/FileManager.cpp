#include "StdAfx.h"
#include "FileManager.h"

CFileManager::CFileManager(void)
{

}


CFileManager::~CFileManager(void)
{
}

void CFileManager::AddFileItem(LPCTSTR strFilePath )
{
    CFileItem m_Item;
    m_Item.m_nID = m_FileArray.GetCount() + 1;
    m_Item.m_strFilePath = strFilePath;
    m_Item.m_strFileName = GetFileName(strFilePath);
    m_Item.m_bHidden = TRUE;

    m_FileArray.Add(m_Item);

    //保存文件列表
    SaveFileList();

}

void CFileManager::AddFileItem( CFileItem newItem )
{
    m_FileArray.Add(newItem);

    //保存文件列表
    SaveFileList();
}

void CFileManager::SetFileListHiddenState(UINT nID, BOOL bHidden )
{
    CString strSection;
    int nCount = m_FileArray.GetCount();
    int nIdx;
    for (nIdx = 0; nIdx < nCount; ++nIdx)
    {
        if (m_FileArray.GetAt(nIdx).m_nID == nID)
        {
            //设置列表中为隐藏
            m_FileArray.GetAt(nIdx).m_bHidden = bHidden;

            //更新列表文件内容
            strSection.Format(L"%s%d", CFM_SECTION_PREFIX, nID);
            WriteFileListDataInt(strSection, CFM_KEY_HIDDEN, bHidden);
            break;
        }
    }
}

void CFileManager::DeleteFileItem( UINT nID )
{
    int nCount = m_FileArray.GetCount();
    int nIdx = 0;

    for (nIdx  = 0; nIdx < nCount; ++nIdx)
    {
        if (m_FileArray.GetAt(nIdx).m_nID == nID)
        {
            m_FileArray.RemoveAt(nIdx);
            break;
        }
    }

    //刷新索引
    RefreshFileItemIndex();

    //重新保存文件
    SaveFileList();
}

void CFileManager::ReadFileList()
{

    if (!PathFileExists(m_strFileListFilePath))
    {
        HANDLE hFile = CreateFile(m_strFileListFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        CloseHandle(hFile);
        return;
    }

    int nIdx;
    CString strSectionName;
    TCHAR szBuf[20];
    for (nIdx = 1; nIdx <= MAX_FOLDER_NUM; ++nIdx)
    {
        strSectionName.Format(L"%s%d", CFM_SECTION_PREFIX, nIdx);
        DWORD dwRead = GetPrivateProfileSection(strSectionName, szBuf, 20, m_strFileListFilePath);
        if (dwRead != 0) //节点存在
        {
            CFileItem pItem;
            pItem.m_nID = GetFileListDataInt(strSectionName, CFM_KEY_ID);
            pItem.m_strFileName = GetFileListDataString(strSectionName, CFM_KEY_FOLDER_NAME);
            pItem.m_strFilePath = GetFileListDataString(strSectionName, CFM_KEY_FOLDER_PATH);
            pItem.m_bHidden = GetFileListDataInt(strSectionName, CFM_KEY_HIDDEN);

            AddFileItem(pItem);
        }
        else if (dwRead == 0)
        {
            break;
        }

    }
}

void CFileManager::SaveFileList()
{
    HANDLE hFile = CreateFile(m_strFileListFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    CloseHandle(hFile);

    CString strSectionName;

    int nFileCount = GetFileItemCount();
    for (int nIdx = 0; nIdx < nFileCount; ++nIdx)
    {
        strSectionName.Format(L"%s%d", CFM_SECTION_PREFIX, m_FileArray.GetAt(nIdx).m_nID);

        WriteFileListSection(strSectionName);

        WriteFileListDataInt(strSectionName, CFM_KEY_ID, m_FileArray.GetAt(nIdx).m_nID);
        WriteFileListDataString(strSectionName, CFM_KEY_FOLDER_NAME, m_FileArray.GetAt(nIdx).m_strFileName);
        WriteFileListDataString(strSectionName, CFM_KEY_FOLDER_PATH, m_FileArray.GetAt(nIdx).m_strFilePath);
        WriteFileListDataInt(strSectionName, CFM_KEY_HIDDEN, m_FileArray.GetAt(nIdx).m_bHidden);
    }
}


UINT CFileManager::GetFileItemCount()
{
    return m_FileArray.GetCount();
}

CString CFileManager::GetFileName( CString strFilePath )
{
    CString strFileName = L"";

    int nPos = strFilePath.ReverseFind(L'\\');

    strFileName = strFilePath.Right(strFilePath.GetLength() - nPos-1);

    return strFileName;
}

void CFileManager::RefreshFileItemIndex()
{
    int nCount = m_FileArray.GetCount();
    int nIdx;

    for (nIdx  = 0; nIdx < nCount; ++nIdx)
    {
        if (m_FileArray.GetAt(nIdx).m_nID  != (nIdx + 1))
        {
            m_FileArray.GetAt(nIdx).m_nID = nIdx +1;
        }
    }
}

CString CFileManager::GetSystemDir()
{
    CString strSystemPath;
    GetWindowsDirectory(strSystemPath.GetBuffer(MAX_PATH+1), MAX_PATH);
    strSystemPath.ReleaseBuffer();

    return strSystemPath;
}

BOOL CFileManager::WriteFileListSection(LPCTSTR strSectionName )
{
    WritePrivateProfileSection(strSectionName, NULL, m_strFileListFilePath);
    return TRUE;
}

BOOL CFileManager::WriteFileListDataString(LPCTSTR strSectionName, LPCTSTR strKey, LPCTSTR strValue )
{
    WritePrivateProfileString(strSectionName, strKey, strValue, m_strFileListFilePath);
    return TRUE;
}

BOOL CFileManager::WriteFileListDataInt(LPCTSTR strSectionName, LPCTSTR strKey, UINT nValue )
{
    CString strValue;
    _itow_s(nValue, strValue.GetBuffer(100), 100, 10);

    WritePrivateProfileString(strSectionName, strKey, strValue, m_strFileListFilePath);

    strValue.ReleaseBuffer();

    return TRUE;
}

void CFileManager::ReloadFileList()
{
    m_FileArray.RemoveAll();
    ReadFileList();
}

UINT CFileManager::GetFileListDataInt( LPCTSTR strSectionName, LPCTSTR strKey )
{
    UINT nValue = GetPrivateProfileInt(strSectionName, strKey, 0, m_strFileListFilePath);

    return nValue;
}

CString CFileManager::GetFileListDataString( LPCTSTR strSectionName, LPCTSTR strKey )
{
    CString strValue;
    
    GetPrivateProfileString(strSectionName, strKey, NULL, strValue.GetBuffer(MAX_PATH), MAX_PATH, m_strFileListFilePath);

    strValue.ReleaseBuffer();

    return strValue;
}

CString CFileManager::GetItemFileName( UINT nIndex )
{
    return m_FileArray.GetAt(nIndex).m_strFileName;
}

CString CFileManager::GetItemFilePath( UINT nIndex )
{
    return m_FileArray.GetAt(nIndex).m_strFilePath;
}

UINT CFileManager::GetItemFileId( UINT nIndex )
{
    return m_FileArray.GetAt(nIndex).m_nID;
}

UINT CFileManager::GetItemFileState( UINT nIndex )
{
    return m_FileArray.GetAt(nIndex).m_bHidden;
}

void CFileManager::Initialize()
{
    //m_strFileListFilePath = g_ConfigManager.GetSystemDir() + L"\\FList.dat";

    m_strFileListFilePath = GetSystemDir();
    m_strFileListFilePath += "\\AFL.dat";

    //MessageBox(NULL,m_strFileListFilePath, NULL, MB_OK);
    ReadFileList();
}
