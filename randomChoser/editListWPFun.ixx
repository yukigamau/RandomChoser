module;
#include <Windows.h>
export module editListWPFun;

export LRESULT editListOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT editListOnCtlColorButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT editListOnCtlColorEdit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT editListOnCtlColorStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);