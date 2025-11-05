#include "FileInterface.hpp"
#include <filesystem>
#include <fstream>

#include "../../Utils/StringUtils.hpp"

struct FileData {
    std::ifstream stream;
};

FileInterface::FileInterface() = default;
FileInterface::~FileInterface() { CloseAll(); }

Rml::FileHandle FileInterface::Open(const Rml::String& path) {
    std::filesystem::path fs_path(string_utils::utf8_to_u32(path));
    auto file = std::make_unique<FileData>();

    file->stream.open(fs_path, std::ios::binary);
    if (!file->stream.is_open())
        return 0;

    auto handle = reinterpret_cast<Rml::FileHandle>(file.get());
    m_open_files[handle] = std::move(file);
    return handle;
}

void FileInterface::Close(Rml::FileHandle file) {
    auto it = m_open_files.find(file);
    if (it != m_open_files.end()) {
        it->second->stream.close();
        m_open_files.erase(it);
    }
}

size_t FileInterface::Read(void* buffer, size_t size, Rml::FileHandle file) {
    auto it = m_open_files.find(file);
    if (it != m_open_files.end() && it->second->stream.is_open()) {
        it->second->stream.read(reinterpret_cast<char*>(buffer), size);
        return static_cast<size_t>(it->second->stream.gcount());
    }
    return 0;
}

bool FileInterface::Seek(Rml::FileHandle file, long offset, int origin) {
    auto it = m_open_files.find(file);
    if (!it->second || !it->second->stream.is_open()) return false;

    std::ios_base::seekdir dir;
    switch (origin) {
    case SEEK_SET: dir = std::ios::beg; break;
    case SEEK_CUR: dir = std::ios::cur; break;
    case SEEK_END: dir = std::ios::end; break;
    default: return false;
    }

    it->second->stream.clear();
    it->second->stream.seekg(offset, dir);
    return !it->second->stream.fail();
}

size_t FileInterface::Tell(Rml::FileHandle file) {
    auto it = m_open_files.find(file);
    if (it->second && it->second->stream.is_open()) {
        return static_cast<size_t>(it->second->stream.tellg());
    }
    return 0;
}

void FileInterface::CloseAll() {
    for (auto& [handle, file] : m_open_files) {
        file->stream.close();
    }
    m_open_files.clear();
}

void FileInterface::WriteToFile(const Rml::String& path, const Rml::String& data)
{
    std::filesystem::path fs_path(string_utils::utf8_to_u32(path));
    std::ofstream ofs(fs_path, std::ios::binary);

    ofs.write(data.data(), data.size());

    ofs.close();
}