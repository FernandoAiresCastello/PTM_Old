#include "BootMenu.h"
#include "GlobalDefs.h"
#include "ErrorMessages.h"

int SelectedIndex = 0;
int CursorChar = 16;
int ActiveForeColor = 15;
int InactiveForeColor = 7;
int BackColor = 0x80;

void PrintPath(TWindow* wnd, std::string path, int y, bool highlight)
{
	int ch = 0;
	int x = 0;

	if (highlight)
		wnd->DrawTile(CursorChar, ActiveForeColor, InactiveForeColor, 0, y, true, true);

	path = " " + path;

	for (int i = 0; i < wnd->Cols; i++) {
		if (i < path.length()) {
			ch = path[i];
			if (highlight)
				wnd->DrawTile(ch, ActiveForeColor, BackColor, x, y, true, true);
			else
				wnd->DrawTile(ch, InactiveForeColor, BackColor, x, y, true, true);
		}
		x++;
	}
}

std::string ShowBootMenu(TWindow* wnd, std::vector<std::string>& files)
{
	std::string selectedFile = "";
	wnd->SetPixelSize(2, 2);
	
	while (selectedFile.empty()) {
		wnd->SetBackColor(BackColor);
		wnd->Clear();

		for (int i = 0; i < files.size(); i++) {
			auto file = files[i];
			auto name = String::GetFirstChars(file, file.length() - 5);
			PrintPath(wnd, name, i, i == SelectedIndex);
		}
		wnd->Update();

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			return "";
		}
		else if (e.type == SDL_KEYDOWN) {
			SDL_Keycode key = e.key.keysym.sym;
			if (key == SDLK_x && TKey::Ctrl() && TKey::Alt()) {
				return "";
			}
			else if (key == SDLK_DOWN) {
				if (SelectedIndex < files.size() - 1)
					SelectedIndex++;
			}
			else if (key == SDLK_UP) {
				if (SelectedIndex > 0)
					SelectedIndex--;
			}
			else if (key == SDLK_RETURN) {
				if (TKey::Alt())
					wnd->ToggleFullscreen();
				else
					selectedFile = files[SelectedIndex];
			}
		}
	}

	return selectedFile;
}
