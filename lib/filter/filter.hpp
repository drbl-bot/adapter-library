#pragma once

#include <utility>

template <typename FlowType, typename Func>
class FilterHolder {
 private:
    FlowType& data_;
    Func func_;

    using PureIter = decltype(data_.begin());

    struct IterHolder {
     public:
        IterHolder(PureIter data, FlowType& flow_ref, Func func) : flow_ref_(flow_ref), data_(data), func_(func) {}

        auto operator++() {
            ++data_;
            if (data_ == flow_ref_.end())
                return this;
            auto operand = *data_;
            while ((data_ != flow_ref_.end()) && (operand = *data_, !func_(operand))) {
                ++data_;
            }

            return this;
        }

        auto operator*() { return *data_;}

        bool operator==(const IterHolder& rhs) const { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) const { return data_ != rhs.data_; }

     private:
        FlowType& flow_ref_;
        PureIter data_;
        Func func_;
    };

 public:
    FilterHolder(FlowType& data, Func ff) : data_(data), func_(ff) {}

    auto begin() const { 
        IterHolder obj(data_.begin(), data_, func_);
        auto operand = *obj;
        if (!func_(operand)) {
            ++obj;
        }

        return obj;
    }

    auto end() const {
        return IterHolder(data_.end(), data_, func_);
    }
};


template <typename Func>
class FilterData {
 private:
    Func func_;

 public:
    FilterData(Func ff) : func_(ff) {}

    template <typename FlowType>
    auto exec(FlowType&& val) const {
        return FilterHolder<FlowType, Func> (std::forward<FlowType>(val), func_);
    }
    
};

template <typename Func>
auto Filter(Func f) {
    return FilterData<Func>(f);
}