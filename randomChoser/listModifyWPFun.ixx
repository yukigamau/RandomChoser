module;
#include <Windows.h>
export module listModifyWPFun;

export LRESULT listModifyOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT listModifyOnCtlColorEdit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT listModifyOnCtlColorStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);