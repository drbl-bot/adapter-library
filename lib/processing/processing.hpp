#pragma once

#include <utility>

#include "../dir/dir.hpp"
#include "../aggregate_by_key/aggregate_by_key.hpp"
#include "../open_files/open_files.hpp"
#include "../as_data_flow/as_data_flow.hpp"
#include "../drop_nullopt/drop_nullopt.hpp"
#include "../filter/filter.hpp"
#include "../join/join.hpp"
#include "../split/split.hpp"
#include "../split_expected/split_expected.hpp"
#include "../transform/transform.hpp"

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;
};

template<typename T, typename Y, 
        typename = decltype(std::declval<T>().begin()), typename = decltype(std::declval<T>().end())>
auto operator|(T&& lhs, Y&& rhs) {
    return rhs.exec(lhs);
}

class Write {
    public:
    Write(std::ostream& out, char sep = 0) : out_(out), sep_(sep), char_sep_(1) {}
    Write(std::ostream& out, std::string sep) : out_(out), str_sep_(sep) {}
    
    template <typename T>
    auto exec(const T& val) const {
        if (char_sep_)
            for(auto i : val) {
                out_  << i << sep_;
            }
        else
            for(auto i : val) {
                out_  << i << str_sep_ ;
            }

        return val;
    }

    private:
    std::ostream& out_;

    char sep_;
    std::string str_sep_;
    bool char_sep_ = 0;
};

class Out {
    public:
    Out(std::ostream& out) : out_(out) {}
    
    template <typename T>
    auto exec(const T& val) const {
        for(auto i = val.begin(); i != val.end(); ++i) {
            out_ << *i;
        }

        return val;
    }

    private:
    std::ostream& out_;
};

class AsVector {
    public:

    template<typename T>
    auto exec(T&& val) const {
        std::vector<std::remove_cvref_t<decltype(*val.begin())>> result;
        for(auto&& i : val) {
            result.push_back(std::move(i));
        }

        return result;
    }
};