#pragma once

template<typename FlowType, typename Func>
class TransformHolder {
 private:
    FlowType& data_;
    Func func_;

    using PureIter = decltype(data_.begin());
    
    struct IterHolder{
     public:
        IterHolder(PureIter data, Func func) : data_(data), func_(func) {}

        auto operator++() {
            ++data_;
            return this;
        }
        
        auto operator*() { 
            auto operand = *data_;

            return func_(operand); 
        }

        bool operator==(const IterHolder& rhs) const { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) const { return data_ != rhs.data_; }


     private:
        PureIter data_;
        Func func_;
    };

 public:
    TransformHolder(FlowType& data, Func func) : data_(data), func_(func) {}

    auto begin() const {
        return IterHolder(data_.begin(), func_);
    }

    auto end() const {
        return IterHolder(data_.end(), func_);
    }
};

template<typename Func>
class TransformData {
 private:
    Func func_;
 
 public:
    TransformData(Func func) : func_(func) {}

    template<typename FlowType>
    auto exec(FlowType&& data) const {   
        return TransformHolder<FlowType, Func>(data, func_);
    }   
};

template<typename Func>
auto Transform(Func func) {
    return TransformData<Func>(func);
}