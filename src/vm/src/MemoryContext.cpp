//
// Created by Stepan Usatiuk on 25.12.2023.
//

#include "MemoryContext.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <sstream>
#include <unordered_set>

MemoryContext::MemoryContext() {
#ifndef NO_THREADS
    _gc_thread = std::thread(std::bind(&MemoryContext::gc_thread_entry, this));
#endif
}

MemoryContext::~MemoryContext() {
    // Three times because the first one might not start it as it's been running already
    // second one as it might skip something because something has been dirtied
    // and the third one everything should be cleaned
    request_gc_and_wait();
    request_gc_and_wait();
    request_gc_and_wait();

    assert(cell_count() == 0);

#ifndef NO_THREADS
    _gc_thread_stop = true;
    {
        std::lock_guard l(_gc_request_m);
        _gc_request_cv.notify_all();
    }
    _gc_thread.join();
#endif
}


void MemoryContext::add_root(Cell *c) {
    {
#ifndef NO_THREADS
        std::lock_guard l(_new_roots_lock);
#endif
        _new_roots[c]++;
        Logger::log(
                Logger::MemoryContext,
                [&](std::ostream &out) {
                    out << "new root: " << c << " T: " << static_cast<int>(c->_type) << " NUM: " << _new_roots[c];
                },
                Logger::TRACE);
    }
}
void MemoryContext::remove_root(Cell *c) {
    {
#ifndef NO_THREADS
        std::lock_guard l(_new_roots_lock);
#endif
        _new_roots[c]--;
        Logger::log(
                Logger::MemoryContext,
                [&](std::ostream &out) {
                    out << "del root: " << c << " T: " << static_cast<int>(c->_type) << " NUM: " << _new_roots[c];
                },
                Logger::TRACE);
        if (_new_roots[c] == 0) _new_roots.erase(c);
    }
}

void MemoryContext::gc_thread_entry() {
#ifndef NO_THREADS
    while (!_gc_thread_stop) {
        {
            std::unique_lock l(_gc_request_m);
            _gc_request_cv.wait_for(l, std::chrono::seconds(1), [&] { return _gc_request || _gc_thread_stop; });
        }
        if (_gc_thread_stop) return;
#endif
        Logger::log(Logger::MemoryContext, [&](std::ostream &out) { out << "gc start "; }, Logger::DEBUG);
        auto gcstart = std::chrono::high_resolution_clock::now();

        std::queue<Cell *> toVisit;

        auto visitAll = [&]() {
            while (!toVisit.empty()) {
                Cell *c = toVisit.front();
                toVisit.pop();

                if (c == nullptr) continue;
                if (c->_live) continue;
                c->_live = true;
                Logger::log(
                        Logger::MemoryContext,
                        [&](std::ostream &out) { out << "visiting c " << c << " " << static_cast<int>(c->_type); },
                        Logger::TRACE);

                if (c->_type == CellType::CONS) {
                    ConsCell &cc = dynamic_cast<ConsCell &>(*c);
                    toVisit.emplace(cc._car);
                    toVisit.emplace(cc._cdr);
                }
            }
        };

        {
            auto start = std::chrono::high_resolution_clock::now();
            decltype(_new_roots) new_roots;
            {
                decltype(_temp_cells) temp_cells;
                {
#ifndef NO_THREADS
                    std::lock_guard l(_new_roots_lock);
#endif
                    std::swap(new_roots, _new_roots);
                    std::swap(temp_cells, _temp_cells);
                }
                _cells.splice(_cells.end(), temp_cells);
            }


            for (auto const &r: new_roots) {
                Logger::log(
                        Logger::MemoryContext,
                        [&](std::ostream &out) { out << "processing new " << r.first << " diff " << r.second; },
                        Logger::TRACE);
                if (r.second == 0) continue;
                _roots[r.first] += r.second;
                if (_roots[r.first] <= 0) _roots.erase(r.first);
            }

            auto stop = std::chrono::high_resolution_clock::now();

            Logger::log(
                    Logger::MemoryContext,
                    "New roots processing time: " +
                            std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()),
                    Logger::INFO);
        }

        {
            auto start = std::chrono::high_resolution_clock::now();
            std::for_each(_cells.begin(), _cells.end(), [&](Cell *c) { c->_live = false; });

            for (const auto &r: _roots) {
                Logger::log(
                        Logger::MemoryContext,
                        [&](std::ostream &out) { out << "processing r " << r.first << " diff " << r.second; },
                        Logger::TRACE);
                toVisit.emplace(r.first);
            }
            visitAll();
            auto stop = std::chrono::high_resolution_clock::now();
            Logger::log(
                    Logger::MemoryContext, [&](std::ostream &out) { out << "Scanned " << _roots.size() << " roots"; },
                    Logger::DEBUG);
            Logger::log(
                    Logger::MemoryContext,
                    "Roots scan time: " +
                            std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()),
                    Logger::INFO);
        }

#ifndef NO_THREADS
        {
            auto start = std::chrono::high_resolution_clock::now();
            decltype(_gc_dirty_notif_queue) dirtied;
            {
                std::lock_guard dql(_gc_dirty_notif_queue_lock);
                std::swap(dirtied, _gc_dirty_notif_queue);
            }
            while (!dirtied.empty()) {
                for (const auto &r: dirtied) {
                    Logger::log(
                            Logger::MemoryContext, [&](std::ostream &out) { out << "processing dirty " << r; },
                            Logger::DEBUG);
                    toVisit.emplace(r);
                }
                visitAll();

                dirtied = {};
                {
                    std::lock_guard dql(_gc_dirty_notif_queue_lock);
                    std::swap(dirtied, _gc_dirty_notif_queue);
                }
            }

            auto stop = std::chrono::high_resolution_clock::now();
            Logger::log(
                    Logger::MemoryContext,
                    "Dirty scan time: " +
                            std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()),
                    Logger::INFO);
        }
#endif
        {
            auto start = std::chrono::high_resolution_clock::now();

            uint64_t freed = 0;

            _cells.remove_if([&](Cell *l) {
                if (!l->_live) {
                    freed += 1;
                    Logger::log(
                            Logger::MemoryContext, [&](std::ostream &out) { out << "deleting: " << l; }, Logger::TRACE);
                    delete l;
                    return true;
                }
                return false;
            });

            auto stop = std::chrono::high_resolution_clock::now();
            Logger::log(
                    Logger::MemoryContext,
                    "Sweep time: " +
                            std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()),
                    Logger::INFO);
            _cells_num = _cells.size();
            Logger::log(Logger::MemoryContext,
                        "GC Freed: " + std::to_string(freed) + " cells left: " + std::to_string(_cells_num),
                        Logger::INFO);
        }

        auto gcstop = std::chrono::high_resolution_clock::now();
        Logger::log(
                Logger::MemoryContext,
                "GC total time: " +
                        std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(gcstop - gcstart).count()),
                Logger::INFO);

#ifndef NO_THREADS
        {
            std::unique_lock l(_gc_done_m);
            std::unique_lock l2(_gc_request_m);
            _gc_done = true;
            _gc_request = false;
            _gc_done_cv.notify_all();
        }
    }
#endif
}
MemoryContext &MemoryContext::get() {
    static MemoryContext mc;
    return mc;
}
