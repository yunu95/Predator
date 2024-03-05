#include "FileSystem.h"

#include "Application.h"

#include <Windows.h>

namespace application
{
	namespace editor
	{
		namespace fileSystem
		{
			std::filesystem::path SaveFileDialog(const char* filter, const char* initialDir)
			{
				OPENFILENAMEA ofn;       // common dialog box structure
				CHAR szFile[260] = { 0 };       // if using TCHAR macros

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = (HWND)Application::GetInstance().GetWindowHandle();
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = filter;
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (initialDir != "")
				{
					ofn.lpstrInitialDir = initialDir;
				}

				if (GetSaveFileNameA(&ofn) == TRUE)
				{
					std::string fp = ofn.lpstrFile;
					std::replace(fp.begin(), fp.end(), '\\', '/');
					return std::filesystem::path(fp);
				}

				return std::filesystem::path();
			}

			std::filesystem::path LoadFileDialog(const char* filter, const char* initialDir)
			{
				OPENFILENAMEA ofn;       // common dialog box structure
				CHAR szFile[260] = { 0 };       // if using TCHAR macros

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = (HWND)Application::GetInstance().GetWindowHandle();
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = filter;
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (initialDir != "")
				{
					ofn.lpstrInitialDir = initialDir;
				}

				if (GetOpenFileNameA(&ofn) == TRUE)
				{
					std::string fp = ofn.lpstrFile;
					std::replace(fp.begin(), fp.end(), '\\', '/');
					return std::filesystem::path(fp);
				}

				return std::filesystem::path();
			}
		}
	}
}
