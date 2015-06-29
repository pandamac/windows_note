#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

int main()
{
	printf("Locale is %s\n", setlocale(LC_ALL, NULL));
	setlocale(LC_ALL,"");
	printf("Locale is %s\n", setlocale(LC_ALL, NULL));

	char c = 'A';
	printf("%c\n", c);

	wchar_t c2 = L'中';
	wprintf(L"%c\n",c2);

	char s[] = "hello";
	char s3[] = "中华人民共和国";
	wchar_t s2[] = L"中华人民共和国";
	printf("%s\n", s);
	wprintf(L"%s\n", s2);

	printf("%d\n", strlen(s));
	printf("%d\n", wcslen(s2));
	printf("%d\n", strlen(s3));

	system("pause");
	return 0;
}