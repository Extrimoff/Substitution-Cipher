#pragma once
#include <RmlUi/Core/FileInterface.h>
#include <unordered_map>
#include <memory>

class FileInterface : public Rml::FileInterface {
public:
    FileInterface();
    ~FileInterface() override;

    Rml::FileHandle Open(const Rml::String& path) override;
    void Close(Rml::FileHandle file) override;
    size_t Read(void* buffer, size_t size, Rml::FileHandle file) override;
    bool Seek(Rml::FileHandle file, long offset, int origin) override;
    size_t Tell(Rml::FileHandle file) override;

    static void WriteToFile(const Rml::String& path, const Rml::String& data);

private:
    void CloseAll();

    std::unordered_map<Rml::FileHandle, std::unique_ptr<struct FileData>> m_open_files;
};
