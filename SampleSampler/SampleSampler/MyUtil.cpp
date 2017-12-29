#include "MyUtil.h"

void CreateDir(const char *dir)
{
	int m = 0, n;
	string str1, str2;

	str1 = dir;
	str2 = str1.substr( 0, 2 );
	str1 = str1.substr( 3, str1.size() );

	while( m >= 0 )
	{
		m = str1.find('\\');

		str2 += '\\' + str1.substr( 0, m );    
		n = _access( str2.c_str(), 0 ); //判断该目录是否存在
		if( n == -1 )
		{
			_mkdir( str2.c_str() );     //创建目录
		}

		str1 = str1.substr( m+1, str1.size() );
	}
}

void GetAllFileNames(const string &dir, vector<string> &exts, vector<string> &fileNames)
{
	WIN32_FIND_DATAA findFileData;
	HANDLE hFind;
	PVOID oldValue = NULL;

	char *name;
	string filter;
	vector<string>::iterator it;

	if( Wow64DisableWow64FsRedirection( &oldValue ) )
	{
		for( it = exts.begin(); it != exts.end(); it++ )
		{
			filter = dir + '\\' + *it;
			hFind = FindFirstFileA( filter.c_str(), &findFileData );
			if( INVALID_HANDLE_VALUE != hFind )
			{
				name = findFileData.cFileName;
				fileNames.push_back( name );
				while( 0 != FindNextFileA(hFind, &findFileData) )
				{
					name = findFileData.cFileName;
					fileNames.push_back( name );
				}	
			}
		}
		FindClose( hFind );

		Wow64RevertWow64FsRedirection( oldValue );
	}
	else
	{
		for( it = exts.begin(); it != exts.end(); it++ )
		{
			filter = dir + '\\' + *it;
			hFind = FindFirstFileA( filter.c_str(), &findFileData );
			if( INVALID_HANDLE_VALUE != hFind )
			{
				name = findFileData.cFileName;
				fileNames.push_back( name );
				while( 0 != FindNextFileA(hFind, &findFileData) )
				{
					name = findFileData.cFileName;
					fileNames.push_back( name );
				}	
			}
		}
		FindClose( hFind );	
	}
}

FILE* GetPrmsPtr(FILE *fpTop, const string &item)
{
	char lineBuffer[255];
	FILE *fp = fpTop;
	while(!feof(fp))
	{
		fscanf_s(fp, "%s", lineBuffer, 255);
		fscanf_s(fp, "%*[^\n]");
		lineBuffer[item.size()] = '\0';
		if(0 == strcmp(lineBuffer, item.c_str()))
		{
			break;
		}
	}
	return (!feof(fp)) ? fp : NULL;
}

string int2string( int a )
{
	char buffer[40];
	sprintf_s( buffer, "%d", a );
	return string( buffer );
}

void StringSplit(const string &str, vector<string> &ret)
{
	int i, m = 0;
	string s;
	ret.clear();
	for(i = 0; i < (int)str.size(); i++)
	{
		char c = str[i];
		if((' '==c) || ('\t'==c))
		{
			s = str.substr(m, i-m);
			if(s.size() > 0)
			{
				ret.push_back(s);
			}
			m = i + 1;
		}
	}
	if(m < i)
	{
		s = str.substr(m, i-m);
		ret.push_back(s);
	}
}



