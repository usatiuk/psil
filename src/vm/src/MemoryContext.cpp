//
// Created by Stepan Usatiuk on 25.12.2023.
//

#include "MemoryContext.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <sstream>
#include <unordered_set>

std::atomic<MemoryContext *> CURRENT_MC = nullptr;

MemoryContext::MemoryContext() {
    MemoryContext *expected = nullptr;
    if (!CURRENT_MC.compare_exchange_strong(expected, this)) throw std::runtime_error("MC already exists!");

    _gc_thread = std::thread(std::bind(&MemoryContext::gc_thread_entry, this));
}

MemoryContext::~MemoryContext() {
    MemoryContext *expected = this;
    if (!CURRENT_MC.compare_exchange_strong(expected, nullptr)) {
        std::cerr << "Global MC pointer was overwritten!" << std::endl;
        std::abort();
    }
    _gc_thread_stop = true;
    _gc_request_cv.notify_all();
    _gc_thread.join();
}


void MemoryContext::add_root(Cell *c) {
    {
        std::lock_guard l(_new_roots_lock);
        _new_roots[c]++;
        //        {
        //            std::stringstream out;
        //            out << "new root: " << c << " T: " << static_cast<int>(c->_type) << " NUM: " << _new_roots[c] << "\n";
        //            std::cerr << out.str();
        //        }
    }
}
void MemoryContext::remove_root(Cell *c) {
    {
        std::lock_guard l(_new_roots_lock);
        _new_roots[c]--;
        //        {
        //            std::stringstream out;
        //            out << "del root: " << c << " T: " << static_cast<int>(c->_type) << " NUM: " << _new_roots[c] << "\n";
        //            std::cerr << out.str();
        //        }
        if (_new_roots[c] == 0) _new_roots.erase(c);
    }
}

void MemoryContext::gc_thread_entry() {
    while (true) {
        {
            std::unique_lock l(_gc_request_m);
            _gc_request_cv.wait(l, [&] { return _gc_request || _gc_thread_stop; });
        }
        if (_gc_thread_stop) return;

        //        {
        //            std::stringstream out;
        //            out << "gc start " << '\n';
        //            std::cerr << out.str();
        //        }
        auto gcstart = std::chrono::high_resolution_clock::now();

        std::queue<Cell *> toVisit;

        auto visitAll = [&]() {
            while (!toVisit.empty()) {
                Cell *c = toVisit.front();
                toVisit.pop();

                if (c == nullptr) continue;
                if (c->_live) continue;
                c->_live = true;
                //
                //                {
                //                    std::stringstream out;
                //                    out << "processing c " << c << " " << static_cast<int>(c->_type) << "\n";
                //                    std::cerr << out.str();
                //                }

                if (auto cc = dynamic_cast<ConsCell *>(c)) {
                    //                                        {std::stringstream out; out  << "processing car " << cc._car << "\n"; std::cerr<<out.str();}
                    toVisit.emplace(cc->_car);
                    //                    {std::stringstream out; out  << "real car " << toVisit.back() << "\n"; std::cerr<<out.str();}
                    //                    {std::stringstream out; out  << "processing cdr " << cc._cdr << "\n"; std::cerr<<out.str();}
                    toVisit.emplace(cc->_cdr);
                    //                    {std::stringstream out; out  << "real cdr " << toVisit.back() << "\n"; std::cerr<<out.str();}
                }
            }
        };

        {
            auto start = std::chrono::high_resolution_clock::now();
            decltype(_new_roots) new_roots;
            {
                decltype(_temp_cells) temp_cells;
                {
                    std::lock_guard l(_new_roots_lock);
                    std::swap(new_roots, _new_roots);
                    std::swap(temp_cells, _temp_cells);
                }
                _cells.splice(_cells.end(), temp_cells);
            }


            for (auto const &r: new_roots) {
                //                {
                //                    std::stringstream out;
                //                    out << "processing new " << r.first << " diff " << r.second << "\n";
                //                    std::cerr << out.str();
                //                }
                if (r.second == 0) continue;
                _roots[r.first] += r.second;
                if (_roots[r.first] <= 0)
                    _roots.erase(r.first);
            }

            auto stop = std::chrono::high_resolution_clock::now();
            {
                std::stringstream out;
                out << "New roots time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "\n";
                std::cout << out.str();
            }
        }

        {
            auto start = std::chrono::high_resolution_clock::now();
            std::for_each(_cells.begin(), _cells.end(), [&](Cell *c) {
                c->_live = false;
            });

            for (const auto &r: _roots) {
                //                {
                //                    std::stringstream out;
                //                    out << "processing r " << r.first << " diff " << r.second << "\n";
                //                    std::cerr << out.str();
                //                }
                toVisit.emplace(r.first);
            }
            visitAll();
            auto stop = std::chrono::high_resolution_clock::now();
            //            {
            //                std::stringstream out;
            //                out << "Scanned " << _roots.size() << " roots" << std::endl;
            //                std::cerr << out.str();
            //            }
            {
                std::stringstream out;
                out << "Roots scan time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "\n";
                std::cout << out.str();
            }
        }

        {
            auto start = std::chrono::high_resolution_clock::now();
            decltype(_gc_dirty_notif_queue) dirtied;
            {
                std::lock_guard dql(_gc_dirty_notif_queue_lock);
                std::swap(dirtied, _gc_dirty_notif_queue);
            }
            while (!dirtied.empty()) {
                for (const auto &r: dirtied) {
                    //                    {
                    //                        std::stringstream out;
                    //                        out << "processing dirty " << r << "\n";
                    //                        std::cerr << out.str();
                    //                    }
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
            {
                std::stringstream out;
                out << "Dirty mark time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "\n";
                std::cout << out.str();
            }
        }
        {
            auto start = std::chrono::high_resolution_clock::now();

            uint64_t freed = 0;

            _cells.remove_if([&](Cell *l) {
                if (!l->_live) {
                    freed += 1;

                    //                    {
                    //                        std::stringstream out;
                    //                        out << "deleting: " << l << "\n";
                    //                        std::cerr << out.str();
                    //                    }

                    delete l;
                    return true;
                }
                return false;
            });

            auto stop = std::chrono::high_resolution_clock::now();
            {
                std::stringstream out;
                out << "Sweep time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "\n";
                std::cout << out.str();
            }
            _cells_num = _cells.size();
            {
                std::stringstream out;
                out << "GC Freed " << freed << " cells left: " << _cells_num << " \n";
                std::cout << out.str();
            }
        }

        auto gcstop = std::chrono::high_resolution_clock::now();
        {
            std::stringstream out;
            out << "GC total time: " << std::chrono::duration_cast<std::chrono::microseconds>(gcstop - gcstart).count() << "\n";
            std::cout << out.str();
        }


        {
            std::unique_lock l(_gc_done_m);
            std::unique_lock l2(_gc_request_m);
            _gc_done = true;
            _gc_request = false;
            _gc_done_cv.notify_all();
        }
    }
}