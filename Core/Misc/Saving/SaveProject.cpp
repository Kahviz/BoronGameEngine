#include "SaveProject.h"
#include <fstream>
#include "GLOBALS.h"
#include "Instance.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void SaveProject::Save(const std::vector<std::unique_ptr<Instance>>& Drawables)
{
    std::string path = savings + "\\" + ProjectName;

    if (fs::exists(path))
    {
        fs::remove_all(path);
    }

    fs::create_directories(path);

    std::ofstream file(path + "\\save.BGEproject");

    for (auto& Drawable : Drawables) {
        file << "-\n";
        file << "Name: " << Drawable->Name << "\n";
        file << "Anchored: " << Drawable->Anchored << "\n";
        file << "Size: " << Drawable->transform.Size << "\n";
        file << "Orientation: " << Drawable->transform.Orientation << "\n";
        file << "Position: " << Drawable->transform.Position << "\n";
        file << "Color: " << Drawable->color << "\n";
        file << "CanDraw" << Drawable->CanDraw() << "\n";
    }

    file.close();
}

std::vector<std::unique_ptr<Instance>> SaveProject::Load()
{
    std::ifstream file(savings + "\\" + ProjectName + "\\save.BGEproject");

    if (!file.is_open()) {
        std::cout << "File not found\n";
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::cout << line << "\n";
    }

    return std::vector<std::unique_ptr<Instance>>();
}
