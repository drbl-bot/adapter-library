#pragma once

#include <type_traits>
#include <utility>
#include <optional>
#include <iostream>

template<typename LhsType, typename RhsType>
struct JoinResult {
    std::decay_t<LhsType> first;
    std::optional<std::decay_t<RhsType>> second;
};

template<typename T, typename U, typename W, typename V>
bool operator==(const JoinResult<T,U>& lhs, const JoinResult<W,V>& rhs) {
    if (lhs.first == rhs.first && lhs.second == rhs.second)
        return true;
    return false;
}

template<typename LhsType, typename RhsType>
std::ostream& operator<<(std::ostream& out, JoinResult<LhsType, RhsType>& value) {
    if (value.second)
        out << value.first << ' ' << *value.second << '\n';
    else
        out << value.first << ' ' << "nullopt" << '\n';
    return out;
}


template<typename LhsType, typename RhsType>
class JoinHolder {
    public:
    JoinHolder(LhsType lhs_value, RhsType rhs_value) : lhs_data_(lhs_value), rhs_data_(rhs_value) {}
    
    struct IterHolder{
        public:
        void hit() {
            hitted_ = 1;
        }
        using PureIter = decltype(std::declval<LhsType>().begin());
        IterHolder(PureIter data, PureIter data_end, RhsType rhs_data) : data_(data), data_end_(data_end), rhs_data_(rhs_data), current_rhs_iter_(rhs_data_.begin()) {}

        auto operator++() {
            if (null_opt_ == true) {
                null_opt_ = 0;
            } else {
                ++current_rhs_iter_;
            }

            if (rerise()) return this;

            while ((current_rhs_iter_ != rhs_data_.end()) & (current_rhs_iter_->key != data_->key)) {
                ++current_rhs_iter_;
                if (rerise()) return this;

                if (data_ == data_end_) 
                    return this;
            }

            hitted_ = 1;
            return this;
        }
        
        auto operator*() {
            if (null_opt_)  {
                hitted_ = 1;
                return JoinResult<decltype(data_->value), decltype(current_rhs_iter_->value)>(
                              data_->value, std::nullopt); 
            }

            return JoinResult<decltype(data_->value), decltype(current_rhs_iter_->value)>(
                              data_->value, current_rhs_iter_->value); 
        }

        bool operator==(const IterHolder& rhs) { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) { return data_ != rhs.data_; }

        bool check() {
            return current_rhs_iter_->key != data_->key;
        }

        private:
        bool hitted_ = 0;
        bool null_opt_ = 0;
        
        PureIter reminder_;
        PureIter data_;
        PureIter data_end_;
        RhsType rhs_data_;
        decltype(rhs_data_.begin()) current_rhs_iter_;

        bool rerise() {
            if (current_rhs_iter_ == rhs_data_.end()) {
                if (hitted_ == 0) {
                    null_opt_  = 1;
                    return 1;
                }

                hitted_ = 0;
                ++data_;
                current_rhs_iter_ = rhs_data_.begin();
            }

            return 0;
        }
    };

    auto begin() const {
        IterHolder obj(lhs_data_.begin(), lhs_data_.end(), rhs_data_);
        if(obj.check()) { 
            ++obj;  
        } else { 
            obj.hit(); 
        }

        return obj;
    }

    auto end() const {
        return IterHolder(lhs_data_.end(), lhs_data_.end(), rhs_data_);
    }

    private:
    LhsType lhs_data_;
    RhsType rhs_data_;
};

template<typename DataType>
class JoinData {
    public:
    JoinData(DataType data) : data_(data) {}

    template<typename T>
    auto exec(const T& val) const {
        return JoinHolder<T, DataType>(val, data_);
    }

    private:
    DataType data_;
};

template<typename T>
auto Join(T value) {
    return JoinData<T>(value);
}


template<typename LhsType, typename RhsType, typename LhsFunc, typename RhsFunc>
class JoinHolderFunc {
    public:
    JoinHolderFunc(LhsType lhs_value, RhsType rhs_value, LhsFunc lhs_func, RhsFunc rhs_func) : lhs_data_(lhs_value),
                    rhs_data_(rhs_value), lhs_func_(lhs_func), rhs_func_(rhs_func) {}
    
    struct IterHolder{
        public:
        void hit() {
            hitted_ = 1;
        }
        using PureIter = decltype(std::declval<LhsType>().begin());
        IterHolder(PureIter data, PureIter data_end, RhsType rhs_data) : data_(data), data_end_(data_end), rhs_data_(rhs_data), current_rhs_iter_(rhs_data_.begin()) {}

        auto operator++() {
            if (null_opt_ == true) {
                null_opt_ = 0;
            } else {
                ++current_rhs_iter_;
            }

            if (rerise()) return this;

            while ((current_rhs_iter_ != rhs_data_.end()) & (rhs_func_(*current_rhs_iter_) != lhs_func_(*data_))) {
                ++current_rhs_iter_;
                if (rerise()) return this;

                if (data_ == data_end_) 
                    return this;
            }

            hitted_ = 1;
            return this;
        }
        
        auto operator*() {
            if (null_opt_)  {
                hitted_ = 1;
                return JoinResult<decltype(*data_), decltype(*current_rhs_iter_)>(
                              *data_, std::nullopt); 
            }

            return JoinResult<decltype(*data_), decltype(*current_rhs_iter_)>(
                              *data_, *current_rhs_iter_); 
        }

        bool operator==(const IterHolder& rhs) { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) { return data_ != rhs.data_; }

        bool check() {
            return rhs_func_(*current_rhs_iter_) != lhs_func_(*data_);
        }

        private:
        LhsFunc lhs_func_;
        RhsFunc rhs_func_;

        bool hitted_ = 0;
        bool null_opt_ = 0;
        
        PureIter reminder_;
        PureIter data_;
        PureIter data_end_;
        RhsType rhs_data_;
        decltype(rhs_data_.begin()) current_rhs_iter_;

        bool rerise() {
            if (current_rhs_iter_ == rhs_data_.end()) {
                if (hitted_ == 0) {
                    null_opt_  = 1;
                    return 1;
                }

                hitted_ = 0;
                ++data_;
                current_rhs_iter_ = rhs_data_.begin();
            }

            return 0;
        }
    };

    auto begin() const {
        IterHolder obj(lhs_data_.begin(), lhs_data_.end(), rhs_data_);
        if(obj.check()) { 
            ++obj;  
        } else { 
            obj.hit(); 
        }

        return obj;
    }

    auto end() const {
        return IterHolder(lhs_data_.end(), lhs_data_.end(), rhs_data_);
    }

    private:
    LhsFunc lhs_func_;
    RhsFunc rhs_func_;
    LhsType lhs_data_;
    RhsType rhs_data_;
};

template<typename DataType, typename LhsFunc, typename RhsFunc>
class JoinDataFunc {
    public:
    JoinDataFunc(DataType data, LhsFunc lhs_func, RhsFunc rhs_func) : data_(data), lhs_func_(lhs_func), rhs_func_(rhs_func) {}

    template<typename T>
    auto exec(const T& val) const {
        return JoinHolderFunc<T, DataType, LhsFunc, RhsFunc>(val, data_, lhs_func_, rhs_func_);
    
    }

    private:
    DataType data_;
    LhsFunc lhs_func_;
    RhsFunc rhs_func_;
};

template<typename T, typename LhsFunc, typename RhsFunc>
auto Join(T value, LhsFunc lsh_func, RhsFunc rhs_func) {
    return JoinDataFunc<T, LhsFunc, RhsFunc>(value, lsh_func, rhs_func);
}
