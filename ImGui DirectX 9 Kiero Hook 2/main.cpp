#include "includes.h"
#include <Windows.h>
#include "imgui/imgui_internal.h"

// function prototypes for custom controls
bool CustomCheckbox(const char* label, bool* v);
int CustomTabBar(int selectedTab, const char* tabs[], int tabCount, float tabHeight, float tabWidth, ImU32 accentColor, ImU32 tabBg, ImU32 tabBgActive);
bool CustomCheckboxVerbal(const char* label, bool* v);
bool CustomSliderVerbal(const char* label, float* v, float min, float max, float step);

#ifdef _WIN64
#define GWL_WNDPROC GWLP_WNDPROC
#endif

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

EndScene oEndScene = NULL;
WNDPROC oWndProc;
static HWND window = NULL;
static bool showMenu = true;
static int selectedTab = 0; // 0: Main, 1: Visuals, 2: Misc, 3: Config
static bool prevShowMenu = true;
static bool centeredOnce = false;

void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(pDevice);

	// use a clean, readable font (Segoe UI or Consolas)
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f);

	// blocky, minimal
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	// solid, almost black background
	colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.0f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);
	colors[ImGuiCol_Border] = ImVec4(0.18f, 0.22f, 0.32f, 1.0f); // blue border
	colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
	colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.0f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.22f, 0.32f, 0.7f); // blue accent
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.22f, 0.32f, 1.0f); // blue accent
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.18f, 0.22f, 0.32f, 0.7f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.32f, 1.0f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.18f, 0.22f, 0.32f, 1.0f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.18f, 0.52f, 0.92f, 1.0f); // blue
	colors[ImGuiCol_SliderGrab] = ImVec4(0.18f, 0.52f, 0.92f, 0.7f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.18f, 0.52f, 0.92f, 1.0f);
	colors[ImGuiCol_Button] = ImVec4(0.13f, 0.14f, 0.17f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.52f, 0.92f, 0.7f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.52f, 0.92f, 1.0f);
	colors[ImGuiCol_Header] = ImVec4(0.18f, 0.22f, 0.32f, 0.7f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.52f, 0.92f, 0.7f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.18f, 0.52f, 0.92f, 1.0f);
	colors[ImGuiCol_Separator] = ImVec4(0.18f, 0.22f, 0.32f, 1.0f);
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.0f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.48f, 0.54f, 1.0f);
	colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.14f, 0.17f, 1.0f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.18f, 0.52f, 0.92f, 0.7f);
	colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.52f, 0.92f, 1.0f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.22f, 0.32f, 1.0f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.18f, 0.52f, 0.92f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.18f, 0.52f, 0.92f, 0.9f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.18f, 0.52f, 0.92f, 1.0f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.18f, 0.52f, 0.92f, 0.7f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.08f, 0.09f, 0.11f, 0.6f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.08f, 0.09f, 0.11f, 0.8f);

	// blocky, no rounding
	style.WindowRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.WindowBorderSize = 1.0f;
	style.FrameBorderSize = 1.0f;
	style.TabBorderSize = 1.0f;
	style.WindowPadding = ImVec2(12.0f, 12.0f);
	style.FramePadding = ImVec2(8.0f, 4.0f);
	style.ItemSpacing = ImVec2(10.0f, 8.0f);
	style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
	style.Alpha = 1.0f;
}

bool init = false;
long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (!init)
	{
		InitImGui(pDevice);
		init = true;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// if (GetAsyncKeyState(VK_INSERT) & 1) {
	// 	showMenu = !showMenu;
	// }

	// prevShowMenu = showMenu;

	// if (showMenu) {
	// 	ImGuiIO& io = ImGui::GetIO();
	// 	ImVec2 windowSize = ImVec2(540, 420);
	// 	ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
	// 	ImGuiCond posCond = centeredOnce ? ImGuiCond_Once : ImGuiCond_Always;
	// 	ImGui::SetNextWindowPos(windowPos, posCond, ImVec2(0.5f, 0.5f));
	// 	if (!centeredOnce) centeredOnce = true;
	// 	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
	// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	// 	ImGui::Begin("skibidiware.cc", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	// 	ImVec2 winPos = ImGui::GetWindowPos();
	// 	ImVec2 winSize = ImGui::GetWindowSize();
	// 	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	// 	ImU32 borderOuter = IM_COL32(30, 30, 30, 255);
	// 	ImU32 borderInner = IM_COL32(60, 60, 60, 255);
	// 	ImU32 accent = IM_COL32(0, 255, 255, 255);
	// 	ImU32 bg = IM_COL32(16, 16, 16, 255);
	// 	float border = 2.0f;
	// 	float innerOffset = 3.0f;
	// 	float margin = 12.0f;
	// 	draw_list->AddRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), borderOuter, 0.0f, 0, border);
	// 	draw_list->AddRect(ImVec2(winPos.x + innerOffset, winPos.y + innerOffset), ImVec2(winPos.x + winSize.x - innerOffset, winPos.y + winSize.y - innerOffset), borderInner, 0.0f, 0, 1.0f);
	// 	draw_list->AddRectFilled(ImVec2(winPos.x + border, winPos.y + border), ImVec2(winPos.x + winSize.x - border, winPos.y + winSize.y - border), bg, 0.0f);
	// 	float headerHeight = 32.0f;
	// 	ImGui::SetCursorPos(ImVec2(0, 0));
	// 	ImGui::InvisibleButton("##move", ImVec2(winSize.x, headerHeight));
	// 	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
	// 		ImVec2 delta = ImGui::GetIO().MouseDelta;
	// 		ImVec2 newPos = ImGui::GetWindowPos();
	// 		newPos.x += delta.x;
	// 		newPos.y += delta.y;
	// 		ImGui::SetWindowPos(newPos);
	// 	}
	// 	ImGui::SetCursorPos(ImVec2(margin, 7));
	// 	ImGui::PushFont(ImGui::GetFont());
	// 	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
	// 	ImGui::Text("skibidiware.cc");
	// 	ImGui::PopStyleColor();
	// 	ImGui::PopFont();
	// 	draw_list->AddLine(ImVec2(winPos.x + innerOffset, winPos.y + headerHeight + innerOffset), ImVec2(winPos.x + winSize.x - innerOffset, winPos.y + headerHeight + innerOffset), accent, 2.0f);
	// 	const char* tabs[] = { "Main", "Visuals", "Misc", "Config" };
	// 	int tabCount = 4;
	// 	float tabBarHeight = 28.0f;
	// 	float tabWidth = (winSize.x - 2 * innerOffset) / tabCount;
	// 	ImVec2 tabStart = ImVec2(winPos.x + innerOffset, winPos.y + headerHeight + innerOffset + 2);
	// 	for (int i = 0; i < tabCount; ++i) {
	// 		ImVec2 tabMin = ImVec2(tabStart.x + i * tabWidth, tabStart.y);
	// 		ImVec2 tabMax = ImVec2(tabMin.x + tabWidth, tabMin.y + tabBarHeight);
	// 		bool selected = (selectedTab == i);
	// 		ImU32 tabBg = selected ? IM_COL32(24, 24, 24, 255) : IM_COL32(16, 16, 16, 255);
	// 		draw_list->AddRectFilled(tabMin, tabMax, tabBg, 0.0f);
	// 		ImVec2 textSize = ImGui::CalcTextSize(tabs[i]);
	// 		ImVec2 textPos = ImVec2(tabMin.x + (tabWidth - textSize.x) * 0.5f, tabMin.y + (tabBarHeight - textSize.y) * 0.5f);
	// 		draw_list->AddText(textPos, IM_COL32(200, 200, 200, 255), tabs[i]);
	// 		if (selected) {
	// 			draw_list->AddLine(ImVec2(tabMin.x, tabMax.y - 2), ImVec2(tabMax.x, tabMax.y - 2), accent, 2.0f);
	// 		}
	// 		if (ImGui::IsMouseHoveringRect(tabMin, tabMax) && ImGui::IsMouseClicked(0)) selectedTab = i;
	// 	}
	// 	draw_list->AddLine(ImVec2(winPos.x + innerOffset, winPos.y + headerHeight + tabBarHeight + innerOffset), ImVec2(winPos.x + winSize.x - innerOffset, winPos.y + headerHeight + tabBarHeight + innerOffset), accent, 2.0f);
	// 	if (selectedTab == 0) {
	// 		float groupPadding = 16.0f;
	// 		float groupMargin = 16.0f;
	// 		float groupWidth = (winSize.x - 2 * innerOffset - groupPadding - 2 * groupMargin - 8.0f) * 0.5f;
	// 		float groupHeight = winSize.y - (headerHeight + tabBarHeight + 2 * groupMargin + 32.0f);
	// 		ImVec2 groupPosL = ImVec2(innerOffset + groupMargin, headerHeight + tabBarHeight + groupMargin + 12.0f);
	// 		ImVec2 groupPosR = ImVec2(innerOffset + groupWidth + groupPadding + groupMargin + 8.0f, headerHeight + tabBarHeight + groupMargin + 12.0f);
	// 		ImGui::SetCursorPos(groupPosL);
	// 		ImGui::BeginChild("##leftgroup", ImVec2(groupWidth, groupHeight), true, ImGuiWindowFlags_NoScrollbar);
	// 		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
	// 		ImGui::Text("Aimbot");
	// 		ImGui::Separator();
	// 		ImGui::Indent(12.0f);
	// 		static bool enabled = false;
	// 		static bool teamCheck = false;
	// 		static bool prediction = false;
	// 		static float maxDist = 1000.0f;
	// 		ImGui::Spacing();
	// 		CustomCheckboxVerbal("Enabled", &enabled);
	// 		ImGui::Spacing();
	// 		CustomCheckboxVerbal("Teamcheck", &teamCheck);
	// 		ImGui::Spacing();
	// 		CustomCheckboxVerbal("Prediction", &prediction);
	// 		ImGui::Spacing();
	// 		CustomSliderVerbal("Maximum Distance", &maxDist, 0.0f, 2000.0f, 1.0f);
	// 		ImGui::Unindent(12.0f);
	// 		ImGui::EndChild();
	// 		ImGui::SetCursorPos(groupPosR);
	// 		ImGui::BeginChild("##rightgroup", ImVec2(groupWidth, groupHeight), true, ImGuiWindowFlags_NoScrollbar);
	// 		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
	// 		ImGui::Text("Settings");
	// 		ImGui::Separator();
	// 		ImGui::Indent(12.0f);
	// 		static float predX = 15.0f;
	// 		static float predY = 15.0f;
	// 		static float smooth = 1.0f;
	// 		ImGui::Spacing();
	// 		CustomSliderVerbal("Prediction X", &predX, 0.0f, 30.0f, 1.0f);
	// 		ImGui::Spacing();
	// 		CustomSliderVerbal("Prediction Y", &predY, 0.0f, 30.0f, 1.0f);
	// 		ImGui::Spacing();
	// 		CustomSliderVerbal("Smoothness", &smooth, 1.0f, 10.0f, 1.0f);
	// 		ImGui::Unindent(12.0f);
	// 		ImGui::EndChild();
	// 	} else {
	// 		ImGui::SetCursorPos(ImVec2(margin, headerHeight + tabBarHeight + margin + 12.0f));
	// 		ImGui::Text("%s tab coming soon...", tabs[selectedTab]);
	// 	}
	// 	ImGui::End();
	// 	ImGui::PopStyleVar(3);
	// }

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return oEndScene(pDevice);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	DWORD wndProcId;
	GetWindowThreadProcessId(handle, &wndProcId);

	if (GetCurrentProcessId() != wndProcId)
		return TRUE; // skip to next window

	window = handle;
	return FALSE; // window found abort search
}

HWND GetProcessWindow()
{
	window = NULL;
	EnumWindows(EnumWindowsCallback, NULL);
	return window;
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	// initialize sdk and dump netvars before anything else
	InitializeGModSDK();
	bool attached = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			kiero::bind(42, (void**)& oEndScene, hkEndScene);
			do
				window = GetProcessWindow();
			while (window == NULL);
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
			attached = true;
		}
	} while (!attached);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}

// helper for custom checkbox
bool CustomCheckbox(const char* label, bool* v) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) return false;
	ImGuiContext& g = *GImGui;
	ImGuiID id = window->GetID(label);
	ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	float box_sz = ImGui::GetFrameHeight();
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImRect check_bb(pos, ImVec2(pos.x + box_sz, pos.y + box_sz));
	ImGui::ItemSize(check_bb, ImGui::GetStyle().FramePadding.y);
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(check_bb, id, &hovered, &held);
	if (pressed) *v = !*v;
	ImU32 col_bg = *v ? ImColor(46, 143, 255, 255) : ImColor(30, 32, 38, 255);
	ImU32 col_border = hovered ? ImColor(46, 143, 255, 255) : ImColor(60, 60, 70, 255);
	window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, col_bg, 2.0f);
	window->DrawList->AddRect(check_bb.Min, check_bb.Max, col_border, 2.0f, 0, 2.0f);
	if (*v) {
		float pad = box_sz * 0.22f;
		window->DrawList->AddLine(ImVec2(pos.x + pad, pos.y + box_sz * 0.55f), ImVec2(pos.x + box_sz * 0.45f, pos.y + box_sz - pad), IM_COL32(255,255,255,255), 2.0f);
		window->DrawList->AddLine(ImVec2(pos.x + box_sz * 0.45f, pos.y + box_sz - pad), ImVec2(pos.x + box_sz - pad, pos.y + pad), IM_COL32(255,255,255,255), 2.0f);
	}
	ImGui::SameLine(0, 8);
	ImGui::TextUnformatted(label);
	return pressed;
}

// custom tab bar
int CustomTabBar(int selectedTab, const char* tabs[], int tabCount, float tabHeight, float tabWidth, ImU32 accentColor, ImU32 tabBg, ImU32 tabBgActive) {
	ImVec2 cursor = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 winPos = ImGui::GetWindowPos();
	ImVec2 winSize = ImGui::GetWindowSize();
	int newSelected = selectedTab;
	for (int i = 0; i < tabCount; ++i) {
		ImVec2 tabMin = ImVec2(cursor.x + i * tabWidth, cursor.y);
		ImVec2 tabMax = ImVec2(tabMin.x + tabWidth, tabMin.y + tabHeight);
		bool hovered = ImGui::IsMouseHoveringRect(tabMin, tabMax);
		ImU32 bg = (i == selectedTab) ? tabBgActive : (hovered ? accentColor : tabBg);
		draw_list->AddRectFilled(tabMin, tabMax, bg, 0.0f);
		if (i == selectedTab) {
			draw_list->AddRect(tabMin, tabMax, accentColor, 0.0f, 0, 2.0f);
		}
		ImVec2 textSize = ImGui::CalcTextSize(tabs[i]);
		ImVec2 textPos = ImVec2(tabMin.x + (tabWidth - textSize.x) * 0.5f, tabMin.y + (tabHeight - textSize.y) * 0.5f);
		draw_list->AddText(textPos, IM_COL32(240,240,255,255), tabs[i]);
		if (hovered && ImGui::IsMouseClicked(0)) newSelected = i;
	}
	ImGui::Dummy(ImVec2(tabWidth * tabCount, tabHeight));
	return newSelected;
}

// add new helpers for custom checkbox and slider
bool CustomCheckboxVerbal(const char* label, bool* v) {
	ImVec2 p = ImGui::GetCursorScreenPos();
	float sz = 18.0f;
	ImDrawList* dl = ImGui::GetWindowDrawList();
	ImU32 border = IM_COL32(80, 80, 80, 255);
	ImU32 fill = *v ? IM_COL32(0, 255, 255, 255) : IM_COL32(16, 16, 16, 255);
	dl->AddRect(p, ImVec2(p.x + sz, p.y + sz), border, 0.0f, 0, 2.0f);
	if (*v) dl->AddRectFilled(ImVec2(p.x + 3, p.y + 3), ImVec2(p.x + sz - 3, p.y + sz - 3), fill, 0.0f);
	if (ImGui::InvisibleButton(label, ImVec2(sz, sz))) *v = !*v;
	ImGui::SameLine();
	ImGui::Text(label);
	return *v;
}

bool CustomSliderVerbal(const char* label, float* v, float min, float max, float step) {
	ImVec2 p = ImGui::GetCursorScreenPos();
	float w = 120.0f;
	float h = 18.0f;
	ImDrawList* dl = ImGui::GetWindowDrawList();
	ImU32 border = IM_COL32(80, 80, 80, 255);
	ImU32 fill = IM_COL32(0, 255, 255, 255);
	float t = (*v - min) / (max - min);
	dl->AddRect(p, ImVec2(p.x + w, p.y + h), border, 0.0f, 0, 2.0f);
	dl->AddRectFilled(p, ImVec2(p.x + w * t, p.y + h), fill, 0.0f);
	// snap value to step
	float snapped = min + roundf((*v - min) / step) * step;
	if (snapped < min) snapped = min;
	if (snapped > max) snapped = max;
	*v = snapped;
	char buf[32];
	if (step >= 1.0f)
		sprintf_s(buf, "%.0f", *v);
	else
		sprintf_s(buf, "%.2f", *v);
	ImVec2 textSz = ImGui::CalcTextSize(buf);
	ImVec2 textPos = ImVec2(p.x + w * 0.5f - textSz.x * 0.5f, p.y + h * 0.5f - textSz.y * 0.5f);
	// draw a black shadow for the value text, then white text for contrast
	dl->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0,0,0,200), buf);
	dl->AddText(textPos, IM_COL32(255,255,255,255), buf);
	bool changed = false;
	bool active = ImGui::IsItemActive();
	if (ImGui::InvisibleButton(label, ImVec2(w, h))) {
		float mouseX = ImGui::GetIO().MousePos.x - p.x;
		float newT = mouseX / w;
		if (newT < 0) newT = 0;
		if (newT > 1) newT = 1;
		float newValue = min + (max - min) * newT;
		// snap to step
		newValue = min + roundf((newValue - min) / step) * step;
		if (newValue < min) newValue = min;
		if (newValue > max) newValue = max;
		if (*v != newValue) {
			*v = newValue;
			changed = true;
		}
	}
	// dragging support for smoother feel
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
		float mouseX = ImGui::GetIO().MousePos.x - p.x;
		float newT = mouseX / w;
		if (newT < 0) newT = 0;
		if (newT > 1) newT = 1;
		float newValue = min + (max - min) * newT;
		newValue = min + roundf((newValue - min) / step) * step;
		if (newValue < min) newValue = min;
		if (newValue > max) newValue = max;
		if (*v != newValue) {
			*v = newValue;
			changed = true;
		}
	}
	ImGui::SameLine();
	ImGui::Text(label);
	return changed;
}
