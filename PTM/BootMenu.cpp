#include "BootMenu.h"
#include "GlobalDefs.h"
#include "ErrorMessages.h"

int SelectedIndex = 0;

void PrintPath(TWindow* wnd, std::string path, int y, bool highlight)
{
	int x = 0;
	y *= TChar::Height;

	auto ch = 0;

	for (int i = 0; i < wnd->Cols; i++) {
		if (i < path.length()) {
			ch = path[i];
			if (highlight)
				wnd->DrawTile(ch, 0, 15, x, y, false);
			else
				wnd->DrawTile(ch, 15, 0, x, y, false);
		}
		else {
			if (highlight)
				wnd->DrawTile(0, 0, 15, x, y, false);
			else
				wnd->DrawTile(0, 15, 0, x, y, false);
		}
		x += TChar::Width;
	}
}

std::string ShowBootMenu(TWindow* wnd)
{
	wnd->SetTitle(APP_NAME);
	auto files = File::List(".", "*.ptml", false);
	if (files.empty()) {
		MsgBox::Error(APP_NAME, Error.NoProgramFound);
		return "";
	}

	std::string selectedFile = "";

	while (selectedFile.empty()) {
		wnd->SetBackColor(0);
		wnd->Clear();

		for (int i = 0; i < files.size(); i++) {
			auto file = files[i];
			auto name = String::GetFirstChars(file, file.length() - 5);
			PrintPath(wnd, String::Format(" %s ", name.c_str()), i, i == SelectedIndex);
		}
		wnd->Update();

		SDL_Event e;
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
