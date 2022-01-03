#include "Boot.h"
#include "ErrorMessages.h"

#define BOOT_FILENAME "boot"

BootFile Boot;

bool LoadBootFile()
{
	if (!File::Exists(BOOT_FILENAME)) {
		MsgBox::Error(APP_NAME, Error.BootFileNotFound);
		return false;
	}
	auto bootFile = File::ReadLines(BOOT_FILENAME);
	if (bootFile.size() < 1) {
		MsgBox::Error(APP_NAME, Error.BootError);
		return false;
	}

	for (auto& line : bootFile) {
		if (String::Trim(line) == "")
			continue;

		auto config = String::Split(line, '=', true);
		if (config.size() != 2) {
			MsgBox::Error(APP_NAME, Error.BootError);
			return false;
		}

		auto name = String::ToUpper(config[0]);
		auto value = config[1];

		if (name == "AUTORUN") {
			if (!File::Exists(value)) {
				MsgBox::Error(APP_NAME, String::Format(Error.ProgramFileNotFound, value.c_str()));
				return false;
			}
			Boot.Autorun = value;
		}
		else if (name == "INTRO") {
			Boot.ShowIntro = String::ToInt(value) > 0;
		}
		else if (name == "RES_W") {
			Boot.ResWidth = String::ToInt(value);
		}
		else if (name == "RES_H") {
			Boot.ResHeight = String::ToInt(value);
		}
		else if (name == "WND_W") {
			Boot.WndWidth = String::ToInt(value);
		}
		else if (name == "WND_H") {
			Boot.WndHeight = String::ToInt(value);
		}
		else {
			MsgBox::Error(APP_NAME, String::Format(Error.BootError, value.c_str()));
			return false;
		}
	}

	return true;
}
