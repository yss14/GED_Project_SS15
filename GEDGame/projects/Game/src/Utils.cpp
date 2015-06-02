#pragma once
#include "Utils.h"

namespace Utils{
	std::wstring s2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		std::wstring r(len, L'\0');
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
		return r;
	}

	const std::wstring buildRessourcePath(std::string s){
		std::wstring outDir(TARGET_DIRECTORY);
		outDir += L"resources\\";
		std::wstring tmp = Utils::s2ws(s);
		outDir += tmp;

		const std::wstring exportString = outDir;

		return exportString;
	}
}