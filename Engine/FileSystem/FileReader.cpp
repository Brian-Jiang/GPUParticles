
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>


// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Utils/StringOps.h"

// Sibling/Children includes
#include "FileReader.h"
	
FileReader::FileReader(const char *filename)
	#if !PE_USE_C_STYLE_FILE_READ
	: m_file(filename)
	#endif
{
	bool open = false;
	#if PE_USE_C_STYLE_FILE_READ
		fr = fopen(filename, "rt");
		open = fr != NULL;
	#else
		open = m_file.is_open();
	#endif
	if (!open)
	{
		PEINFO("PE::Error:: Failed to open file: %s\n", filename);
    }
	else
	{
		PEINFO("PE::Progress:: Opened file: %s\n", filename);
	}

	assert(open);
}

FileReader::~FileReader()
{
	#if PE_USE_C_STYLE_FILE_READ
		fclose(fr);
	#else
		m_file.close();
	#endif
}

void FileReader::readIntoBuffer(char *&pdata, PrimitiveTypes::UInt32 &size)
{
	#if PE_USE_C_STYLE_FILE_READ
		if(fseek(fr,0,SEEK_END)!=0) 
			return;
		size = ftell(fr);
		fseek(fr,0,0);

		pdata = (char *)malloc(size * 2); // allow space for "//n"

		size_t read = fread(pdata,1,size,fr);
		pdata[read++] = '\n';
		size = read;
	#else
		char str[1024];

		m_file.seekg(0, std::ios_base::end);
		size = (PrimitiveTypes::UInt32)(m_file.tellg());
		pdata = (char *)malloc(size * 2); // allow space for "//n"
		m_file.seekg(0);
		int curSize = 0;
		char *start = (char *)(pdata);

		while (!m_file.eof())
		{
			m_file.getline(str, 1024);
			int got = (int)(m_file.gcount());
			// check flag for eol assert(m_file.g
			if (got != 0)
			{
				for (int i = 0; i < got; i++)
				{
					if (str[i] != '\\' )
					{
						start[curSize++] = str[i];
					}
					else
					{
						start[curSize++] = '\\';
						start[curSize++] = '\\';
					}
				}
				//memcpy(&start[curSize], str, got);
				//curSize += got;
			
				// add "//n" instead of "/0"
				start[curSize-1] = '\\';
				start[curSize++] = 'n';
			}
			else
			{
				start[curSize++] = '\\';
				start[curSize++] = 'n';
			}
		}
		size = curSize;
		start[curSize++] = '\0'; // eos

		//m_file.read((char *)(pdata), size);
	#endif
}

void FileReader::nextInt32(PrimitiveTypes::Int32 &dest)
{
	#if PE_USE_C_STYLE_FILE_READ
		fscanf(fr, "%d", &dest);
	#else
		m_file >> dest;
	#endif
}

void FileReader::nextFloat32(PrimitiveTypes::Float32 &dest)
{
	#if PE_USE_C_STYLE_FILE_READ
		fscanf(fr, "%f", &dest);
	#else
		m_file >> dest;
	#endif
}

bool FileReader::nextNonEmptyLine(char *dest, PrimitiveTypes::Int32 max)
{
	#if PE_USE_C_STYLE_FILE_READ
		if (fgets(dest, max, fr) == NULL)
			return false;
	#else
		if (m_file.eof())
		{
			return false; // cant read anymore
		}

		m_file.getline(dest, max);
	#endif

	
	int len = StringOps::length(dest);

	#if APIABSTRACTION_PS3 || PE_USE_C_STYLE_FILE_READ
		//check for Caret Return character at the end
		while (len && (dest[len-1] == '\x0D' || dest[len-1] == '\x0A'))
		{
			dest[len-1] = '\0';
			len--;
		}
	#endif

	if (len == 0 || (StringOps::strcmp(dest, " ") == 0)) // read until get non empty line
	{
		return nextNonEmptyLine(dest, max); // search deeper
	}
	else
	{
		return true; // good line
	}
}



//-----------------------------------------------------------------------------
// uint32_t getFileLen(char *filename)
// Description: 
// returns the length of the file in bytes
// Returns: 
// 0 on length of file
// Notes:
//-----------------------------------------------------------------------------
unsigned int FileReader::getFileLen(char *filename)
{
	FILE* fp;
	unsigned int fileLen;

	if((fp = fopen(filename,"rb"))==NULL)
		return 0;

	if(fseek(fp,0,SEEK_END)!=0) 
		return 0;

	fileLen = ftell(fp);
	fclose(fp);
	return fileLen;
}

//-----------------------------------------------------------------------------
// long LoadFile(char *filename,unsigned int bytes,unsigned char *buffer)
// Description: 
// loads bytes amount of data into buffer from filename
// Returns: 
// 0 on failure or the number of bytes read
// Notes:
//-----------------------------------------------------------------------------
long //size of file in bytes
	 FileReader::LoadFile
	(
	char *filename,					//name of file to read	
	unsigned int bytes,				//number of bytes to read
	unsigned char *buffer			//load the file here
	)
{
	FILE* fp;

	if((fp = fopen(filename,"rb"))==NULL)
		return 0;

	if(fread(buffer,1,bytes,fp)!= bytes)
		return 0;

	fclose(fp);
	return bytes;
}


//-----------------------------------------------------------------------------
