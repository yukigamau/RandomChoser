import window;

void window::WindowPages::ini(HINSTANCE hInstance)
{
	iniDpi();
	this->hInstance = hInstance;
	style.getWindowStyle();

	listModify.process = window::listModifyWP;
}