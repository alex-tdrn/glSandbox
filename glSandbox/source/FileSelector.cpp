#include "FileSelector.h"

#include <imgui.h>

FileSelector::FileSelector(std::vector<std::string>&& extensions,
	std::filesystem::path startingPath)
	: extensions(extensions),
	file(startingPath.has_filename() ? startingPath.remove_filename() : startingPath)
{
}

bool FileSelector::fileChosen() const
{
	return _fileChosen;
}

bool FileSelector::cancelled() const
{
	return !_fileChosen && !_explorerOpen;
}

std::filesystem::path FileSelector::getFile() const
{
	return file;
}

void FileSelector::drawUI()
{
	if(_fileChosen || !_explorerOpen)
		return;
	ImGui::Begin("Import", &_explorerOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	ImGui::Text(file.string().data());
	if(ImGui::Selectable(".."))
		file = file.parent_path();
	std::vector<std::filesystem::directory_entry> folders;
	std::vector<std::filesystem::directory_entry> files;
	for(auto const& part : std::filesystem::directory_iterator(file))
	{
		if(part.is_directory())
			folders.push_back(part);
		else
			files.push_back(part);
	}
	ImVec2 elementSize{ImGui::GetContentRegionAvail().x, 0.0f};
	for(auto const& folder : folders)
	{
		if(ImGui::Selectable(folder.path().filename().string().data()))
			file = folder.path();
	}
	ImGui::Separator();
	for(auto const& f: files)
	{
		std::string filename = f.path().filename().string();
		bool hasExtension = false;
		for(auto& extension : extensions)
		{
			if(f.path().extension() == extension)
			{
				hasExtension = true;
				break;
			}
		}
		if(!hasExtension)
		{
			ImGui::Text(filename.data());
		}
		else if(ImGui::Selectable(filename.data()))
		{
			file = f;
			_fileChosen = true;
			_explorerOpen = false;
		}
	}
	ImGui::End();
}
