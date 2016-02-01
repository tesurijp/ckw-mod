/*-----------------------------------------------------------------------------
 * File: winmng.cpp
 *-----------------------------------------------------------------------------
 * Copyright (c) 2004-2005  Kazuo Ishii <k-ishii@wb4.so-net.ne.jp>
 *				- original version
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *---------------------------------------------------------------------------*/

#include "ckw.h"
#include "winmng.h"

constexpr LPCTSTR TT_FILEMAPNAME = L"__ckw_window_memfile";
HANDLE winmng::m_hMap = 0;
winmng::winList *winmng::m_winList = nullptr;

winmng::winmng() : m_win(nullptr)
{
	m_hMap = ::CreateFileMapping((HANDLE)-1, nullptr, PAGE_READWRITE,
					0, sizeof(winList), TT_FILEMAPNAME);
	bool firstCreate = false;
	if (m_hMap != nullptr) {
		firstCreate = (GetLastError() != ERROR_ALREADY_EXISTS);
		m_winList = static_cast<winList*>(MapViewOfFile(m_hMap,FILE_MAP_WRITE,0,0,0));
	}
	m_winList = static_cast<winList*>(MapViewOfFile(m_hMap,FILE_MAP_WRITE,0,0,0));

	if (firstCreate == true) {
		m_winList->count = 0;
	}
}

winmng::~winmng()
{
	unregister_window();

	if (m_winList != nullptr) {
		::UnmapViewOfFile(static_cast<LPCVOID>(m_winList));
	}
	if (m_hMap) {
		::CloseHandle(m_hMap);
	}
}

bool winmng::register_window( HWND _win )
{
	if (m_winList->count >= winmng::MAXWIN) {
		return false;
	}

	if (_win != nullptr) {
		int i = 0;
		while (i < m_winList->count) {
			if (m_winList->list[i] == _win) {
				return false;
			}
			i++;
		}

		m_win = _win;
		m_winList->list[m_winList->count] = _win;
		m_winList->count++;

		return true;
	}

	return true;
}

bool winmng::unregister_window()
{
	if (m_win == nullptr) {
		return false;
	}

	int i = 0;
	while ((i<m_winList->count) && (m_winList->list[i]!=m_win)) {
		i++;
	}
	if (m_winList->list[i]!=m_win) {
		return false;
	}
	for (int j=i ; j<m_winList->count-1 ; j++) {
		m_winList->list[j] = m_winList->list[j+1];
	}
	if (m_winList->count>0) {
		m_winList->count--;
	}

	m_win = nullptr;
	return true;
}

void winmng::select_window(int _winid)
{
	if ((_winid >= 0) && (_winid < m_winList->count)) {
		if (::IsIconic(m_winList->list[_winid])) {
			::ShowWindow(m_winList->list[_winid],SW_RESTORE);
		}
		else {
			::ShowWindow(m_winList->list[_winid],SW_SHOW);
		}
		::SetForegroundWindow(m_winList->list[_winid]);
	}
}

void winmng::select_next_window(int _next)
{
	if (m_win == nullptr || m_winList->count <= 1) {
		return;
	}

	int i = 0;
	while ((i < m_winList->count) && (m_winList->list[i] != m_win)) {
		i++;
	}

	if (m_winList->list[i] != m_win) {
		return;
	}

	do {
		i += _next;
		if (i >= m_winList->count) {
			i = 0;
		}
		else if (i < 0) {
			i = m_winList->count-1;
		}

		if (m_winList->list[i] == m_win) {
			break;
		}
	} while (!IsWindowVisible(m_winList->list[i]));

	select_window(i);
}
