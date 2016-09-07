#include "HeaderHider.hpp"

HeaderHider::HeaderHider(DWORD baseAddr)
	: m_baseAddr{ baseAddr }, m_idh{ new IMAGE_DOS_HEADER }, m_inh{ new IMAGE_NT_HEADERS } {
}

HeaderHider::HeaderHider(HMODULE baseAddr)
	: HeaderHider((DWORD)baseAddr) {

}

bool HeaderHider::hide(void) {
	PIMAGE_DOS_HEADER tmpIDH = (PIMAGE_DOS_HEADER)m_baseAddr;
	if (tmpIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	DWORD old = 0;
	
	m_lfanew = tmpIDH->e_lfanew;
	PIMAGE_NT_HEADERS tmpINH = (PIMAGE_NT_HEADERS)(m_baseAddr + m_lfanew);

	if (!VirtualProtect(tmpINH, sizeof(IMAGE_NT_HEADERS), PAGE_READWRITE, &old))	// set read/write protections on the NT headers
		return false;	
	memcpy((LPVOID)m_inh.get(), tmpINH, sizeof(IMAGE_NT_HEADERS));					// store the original headers in our member variable
	RtlSecureZeroMemory(tmpINH, sizeof(IMAGE_NT_HEADERS));							// erase the headers
	tmpINH->Signature = IMAGE_NT_SIGNATURE;											// replace the signature with a valid one
	if (!VirtualProtect(tmpINH, sizeof(IMAGE_NT_HEADERS), old, NULL))				// set the protections to what they previously were
		return false;

	if (!VirtualProtect(tmpIDH, sizeof(IMAGE_DOS_HEADER), PAGE_READWRITE, &old))	// set read/write protections on the DOS headers
		return false;
	memcpy((LPVOID)m_idh.get(), tmpIDH, sizeof(IMAGE_DOS_HEADER));					// store the original headers in our member variable
	RtlSecureZeroMemory(tmpIDH, sizeof(IMAGE_DOS_HEADER));							// erase the headers
	tmpIDH->e_lfanew = m_lfanew;													// restore the offset to the PE header
	tmpIDH->e_magic = IMAGE_DOS_SIGNATURE;											// replace the signature with a valid one
	if (!VirtualProtect(tmpIDH, sizeof(IMAGE_DOS_HEADER), old, NULL))				// the the protections to what they previously were
		return false;
	return true;
}

bool HeaderHider::restore(void) {
	DWORD old;
	PIMAGE_DOS_HEADER tmpIDH = (PIMAGE_DOS_HEADER)m_baseAddr;
	PIMAGE_NT_HEADERS tmpINH = (PIMAGE_NT_HEADERS)((DWORD)m_baseAddr + m_lfanew);

	if (!VirtualProtect(tmpINH, sizeof(IMAGE_NT_HEADERS), PAGE_READWRITE, &old))
		return false;
	memcpy(tmpINH, m_inh.get(), sizeof(IMAGE_NT_HEADERS));
	if (!VirtualProtect(tmpINH, sizeof(IMAGE_NT_HEADERS), old, NULL))
		return false;

	if (!VirtualProtect(tmpIDH, sizeof(IMAGE_DOS_HEADER), PAGE_READWRITE, &old))
		return false;
	memcpy(tmpIDH, m_idh.get(), sizeof(IMAGE_DOS_HEADER));
	if (!VirtualProtect(tmpIDH, sizeof(IMAGE_DOS_HEADER), old, NULL))
		return false;

	return true;
}
