module;
#include <Windows.h>
export module passwordWPFun;

export LRESULT passwordOnCommand		(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT passwordOnCreate			(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT passwordOnCtlColorBtn	(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT passwordOnCtlColorEdit	(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT passwordOnCtlColorStatic	(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);