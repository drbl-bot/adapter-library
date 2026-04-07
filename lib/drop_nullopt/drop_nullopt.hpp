#pragma once

#include <utility>

template <typename FlowType>
class DropNulloptHolder {
 private:
    FlowType& data_;

    using PureIter = decltype(data_.begin());
    
    struct IterHolder {
     private:
        PureIter data_;
        FlowType& flow_ref_;

        public:
        IterHolder(PureIter data, FlowType& flow_ref) : data_(data), flow_ref_(flow_ref) {}
        
        operator bool() { return (bool) *data_;}

        auto operator++() {
            ++data_;
            if (data_ == flow_ref_.end())
                return this;
            while ((data_ != flow_ref_.end()) && (!(*data_))) {
                ++data_;
            }

            return this;
        }

        auto operator*() { return **data_;}

        bool operator==(const IterHolder& rhs) const { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) const { return data_ != rhs.data_; }
    };

 public:
    DropNulloptHolder(FlowType& data) : data_(data) {}

    auto begin() const {
        IterHolder obj(data_.begin(), data_);
        if (!obj) { ++obj; }

        return obj;
    }

    auto end() const {
        return IterHolder(data_.end(), data_);
    }
};

class DropNullopt {
 public:
    template<typename FlowType>
    auto exec(FlowType&& val) const {
        return DropNulloptHolder(std::forward<FlowType>(val));
    }
};