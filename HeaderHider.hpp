#ifndef HEADERHIDER_HPP
#define HEADERHIDER_HPP

#include <memory>
#include <Windows.h>

class HeaderHider {
public:
	HeaderHider(DWORD baseAddr);
	HeaderHider(HMODULE baseAddr);
	bool hide(void);
	bool restore(void);

private:
	LONG m_lfanew{ 0 };
	DWORD m_baseAddr{ 0 };
	std::shared_ptr<IMAGE_DOS_HEADER> m_idh;
	std::shared_ptr<IMAGE_NT_HEADERS> m_inh;
};

#endif
