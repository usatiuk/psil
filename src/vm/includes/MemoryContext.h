//
// Created by Stepan Usatiuk on 25.12.2023.
//

#ifndef PSIL_MEMORYCONTEXT_H
#define PSIL_MEMORYCONTEXT_H

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <set>
#include <thread>

#include "Cell.h"

class MemoryContext {
public:
    class Handle;

    MemoryContext();
    ~MemoryContext();

    template<typename CT, typename... Args>
    Handle create_cell(Args... args) {
        return alloc_cell<CT>(std::forward<Args>(args)...);
    }

    //    template<>
    //    Handle create_cell<NumAtomCell>(CellValType val) {
    //        {
    //            std::lock_guard l(_cells_lock);
    //            if (_numatom_index.contains(val)) {
    //                _numatom_index.at(val)->live = true;
    //                return _numatom_index.at(val);
    //            }
    //        }
    //        Handle newc = alloc_cell<NumAtomCell>(val);
    //        {
    //            std::lock_guard l(_cells_lock);
    //            _numatom_index.emplace(val, newc.get());
    //        }
    //        return newc;
    //    }
    //
    //    template<>
    //    Handle create_cell<StrAtomCell>(std::string val) {
    //        {
    //            std::lock_guard l(_cells_lock);
    //            if (_stratom_index.contains(val)) {
    //                _stratom_index.at(val)->live = true;
    //                return _stratom_index.at(val);
    //            }
    //        }
    //        Handle newc = alloc_cell<StrAtomCell>(val);
    //        {
    //            std::lock_guard l(_cells_lock);
    //            _stratom_index.emplace(std::move(val), newc.get());
    //        }
    //        return newc;
    //    }

    class Handle {
    public:
        Handle(Cell *target);
        ~Handle();

        Handle(Handle const &other);
        Handle &operator=(Handle other);

        Cell *operator->() const { return _target; }
        Cell &operator*() const { return *_target; }
        Cell *get() const noexcept { return _target; }

        bool operator==(const Handle &rhs) const {
            if (_target == rhs._target) {
                return true;
            } else if ((_target != nullptr && rhs._target != nullptr) && (_target->_type == rhs._target->_type)) {
                if (_target->_type == CellType::NUMATOM) {
                    return dynamic_cast<NumAtomCell &>(*_target)._val == dynamic_cast<NumAtomCell &>(*rhs._target)._val;
                } else if (_target->_type == CellType::STRATOM) {
                    return dynamic_cast<StrAtomCell &>(*_target)._val == dynamic_cast<StrAtomCell &>(*rhs._target)._val;
                }
            }
            return false;
        }

    private:
        Cell *_target = nullptr;
    };

    void request_gc_and_wait() {
        std::unique_lock l(_gc_done_m);
        _gc_done = false;
        {
            request_gc();
        }
        if (!_gc_done)
            _gc_done_cv.wait(l, [&] { return _gc_done.load(); });
    }

    void request_gc() {
        std::lock_guard l(_gc_request_m);
        _gc_request = true;
        _gc_request_cv.notify_all();
    }

    size_t cell_count() {
        return _cells_num;
    }

    void run_dirty(const Handle &h, std::function<void()> f);

private:
    template<typename CT, typename... Args>
    Handle alloc_cell(Args... args) {

        size_t tcellnum;
        {
            std::lock_guard tmplg(_new_roots_lock);
            tcellnum = _temp_cells.size();
        }

        if ((_cells_num + tcellnum) >= (_cell_limit)) {
            // We might need to run GC twice as it has to process the messages;
            std::cerr << "Running forced GC" << std::endl;
            for (int i = 0; i < 3 && (_cells_num + tcellnum) >= (_cell_limit); i++) {
                request_gc_and_wait();
                {
                    std::lock_guard tmplg(_new_roots_lock);
                    tcellnum = _temp_cells.size();
                }
            }
            if ((_cells_num + tcellnum) >= (_cell_limit)) {
                throw std::runtime_error("Out of cells");
            }
        }

        CT *cell = new CT(std::forward<Args>(args)...);

        {
            std::lock_guard tmplg(_new_roots_lock);
            Handle ret(cell);
            _temp_cells.emplace(cell);
            if ((_cells_num + _temp_cells.size() + 1) >= (size_t) (_cell_limit / 2)) {
                request_gc();
            }
            return ret;
        }
    }


    void add_root(Cell *c);
    void remove_root(Cell *c);

    std::mutex _cells_lock;
    std::set<Cell *> _cells;
    std::atomic<size_t> _cells_num = 0;
    std::set<Cell *> _temp_cells;
    std::unordered_map<CellValType, Cell *> _numatom_index;
    std::unordered_map<std::string, Cell *> _stratom_index;

    static constexpr size_t _cell_limit = {50000};

    void gc_thread_entry();

    std::map<Cell *, int64_t> _roots;
    std::map<Cell *, int64_t> _new_roots;
    std::recursive_mutex _new_roots_lock;

    std::set<Cell *> _gc_dirty_notif_queue;
    std::mutex _gc_dirty_notif_queue_lock;

    std::atomic<bool> _gc_request = false;
    std::mutex _gc_request_m;
    std::condition_variable _gc_request_cv;

    std::atomic<bool> _gc_done = false;
    std::mutex _gc_done_m;
    std::condition_variable _gc_done_cv;

    std::thread _gc_thread;
    std::atomic<bool> _gc_thread_stop = false;
};

using MCHandle = MemoryContext::Handle;

extern std::atomic<MemoryContext *> CURRENT_MC;

#endif//PSIL_MEMORYCONTEXT_H
