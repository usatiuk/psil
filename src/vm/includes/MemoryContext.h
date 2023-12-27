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
#include "Handle.h"

class Handle;

class MemoryContext {
public:
    friend Handle;

    MemoryContext();
    ~MemoryContext();

    template<typename CT, typename... Args>
    Handle create_cell(Args... args) {
        return alloc_cell<CT>(std::forward<Args>(args)...);
    }

    template<>
    Handle create_cell<NumAtomCell>(CellValType val) {
        std::optional<Handle> ret = run_dirty<std::optional<Handle>>([&](std::function<void(Cell *)> dirty) -> std::optional<Handle> {
            std::lock_guard il(_indexes_lock);
            if (_numatom_index.contains(val)) {
                dirty(_numatom_index.at(val));
                return _numatom_index.at(val);
            } else {
                return std::nullopt;
            }
        });

        if (ret.has_value()) return *ret;

        Handle newc = alloc_cell<NumAtomCell>(val);
        {
            std::lock_guard il(_indexes_lock);
            _numatom_index.emplace(val, newc.get());
        }
        return newc;
    }

    template<>
    Handle create_cell<StrAtomCell>(std::string val) {
        std::optional<Handle> ret = run_dirty<std::optional<Handle>>([&](std::function<void(Cell *)> dirty) -> std::optional<Handle> {
            std::lock_guard il(_indexes_lock);
            if (_stratom_index.contains(val)) {
                dirty(_stratom_index.at(val));
                return _stratom_index.at(val);
            } else {
                return std::nullopt;
            }
        });

        if (ret.has_value()) return *ret;

        Handle newc = alloc_cell<StrAtomCell>(val);
        {
            std::lock_guard il(_indexes_lock);
            _stratom_index.emplace(val, newc.get());
        }
        return newc;
    }


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

    template<typename R>
    R run_dirty(const std::function<R(std::function<void(Cell *)>)> &f) {
        std::lock_guard l(_gc_dirty_notif_queue_lock);
        return f([&](Cell *c) { _gc_dirty_notif_queue.emplace(c); });
    }

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
            _temp_cells.emplace_back(cell);
            if ((_cells_num + _temp_cells.size() + 1) >= (size_t) (_cell_limit / 2)) {
                request_gc();
            }
            return ret;
        }
    }


    void add_root(Cell *c);
    void remove_root(Cell *c);

    std::list<Cell *> _cells;
    std::atomic<size_t> _cells_num = 0;
    std::list<Cell *> _temp_cells;

    std::mutex _indexes_lock;
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

extern std::atomic<MemoryContext *> CURRENT_MC;

#endif//PSIL_MEMORYCONTEXT_H
