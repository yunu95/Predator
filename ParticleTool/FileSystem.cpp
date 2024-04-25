#include "FileSystem.h"

#include <Windows.h>

namespace application
{
	namespace fileSystem
	{
		HWND g_handle = nullptr;

		void SetHWND(void* handle)
		{
			g_handle = static_cast<HWND>(handle);
		}

		std::filesystem::path SaveFileDialog(const char* filter, const char* initialDir)
		{
			OPENFILENAMEA ofn;       // common dialog box structure
			CHAR szFile[260] = { 0 };       // if using TCHAR macros

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = g_handle;
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
			ofn.hwndOwner = g_handle;
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

		std::vector<std::filesystem::path> GetSubdirectories(const std::filesystem::path& directoryPath)
		{
			std::vector<std::filesystem::path> subdirectories;
			WIN32_FIND_DATAA findData;
			HANDLE hFind = FindFirstFileA((directoryPath.string() + "\\*").c_str(), &findData);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
						{
							subdirectories.push_back(findData.cFileName);
						}
					}
				} while (FindNextFileA(hFind, &findData));
				FindClose(hFind);
			}

			return subdirectories;
		}
	}
}
