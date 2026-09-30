
#include "LFUPolicy.h"
#include <algorithm>

namespace bufman{

    void LFUPolicy::init(std::size_t){
        buckets_.clear();
        slots_.clear();
        min_freq_ = 0;
    }

    void LFUPolicy::on_load(std::size_t frame){
        on_remove(frame);
        buckets_[1].push_front(frame);
        slots_[frame] = {1, buckets_[1].begin()};
        min_freq_ = 1;
    }

    void LFUPolicy::on_access(std::size_t frame){
        auto found = slots_.find(frame);
        if (found == slots_.end()){
            on_load(frame);
            return;
        }
        Slot &slot = found->second;
        std::size_t old_count = slot.count;
        buckets_[old_count].erase(slot.pos);
        if (buckets_[old_count].empty()){
            buckets_.erase(old_count);
            if (min_freq_ == old_count){
                min_freq_++;
            }
        }

        slot.count++;
        buckets_[slot.count].push_front(frame);
        slot.pos = buckets_[slot.count].begin();
    }

    void LFUPolicy::on_remove(std::size_t frame){
        auto found = slots_.find(frame);
        if (found == slots_.end()){
            return;
        }
        std::size_t count = found->second.count;
        buckets_[count].erase(found->second.pos);
        if (buckets_[count].empty()){
            buckets_.erase(count);         
        }
        slots_.erase(found);
        min_freq_ = 0;
        for (const auto &bucket : buckets_){
            if (min_freq_ == 0 || bucket.first < min_freq_){
                min_freq_ = bucket.first;
            }
        }

    }

    std::optional<std::size_t> LFUPolicy::pick_victim(const std::vector<std::size_t>& candidates) const{
        bool found = false;
        std::size_t lowest = 0;
        for(const std::size_t frame : candidates){
            auto it = slots_.find(frame);
            if(it == slots_.end()){
                continue;
            }
            if(!found || it->second.count < lowest){
                found = true;
                lowest = it->second.count;
            }
        }
        if(!found){
            return std::nullopt;
        }

        const std::list<std::size_t> &bucket = buckets_.find(lowest)->second;
        for(auto it = bucket.rbegin(); it != bucket.rend(); ++it){
            if(std::find(candidates.begin(), candidates.end(), *it) != candidates.end()){
                return *it;
            }
        }
        return std::nullopt;
    }

}