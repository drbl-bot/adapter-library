#pragma once

#include <sstream>
#include <fstream>
#include <iostream>

template <typename FlowType>    
class OpenFilesHolder {
 private:
    FlowType& data_;

    using PureIter = decltype(data_.begin());

    struct IterHolder {
     public:
        IterHolder(PureIter data, FlowType& flow_ref) : flow_ref_(flow_ref), data_(data) {}

        auto operator++() {
            ++data_;
            return *this;
        }

        auto operator*() {
            std::stringstream ss;

            std::ifstream file(*data_, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "files are not openning";
            }

            ss << file.rdbuf();
            file.close();
            return ss;
        }

        bool operator==(const IterHolder& rhs) const { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) const { return data_ != rhs.data_; }

     private:
        FlowType& flow_ref_;
        PureIter data_;
    };

 public:
    OpenFilesHolder(FlowType& data) : data_(data) {}

    auto begin() const { 
        return IterHolder(data_.begin(), data_);
    }

    auto end() const {
        return IterHolder(data_.end(), data_);
    }
};


class OpenFiles {
 public: 
    template <typename FlowType>
    auto exec(FlowType&& val) const {
        return OpenFilesHolder<FlowType>(std::forward<FlowType>(val));
    }
};