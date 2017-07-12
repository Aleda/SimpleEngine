#pragma once

#include "Singleton.h"
#include "Event/Event.h"

/*
管理输入 包括：鼠标移动，滚动 键盘按键等
*/
struct MousePos
{
	double x,y;
	MousePos(double x,double y) { this->x = x ; this->y = y ;}
};

typedef void (*MouseButtonCallbackPtr)(GLFWwindow* window, int button, int action, int mods);

class InputManager final : public Singleton<InputManager>
{
typedef void (*KeyCallbackPtr)(GLFWwindow*,int,int,int,int);


public:
	static void KeyCallbackFunc(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void ScrollCallbackFunc(GLFWwindow* window, double xoffset, double yoffset);
	static void MouseCallbackFunc(GLFWwindow* window, double xpos, double ypos);
 	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	friend Singleton<InputManager>;

	void Init(GLFWwindow* window);
	
    void SetKeyCallback();
    void SetScrollCallback();
    void SetMouseCallback();
    void SetMouseButtonCallback();

    bool IsKeyDown(int keyCode) { return mKeys[keyCode]; }
    bool IsKeyUp(int keyCode) { return mKeys[keyCode]; }

    double GetMouseX() { return mMousePos.x; }
    double GetMouseY() { return mMousePos.y; }

    void SetMouseEvent(IMouseEvent* event){ m_IMouseEvent = event; }


private:
	InputManager();
	~InputManager();

	GLFWwindow* m_pWindow;

	static bool	mKeys[1024];

	static bool	mFirstMouse;

	static MousePos mMousePos;

	static IMouseEvent * m_IMouseEvent;

};

