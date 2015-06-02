#include <string>
#include <windows.h>
#include <stdio.h>
#include <wchar.h>

namespace Utils{
	std::wstring s2ws(const std::string& s);
	const std::wstring buildRessourcePath(std::string s);
}
