import window;

void window::WindowPages::ini(HINSTANCE hInstance)
{
	iniDpi();
	this->hInstance = hInstance;
	style.getWindowStyle();

	// 配置页面的过程处理函数
	listModify.process = window::listModifyWP;
}