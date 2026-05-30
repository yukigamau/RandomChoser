module;
#include <Windows.h>
export module chooseWPFun;

export LRESULT CALLBACK chooseOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK chooseOnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);