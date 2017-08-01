#include "stdafx.h"
#include "EventEngine.h"
#include <vector>

using namespace std;

EventEngine::EventEngine(DWORD input, DWORD output)
	: _console(GetStdHandle(input)), _graphics(output)
{

	GetConsoleMode(_console, &_consoleMode);
	SetConsoleMode(_console, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
	SetConsoleTitle(L"Final Project 2017");
	
}

void EventEngine::run(Control &c)
{
	int lock = 0;
	Focused::instance()->setGraph(_graphics);

	/* checking if the main panel is inside the Console 
	* if no- change the console size 
	*/
	for (bool redraw = true;;)
	{
		if (redraw)
		{
			//_graphics.setBackground(c.getBackground());
			//_graphics.setForeground(c.getForeground());
			_graphics.clearScreen();
			_graphics.setCursorVisibility(false);

			for (size_t p = 0; p < 5; ++p)
				c.draw(_graphics, 0, 0 , p);
			redraw = false;
		}
		if (lock == 0) {
			if (Control::getFocus()) {
				_graphics.setCursorVisibility(true);
				Control::setFocus(*Control::getFocus());
			}
			lock = 1;
		}
		INPUT_RECORD record;
		DWORD count;
		ReadConsoleInput(_console, &record, 1, &count);
		switch (record.EventType)
		{
		case KEY_EVENT:
		{
			auto f = Control::getFocus();
			if (f != nullptr && record.Event.KeyEvent.bKeyDown)
			{
				auto code = record.Event.KeyEvent.wVirtualKeyCode;
				auto chr = record.Event.KeyEvent.uChar.AsciiChar;
				if (code == VK_TAB) {
					moveFocus(c, f);
					redraw = false;
					//lock = 0;
					f->restCursor();
				}
				else if (code == VK_RIGHT) {
					f->keyDown(code, chr);
					redraw = false;
				}
				else if (code == VK_LEFT) {
					f->keyDown(code, chr);
					redraw = false;
				}
				else if (code == VK_BACK) {
					f->keyDown(code, chr);
					redraw = true;
					lock = 0;
				}
				else if (code == VK_DOWN || code == VK_UP) {
				}
				else {
					f->keyDown(code, chr);
					redraw = true;
					lock = 0;
				}

			}
			break;
		}
		case MOUSE_EVENT:
		{
			auto button = record.Event.MouseEvent.dwButtonState;
			auto coord = record.Event.MouseEvent.dwMousePosition;
			auto x = coord.X - c.getLeft();
			auto y = coord.Y - c.getTop();
			if (button == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				c.mousePressed(x, y, button == FROM_LEFT_1ST_BUTTON_PRESSED);
				redraw = true;
				lock = 0;
			}
			else if (button == VK_MBUTTON) {
				redraw = true;

			}
			else {
				redraw = false;
			}
			break;
		}
		default:
			break;
		}
	}
}

EventEngine::~EventEngine()
{
	SetConsoleMode(_console, _consoleMode);
}

void EventEngine::moveFocus(Control &main, Control *focused)
{
	vector<Control*> controls;
	focused->getAllControls(&controls);
		if(controls.size()==0)
			main.getAllControls(&controls);
	auto it = find(controls.begin(), controls.end(), focused);
	do
		if (++it == controls.end())
			it = controls.begin();
	while (!(*it)->canGetFocus());
	Control::setFocus(**it);
}