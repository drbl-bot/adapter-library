#pragma once

template <typename ContType>
class DataFlow{
 private:
    ContType& data_;

 public:
    using value_type = decltype(*data_.begin());
    
    DataFlow(ContType& data) : data_(data) {}

    auto begin() const {
        return data_.begin();
    }

    auto end() const {
        return data_.end();
    }
};

template<typename ContType>
auto AsDataFlow(ContType&& data) {
    return DataFlow(data);
}
