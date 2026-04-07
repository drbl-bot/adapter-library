#pragma once

#include <unordered_map>
#include <utility>

template <typename FlowType, typename Value, typename AggFunc, typename KeyFunc>
class AggregateByKeyHolder {
 private:
    using KeyType = std::invoke_result_t<KeyFunc, decltype(*std::declval<FlowType>().begin())>;

    std::unordered_map<KeyType, Value> data_;

 public:
    AggregateByKeyHolder(FlowType flow, Value first_val, AggFunc agg_funcunc, KeyFunc key_funcunc) {
        std::unordered_map<KeyType, Value> extra_map;

        for (const auto& input : flow) {
            auto key = key_funcunc(input);
            auto iter = extra_map.find(key);
            if (iter == extra_map.end()) {
                auto [new_iter, inserted] = extra_map.emplace(key, first_val);
                iter = new_iter;
            }

            agg_funcunc(input, iter->second);
        }

        for (auto i : extra_map) {
            data_.insert(i);
        }
    }

    auto begin() const {
        return data_.begin();
    }

    auto end() const {
        return data_.end();
    }
};

template<typename Value, typename AggFunc, typename KeyFunc>
class AggregateByKeyData {
 private:
    Value first_val_;
    AggFunc agg_func_;
    KeyFunc key_func_;
 public:
    AggregateByKeyData(Value val, AggFunc agg_func, KeyFunc key_func) : first_val_(val), agg_func_(agg_func), key_func_(key_func) {}

    
    template<typename FlowType>
    auto exec(FlowType&& val) const {
        return AggregateByKeyHolder<std::decay_t<FlowType>, Value, AggFunc, KeyFunc>(
            std::forward<FlowType>(val), first_val_, agg_func_, key_func_);
    };
};

template <typename Value, typename AggFunc, typename KeyFunc>
auto AggregateByKey(Value first_val, AggFunc agg_funcunc, KeyFunc key_func) {
    return AggregateByKeyData<Value, AggFunc, KeyFunc>(first_val, agg_funcunc, key_func);
}

