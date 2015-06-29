LPCTSTR FindOneOf(
		_In_z_ LPCTSTR p1,             //xxx.exe /server
		_In_z_ LPCTSTR p2) throw()     //-/
	{
		while (p1 != NULL && *p1 != _T('\0'))
		{
			LPCTSTR p = p2;
			while (p != NULL && *p != _T('\0'))
			{
				if (*p1 == *p)
					return CharNext(p1);//×îºó·µ»Øserver
				p = CharNext(p);
			}
			p1 = CharNext(p1);
		}
		return NULL;
	}