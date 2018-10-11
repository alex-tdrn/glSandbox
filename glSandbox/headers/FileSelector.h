#pragma once
#include <string>
#include <filesystem>
#include <optional>
class FileSelector
{
private:
	bool _fileChosen = false;
	bool _explorerOpen = true;
	std::filesystem::path file;
	std::vector<std::string> const extensions;

public:
	FileSelector() = delete;
	FileSelector(std::vector<std::string>&& extensions, 
		std::filesystem::path startingPath = std::filesystem::current_path());
	~FileSelector() = default;

public:
	bool fileChosen() const;
	bool cancelled() const;
	std::filesystem::path getFile() const;
	void drawUI();

};