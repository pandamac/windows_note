#include <iostream>
#include <map>
#include <string>
using namespace std;

map<char, int> m;
char alphabet[65] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U',
	'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
	'c', 'd', 'e', 'f', 'g', 'h', 'i',
	'j', 'k', 'l', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w',
	'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+',
	'/', '='
};
void InitMap()
{
	int i;
	for (i = 0; i != 26 ; ++i)
	{
		m.insert(make_pair('A' + i, i));
	}
	for (i = 26; i != 52; ++i)
	{
		m.insert(make_pair('a' + i - 26, i));
	}
	for (i = 52; i != 62; ++i)
	{
		m.insert(make_pair('0' + i - 52, i));
	}
	m.insert(make_pair('+', 62));
	m.insert(make_pair('/', 63));
	m.insert(make_pair('=', 64));
}
void DBase64(const char* input, string &output)
{
	int inLen = strlen(input);
	int nCount = inLen / 4;
	unsigned char temp[3] = {0};
	int i;
	for (i = 0; i != nCount - 1; ++i)
	{
		temp[0] = (m[input[4 * i]] << 0x02) | (m[input[4 * i + 1]] >> 0x04);
		temp[1] = (m[input[4 * i + 1]] << 0x04) | (m[input[4 * i + 2]] >> 0x02);
		temp[2] = (m[input[4 * i + 2]] << 0x06) | (m[input[4 * i + 3]]);

		for (int j = 0; j != 3; ++j)
			output += temp[j];
		memset(temp, 0, 3);
	}
	if (input[inLen -2] != '=' && input[inLen - 1] != '=')
	{
		temp[0] = (m[input[4 * i]] << 0x02) | (m[input[4 * i + 1]] >> 0x04);
		temp[1] = (m[input[4 * i + 1]] << 0x04) | (m[input[4 * i + 2]] >> 0x02);
		temp[2] = (m[input[4 * i + 2]] << 0x06) | (m[input[4 * i + 3]]);

		for (int j = 0; j != 3; ++j)
			output += temp[j];
		memset(temp, 0, 3);
	}
	else
	{
		if (input[inLen - 1]  == '=' && input[inLen - 2] != '=')
		{
			temp[0] = (m[input[4 * i]] << 0x02) | (m[input[4 * i + 1]] >> 0x04);
			output += temp[0];
			temp[1] = (m[input[4 * i + 1]] << 0x04) | (m[input[4 * i + 2]] >> 0x02);
			output += temp[1];
		}
		else
		{
			temp[0] = (m[input[4 * i]] << 0x02) | (m[input[4 * i + 1]] >> 0x04);
			output += temp[0];
		}
	}
}
void EBase64(const char* input, string &output)
{
	int inLen = strlen(input);
	int nCount = inLen / 3;
	int nRemain = inLen % 3;
	unsigned char temp[4] = {0};
	int i;
	for (i = 0; i != nCount; ++i)
	{
		temp[0] = input[3 * i] & 0xFC;
		temp[1] = (input[3 * i] << 0x06) | ((input[3 * i + 1] & 0xF0) >> 0x02);
		temp[2] = (input[3 * i + 1] << 0x04) | ((input[3 * i + 2] & 0xC0) >> 0x04);
		temp[3] = (input[3 * i + 2] & 0x3F) << 0x02;
	//	temp[0] = temp[3] >> 0x02;
		for (int j = 0; j != 4; ++j)
		{
			temp[j] = temp[j] >> 0x02;
			output += alphabet[temp[j]] ;
		}
		memset(temp, 0, 4);
	}
	if (nRemain == 2)
	{
		temp[0] = input[3 * i] & 0xFC;
		temp[0] = temp[0] >> 0x02;
		temp[1] = (input[3 * i] << 0x06) | ((input[3 * i + 1] & 0xF0) >> 0x02);
		temp[1] = temp[1] >> 0x02;
		temp[2] = input[3 * i + 1] << 0x04;
		temp[2] = temp[2] >> 0x02;
		temp[3] = 64;
		for (int j = 0; j != 4; ++j)
		{
			output += alphabet[temp[j]] ;
		}
		memset(temp, 0, 4);
	}
	if (nRemain == 1)
	{
		temp[0] = input[3 * i] & 0xFC;
		temp[0] = temp[0] >> 0x02;
		temp[1] = (input[3 * i] << 0x06);
		temp[1] = temp[1] >>0x02;
		temp[2] = 64;
		temp[3] = 64;
		for (int j = 0; j != 4; ++j)
		{
			output += alphabet[temp[j]] ;
		}
	}
}

int main()
{

	InitMap();
	//cout<<"hello,world!I am"<<8<<"today"<<endl;
// 	string str;
// 	string str1;
// 	str = "abcde";
// 	string strRes;
// 	EBase64(str.c_str(), strRes);	
//	cout << strRes << endl;
char str[] = ""
	DBase64("", str1);
	cout << str1 << endl;
	return 0;
}