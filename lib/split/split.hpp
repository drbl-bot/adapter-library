#pragma once

#include <string>

template<typename FlowType>
class SplitHolder {
 private:
    FlowType& data_;
    std::string seps_;

    using PureIter = decltype(data_.begin());
    
    struct IterHolder {
     private:
           PureIter data_;
           std::string seps_;
           FlowType& flow_ref_;
           bool reline = false;
           std::string curr_line = "";
           std::string curr_seg_ = "";
    
           void seg_in_next_line() {
               if (data_ != flow_ref_.end()) {
                   curr_line = (*data_).str();
                   seg_in_curr_line();
               } else {
                   curr_seg_ = "";
                   curr_line = "";
               }
           }
    
           void seg_in_curr_line() {
               if (curr_line.empty()) {
                   reline = true;
                   curr_seg_ = "";
                   return;
               }
    
               size_t min_pos = std::string::npos;
               for (size_t i = 0; i < seps_.length(); ++i) {
                   size_t pos = curr_line.find(seps_[i]);
                   if (pos != std::string::npos) {
                       min_pos = std::min(min_pos, pos);
                   }
               }
    
               if (min_pos == std::string::npos) {
                   curr_seg_ = curr_line;
                   curr_line = "";
                   reline = true;
               } else {
                   curr_seg_ = curr_line.substr(0, min_pos);
                   curr_line.erase(0, min_pos + 1);
               }
           }

     public:
        IterHolder(PureIter data, std::string seps, FlowType& flow_ref) : data_(data), seps_(seps), flow_ref_(flow_ref) {
            if (data_ != flow_ref_.end()) { seg_in_next_line(); }
        }

        auto operator++() {
            if (reline) {
                reline = false;
                ++data_;
                if (data_ != flow_ref_.end()) {
                    seg_in_next_line();
                }
            } else {
                seg_in_curr_line();
            }
            return *this;
        }

        auto operator*() { return curr_seg_; }

        bool operator==(const IterHolder& rhs) const { return data_ == rhs.data_; }
        bool operator!=(const IterHolder& rhs) const { return data_ != rhs.data_; }

    };

 public:
    SplitHolder(FlowType& data, std::string seps) : data_(data), seps_(seps) {}

    auto begin() const {
        return IterHolder(data_.begin(), seps_, data_);
    }

    auto end() const {
        return IterHolder(data_.end(), seps_, data_);
    }
};

class SplitData {
 private:
    std::string seps_;

 public:
    SplitData(std::string args) : seps_(args) {}

    template <typename FlowType>
    auto exec(FlowType&& val) const {
        return SplitHolder<FlowType>(std::forward<FlowType>(val), seps_);
    }
};

SplitData Split(std::string arg);