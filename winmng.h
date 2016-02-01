#pragma once

class winmng {
public:
	winmng();
	~winmng();
	bool register_window(HWND _win);
	bool unregister_window();
	void select_next_window(int _next);


private:
	void select_window(int _winid);

	/* shared memory */
	static constexpr int MAXWIN = 256;
	struct winList {
		HWND list[MAXWIN];
		int count;
	};

	HWND m_win;
	static HANDLE m_hMap;
	static winList *m_winList;
};

