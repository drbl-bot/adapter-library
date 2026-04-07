#pragma once

#include <filesystem>
#include <string>

class DirHolder {
 private:
    std::string path_;
    bool recursive_;

    struct IterHolder {
     private:
        std::filesystem::directory_entry current_;

        std::filesystem::directory_iterator non_recursive_current_;
        std::filesystem::directory_iterator non_recursive_end_;
        
        std::filesystem::recursive_directory_iterator recursive_current_;
        std::filesystem::recursive_directory_iterator recursive_end_;
        bool recursive_;
        
     public:
        IterHolder(std::string path, bool recursive, bool end = 0) 
        : recursive_(recursive) {
            if (recursive) {
                if (end)
                    recursive_current_ = std::filesystem::recursive_directory_iterator();
                else
                    recursive_current_ = std::filesystem::recursive_directory_iterator(path);

                recursive_end_ = std::filesystem::recursive_directory_iterator();
                if (recursive_current_ != recursive_end_) {
                    current_ = *recursive_current_;
                }
            } else {
                if (end)
                    non_recursive_current_ = std::filesystem::directory_iterator();
                else
                    non_recursive_current_ = std::filesystem::directory_iterator(path);

                non_recursive_end_ = std::filesystem::directory_iterator();
                if (non_recursive_current_ != non_recursive_end_) {
                    current_ = *non_recursive_current_;
                }
            }
        }

        auto operator*() const { return current_.path(); }

        auto operator++() {
            if (recursive_) {
                if (recursive_current_ != recursive_end_) {
                    ++recursive_current_;
                    if (recursive_current_ != recursive_end_) {
                        current_ = *recursive_current_;
                    }
                }
            } else {
                if (non_recursive_current_ != non_recursive_end_) {
                    ++non_recursive_current_;
                    if (non_recursive_current_ != non_recursive_end_) {
                        current_ = *non_recursive_current_;
                    }
                }
            }
            return *this;
        }

        bool operator==(const IterHolder& other) const {
            if (recursive_ != other.recursive_) return false;
            if (recursive_) {
                return recursive_current_ == other.recursive_current_;
            } else {
                return non_recursive_current_ == other.non_recursive_current_;
            }
        }

        bool operator!=(const IterHolder& other) const {
            return !(*this == other);
        }
    };
    
 public:
    DirHolder(const std::string& path, bool recursive) : path_(path), recursive_(recursive) {}

    auto begin() const {
        return IterHolder(path_, recursive_);
    }

    auto end() const {
        return IterHolder(path_, recursive_, 1);
    }
};

DirHolder Dir(const std::string& path, bool rec);