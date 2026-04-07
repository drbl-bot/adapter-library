#pragma once

#include <expected>

template <typename T, typename = void>
struct is_expected : std::false_type {};

template <typename T, typename E>
struct is_expected<std::expected<T, E>, std::void_t<typename std::expected<T, E>::value_type, typename std::expected<T, E>::error_type>> : std::true_type {};

template <typename T>
inline constexpr bool is_expected_v = is_expected<T>::value;


template <typename FlowType, bool Exp>
class SplitExpectHolder {
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
            while ((data_ != flow_ref_.end()) && ((bool(*data_) ^ Exp))) {
                ++data_;
            }

            return this;
        }

        auto operator*() { 
            if constexpr (Exp) 
                return **data_;
            else
                return (*data_).error();
        }

        bool operator==(const IterHolder& rhs) { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) { return data_ != rhs.data_; }
    };

 public:
    SplitExpectHolder(FlowType& data) : data_(data) {}

    auto begin() const {
        IterHolder obj(data_.begin(), data_);
        if (obj ^ Exp) { ++obj; }

        return obj;
    }

    auto end() const {
        return IterHolder(data_.end(), data_);
    }
};

class SplitExpectedData {
    public:
    template<typename T>
    auto exec(T&& val) const {
        return std::pair<SplitExpectHolder<T, 0>, SplitExpectHolder<T, 1>>{SplitExpectHolder<T, 0>(val), SplitExpectHolder<T, 1>(val)};
    }
};

SplitExpectedData SplitExpected();

template <typename FlowType, typename ExpFunc, bool Exp>
class SplitExpectHolderFunc {
 private:
    FlowType& data_;
    ExpFunc func_;

    using PureIter = decltype(data_.begin());
    
    struct IterHolder {
     private:
        PureIter data_;
        ExpFunc func_;
        FlowType& flow_ref_;

        public:
        IterHolder(PureIter data, ExpFunc func, FlowType& flow_ref) : data_(data), func_(func), flow_ref_(flow_ref) {}
        
        operator bool() { 
            if constexpr (is_expected_v<decltype(*data_)>)
                return bool(*data_);
            else
                return bool(func_(*data_));
        }

        auto operator++() {
            if constexpr (is_expected_v<decltype(*data_)>) {
                ++data_;
                if (data_ == flow_ref_.end())
                    return this;
                while ((data_ != flow_ref_.end()) && ((bool(*data_) ^ Exp))) {
                    ++data_;
                }
            } else {
                ++data_;
                if (data_ == flow_ref_.end())
                    return this;
                while ((data_ != flow_ref_.end()) && ((bool(func_(*data_)) ^ Exp))) {
                    ++data_;
                }
            }


            return this;
        }

        auto operator*() { 
            if constexpr (is_expected_v<decltype(*data_)>) {
                if constexpr (Exp) 
                    return **data_;
                else
                    return (*data_).error();
            } else {
                if constexpr (Exp) 
                    return *(func_(*data_));
                else
                    return func_(*data_).error();
            }
        }

        bool operator==(const IterHolder& rhs) { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) { return data_ != rhs.data_; }
    };

 public:
    SplitExpectHolderFunc(FlowType& data, ExpFunc func) : data_(data), func_(func) {}

    auto begin() const {
        IterHolder obj(data_.begin(), func_, data_);
        if (obj ^ Exp) { ++obj; }

        return obj;
    }

    auto end() const {
        return IterHolder(data_.end(), func_, data_);
    }
};


template<typename ExpFunc>
class SplitExpectedDataFunc {
 private:
    ExpFunc func_;

 public:
    SplitExpectedDataFunc(ExpFunc func) : func_(func) {}

    template<typename T>
    auto exec(T&& val) const {
        return std::pair<SplitExpectHolderFunc<T, ExpFunc, 0>, SplitExpectHolderFunc<T, ExpFunc, 1>>{
            SplitExpectHolderFunc<T, ExpFunc, 0>(std::forward<T>(val), func_), SplitExpectHolderFunc<T, ExpFunc, 1>(std::forward<T>(val), func_)
        };
    }
};



template<typename ExpFunc>
auto SplitExpected(ExpFunc func) {
    return SplitExpectedDataFunc<ExpFunc>(func);
}