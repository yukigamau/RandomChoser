module;
#include <Windows.h>
export module window:windowRestart;

export namespace window
{
    void restart()
    {
        wchar_t path[MAX_PATH];
        GetModuleFileName(nullptr, path, MAX_PATH);

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        bool ifCreateProcess = CreateProcess(
            path,       // 当前程序路径
            nullptr,    // 参数（可自定义）
            nullptr, nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi);

        if (!ifCreateProcess)
        {
            MessageBox(nullptr, L"重启应用失败", L"错误", MB_ICONERROR);
            return;
        }

        // 新进程启动成功，关闭句柄
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        // 关闭当前窗口（走正常流程）
        PostMessage(GetActiveWindow(), WM_CLOSE, 0, 0);
    }
}