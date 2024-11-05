// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"



#include<cstdlib>
#include<iostream>
#include<string.h>
#include<fstream>
#if PE_PLAT_IS_WIN32
#include <io.h>

#include "String.h"
#include "Dir.h"



// TODO this might be Win32 specific. for ps3, use dirent.h and this code 
/*
void listFile()
{
	DIR *pDIR;
	struct dirent *entry;
	if (pDIR=opendir("."))
	{
		while (entry = readdir(pDIR))
		{
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				PEINFO(entry->d_name); //<< "\n";
			}
		}
		closedir(pDIR);
	}
}
*/

void listFile(PE::GameContext &context, PE::MemoryArena arena, PEString &path)
{
	PEString pathTmp(context, arena);
	pathTmp.append(path);
	pathTmp.append("*.luah");

	struct _finddata_t c_file;
	long hFile;

	// first look for *.luah files

	
	if ( (hFile = _findfirst(pathTmp.m_data.getFirstPtr(), &c_file)) == -1L )
	{
		//PEINFO("No *.luah files in current directory");
	}
	else
	{
		do
		{
			char buf[255];
			sprintf_s<255>(buf, "%s\n", c_file.name);
			PEINFOSTR(buf);
		} while ( _findnext(hFile, &c_file) == 0 );
		_findclose(hFile);
	}

	// parse sub folders
	pathTmp.decreaseSize(5); // strip ".luah"

	//PEINFO(pathTmp.m_data.getFirstPtr());
	
	struct _finddata_t dir_file;
	
	if ( (hFile = _findfirst(pathTmp.m_data.getFirstPtr(), &dir_file)) == -1L )
	{
		//PEINFO("No * files in current directory");
	}
	else
	{
		do
		{
			if (StringOps::strcmp(dir_file.name, ".") != 0 &&
				StringOps::strcmp(dir_file.name, "..") != 0 &&
				StringOps::strcmp(dir_file.name, ".svn"))
			{
				PEString t(context, arena);
				t.append(path);
				t.append(dir_file.name);


				//PEINFO(t.m_data.getFirstPtr());
				if (GetFileAttributesA(t.m_data.getFirstPtr()) & FILE_ATTRIBUTE_DIRECTORY)
				{
					//PEINFO("Directory: %s\n", dir_file.name);
					t.append("\\");
					listFile(context, arena, t);
				}
				t.m_data.reset(0);
			}
		} while ( _findnext(hFile, &dir_file) == 0 );
		_findclose(hFile);
	}

	pathTmp.m_data.reset(0);
}
#endif

