#include "PEString.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"

char PEString::s_buf[PEString::BUF_SIZE];


const char* PEString::generateScriptPathname(PE::GameContext &context, const char *filename, const char *module, const char *folder, char *out_path, int len)
{
	if (module != NULL && StringOps::length(module) > 0)
	{
#if APIABSTRACTION_IOS || APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
		StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "Code/", out_path, len);
		StringOps::concat(out_path, module, out_path, len);
		StringOps::concat(out_path, "/", out_path, len);
		StringOps::concat(out_path, folder, out_path, len);
		StringOps::concat(out_path, "/", out_path, len);
#else
		StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "Code\\", out_path, len);
		StringOps::concat(out_path, module, out_path, len);
		StringOps::concat(out_path, "\\", out_path, len);
		StringOps::concat(out_path, folder, out_path, len);
		StringOps::concat(out_path, "\\", out_path, len);

#endif
	}
	else
	{
		// if package is not provided default to Default package
#if APIABSTRACTION_IOS
		StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "Code/PrimeEngine/", out_path, len);
		StringOps::concat(out_path, folder, out_path, len);
		StringOps::concat(out_path, "/", out_path, len);
#else
		StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "Code\\PrimeEngine\\", out_path, len);
		StringOps::concat(out_path, folder, out_path, len);
		StringOps::concat(out_path, "\\", out_path, len);
#endif
	}

	StringOps::concat(out_path, filename, out_path, len);
	return out_path;
}

void PEString::generatePathname(PE::GameContext &context, const char *filename, const char *package, const char *assetType, char *out_path, int len)
{
	if (package != NULL && StringOps::length(package) > 0)
	{
#if APIABSTRACTION_IOS || APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
        StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "AssetsOut/", out_path, len);
        StringOps::concat(out_path, package, out_path, len);
        StringOps::concat(out_path, "/", out_path, len);
        StringOps::concat(out_path, assetType, out_path, len);
        StringOps::concat(out_path, "/", out_path, len);
#else
        StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "AssetsOut\\", out_path, len);
        StringOps::concat(out_path, package, out_path, len);
        StringOps::concat(out_path, "\\", out_path, len);
        StringOps::concat(out_path, assetType, out_path, len);
        StringOps::concat(out_path, "\\", out_path, len);
        
#endif
	}
	else
	{
		// if package is not provided default to Default package
#if APIABSTRACTION_IOS
		StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "AssetsOut/Default/", out_path, len);
        StringOps::concat(out_path, assetType, out_path, len);
        StringOps::concat(out_path, "/", out_path, len);
#else
		StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "AssetsOut\\Default\\", out_path, len);
        StringOps::concat(out_path, assetType, out_path, len);
        StringOps::concat(out_path, "\\", out_path, len);
#endif
	}
    
	StringOps::concat(out_path, filename, out_path, len);
}

void PEString::set(const char *zstr)
{
	PrimitiveTypes::UInt32 l = StringOps::length(zstr);
	m_data.reset(l + 1);
	StringOps::writeToString(zstr, m_data.m_dataHandle.getObject<char>(), l + 1);
	m_data.m_size = l + 1;
}

void PEString::set(const char *str, int sizeNoZero)
{
    m_data.reset(sizeNoZero+1);
	memmove(m_data.m_dataHandle.getObject(), str, sizeNoZero);
    *(m_data.m_dataHandle.getObject<char>() + sizeNoZero) = '\0';
	m_data.m_size = sizeNoZero+1;
}


void PEString::append(PrimitiveTypes::Int32 val)
{
	char str[256];
	PrimitiveTypes::UInt32 l = StringOps::length(m_data.m_dataHandle.getObject<char>());
	StringOps::writeToString(m_data.m_dataHandle.getObject<char>(), str, l+1);
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA
	sprintf_s(str, 256, "%s%d", str, val);
#else
	sprintf(str, "%s%d", str, val);
#endif
	set(str);
}

void PEString::append(PrimitiveTypes::Float32 val)
{
	char str[256];
	PrimitiveTypes::UInt32 l = StringOps::length(m_data.m_dataHandle.getObject<char>());
	StringOps::writeToString(m_data.m_dataHandle.getObject<char>(), str, l+1);
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA
	sprintf_s(str, 256,"%s%f", str, val);
#else
	sprintf(str, "%s%f", str, val);
#endif

	set(str);
}

void PEString::append(const char *val)
{
	char str[256];
	PrimitiveTypes::UInt32 l = StringOps::length(m_data.m_dataHandle.getObject<char>());
	StringOps::writeToString(m_data.m_dataHandle.getObject<char>(), str, l+1);

#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA
	sprintf_s(str, 256, "%s%s", str, val);
#else
	sprintf(str, "%s%s", str, val);
#endif

	set(str);
}

void PEString::append(PEString &val)
{
	PrimitiveTypes::UInt32 l = StringOps::length(m_data.m_dataHandle.getObject<char>());
	StringOps::writeToString(m_data.m_dataHandle.getObject<char>(), PEString::s_buf, l+1);

	PrimitiveTypes::UInt32 l2 = StringOps::length(val.m_data.m_dataHandle.getObject<char>());
	StringOps::writeToString(val.m_data.m_dataHandle.getObject<char>(), PEString::s_buf + l, l2+1);

	set(PEString::s_buf);
}
