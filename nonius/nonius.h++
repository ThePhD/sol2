// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// This file was automatically generated on 2015-09-07T10:52:33.316574Z
// Do not edit it directly

#ifndef NONIUS_SINGLE_INCLUDE_HPP
#define NONIUS_SINGLE_INCLUDE_HPP

// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Single header root

// #included from: nonius.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Main header

#ifndef NONIUS_HPP
#define NONIUS_HPP

// #included from: clock.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Clocks

#define NONIUS_CLOCK_HPP

#if defined(_MSC_VER) && (_MSC_VER) < 1900
#if !defined(NONIUS_USE_BOOST_CHRONO)
#define NONIUS_USE_BOOST_CHRONO
#endif // NONIUS_USE_BOOST_CHRONO
#endif // MSVC <chrono> is borken and had little to no testing done before shipping (Dev14/VS15 CTP fixes it)

#if defined(NONIUS_USE_BOOST_CHRONO)
#include <boost/chrono.hpp>
#else
#include <chrono>
#include <ratio>
#endif

namespace nonius {
#if defined(NONIUS_USE_BOOST_CHRONO)
    namespace chrono = boost::chrono;
    template <unsigned Num, unsigned Den = 1>
    using ratio = boost::ratio<Num, Den>;
#else
    namespace chrono = std::chrono;
    template <unsigned Num, unsigned Den = 1>
    using ratio = std::ratio<Num, Den>;
#endif
    using milli = ratio<1,       1000>;
    using micro = ratio<1,    1000000>;
    using nano  = ratio<1, 1000000000>;

    template <typename Clock>
    using Duration = typename Clock::duration;
    template <typename Clock>
    using FloatDuration = chrono::duration<double, typename Clock::period>;

    template <typename Clock>
    using TimePoint = typename Clock::time_point;

    using default_clock = chrono::high_resolution_clock;

    template <typename Clock>
    struct now {
        TimePoint<Clock> operator()() const {
            return Clock::now();
        }
    };

    using fp_seconds = chrono::duration<double, ratio<1>>;
} // namespace nonius

// #included from: benchmark.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Benchmark

#define NONIUS_BENCHMARK_HPP

// #included from: configuration.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Run configuration

#define NONIUS_CONFIGURATION_HPP

#include <string>

namespace nonius {
    struct configuration {
    public:
        int samples = 100;
        double confidence_interval = 0.95;
        int resamples = 100000;
        std::string title = "benchmarks";
        std::string output_file;
        std::string reporter;
        bool list_benchmarks = false;
        bool list_reporters = false;
        bool no_analysis = false;
        bool verbose = false;
        bool summary = false;
        bool help = false;
    };
} // namespace nonius

// #included from: environment.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Environment information

#define NONIUS_ENVIRONMENT_HPP

// #included from: outlier_classification.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Outlier information

#define NONIUS_OUTLIERS_HPP

namespace nonius {
    struct outlier_classification {
        int samples_seen = 0;
        int low_severe = 0;     // more than 3 times IQR below Q1
        int low_mild = 0;       // 1.5 to 3 times IQR below Q1
        int high_mild = 0;      // 1.5 to 3 times IQR above Q3
        int high_severe = 0;    // more than 3 times IQR above Q3

        int total() const {
            return low_severe + low_mild + high_mild + high_severe;
        }
    };
} // namespace nonius

namespace nonius {
    template <typename Duration>
    struct environment_estimate {
        Duration mean;
        outlier_classification outliers;

        template <typename Duration2>
        operator environment_estimate<Duration2>() const {
            return { mean, outliers };
        }
    };
    template <typename Clock = default_clock>
    struct environment {
        environment_estimate<FloatDuration<Clock>> clock_resolution;
        environment_estimate<FloatDuration<Clock>> clock_cost;
        //estimate function_cost;
    };
} // namespace nonius

// #included from: execution_plan.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Execution plan

#define NONIUS_EXECUTION_PLAN_HPP

namespace nonius {
    template <typename Duration>
    struct execution_plan {
        int iterations_per_sample;
        Duration estimated_duration;

        template <typename Duration2>
        operator execution_plan<Duration2>() const {
            return { iterations_per_sample, estimated_duration };
        }
    };
} // namespace nonius

// #included from: chronometer.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// User-facing chronometer

#define NONIUS_CHRONOMETER_HPP

// #included from: detail/complete_invoke.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Invoke with a special case for void

#define NONIUS_DETAIL_COMPLETE_INVOKE_HPP

#include <type_traits>
#include <utility>

namespace nonius {
    namespace detail {
        template <typename T>
        struct complete_type { using type = T; };
        template <>
        struct complete_type<void> { struct type {}; };

        template <typename T>
        using CompleteType = typename complete_type<T>::type;

        template <typename Result>
        struct complete_invoker {
            template <typename Fun, typename... Args>
            static Result invoke(Fun&& fun, Args&&... args) {
                return std::forward<Fun>(fun)(std::forward<Args>(args)...);
            }
        };
        template <>
        struct complete_invoker<void> {
            template <typename Fun, typename... Args>
            static CompleteType<void> invoke(Fun&& fun, Args&&... args) {
                std::forward<Fun>(fun)(std::forward<Args>(args)...);
                return {};
            }
        };
        template <typename Sig>
        using ResultOf = typename std::result_of<Sig>::type;

        // invoke and not return void :(
        template <typename Fun, typename... Args>
        CompleteType<ResultOf<Fun(Args...)>> complete_invoke(Fun&& fun, Args&&... args) {
            return complete_invoker<ResultOf<Fun(Args...)>>::invoke(std::forward<Fun>(fun), std::forward<Args>(args)...);
        }

        template <typename> struct always_true : std::true_type {};
        struct is_callable_tester {
            template <typename Fun, typename... Args>
            always_true<decltype(std::declval<Fun>()(std::declval<Args>()...))> static test(int);
            template <typename...>
            std::false_type static test(...);
        };
        template <typename T>
        struct is_callable;
        template <typename Fun, typename... Args>
        struct is_callable<Fun(Args...)> : decltype(is_callable_tester::test<Fun, Args...>(0)) {};
    } // namespace detail
} // namespace nonius

namespace nonius {
    namespace detail {
        struct chronometer_concept {
            virtual void start() = 0;
            virtual void finish() = 0;
            virtual ~chronometer_concept() = default;
        };
        template <typename Clock>
        struct chronometer_model final : public chronometer_concept {
            void start() override { started = Clock::now(); }
            void finish() override { finished = Clock::now(); }

            TimePoint<Clock> started;
            TimePoint<Clock> finished;
        };
    } // namespace detail

    struct chronometer {
    public:
        template <typename Fun>
        void measure(Fun&& fun) { measure(std::forward<Fun>(fun), detail::is_callable<Fun(int)>()); }

        int runs() const { return k; }

        chronometer(detail::chronometer_concept& meter, int k)
        : impl(&meter), k(k) {}

    private:
        template <typename Fun>
        void measure(Fun&& fun, std::false_type) {
            measure([&fun](int) { fun(); });
        }
        template <typename Fun>
        void measure(Fun&& fun, std::true_type) {
            impl->start();
            for(int i = 0; i < k; ++i) fun(i);
            impl->finish();
        }

        detail::chronometer_concept* impl;
        int k;
    };
} // namespace nonius

// #included from: detail/measure.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Measure

#define NONIUS_DETAIL_MEASURE_HPP

// #included from: detail/timing.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Timing

#define NONIUS_DETAIL_TIMING_HPP

#include <tuple>
#include <type_traits>

namespace nonius {
    template <typename Duration, typename Result>
    struct timing {
        Duration elapsed;
        Result result;
        int iterations;
    };
    template <typename Clock, typename Sig>
    using TimingOf = timing<Duration<Clock>, detail::CompleteType<detail::ResultOf<Sig>>>;
} // namespace nonius

#include <utility>

namespace nonius {
    namespace detail {
        template <typename Clock = default_clock, typename Fun, typename... Args>
        TimingOf<Clock, Fun(Args...)> measure(Fun&& fun, Args&&... args) {
            auto start = Clock::now();
            auto&& r = detail::complete_invoke(fun, std::forward<Args>(args)...);
            auto end = Clock::now();
            auto delta = end - start;
            return { delta, std::forward<decltype(r)>(r), 1 };
        }
    } // namespace detail
} // namespace nonius

// #included from: detail/benchmark_function.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Dumb std::function implementation for consistent call overhead

#define NONIUS_DETAIL_BENCHMARK_FUNCTION_HPP

#include <type_traits>
#include <utility>
#include <memory>

namespace nonius {
    namespace detail {
        template <typename T>
        using Decay = typename std::decay<T>::type;
        template <typename T, typename U>
        struct is_related
        : std::is_same<Decay<T>, Decay<U>> {};

        struct benchmark_function {
        private:
            struct concept {
                virtual void call(chronometer meter) const = 0;
                virtual concept* clone() const = 0;
                virtual ~concept() = default;
            };
            template <typename Fun>
            struct model : public concept {
                model(Fun&& fun) : fun(std::move(fun)) {}
                model(Fun const& fun) : fun(fun) {}

                model<Fun>* clone() const override { return new model<Fun>(*this); }

                void call(chronometer meter) const override {
                    call(meter, is_callable<Fun(chronometer)>());
                }
                void call(chronometer meter, std::true_type) const {
                    fun(meter);
                }
                void call(chronometer meter, std::false_type) const {
                    meter.measure(fun);
                }

                Fun fun;
            };
        public:
            template <typename Fun,
                      typename std::enable_if<!is_related<Fun, benchmark_function>::value, int>::type = 0>
            benchmark_function(Fun&& fun)
            : f(new model<typename std::decay<Fun>::type>(std::forward<Fun>(fun))) {}

            benchmark_function(benchmark_function&& that)
            : f(std::move(that.f)) {}

            benchmark_function(benchmark_function const& that)
            : f(that.f->clone()) {}

            benchmark_function& operator=(benchmark_function&& that) {
                f = std::move(that.f);
                return *this;
            }

            benchmark_function& operator=(benchmark_function const& that) {
                f.reset(that.f->clone());
                return *this;
            }

            void operator()(chronometer meter) const { f->call(meter); }
        private:
            std::unique_ptr<concept> f;
        };
    } // namespace detail
} // namespace nonius

// #included from: detail/repeat.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// repeat algorithm

#define NONIUS_DETAIL_REPEAT_HPP

#include <type_traits>
#include <utility>

namespace nonius {
    namespace detail {
        template <typename Fun>
        struct repeater {
            void operator()(int k) const {
                for(int i = 0; i < k; ++i) {
                    fun();
                }
            }
            Fun fun;
        };
        template <typename Fun>
        repeater<typename std::decay<Fun>::type> repeat(Fun&& fun) {
            return { std::forward<Fun>(fun) };
        }
    } // namespace detail
} // namespace nonius

// #included from: detail/run_for_at_least.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Run a function for a minimum amount of time

#define NONIUS_RUN_FOR_AT_LEAST_HPP

// #included from: timeout_error.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Exception to be thrown when a process takes too long to run

#define NONIUS_TIMEOUT_ERROR_HPP

// #included from: detail/noexcept.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Cross-compiler noexcept support

#define NONIUS_DETAIL_NOEXCEPT_HPP

#ifdef _MSC_VER
#define NONIUS_NOEXCEPT throw()
#else
#define NONIUS_NOEXCEPT noexcept
#endif // _MSC_VER

#include <sstream>
#include <string>
#include <exception>

namespace nonius {
    struct timeout_error : virtual std::exception {
    public:
        timeout_error(int seed, int iters) {
            std::ostringstream ss;
            ss << "took too long to run; seed: " << seed << ", iters: " << iters;
            message = ss.str();
        }

        char const* what() const NONIUS_NOEXCEPT override {
            return message.c_str();
        }

    private:
        std::string message;
    };
} // namespace nonius

#include <utility>

namespace nonius {
    namespace detail {
        template <typename Clock = default_clock, typename Fun>
        TimingOf<Clock, Fun(int)> run_for_at_least(Duration<Clock> how_long, int seed, Fun&& fun) {
            auto iters = seed;
            auto start = Clock::now();
            while(true) {
                auto now = Clock::now();
                if(now - start > how_long * 10) {
                    throw timeout_error(seed, iters);
                }
                auto r = detail::measure<Clock>(fun, iters);
                if(r.elapsed >= how_long) {
                    return { r.elapsed, std::move(r.result), iters };
                }
                iters *= 2;
            }
        }
    } // namespace detail
} // namespace nonius

// #included from: detail/unique_name.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Unique name generator macro

#define NONIUS_DETAIL_UNIQUE_NAME_HPP

#define NONIUS_DETAIL_UNIQUE_NAME_LINE_CAT(name, id) NONIUS_ ## name ## _ ## id
#define NONIUS_DETAIL_UNIQUE_NAME_LINE(name, id) NONIUS_DETAIL_UNIQUE_NAME_LINE_CAT(name, id)
#ifdef __COUNTER__
#define NONIUS_DETAIL_UNIQUE_NAME(name) NONIUS_DETAIL_UNIQUE_NAME_LINE(name, __COUNTER__)
#else // __COUNTER__
#define NONIUS_DETAIL_UNIQUE_NAME(name) NONIUS_DETAIL_UNIQUE_NAME_LINE(name, __LINE__)
#endif // __COUNTER__

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <cmath>

namespace nonius {
    namespace detail {
        const auto warmup_iterations = 10000;
        const auto warmup_time = chrono::milliseconds(100);
        const auto minimum_ticks = 1000;
    } // namespace detail

    struct benchmark {
        benchmark(std::string name, detail::benchmark_function fun)
        : name(std::move(name)), fun(std::move(fun)) {}

        void operator()(chronometer meter) const {
            fun(meter);
        }

        template <typename Clock>
        execution_plan<FloatDuration<Clock>> prepare(configuration cfg, environment<FloatDuration<Clock>> env) const {
            auto min_time = env.clock_resolution.mean * detail::minimum_ticks;
            auto run_time = std::min(min_time, decltype(min_time)(detail::warmup_time));
            auto&& test = detail::run_for_at_least<Clock>(chrono::duration_cast<Duration<Clock>>(run_time), 1, [this](int k) {
                detail::chronometer_model<Clock> model;
                (*this)(chronometer(model, k));
            });
            int new_iters = static_cast<int>(std::ceil(min_time * test.iterations / test.elapsed));
            return { new_iters, test.elapsed / test.iterations * new_iters * cfg.samples };
        }

        template <typename Clock>
        std::vector<FloatDuration<Clock>> run(configuration cfg, environment<FloatDuration<Clock>> env, execution_plan<FloatDuration<Clock>> plan) const {
            // warmup a bit
            detail::run_for_at_least<Clock>(chrono::duration_cast<Duration<Clock>>(detail::warmup_time), detail::warmup_iterations, detail::repeat(now<Clock>{}));

            std::vector<FloatDuration<Clock>> times;
            times.reserve(cfg.samples);
            std::generate_n(std::back_inserter(times), cfg.samples, [this, env, plan]{
                    detail::chronometer_model<Clock> model;
                    (*this)(chronometer(model, plan.iterations_per_sample));
                    auto elapsed = model.finished - model.started;
                    auto sample_time = elapsed - env.clock_cost.mean;
                    if(sample_time < FloatDuration<Clock>::zero()) sample_time = FloatDuration<Clock>::zero();
                    return (sample_time / plan.iterations_per_sample);
            });
            return times;
        }

        std::string name;
        detail::benchmark_function fun;
    };

    using benchmark_registry = std::vector<benchmark>;

    inline benchmark_registry& global_benchmark_registry() {
        static benchmark_registry registry;
        return registry;
    }

    struct benchmark_registrar {
        template <typename Fun>
        benchmark_registrar(benchmark_registry& registry, std::string name, Fun&& registrant) {
            registry.emplace_back(std::move(name), std::forward<Fun>(registrant));
        }
    };
} // namespace nonius

#define NONIUS_BENCHMARK(name, ...) \
    namespace { static ::nonius::benchmark_registrar NONIUS_DETAIL_UNIQUE_NAME(benchmark_registrar) (::nonius::global_benchmark_registry(), name, __VA_ARGS__); }

// #included from: constructor.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Eric Tremblay <eric_tremblay89@hotmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Constructor and destructor helpers

#define NONIUS_CONSTRUCTOR_HPP

#include <type_traits>

namespace nonius {
    namespace detail {
        template <typename T, bool Destruct>
        struct object_storage
        {
            typedef typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type TStorage;

            object_storage() : data() {}

            object_storage(const object_storage& other)
            {
                new(&data) T(other.stored_object());
            }

            object_storage(object_storage&& other)
            {
                new(&data) T(std::move(other.stored_object()));
            }

            ~object_storage() { destruct_on_exit<T>(); }

            template <typename... Args>
            void construct(Args&&... args)
            {
                new (&data) T(std::forward<Args>(args)...);
            }

            template <bool AllowManualDestruction = !Destruct>
            typename std::enable_if<AllowManualDestruction>::type destruct()
            {
                stored_object().~T();
            }

        private:
            // If this is a constructor benchmark, destruct the underlying object
            template <typename U>
            void destruct_on_exit(typename std::enable_if<Destruct, U>::type* = 0) { destruct<true>(); }
            // Otherwise, don't
            template <typename U>
            void destruct_on_exit(typename std::enable_if<!Destruct, U>::type* = 0) { }

            T& stored_object()
            {
                return *static_cast<T*>(static_cast<void*>(&data));
            }

            TStorage data;
        };
    }

    template <typename T>
    using storage_for = detail::object_storage<T, true>;

    template <typename T>
    using destructable_object = detail::object_storage<T, false>;
}

// #included from: go.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Runner entry point

#define NONIUS_GO_HPP

// #included from: reporter.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Reporter interface

#define NONIUS_REPORTER_HPP

// #included from: sample_analysis.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Benchmark results

#define NONIUS_BENCHMARK_RESULTS_HPP

// #included from: estimate.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Statistics estimates

#define NONIUS_ESTIMATE_HPP

namespace nonius {
    template <typename Duration>
    struct estimate {
        Duration point;
        Duration lower_bound;
        Duration upper_bound;
        double confidence_interval;

        template <typename Duration2>
        operator estimate<Duration2>() const {
            return { point, lower_bound, upper_bound, confidence_interval };
        }
    };
} // namespace nonius

#include <algorithm>
#include <vector>
#include <string>
#include <iterator>

namespace nonius {
    template <typename Duration>
    struct sample_analysis {
        std::vector<Duration> samples;
        estimate<Duration> mean;
        estimate<Duration> standard_deviation;
        outlier_classification outliers;
        double outlier_variance;

        template <typename Duration2>
        operator sample_analysis<Duration2>() const {
            std::vector<Duration2> samples2;
            samples2.reserve(samples.size());
            std::transform(samples.begin(), samples.end(), std::back_inserter(samples2), [](Duration d) { return Duration2(d); });
            return {
                std::move(samples2),
                mean,
                standard_deviation,
                outliers,
                outlier_variance,
            };
        }
    };
} // namespace nonius

#include <vector>
#include <string>
#include <ios>
#include <ostream>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <exception>

namespace nonius {
    struct bad_stream : virtual std::exception {
        char const* what() const NONIUS_NOEXCEPT override {
            return "failed to open file";
        }
    };

    struct reporter {
    public:
        virtual ~reporter() = default;

        void configure(configuration& cfg) {
            if(cfg.output_file.empty()) {
                os = [&]() -> std::ostream& { return std::cout; };
            } else {
                auto ofs = std::make_shared<std::ofstream>(cfg.output_file);
                os = [ofs]() -> std::ostream& { return *ofs; };
            }
            report_stream().exceptions(std::ios::failbit);
            if(!report_stream()) throw bad_stream();
            do_configure(cfg);
        }

        void warmup_start() {
            do_warmup_start();
        }
        void warmup_end(int iterations) {
            do_warmup_end(iterations);
        }
        void estimate_clock_resolution_start() {
            do_estimate_clock_resolution_start();
        }
        void estimate_clock_resolution_complete(environment_estimate<fp_seconds> estimate) {
            do_estimate_clock_resolution_complete(estimate);
        }

        void estimate_clock_cost_start() {
            do_estimate_clock_cost_start();
        }
        void estimate_clock_cost_complete(environment_estimate<fp_seconds> estimate) {
            do_estimate_clock_cost_complete(estimate);
        }

        void suite_start() {
            do_suite_start();
        }
        void benchmark_start(std::string const& name) {
            do_benchmark_start(name);
        }

        void measurement_start(execution_plan<fp_seconds> plan) {
            do_measurement_start(plan);
        }
        void measurement_complete(std::vector<fp_seconds> const& samples) {
            do_measurement_complete(samples);
        }

        void analysis_start() {
            do_analysis_start();
        }
        void analysis_complete(sample_analysis<fp_seconds> const& analysis) {
            do_analysis_complete(analysis);
        }

        void benchmark_failure(std::exception_ptr error) {
            do_benchmark_failure(error);
        }
        void benchmark_complete() {
            do_benchmark_complete();
        }
        void suite_complete() {
            do_suite_complete();
        }

        virtual std::string description() = 0;

    private:
        virtual void do_configure(configuration& /*cfg*/) {}

        virtual void do_warmup_start() {}
        virtual void do_warmup_end(int /*iterations*/) {}

        virtual void do_estimate_clock_resolution_start() {}
        virtual void do_estimate_clock_resolution_complete(environment_estimate<fp_seconds> /*estimate*/) {}

        virtual void do_estimate_clock_cost_start() {}
        virtual void do_estimate_clock_cost_complete(environment_estimate<fp_seconds> /*estimate*/) {}

        virtual void do_suite_start() {}
        virtual void do_benchmark_start(std::string const& /*name*/) {}

        virtual void do_measurement_start(execution_plan<fp_seconds> /*plan*/) {}
        virtual void do_measurement_complete(std::vector<fp_seconds> const& /*samples*/) {}

        virtual void do_analysis_start() {} // TODO make generic?
        virtual void do_analysis_complete(sample_analysis<fp_seconds> const& /*analysis*/) {}

        virtual void do_benchmark_failure(std::exception_ptr /*error*/) {}
        virtual void do_benchmark_complete() {}
        virtual void do_suite_complete() {}

    protected:
        std::ostream& progress_stream() {
            return std::cout;
        }
        std::ostream& error_stream() {
            return std::cerr;
        }
        std::ostream& report_stream() {
            return os();
        }

    private:
        std::function<std::ostream&()> os;
    };

    using reporter_registry = std::unordered_map<std::string, std::unique_ptr<reporter>>;

    inline reporter_registry& global_reporter_registry() {
        static reporter_registry registry;
        return registry;
    }

    struct reporter_registrar {
        reporter_registrar(reporter_registry& registry, std::string name, reporter* registrant) {
            registry.emplace(std::move(name), std::unique_ptr<reporter>(registrant));
        }
    };
} // namespace nonius

#define NONIUS_REPORTER(name, ...) \
    namespace { static ::nonius::reporter_registrar NONIUS_DETAIL_UNIQUE_NAME(reporter_registrar) (::nonius::global_reporter_registry(), name, new __VA_ARGS__()); } \
    static_assert(true, "")

// #included from: reporters/standard_reporter.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Standard reporter

#define NONIUS_REPORTERS_STANDARD_REPORTER_HPP

// #included from: detail/pretty_print.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Pretty printing routines

#define NONIUS_PRETTY_PRINT_HPP

#include <ratio>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <memory>
#include <utility>

namespace nonius {
    namespace detail {
        inline double get_magnitude(fp_seconds secs) {
            if(secs.count() >= 2.e0) {
                return 1.e0;
            } else if(secs.count() >= 2.e-3) {
                return 1.e3;
            } else if(secs.count() >= 2.e-6) {
                return 1.e6;
            } else {
                return 1.e9;
            }
        }
        inline std::string units_for_magnitude(double magnitude) {
            if(magnitude <= 1.e0) return "s";
            else if(magnitude <= 1.e3) return "ms";
            else if(magnitude <= 1.e6) return "μs";
            else return "ns";
        }
        inline std::string pretty_duration(fp_seconds secs) {
            auto magnitude = get_magnitude(secs);
            auto units = units_for_magnitude(magnitude);
#ifdef _MSC_VER
            if(units == "μs") units = "us";
#endif
            std::ostringstream ss;
            ss << std::setprecision(ss.precision());
            ss << (secs.count() * magnitude) << ' ' << units;
            return ss.str();
        }
        inline std::string percentage(double d) {
            std::ostringstream ss;
            ss << std::setprecision(3);
            if(d != 0 && d < 1e-5) {
                ss << std::fixed;
                ss << 0.0001 << "%";
            } else {
                ss.unsetf(std::ios::floatfield);
                ss << (100. * d) << "%";
            }
            return ss.str();
        }
        inline std::string percentage_ratio(double part, double whole) {
            return percentage(part / whole);
        }
    } // namespace detail
} // namespace nonius

#include <ratio>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <memory>

namespace nonius {
    struct standard_reporter : reporter {
    private:
        std::string description() override {
            return "the standard reporter";
        }

        void do_configure(configuration& cfg) override {
            n_samples = cfg.samples;
            verbose = cfg.verbose;
            summary = cfg.summary;
            n_resamples = cfg.resamples;
        }

        void do_warmup_start() override {
            if(verbose) report_stream() << "warming up\n";
        }
        void do_estimate_clock_resolution_start() override {
            if(verbose) report_stream() << "estimating clock resolution\n";
        }
        void do_estimate_clock_resolution_complete(environment_estimate<fp_seconds> estimate) override {
            if(!summary) {
                if(!verbose) report_stream() << "clock resolution: ";
                print_environment_estimate(estimate, estimate.outliers.samples_seen + 2);
            }
        }

        void do_estimate_clock_cost_start() override {
            if(verbose) report_stream() << "estimating cost of a clock call\n";
        }
        void do_estimate_clock_cost_complete(environment_estimate<fp_seconds> estimate) override {
            if(verbose) print_environment_estimate(estimate, estimate.outliers.samples_seen);
        }

        void do_benchmark_start(std::string const& name) override {
            report_stream() << '\n';
            if(!summary) report_stream() << "benchmarking ";
            report_stream() << name << "\n";
            current = name;
        }

        void do_measurement_start(execution_plan<fp_seconds> plan) override {
            report_stream() << std::setprecision(7);
            report_stream().unsetf(std::ios::floatfield);
            if(!summary) report_stream() << "collecting " << n_samples << " samples, " << plan.iterations_per_sample << " iterations each, in estimated " << detail::pretty_duration(plan.estimated_duration) << "\n";
        }
        void do_analysis_start() override {
            if(verbose) report_stream() << "bootstrapping with " << n_resamples << " resamples\n";
        }
        void do_benchmark_failure(std::exception_ptr) override {
            error_stream() << current << " failed to run successfully\n";
            report_stream() << "benchmark aborted\n";
        }
        void do_analysis_complete(sample_analysis<fp_seconds> const& analysis) override {
            print_statistic_estimate("mean", analysis.mean);
            print_statistic_estimate("std dev", analysis.standard_deviation);
            if(!summary) print_outliers(analysis.outliers);
            if(verbose) report_stream() << "variance introduced by outliers: " << detail::percentage(analysis.outlier_variance) << "\n";
            const char* effect;
            if(analysis.outlier_variance < 0.01) {
                effect = "unaffected";
            } else if(analysis.outlier_variance < 0.1) {
                effect = "slightly inflated";
            } else if(analysis.outlier_variance < 0.5) {
                effect = "moderately inflated";
            } else {
                effect = "severely inflated";
            }
            report_stream() << "variance is " << effect << " by outliers\n";
        }

        void print_environment_estimate(environment_estimate<fp_seconds> e, int iterations) {
            report_stream() << std::setprecision(7);
            report_stream().unsetf(std::ios::floatfield);
            report_stream() << "mean is " << detail::pretty_duration(e.mean) << " (" << iterations << " iterations)\n";
            if(verbose) print_outliers(e.outliers);
        }
        void print_outlier_count(const char* description, int count, int total) {
            if(count > 0) report_stream() << "  " << count << " (" << detail::percentage_ratio(count, total) << ") " << description << "\n";
        }
        void print_outliers(outlier_classification o) {
            report_stream() << "found " << o.total() << " outliers among " << o.samples_seen << " samples (" << detail::percentage_ratio(o.total(), o.samples_seen) << ")\n";
            if(verbose) {
                print_outlier_count("low severe", o.low_severe, o.samples_seen);
                print_outlier_count("low mild", o.low_mild, o.samples_seen);
                print_outlier_count("high mild", o.high_mild, o.samples_seen);
                print_outlier_count("high severe", o.high_severe, o.samples_seen);
            }
        }
        void print_statistic_estimate(const char* name, estimate<fp_seconds> estimate) {
            report_stream() << std::setprecision(7);
            report_stream().unsetf(std::ios::floatfield);
            report_stream() << name << ": " << detail::pretty_duration(estimate.point);
            if(!summary) {
                report_stream() << ", lb " << detail::pretty_duration(estimate.lower_bound)
                         << ", ub " << detail::pretty_duration(estimate.upper_bound)
                         << ", ci " << std::setprecision(3) << estimate.confidence_interval;
            }
            report_stream() << "\n";
        }

        int n_samples = 0;
        int n_resamples = 0;
        bool verbose = false;
        bool summary = false;

        std::string current;
    };

    NONIUS_REPORTER("", standard_reporter);
    NONIUS_REPORTER("standard", standard_reporter);
} // namespace nonius

// #included from: detail/estimate_clock.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Environment measurement

#define NONIUS_DETAIL_ENVIRONMENT_HPP

// #included from: detail/stats.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014-2015 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Statistical analysis tools

#define NONIUS_DETAIL_ANALYSIS_HPP

#include <boost/math/distributions/normal.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>
#include <array>
#include <random>
#include <numeric>
#include <tuple>
#include <cmath>
#include <utility>
#include <future>
#include <cstddef>

namespace nonius {
    namespace detail {
        using sample = std::vector<double>;

        template <typename Iterator>
        double weighted_average_quantile(int k, int q, Iterator first, Iterator last) {
            auto count = last - first;
            double idx = (count - 1) * k /static_cast<double>(q);
            int j = static_cast<int>(idx);
            double g = idx - j;
            std::nth_element(first, first+j, last);
            auto xj = first[j];
            if(g == 0) return xj;

            auto xj1 = *std::min_element(first+(j+1), last);
            return xj + g * (xj1 - xj);
        }

        template <typename Iterator>
        outlier_classification classify_outliers(Iterator first, Iterator last) {
            std::vector<double> copy(first, last);

            auto q1 = weighted_average_quantile(1, 4, copy.begin(), copy.end());
            auto q3 = weighted_average_quantile(3, 4, copy.begin(), copy.end());
            auto iqr = q3 - q1;
            auto los = q1 - (iqr * 3.);
            auto lom = q1 - (iqr * 1.5);
            auto him = q3 + (iqr * 1.5);
            auto his = q3 + (iqr * 3.);

            outlier_classification o;
            for(; first != last; ++first) {
                auto&& t = *first;
                if(t < los) ++o.low_severe;
                else if(t < lom) ++o.low_mild;
                else if(t > his) ++o.high_severe;
                else if(t > him) ++o.high_mild;
                ++o.samples_seen;
            }
            return o;
        }

        template <typename Iterator>
        double mean(Iterator first, Iterator last) {
            auto count = last - first;
            double sum = std::accumulate(first, last, 0.);
            return sum / count;
        }

        template <typename Iterator>
        double standard_deviation(Iterator first, Iterator last) {
            auto m = mean(first, last);
            double variance = std::accumulate(first, last, 0., [m](double a, double b) {
                        double diff = b - m;
                        return a + diff*diff;
                    }) / (last - first);
            return std::sqrt(variance);
        }

        template <typename URng, typename Iterator, typename Estimator>
        sample resample(URng& rng, int resamples, Iterator first, Iterator last, Estimator& estimator) {
            auto n = last - first;
            std::uniform_int_distribution<decltype(n)> dist(0, n-1);

            sample out;
            out.reserve(resamples);
            std::generate_n(std::back_inserter(out), resamples, [n, first, &estimator, &dist, &rng] {
                std::vector<double> resampled;
                resampled.reserve(n);
                std::generate_n(std::back_inserter(resampled), n, [first, &dist, &rng] { return first[dist(rng)]; });
                return estimator(resampled.begin(), resampled.end());
            });
            std::sort(out.begin(), out.end());
            return out;
        }

        template <typename Estimator, typename Iterator>
        sample jackknife(Estimator&& estimator, Iterator first, Iterator last) {
            auto n = last - first;
            auto second = std::next(first);
            sample results;
            results.reserve(n);

            for(auto it = first; it != last; ++it) {
                std::iter_swap(it, first);
                results.push_back(estimator(second, last));
            }

            return results;
        }

        template <typename Iterator, typename Estimator>
        estimate<double> bootstrap(double confidence_level, Iterator first, Iterator last, sample const& resample, Estimator&& estimator) {
            namespace bm = boost::math;

            auto n_samples = last - first;

            double point = estimator(first, last);
            // Degenerate case with a single sample
            if(n_samples == 1) return { point, point, point, confidence_level };

            sample jack = jackknife(estimator, first, last);
            double jack_mean = mean(jack.begin(), jack.end());
            double sum_squares, sum_cubes;
            std::tie(sum_squares, sum_cubes) = std::accumulate(jack.begin(), jack.end(), std::make_pair(0., 0.), [jack_mean](std::pair<double, double> sqcb, double x) -> std::pair<double, double> {
                        auto d = jack_mean - x;
                        auto d2 = d * d;
                        auto d3 = d2 * d;
                        return { sqcb.first + d2, sqcb.second + d3 };
                    });

            double accel = sum_cubes / (6 * std::pow(sum_squares, 1.5));
            int n = static_cast<int>(resample.size());
            double prob_n = std::count_if(resample.begin(), resample.end(), [point](double x) { return x < point; }) /(double) n;
            // degenerate case with uniform samples
            if(prob_n == 0) return { point, point, point, confidence_level };

            double bias = bm::quantile(bm::normal{}, prob_n);
            double z1 = bm::quantile(bm::normal{}, (1. - confidence_level) / 2.);

            auto cumn = [n](double x) -> int { return std::lround(bm::cdf(bm::normal{}, x) * n); };
            auto a = [bias, accel](double b) { return bias + b / (1. - accel * b); };
            double b1 = bias + z1;
            double b2 = bias - z1;
            double a1 = a(b1);
            double a2 = a(b2);
            auto lo = std::max(cumn(a1), 0);
            auto hi = std::min(cumn(a2), n - 1);

            return { point, resample[lo], resample[hi], confidence_level };
        }

        inline double outlier_variance(estimate<double> mean, estimate<double> stddev, int n) {
            double sb = stddev.point;
            double mn = mean.point / n;
            double mg_min = mn / 2.;
            double sg = std::min(mg_min / 4., sb / std::sqrt(n));
            double sg2 = sg * sg;
            double sb2 = sb * sb;

            auto c_max = [n, mn, sb2, sg2](double x) -> double {
                double k = mn - x;
                double d = k * k;
                double nd = n * d;
                double k0 = -n * nd;
                double k1 = sb2 - n * sg2 + nd;
                double det = k1 * k1 - 4 * sg2 * k0;
                return (int)(-2. * k0 / (k1 + std::sqrt(det)));
            };

            auto var_out = [n, sb2, sg2](double c) {
                double nc = n - c;
                return (nc / n) * (sb2 - nc * sg2);
            };

            return std::min(var_out(1), var_out(std::min(c_max(0.), c_max(mg_min)))) / sb2;
        }

        struct bootstrap_analysis {
            estimate<double> mean;
            estimate<double> standard_deviation;
            double outlier_variance;
        };

        template <typename Iterator>
        bootstrap_analysis analyse_samples(double confidence_level, int n_resamples, Iterator first, Iterator last) {
            static std::random_device entropy;

            auto n = static_cast<int>(last - first); // seriously, one can't use integral types without hell in C++

            auto mean = &detail::mean<Iterator>;
            auto stddev = &detail::standard_deviation<Iterator>;

            auto estimate = [=](double(*f)(Iterator, Iterator)) {
                auto seed = entropy();
                return std::async(std::launch::async, [=]{
                    std::mt19937 rng(seed);
                    auto resampled = resample(rng, n_resamples, first, last, f);
                    return bootstrap(confidence_level, first, last, resampled, f);
                });
            };

            auto mean_future = estimate(mean);
            auto stddev_future = estimate(stddev);

            auto mean_estimate = mean_future.get();
            auto stddev_estimate = stddev_future.get();

            double outlier_variance = detail::outlier_variance(mean_estimate, stddev_estimate, n);

            return { mean_estimate, stddev_estimate, outlier_variance };
        }
    } // namespace detail
} // namespace nonius

#include <algorithm>
#include <iterator>
#include <tuple>
#include <vector>
#include <cmath>

namespace nonius {
    namespace detail {
        template <typename Clock>
        std::vector<double> resolution(int k) {
            std::vector<TimePoint<Clock>> times;
            times.reserve(k+1);
            std::generate_n(std::back_inserter(times), k+1, now<Clock>{});

            std::vector<double> deltas;
            deltas.reserve(k);
            std::transform(std::next(times.begin()), times.end(), times.begin(),
                              std::back_inserter(deltas),
                              [](TimePoint<Clock> a, TimePoint<Clock> b) { return static_cast<double>((a - b).count()); });

            return deltas;
        }

        const auto warmup_seed = 10000;
        const auto clock_resolution_estimation_time = chrono::milliseconds(500);
        const auto clock_cost_estimation_time_limit = chrono::seconds(1);
        const auto clock_cost_estimation_tick_limit = 100000;
        const auto clock_cost_estimation_time = chrono::milliseconds(10);
        const auto clock_cost_estimation_iterations = 10000;

        template <typename Clock>
        int warmup() {
            return run_for_at_least<Clock>(chrono::duration_cast<Duration<Clock>>(warmup_time), warmup_seed, &resolution<Clock>)
                    .iterations;
        }
        template <typename Clock>
        environment_estimate<FloatDuration<Clock>> estimate_clock_resolution(int iterations) {
            auto r = run_for_at_least<Clock>(chrono::duration_cast<Duration<Clock>>(clock_resolution_estimation_time), iterations, &resolution<Clock>)
                    .result;
            return {
                FloatDuration<Clock>(mean(r.begin(), r.end())),
                classify_outliers(r.begin(), r.end()),
            };
        }
        template <typename Clock>
        environment_estimate<FloatDuration<Clock>> estimate_clock_cost(FloatDuration<Clock> resolution) {
            auto time_limit = std::min(resolution * clock_cost_estimation_tick_limit, FloatDuration<Clock>(clock_cost_estimation_time_limit));
            auto time_clock = [](int k) {
                return detail::measure<Clock>([k]{
                    for(int i = 0; i < k; ++i) {
                        volatile auto ignored = Clock::now();
                        (void)ignored;
                    }
                }).elapsed;
            };
            time_clock(1);
            int iters = clock_cost_estimation_iterations;
            auto&& r = run_for_at_least<Clock>(chrono::duration_cast<Duration<Clock>>(clock_cost_estimation_time), iters, time_clock);
            std::vector<double> times;
            int nsamples = static_cast<int>(std::ceil(time_limit / r.elapsed));
            times.reserve(nsamples);
            std::generate_n(std::back_inserter(times), nsamples, [time_clock, &r]{
                        return (time_clock(r.iterations) / r.iterations).count();
                    });
            return {
                FloatDuration<Clock>(mean(times.begin(), times.end())),
                classify_outliers(times.begin(), times.end()),
            };
        }
    } // namespace detail
} // namespace nonius

// #included from: detail/analyse.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Run and analyse one benchmark

#define NONIUS_DETAIL_ANALYSE_HPP

#include <algorithm>
#include <iterator>
#include <vector>

namespace nonius {
    namespace detail {
        template <typename Duration, typename Iterator>
        sample_analysis<Duration> analyse(configuration cfg, environment<Duration>, Iterator first, Iterator last) {
            std::vector<double> samples;
            samples.reserve(last - first);
            std::transform(first, last, std::back_inserter(samples), [](Duration d) { return d.count(); });

            auto analysis = nonius::detail::analyse_samples(cfg.confidence_interval, cfg.resamples, samples.begin(), samples.end());
            auto outliers = nonius::detail::classify_outliers(samples.begin(), samples.end());

            auto wrap_estimate = [](estimate<double> e) {
                return estimate<Duration> {
                    Duration(e.point),
                    Duration(e.lower_bound),
                    Duration(e.upper_bound),
                    e.confidence_interval,
                };
            };
            std::vector<Duration> samples2;
            samples2.reserve(samples.size());
            std::transform(samples.begin(), samples.end(), std::back_inserter(samples2), [](double d) { return Duration(d); });
            return {
                std::move(samples2),
                wrap_estimate(analysis.mean),
                wrap_estimate(analysis.standard_deviation),
                outliers,
                analysis.outlier_variance,
            };
        }
    } // namespace detail
} // namespace nonius

#include <set>
#include <exception>
#include <iostream>
#include <utility>

namespace nonius {
    namespace detail {
        template <typename Clock>
        environment<FloatDuration<Clock>> measure_environment(reporter& rep) {
            rep.warmup_start();
            auto iters = detail::warmup<Clock>();
            rep.warmup_end(iters);

            rep.estimate_clock_resolution_start();
            auto resolution = detail::estimate_clock_resolution<Clock>(iters);
            rep.estimate_clock_resolution_complete(resolution);

            rep.estimate_clock_cost_start();
            auto cost = detail::estimate_clock_cost<Clock>(resolution.mean);
            rep.estimate_clock_cost_complete(cost);

            return { resolution, cost };
        }
    } // namespace detail

    struct benchmark_user_error : virtual std::exception {
        char const* what() const NONIUS_NOEXCEPT override {
            return "a benchmark failed to run successfully";
        }
    };

    template <typename Fun>
    detail::CompleteType<detail::ResultOf<Fun()>> user_code(reporter& rep, Fun&& fun) {
        try {
            return detail::complete_invoke(std::forward<Fun>(fun));
        } catch(...) {
            rep.benchmark_failure(std::current_exception());
            throw benchmark_user_error();
        }
    }

    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last, reporter& rep) {
        rep.configure(cfg);

        auto env = detail::measure_environment<Clock>(rep);

        rep.suite_start();

        for(; first != last; ++first) {
            try {
                rep.benchmark_start(first->name);

                auto plan = user_code(rep, [&first, &cfg, &env]{ return first->template prepare<Clock>(cfg, env); });
                rep.measurement_start(plan);
                auto samples = user_code(rep, [&first, &cfg, &env, &plan]{ return first->template run<Clock>(cfg, env, plan); });
                rep.measurement_complete(std::vector<fp_seconds>(samples.begin(), samples.end()));

                if(!cfg.no_analysis) {
                    rep.analysis_start();
                    auto analysis = detail::analyse(cfg, env, samples.begin(), samples.end());
                    rep.analysis_complete(analysis);
                }

                rep.benchmark_complete();
            } catch(benchmark_user_error const&) {
                continue;
            }
        }

        rep.suite_complete();
    }
    struct duplicate_benchmarks : virtual std::exception {
        char const* what() const NONIUS_NOEXCEPT override {
            return "two or more benchmarks with the same name were registered";
        }
    };
    template <typename Clock = default_clock, typename Iterator>
    void validate_benchmarks(Iterator first, Iterator last) {
        struct strings_lt_through_pointer {
            bool operator()(std::string* a, std::string* b) const { return *a < *b; };
        };
        std::set<std::string*, strings_lt_through_pointer> names;
        for(; first != last; ++first) {
            if(!names.insert(&first->name).second)
                throw duplicate_benchmarks();
        }
    }
    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last, reporter&& rep) {
        go(cfg, first, last, rep);
    }
    struct no_such_reporter : virtual std::exception {
        char const* what() const NONIUS_NOEXCEPT override {
            return "reporter could not be found";
        }
    };
    template <typename Clock = default_clock>
    void go(configuration cfg, benchmark_registry& benchmarks = global_benchmark_registry(), reporter_registry& reporters = global_reporter_registry()) {
        auto it = reporters.find(cfg.reporter);
        if(it == reporters.end()) throw no_such_reporter();
        validate_benchmarks(benchmarks.begin(), benchmarks.end());
        go(cfg, benchmarks.begin(), benchmarks.end(), *it->second);
    }
} // namespace nonius

#ifndef NONIUS_DISABLE_EXTRA_REPORTERS
#ifndef NONIUS_DISABLE_CSV_REPORTER
// #included from: reporters/csv_reporter.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// CSV raw data reporter

#define NONIUS_REPORTERS_CSV_REPORTER_HPP

#include <ios>
#include <iomanip>
#include <algorithm>
#include <string>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <vector>

namespace nonius {
    struct csv_reporter : reporter {
    private:
        std::string description() override {
            return "outputs samples to a CSV file";
        }

        void do_configure(configuration& cfg) override {
            cfg.no_analysis = true;
            n_samples = cfg.samples;
            verbose = cfg.verbose;
        }

        void do_warmup_start() override {
            if(verbose) progress_stream() << "warming up\n";
        }
        void do_estimate_clock_resolution_start() override {
            if(verbose) progress_stream() << "estimating clock resolution\n";
        }
        void do_estimate_clock_cost_start() override {
            if(verbose) progress_stream() << "estimating cost of a clock call\n";
        }

        void do_benchmark_start(std::string const& name) override {
            if(verbose) progress_stream() << "\nbenchmarking " << name << "\n";
            current = name;
        }

        void do_measurement_start(execution_plan<fp_seconds> plan) override {
            report_stream() << std::setprecision(7);
            report_stream().unsetf(std::ios::floatfield);
            if(verbose) progress_stream() << "collecting " << n_samples << " samples, " << plan.iterations_per_sample << " iterations each, in estimated " << detail::pretty_duration(plan.estimated_duration) << "\n";
        }
        void do_measurement_complete(std::vector<fp_seconds> const& samples) override {
            data[current] = samples;
        }

        void do_benchmark_failure(std::exception_ptr) override {
            error_stream() << current << " failed to run successfully\n";
        }

        void do_suite_complete() override {
            if(verbose) progress_stream() << "\ngenerating CSV report\n";
            report_stream() << std::fixed;
            report_stream().precision(std::numeric_limits<double>::digits10);
            bool first = true;
            for(auto&& kv : data) {
                if(!first) report_stream() << ",";
                report_stream() << "\"" << escape(kv.first) << "\""; // TODO escape
                first = false;
            }
            report_stream() << "\n";
            for(int i = 0; i < n_samples; ++i) {
                first = true;
                for(auto&& kv : data) {
                    if(!first) report_stream() << ",";
                    report_stream() << kv.second[i].count();
                    first = false;
                }
                report_stream() << "\n";
            }
            if(verbose) progress_stream() << "done\n";
        }

    private:
        static std::string escape(std::string const& source) {
            auto first = source.begin();
            auto last = source.end();

            auto quotes = std::count(first, last, '"');
            if(quotes == 0) return source;

            std::string escaped;
            escaped.reserve(source.size() + quotes);

            while(first != last) {
                auto next_quote = std::find(first, last, '"');
                std::copy(first, next_quote, std::back_inserter(escaped));
                first = next_quote;
                if(first != last) {
                    ++first;
                    escaped.push_back('"');
                    escaped.push_back('"');
                }
            }

            return escaped;
        }

        int n_samples;
        bool verbose;
        std::string current;
        std::unordered_map<std::string, std::vector<fp_seconds>> data;
    };

    NONIUS_REPORTER("csv", csv_reporter);
} // namespace nonius

#endif // NONIUS_DISABLE_CSV_REPORTER
#ifndef NONIUS_DISABLE_JUNIT_REPORTER
// #included from: reporters/junit_reporter.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// JUnit reporter

#define NONIUS_REPORTERS_JUNIT_REPORTER_HPP

// #included from: detail/escape.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// General escaping routines

#define NONIUS_DETAIL_ESCAPE_HPP

#include <string>
#include <algorithm>
#include <iterator>
#include <utility>

namespace nonius {
    namespace detail {
        inline std::string escape(std::string const& source, std::unordered_map<char, std::string> const& escapes) {
            std::string magic;
            magic.reserve(escapes.size());
            std::transform(escapes.begin(), escapes.end(), std::back_inserter(magic), [](std::pair<char const, std::string> const& p) { return p.first; });

            auto first = source.begin();
            auto last = source.end();

            auto n_magic = std::count_if(first, last, [&magic](char c) { return magic.find(c) != std::string::npos; });

            std::string escaped;
            escaped.reserve(source.size() + n_magic*6);

            while(first != last) {
                auto next_magic = std::find_first_of(first, last, magic.begin(), magic.end());
                std::copy(first, next_magic, std::back_inserter(escaped));
                first = next_magic;
                if(first != last) {
                    auto it = escapes.find(*first);
                    if(it != escapes.end()) {
                        escaped += it->second;
                    }
                    ++first;
                }
            }
            return escaped;
        }
    } // namespace detail
} // namespace nonius

#include <ios>
#include <iomanip>
#include <algorithm>
#include <string>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <vector>
#include <exception>

namespace nonius {
    struct junit_reporter : reporter {
    private:
        std::string description() override {
            return "outputs results to a JUnit-compatible XML file";
        }

        void do_configure(configuration& cfg) override {
            n_samples = cfg.samples;
            confidence_interval = cfg.confidence_interval;
            resamples = cfg.resamples;
            verbose = cfg.verbose;
            title = cfg.title;
        }

        struct result {
            sample_analysis<fp_seconds> analysis;
            std::exception_ptr failure;
        };

        void do_warmup_start() override {
            if(verbose) progress_stream() << "warming up\n";
        }
        void do_estimate_clock_resolution_start() override {
            if(verbose) progress_stream() << "estimating clock resolution\n";
        }
        void do_estimate_clock_cost_start() override {
            if(verbose) progress_stream() << "estimating cost of a clock call\n";
        }

        void do_benchmark_start(std::string const& name) override {
            if(verbose) progress_stream() << "\nbenchmarking " << name << "\n";
            current = name;
        }

        void do_measurement_start(execution_plan<fp_seconds> plan) override {
            report_stream() << std::setprecision(7);
            report_stream().unsetf(std::ios::floatfield);
            if(verbose) progress_stream() << "collecting " << n_samples << " samples, " << plan.iterations_per_sample << " iterations each, in estimated " << detail::pretty_duration(plan.estimated_duration) << "\n";
        }

        void do_analysis_start() override {
            if(verbose) report_stream() << "analysing samples\n";
        }
        void do_analysis_complete(sample_analysis<fp_seconds> const& analysis) override {
            data[current] = { analysis, nullptr };
        }

        void do_benchmark_failure(std::exception_ptr e) override {
            data[current] = { sample_analysis<fp_seconds>(), e };
            error_stream() << current << " failed to run successfully\n";
        }

        void do_suite_complete() override {
            if(verbose) progress_stream() << "\ngenerating JUnit report\n";

            report_stream() << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
            report_stream() << "<testsuite name=\"" << escape(title) << "\" tests=\"" << data.size() << "\"";
            auto failures = std::count_if(data.begin(), data.end(),
                    [](std::pair<std::string const&, result> const& p) {
                        return static_cast<bool>(p.second.failure);
                    });
            if(failures > 0) report_stream() << " errors=\"" << failures << "\"";
            report_stream() << ">\n";

            report_stream() << " <properties>\n";
            report_stream() << "  <property name=\"samples\" value=\"" << n_samples << "\">\n";
            report_stream() << "  <property name=\"confidence_interval\" value=\"" << std::setprecision(3) << confidence_interval << "\">\n";
            report_stream() << "  <property name=\"resamples\" value=\"" << resamples << "\">\n";
            report_stream() << " </properties>\n";

            for(auto tc : data) {
                report_stream() << " <testcase name=\"" << escape(tc.first) << "\"";
                if(tc.second.failure) {
                    report_stream() << ">\n";
                    try {
                        std::rethrow_exception(tc.second.failure);
                    } catch(std::exception const& e) {
                        report_stream() << "  <error message=\"" << escape(e.what()) << "\" />\n";
                    } catch(...) {
                        report_stream() << "  <error message=\"unknown error\" />\n";
                    }
                    report_stream() << " </testcase>\n";
                } else {
                    report_stream() << std::fixed;
                    report_stream().precision(std::numeric_limits<double>::digits10);
                    report_stream() << " time=\"" << tc.second.analysis.mean.point.count() << "\" />\n";
                }
            }

            report_stream() << "</testsuite>\n";

            report_stream() << std::flush;

            if(verbose) progress_stream() << "done\n";
        }

        static std::string escape(std::string const& source) {
            static const std::unordered_map<char, std::string> escapes {
                { '\'', "&apos;" },
                { '"',  "&quot;" },
                { '<',  "&lt;"   },
                { '>',  "&gt;"   },
                { '&',  "&amp;"  },
            };
            return detail::escape(source, escapes);
        }

        int n_samples;
        double confidence_interval;
        int resamples;
        bool verbose;
        std::string title;

        std::string current;
        std::unordered_map<std::string, result> data;
    };

    NONIUS_REPORTER("junit", junit_reporter);
} // namespace nonius

#endif // NONIUS_DISABLE_JUNIT_REPORTER
#ifndef NONIUS_DISABLE_HTML_REPORTER
// #included from: reporters/html_reporter.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// HTML single-chart reporter

#define NONIUS_REPORTERS_HTML_ALL_REPORTER_HPP

// #included from: detail/cpptempl.h
// cpptempl
// =================
// This is a template engine for C++.
//
// Syntax
// =================
// Variables: {$variable_name}
// Loops: {% for person in people %}Name: {$person.name}{% endfor %}
// If: {% for person.name == "Bob" %}Full name: Robert{% endif %}
//
// Copyright
// ==================
// Copyright (c) Ryan Ginstrom
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Modified by: Martinho Fernandes
//
// Usage
// =======================
//     std::string text = "{% if item %}{$item}{% endif %}\n"
//      "{% if thing %}{$thing}{% endif %}" ;
//  cpptempl::data_map data ;
//  data["item"] = cpptempl::make_data("aaa") ;
//  data["thing"] = cpptempl::make_data("bbb") ;
//
//     std::string result = cpptempl::parse(text, data) ;
//
// Handy Functions
// ========================
// make_data() : Feed it a string, data_map, or data_list to create a data entry.
// Example:
//  data_map person ;
//  person["name"] = make_data("Bob") ;
//  person["occupation"] = make_data("Plumber") ;
//  data_map data ;
//  data["person"] = make_data(person) ;
//     std::string result = parse(templ_text, data) ;

#ifndef CPPTEMPL_H
#define CPPTEMPL_H

#ifdef _MSC_VER
#pragma warning( disable : 4996 ) // 'std::copy': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
#pragma warning( disable : 4512 ) // 'std::copy': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
#endif

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>
#include <boost/lexical_cast.hpp>

#include <ostream>

#include <sstream>
#include <boost/algorithm/string.hpp>

namespace cpptempl
{
    // various typedefs

    // data classes
    class Data ;
    class DataValue ;
    class DataList ;
    class DataMap ;

    class data_ptr {
    public:
        data_ptr() {}
        template<typename T> data_ptr(const T& data) {
            this->operator =(data);
        }
        data_ptr(DataValue* data);
        data_ptr(DataList* data);
        data_ptr(DataMap* data);
        data_ptr(const data_ptr& data) {
            ptr = data.ptr;
        }
        template<typename T> void operator = (const T& data);
        void push_back(const data_ptr& data);
        virtual ~data_ptr() {}
        Data* operator ->() {
            return ptr.get();
        }
    private:
        std::shared_ptr<Data> ptr;
    };
    typedef std::vector<data_ptr> data_list ;

    class data_map {
    public:
        data_ptr& operator [](const std::string& key);
        bool empty();
        bool has(const std::string& key);
    private:
        std::unordered_map<std::string, data_ptr> data;
    };

    template<> inline void data_ptr::operator = (const data_ptr& data);
    template<> void data_ptr::operator = (const std::string& data);
    template<> void data_ptr::operator = (const std::string& data);
    template<> void data_ptr::operator = (const data_map& data);
    template<typename T>
    void data_ptr::operator = (const T& data) {
        std::string data_str = boost::lexical_cast<std::string>(data);
        this->operator =(data_str);
    }

    // token classes
    class Token ;
    typedef std::shared_ptr<Token> token_ptr ;
    typedef std::vector<token_ptr> token_vector ;

    // Custom exception class for library errors
    class TemplateException : public std::exception
    {
    public:
        TemplateException(std::string reason) : m_reason(std::move(reason)){}
        char const* what() const NONIUS_NOEXCEPT override {
            return m_reason.c_str();
        }
    private:
        std::string m_reason;
    };

    // Data types used in templates
    class Data
    {
    public:
        virtual bool empty() = 0 ;
        virtual std::string getvalue();
        virtual data_list& getlist();
        virtual data_map& getmap() ;
    };

    class DataValue : public Data
    {
        std::string m_value ;
    public:
        DataValue(std::string value) : m_value(value){}
        std::string getvalue();
        bool empty();
    };

    class DataList : public Data
    {
        data_list m_items ;
    public:
        DataList(const data_list &items) : m_items(items){}
        data_list& getlist() ;
        bool empty();
    };

    class DataMap : public Data
    {
        data_map m_items ;
    public:
        DataMap(const data_map &items) : m_items(items){}
        data_map& getmap();
        bool empty();
    };

    inline data_ptr::data_ptr(DataValue* data) : ptr(data) {}
    inline data_ptr::data_ptr(DataList* data) : ptr(data) {}
    inline data_ptr::data_ptr(DataMap* data) : ptr(data) {}

    // convenience functions for making data objects
    inline data_ptr make_data(std::string val)
    {
        return data_ptr(new DataValue(val)) ;
    }
    inline data_ptr make_data(data_list &val)
    {
        return data_ptr(new DataList(val)) ;
    }
    inline data_ptr make_data(data_map &val)
    {
        return data_ptr(new DataMap(val)) ;
    }
    // get a data value from a data map
    // e.g. foo.bar => data["foo"]["bar"]
    data_ptr parse_val(std::string key, data_map &data) ;

    typedef enum
    {
        TOKEN_TYPE_NONE,
        TOKEN_TYPE_TEXT,
        TOKEN_TYPE_VAR,
        TOKEN_TYPE_IF,
        TOKEN_TYPE_FOR,
        TOKEN_TYPE_ENDIF,
        TOKEN_TYPE_ENDFOR,
    } TokenType;

    // Template tokens
    // base class for all token types
    class Token
    {
    public:
        virtual TokenType gettype() = 0 ;
        virtual void gettext(std::ostream &stream, data_map &data) = 0 ;
        virtual void set_children(token_vector &children);
        virtual token_vector & get_children();
    };

    // normal text
    class TokenText : public Token
    {
        std::string m_text ;
    public:
        TokenText(std::string text) : m_text(text){}
        TokenType gettype();
        void gettext(std::ostream &stream, data_map &data);
    };

    // variable
    class TokenVar : public Token
    {
        std::string m_key ;
    public:
        TokenVar(std::string key) : m_key(key){}
        TokenType gettype();
        void gettext(std::ostream &stream, data_map &data);
    };

    // for block
    class TokenFor : public Token
    {
    public:
        std::string m_key ;
        std::string m_val ;
        token_vector m_children ;
        TokenFor(std::string expr);
        TokenType gettype();
        void gettext(std::ostream &stream, data_map &data);
        void set_children(token_vector &children);
        token_vector &get_children();
    };

    // if block
    class TokenIf : public Token
    {
    public:
        std::string m_expr ;
        token_vector m_children ;
        TokenIf(std::string expr) : m_expr(expr){}
        TokenType gettype();
        void gettext(std::ostream &stream, data_map &data);
        bool is_true(std::string expr, data_map &data);
        void set_children(token_vector &children);
        token_vector &get_children();
    };

    // end of block
    class TokenEnd : public Token // end of control block
    {
        std::string m_type ;
    public:
        TokenEnd(std::string text) : m_type(text){}
        TokenType gettype();
        void gettext(std::ostream &stream, data_map &data);
    };

    std::string gettext(token_ptr token, data_map &data) ;

    void parse_tree(token_vector &tokens, token_vector &tree, TokenType until=TOKEN_TYPE_NONE) ;
    token_vector & tokenize(std::string text, token_vector &tokens) ;

    // The big daddy. Pass in the template and data,
    // and get out a completed doc.
    void parse(std::ostream &stream, std::string templ_text, data_map &data) ;
    std::string parse(std::string templ_text, data_map &data);
    std::string parse(std::string templ_text, data_map &data);

// *********** Implementation ************

    //////////////////////////////////////////////////////////////////////////
    // Data classes
    //////////////////////////////////////////////////////////////////////////

    // data_map
    inline data_ptr& data_map::operator [](const std::string& key) {
        return data[key];
    }
    inline bool data_map::empty() {
        return data.empty();
    }
    inline bool data_map::has(const std::string& key) {
        return data.find(key) != data.end();
    }

    // data_ptr
    template<>
    inline void data_ptr::operator = (const data_ptr& data) {
        ptr = data.ptr;
    }

    template<>
    inline void data_ptr::operator = (const std::string& data) {
        ptr.reset(new DataValue(data));
    }

    template<>
    inline void data_ptr::operator = (const data_map& data) {
        ptr.reset(new DataMap(data));
    }

    inline void data_ptr::push_back(const data_ptr& data) {
        if (!ptr) {
            ptr.reset(new DataList(data_list()));
        }
        data_list& list = ptr->getlist();
        list.push_back(data);
    }

    // base data
    inline std::string Data::getvalue()
    {
        throw TemplateException("Data item is not a value") ;
    }

    inline data_list& Data::getlist()
    {
        throw TemplateException("Data item is not a list") ;
    }
    inline data_map& Data::getmap()
    {
        throw TemplateException("Data item is not a dictionary") ;
    }
    // data value
    inline std::string DataValue::getvalue()
    {
        return m_value ;
    }
    inline bool DataValue::empty()
    {
        return m_value.empty();
    }
    // data list
    inline data_list& DataList::getlist()
    {
        return m_items ;
    }

    inline bool DataList::empty()
    {
        return m_items.empty();
    }
    // data map
    inline data_map& DataMap:: getmap()
    {
        return m_items ;
    }
    inline bool DataMap::empty()
    {
        return m_items.empty();
    }
    //////////////////////////////////////////////////////////////////////////
    // parse_val
    //////////////////////////////////////////////////////////////////////////
    inline data_ptr parse_val(std::string key, data_map &data)
    {
        // quoted string
        if (key[0] == '\"')
        {
			return make_data(boost::trim_copy_if(key, [](char c){ return c == '"'; }));
        }
        // check for dotted notation, i.e [foo.bar]
        size_t index = key.find(".") ;
        if (index == std::string::npos)
        {
            if (!data.has(key))
            {
                return make_data("{$" + key + "}") ;
            }
            return data[key] ;
        }

        std::string sub_key = key.substr(0, index) ;
        if (!data.has(sub_key))
        {
            return make_data("{$" + key + "}") ;
        }
        data_ptr item = data[sub_key] ;
        return parse_val(key.substr(index+1), item->getmap()) ;
    }

    //////////////////////////////////////////////////////////////////////////
    // Token classes
    //////////////////////////////////////////////////////////////////////////

    // defaults, overridden by subclasses with children
    inline void Token::set_children( token_vector & )
    {
        throw TemplateException("This token type cannot have children") ;
    }

    inline token_vector & Token::get_children()
    {
        throw TemplateException("This token type cannot have children") ;
    }

    // TokenText
    inline TokenType TokenText::gettype()
    {
        return TOKEN_TYPE_TEXT ;
    }

    inline void TokenText::gettext( std::ostream &stream, data_map & )
    {
        stream << m_text ;
    }

    // TokenVar
    inline TokenType TokenVar::gettype()
    {
        return TOKEN_TYPE_VAR ;
    }

    inline void TokenVar::gettext( std::ostream &stream, data_map &data )
    {
        stream << parse_val(m_key, data)->getvalue() ;
    }

    // TokenFor
    inline TokenFor::TokenFor(std::string expr)
    {
        std::vector<std::string> elements ;
        boost::split(elements, expr, boost::is_space()) ;
        if (elements.size() != 4u)
        {
            throw TemplateException("Invalid syntax in for statement") ;
        }
        m_val = elements[1] ;
        m_key = elements[3] ;
    }

    inline TokenType TokenFor::gettype()
    {
        return TOKEN_TYPE_FOR ;
    }

    inline void TokenFor::gettext( std::ostream &stream, data_map &data )
    {
        data_ptr value = parse_val(m_key, data) ;
        data_list &items = value->getlist() ;
        for (size_t i = 0 ; i < items.size() ; ++i)
        {
            data_map loop ;
            loop["index"] = make_data(boost::lexical_cast<std::string>(i+1)) ;
            loop["index0"] = make_data(boost::lexical_cast<std::string>(i)) ;
            data["loop"] = make_data(loop);
            data[m_val] = items[i] ;
            for(size_t j = 0 ; j < m_children.size() ; ++j)
            {
                m_children[j]->gettext(stream, data) ;
            }
        }
    }

    inline void TokenFor::set_children( token_vector &children )
    {
        m_children.assign(children.begin(), children.end()) ;
    }

    inline token_vector & TokenFor::get_children()
    {
        return m_children;
    }

    // TokenIf
    inline TokenType TokenIf::gettype()
    {
        return TOKEN_TYPE_IF ;
    }

    inline void TokenIf::gettext( std::ostream &stream, data_map &data )
    {
        if (is_true(m_expr, data))
        {
            for(size_t j = 0 ; j < m_children.size() ; ++j)
            {
                m_children[j]->gettext(stream, data) ;
            }
        }
    }

    inline bool TokenIf::is_true( std::string expr, data_map &data )
    {
        std::vector<std::string> elements ;
        boost::split(elements, expr, boost::is_space()) ;

        if (elements[1] == "not")
        {
            return parse_val(elements[2], data)->empty() ;
        }
        if (elements.size() == 2)
        {
            return ! parse_val(elements[1], data)->empty() ;
        }
        data_ptr lhs = parse_val(elements[1], data) ;
        data_ptr rhs = parse_val(elements[3], data) ;
        if (elements[2] == "==")
        {
            return lhs->getvalue() == rhs->getvalue() ;
        }
        return lhs->getvalue() != rhs->getvalue() ;
    }

    inline void TokenIf::set_children( token_vector &children )
    {
        m_children.assign(children.begin(), children.end()) ;
    }

    inline token_vector & TokenIf::get_children()
    {
        return m_children;
    }

    // TokenEnd
    inline TokenType TokenEnd::gettype()
    {
        return m_type == "endfor" ? TOKEN_TYPE_ENDFOR : TOKEN_TYPE_ENDIF ;
    }

    inline void TokenEnd::gettext( std::ostream &, data_map &)
    {
        throw TemplateException("End-of-control statements have no associated text") ;
    }

    // gettext
    // generic helper for getting text from tokens.

    inline std::string gettext(token_ptr token, data_map &data)
    {
        std::ostringstream stream ;
        token->gettext(stream, data) ;
        return stream.str() ;
    }
    //////////////////////////////////////////////////////////////////////////
    // parse_tree
    // recursively parses list of tokens into a tree
    //////////////////////////////////////////////////////////////////////////
    inline void parse_tree(token_vector &tokens, token_vector &tree, TokenType until)
    {
        while(! tokens.empty())
        {
            // 'pops' first item off list
            token_ptr token = tokens[0] ;
            tokens.erase(tokens.begin()) ;

            if (token->gettype() == TOKEN_TYPE_FOR)
            {
                token_vector children ;
                parse_tree(tokens, children, TOKEN_TYPE_ENDFOR) ;
                token->set_children(children) ;
            }
            else if (token->gettype() == TOKEN_TYPE_IF)
            {
                token_vector children ;
                parse_tree(tokens, children, TOKEN_TYPE_ENDIF) ;
                token->set_children(children) ;
            }
            else if (token->gettype() == until)
            {
                return ;
            }
            tree.push_back(token) ;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    // tokenize
    // parses a template into tokens (text, for, if, variable)
    //////////////////////////////////////////////////////////////////////////
    inline token_vector & tokenize(std::string text, token_vector &tokens)
    {
        while(! text.empty())
        {
            size_t pos = text.find("{") ;
            if (pos == std::string::npos)
            {
                if (! text.empty())
                {
                    tokens.push_back(token_ptr(new TokenText(text))) ;
                }
                return tokens ;
            }
            std::string pre_text = text.substr(0, pos) ;
            if (! pre_text.empty())
            {
                tokens.push_back(token_ptr(new TokenText(pre_text))) ;
            }
            text = text.substr(pos+1) ;
            if (text.empty())
            {
                tokens.push_back(token_ptr(new TokenText("{"))) ;
                return tokens ;
            }

            // variable
            if (text[0] == '$')
            {
                pos = text.find("}") ;
                if (pos != std::string::npos)
                {
                    tokens.push_back(token_ptr (new TokenVar(text.substr(1, pos-1)))) ;
                    text = text.substr(pos+1) ;
                }
            }
            // control statement
            else if (text[0] == '%')
            {
                pos = text.find("}") ;
                if (pos != std::string::npos)
                {
                    std::string expression = boost::trim_copy(text.substr(1, pos-2)) ;
                    text = text.substr(pos+1) ;
                    if (boost::starts_with(expression, "for"))
                    {
                        tokens.push_back(token_ptr (new TokenFor(expression))) ;
                    }
                    else if (boost::starts_with(expression, "if"))
                    {
                        tokens.push_back(token_ptr (new TokenIf(expression))) ;
                    }
                    else
                    {
                        tokens.push_back(token_ptr (new TokenEnd(boost::trim_copy(expression)))) ;
                    }
                }
            }
            else
            {
                tokens.push_back(token_ptr(new TokenText("{"))) ;
            }
        }
        return tokens ;
    }

    inline std::string parse(std::string templ_text, data_map &data)
    {
        std::ostringstream stream ;
        parse(stream, templ_text, data) ;
        return stream.str() ;
    }
    inline void parse(std::ostream &stream, std::string templ_text, data_map &data)
    {
        token_vector tokens ;
        tokenize(templ_text, tokens) ;
        token_vector tree ;
        parse_tree(tokens, tree) ;

        for (size_t i = 0 ; i < tree.size() ; ++i)
        {
            // Recursively calls gettext on each node in the tree.
            // gettext returns the appropriate text for that node.
            // for text, itself;
            // for variable, substitution;
            // for control statement, recursively gets kids
            tree[i]->gettext(stream, data) ;
        }
    }
}

#endif // CPPTEMPL_H
#include <ios>
#include <iomanip>
#include <algorithm>
#include <string>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <vector>

#include <fstream>

namespace nonius {
    struct html_reporter : reporter {
    private:
        static std::string const& template_string() {
            static char const* template_parts[] = {
// generated content broken into pieces because MSVC is in the 1990s.
// #included from: detail/html_report_template.g.h++
"<!DOCTYPE html>\n"
"<html>\n"
" <head>\n"
"  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
"  <title>{$title} - nonius report</title>\n"
"  <style type=\"text/css\">\n"
"#all-together {\n"
"  min-width: 310px;\n"
"  height: 400px;\n"
"  margin: 0 auto;\n"
"}\n"
"  </style>\n"
"  <script type=\"text/javascript\">\n"
"/*! jQuery v1.11.0 | (c) 2005, 2014 jQuery Foundation, Inc. | jquery.org/license */\n"
,
"!function(a,b){\"object\"==typeof module&&\"object\"==typeof module.exports?module.exports=a.document?b(a,!0):function(a){if(!a.document)throw new Error(\"jQuery requires a window with a document\");return b(a)}:b(a)}(\"undefined\"!=typeof window?window:this,function(a,b){var c=[],d=c.slice,e=c.concat,f=c.push,g=c.indexOf,h={},i=h.toString,j=h.hasOwnProperty,k=\"\".trim,l={},m=\"1.11.0\",n=function(a,b){return new n.fn.init(a,b)},o=/^[\\s\\uFEFF\\xA0]+|[\\s\\uFEFF\\xA0]+$/g,p=/^-ms-/,q=/-([\\da-z])/gi,r=function(a,b){return b.toUpperCase()};n.fn=n.prototype={jquery:m,constructor:n,selector:\"\",length:0,toArray:function(){return d.call(this)},get:function(a){return null!=a?0>a?this[a+this.length]:this[a]:d.call(this)},pushStack:function(a){var b=n.merge(this.constructor(),a);return b.prevObject=this,b.context=this.context,b},each:function(a,b){return n.each(this,a,b)},map:function(a){return this.pushStack(n.map(this,function(b,c){return a.call(b,c,b)}))},slice:function(){return this.pushStack(d.apply(this,arguments))},first:function(){return this.eq(0)},last:function(){return this.eq(-1)},eq:function(a){var b=this.length,c=+a+(0>a?b:0);return this.pushStack(c>=0&&b>c?[this[c]]:[])},end:function(){return this.prevObject||this.constructor(null)},push:f,sort:c.sort,splice:c.splice},n.extend=n.fn.extend=function(){var a,b,c,d,e,f,g=arguments[0]||{},h=1,i=arguments.length,j=!1;for(\"boolean\"==typeof g&&(j=g,g=arguments[h]||{},h++),\"object\"==typeof g||n.isFunction(g)||(g={}),h===i&&(g=this,h--);i>h;h++)if(null!=(e=arguments[h]))for(d in e)a=g[d],c=e[d],g!==c&&(j&&c&&(n.isPlainObject(c)||(b=n.isArray(c)))?(b?(b=!1,f=a&&n.isArray(a)?a:[]):f=a&&n.isPlainObject(a)?a:{},g[d]=n.extend(j,f,c)):void 0!==c&&(g[d]=c));return g},n.extend({expando:\"jQuery\"+(m+Math.random()).replace(/\\D/g,\"\"),isReady:!0,error:function(a){throw new Error(a)},noop:function(){},isFunction:function(a){return\"function\"===n.type(a)},isArray:Array.isArray||function(a){return\"array\"===n.type(a)},isWindow:function(a){return null!=a&&a==a.window},isNumeric:function(a){return a-parseFloat(a)>=0},isEmptyObject:function(a){var b;for(b in a)return!1;return!0},isPlainObject:function(a){var b;if(!a||\"object\"!==n.type(a)||a.nodeType||n.isWindow(a))return!1;try{if(a.constructor&&!j.call(a,\"constructor\")&&!j.call(a.constructor.prototype,\"isPrototypeOf\"))return!1}catch(c){return!1}if(l.ownLast)for(b in a)return j.call(a,b);for(b in a);return void 0===b||j.call(a,b)},type:function(a){return null==a?a+\"\":\"object\"==typeof a||\"function\"==typeof a?h[i.call(a)]||\"object\":typeof a},globalEval:function(b){b&&n.trim(b)&&(a.execScript||function(b){a.eval.call(a,b)})(b)},camelCase:function(a){return a.replace(p,\"ms-\").replace(q,r)},nodeName:function(a,b){return a.nodeName&&a.nodeName.toLowerCase()===b.toLowerCase()},each:function(a,b,c){var d,e=0,f=a.length,g=s(a);if(c){if(g){for(;f>e;e++)if(d=b.apply(a[e],c),d===!1)break}else for(e in a)if(d=b.apply(a[e],c),d===!1)break}else if(g){for(;f>e;e++)if(d=b.call(a[e],e,a[e]),d===!1)break}else for(e in a)if(d=b.call(a[e],e,a[e]),d===!1)break;return a},trim:k&&!k.call(\"\\ufeff\\xa0\")?function(a){return null==a?\"\":k.call(a)}:function(a){return null==a?\"\":(a+\"\").replace(o,\"\")},makeArray:function(a,b){var c=b||[];return null!=a&&(s(Object(a))?n.merge(c,\"string\"==typeof a?[a]:a):f.call(c,a)),c},inArray:function(a,b,c){var d;if(b){if(g)return g.call(b,a,c);for(d=b.length,c=c?0>c?Math.max(0,d+c):c:0;d>c;c++)if(c in b&&b[c]===a)return c}return-1},merge:function(a,b){var c=+b.length,d=0,e=a.length;while(c>d)a[e++]=b[d++];if(c!==c)while(void 0!==b[d])a[e++]=b[d++];return a.length=e,a},grep:function(a,b,c){for(var d,e=[],f=0,g=a.length,h=!c;g>f;f++)d=!b(a[f],f),d!==h&&e.push(a[f]);return e},map:function(a,b,c){var d,f=0,g=a.length,h=s(a),i=[];if(h)for(;g>f;f++)d=b(a[f],f,c),null!=d&&i.push(d);else for(f in a)d=b(a[f],f,c),null!=d&&i.push(d);return e.apply([],i)},guid:1,proxy:function(a,b){var c,e,f;return\"string\"==typeof b&&(f=a[b],b=a,a=f),n.isFunction(a)?(c=d.call(arguments,2),e=function(){return a.apply(b||this,c.concat(d.call(arguments)))},e.guid=a.guid=a.guid||n.guid++,e):void 0},now:function(){return+new Date},support:l}),n.each(\"Boolean Number String Function Array Date RegExp Object Error\".split(\" \"),function(a,b){h[\"[object \"+b+\"]\"]=b.toLowerCase()});function s(a){var b=a.length,c=n.type(a);return\"function\"===c||n.isWindow(a)?!1:1===a.nodeType&&b?!0:\"array\"===c||0===b||\"number\"==typeof b&&b>0&&b-1 in a}var t=function(a){var b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s=\"sizzle\"+-new Date,t=a.document,u=0,v=0,w=eb(),x=eb(),y=eb(),z=function(a,b){return a===b&&(j=!0),0},A=\"undefined\",B=1<<31,C={}.hasOwnProperty,D=[],E=D.pop,F=D.push,G=D.push,H=D.slice,I=D.indexOf||function(a){for(var b=0,c=this.length;c>b;b++)if(this[b]===a)return b;return-1},J=\"checked|selected|async|autofocus|autoplay|controls|defer|disabled|hidden|ismap|loop|multiple|open|readonly|required|scoped\",K=\"[\\\\x20\\\\t\\\\r\\\\n\\\\f]\",L=\"(?:\\\\\\\\.|[\\\\w-]|[^\\\\x00-\\\\xa0])+\",M=L.replace(\"w\",\"w#\"),N=\"\\\\[\"+K+\"*(\"+L+\")\"+K+\"*(?:([*^$|!~]?=)\"+K+\"*(?:(['\\\"])((?:\\\\\\\\.|[^\\\\\\\\])*?)\\\\3|(\"+M+\")|)|)\"+K+\"*\\\\]\",O=\":(\"+L+\")(?:\\\\(((['\\\"])((?:\\\\\\\\.|[^\\\\\\\\])*?)\\\\3|((?:\\\\\\\\.|[^\\\\\\\\()[\\\\]]|\"+N.replace(3,8)+\")*)|.*)\\\\)|)\",P=new RegExp(\"^\"+K+\"+|((?:^|[^\\\\\\\\])(?:\\\\\\\\.)*)\"+K+\"+$\",\"g\"),Q=new RegExp(\"^\"+K+\"*,\"+K+\"*\"),R=new RegExp(\"^\"+K+\"*([>+~]|\"+K+\")\"+K+\"*\"),S=new RegExp(\"=\"+K+\"*([^\\\\]'\\\"]*?)\"+K+\"*\\\\]\",\"g\"),T=new RegExp(O),U=new RegExp(\"^\"+M+\"$\"),V={ID:new RegExp(\"^#(\"+L+\")\"),CLASS:new RegExp(\"^\\\\.(\"+L+\")\"),TAG:new RegExp(\"^(\"+L.replace(\"w\",\"w*\")+\")\"),ATTR:new RegExp(\"^\"+N),PSEUDO:new RegExp(\"^\"+O),CHILD:new RegExp(\"^:(only|first|last|nth|nth-last)-(child|of-type)(?:\\\\(\"+K+\"*(even|odd|(([+-]|)(\\\\d*)n|)\"+K+\"*(?:([+-]|)\"+K+\"*(\\\\d+)|))\"+K+\"*\\\\)|)\",\"i\"),bool:new RegExp(\"^(?:\"+J+\")$\",\"i\"),needsContext:new RegExp(\"^\"+K+\"*[>+~]|:(even|odd|eq|gt|lt|nth|first|last)(?:\\\\(\"+K+\"*((?:-\\\\d)?\\\\d*)\"+K+\"*\\\\)|)(?=[^-]|$)\",\"i\")},W=/^(?:input|select|textarea|button)$/i,X=/^h\\d$/i,Y=/^[^{]+\\{\\s*\\[native \\w/,Z=/^(?:#([\\w-]+)|(\\w+)|\\.([\\w-]+))$/,$=/[+~]/,_=/'|\\\\/g,ab=new RegExp(\"\\\\\\\\([\\\\da-f]{1,6}\"+K+\"?|(\"+K+\")|.)\",\"ig\"),bb=function(a,b,c){var d=\"0x\"+b-65536;return d!==d||c?b:0>d?String.fromCharCode(d+65536):String.fromCharCode(d>>10|55296,1023&d|56320)};try{G.apply(D=H.call(t.childNodes),t.childNodes),D[t.childNodes.length].nodeType}catch(cb){G={apply:D.length?function(a,b){F.apply(a,H.call(b))}:function(a,b){var c=a.length,d=0;while(a[c++]=b[d++]);a.length=c-1}}}function db(a,b,d,e){var f,g,h,i,j,m,p,q,u,v;if((b?b.ownerDocument||b:t)!==l&&k(b),b=b||l,d=d||[],!a||\"string\"!=typeof a)return d;if(1!==(i=b.nodeType)&&9!==i)return[];if(n&&!e){if(f=Z.exec(a))if(h=f[1]){if(9===i){if(g=b.getElementById(h),!g||!g.parentNode)return d;if(g.id===h)return d.push(g),d}else if(b.ownerDocument&&(g=b.ownerDocument.getElementById(h))&&r(b,g)&&g.id===h)return d.push(g),d}else{if(f[2])return G.apply(d,b.getElementsByTagName(a)),d;if((h=f[3])&&c.getElementsByClassName&&b.getElementsByClassName)return G.apply(d,b.getElementsByClassName(h)),d}if(c.qsa&&(!o||!o.test(a))){if(q=p=s,u=b,v=9===i&&a,1===i&&\"object\"!==b.nodeName.toLowerCase()){m=ob(a),(p=b.getAttribute(\"id\"))?q=p.replace(_,\"\\\\$&\"):b.setAttribute(\"id\",q),q=\"[id='\"+q+\"'] \",j=m.length;while(j--)m[j]=q+pb(m[j]);u=$.test(a)&&mb(b.parentNode)||b,v=m.join(\",\")}if(v)try{return G.apply(d,u.querySelectorAll(v)),d}catch(w){}finally{p||b.removeAttribute(\"id\")}}}return xb(a.replace(P,\"$1\"),b,d,e)}function eb(){var a=[];function b(c,e){return a.push(c+\" \")>d.cacheLength&&delete b[a.shift()],b[c+\" \"]=e}return b}function fb(a){return a[s]=!0,a}function gb(a){var b=l.createElement(\"div\");try{return!!a(b)}catch(c){return!1}finally{b.parentNode&&b.parentNode.removeChild(b),b=null}}function hb(a,b){var c=a.split(\"|\"),e=a.length;while(e--)d.attrHandle[c[e]]=b}function ib(a,b){var c=b&&a,d=c&&1===a.nodeType&&1===b.nodeType&&(~b.sourceIndex||B)-(~a.sourceIndex||B);if(d)return d;if(c)while(c=c.nextSibling)if(c===b)return-1;return a?1:-1}function jb(a){return function(b){var c=b.nodeName.toLowerCase();return\"input\"===c&&b.type===a}}function kb(a){return function(b){var c=b.nodeName.toLowerCase();return(\"input\"===c||\"button\"===c)&&b.type===a}}function lb(a){return fb(function(b){return b=+b,fb(function(c,d){var e,f=a([],c.length,b),g=f.length;while(g--)c[e=f[g]]&&(c[e]=!(d[e]=c[e]))})})}function mb(a){return a&&typeof a.getElementsByTagName!==A&&a}c=db.support={},f=db.isXML=function(a){var b=a&&(a.ownerDocument||a).documentElement;return b?\"HTML\"!==b.nodeName:!1},k=db.setDocument=function(a){var b,e=a?a.ownerDocument||a:t,g=e.defaultView;return e!==l&&9===e.nodeType&&e.documentElement?(l=e,m=e.documentElement,n=!f(e),g&&g!==g.top&&(g.addEventListener?g.addEventListener(\"unload\",function(){k()},!1):g.attachEvent&&g.attachEvent(\"onunload\",function(){k()})),c.attributes=gb(function(a){return a.className=\"i\",!a.getAttribute(\"className\")}),c.getElementsByTagName=gb(function(a){return a.appendChild(e.createComment(\"\")),!a.getElementsByTagName(\"*\").length}),c.getElementsByClassName=Y.test(e.getElementsByClassName)&&gb(function(a){return a.innerHTML=\"<div class='a'></div><div class='a i'></div>\",a.firstChild.className=\"i\",2===a.getElementsByClassName(\"i\").length}),c.getById=gb(function(a){return m.appendChild(a).id=s,!e.getElementsByName||!e.getElementsByName(s).length}),c.getById?(d.find.ID=function(a,b){if(typeof b.getElementById!==A&&n){var c=b.getElementById(a);return c&&c.parentNode?[c]:[]}},d.filter.ID=function(a){var b=a.replace(ab,bb);return function(a){return a.getAttribute(\"id\")===b}}):(delete d.find.ID,d.filter.ID=function(a){var b=a.replace(ab,bb);return function(a){var c=typeof a.getAttributeNode!==A&&a.getAttributeNode(\"id\");return c&&c.value===b}}),d.find.TAG=c.getElementsByTagName?function(a,b){return typeof b.getElementsByTagName!==A?b.getElementsByTagName(a):void 0}:function(a,b){var c,d=[],e=0,f=b.getElementsByTagName(a);if(\"*\"===a){while(c=f[e++])1===c.nodeType&&d.push(c);return d}return f},d.find.CLASS=c.getElementsByClassName&&function(a"
,
",b){return typeof b.getElementsByClassName!==A&&n?b.getElementsByClassName(a):void 0},p=[],o=[],(c.qsa=Y.test(e.querySelectorAll))&&(gb(function(a){a.innerHTML=\"<select t=''><option selected=''></option></select>\",a.querySelectorAll(\"[t^='']\").length&&o.push(\"[*^$]=\"+K+\"*(?:''|\\\"\\\")\"),a.querySelectorAll(\"[selected]\").length||o.push(\"\\\\[\"+K+\"*(?:value|\"+J+\")\"),a.querySelectorAll(\":checked\").length||o.push(\":checked\")}),gb(function(a){var b=e.createElement(\"input\");b.setAttribute(\"type\",\"hidden\"),a.appendChild(b).setAttribute(\"name\",\"D\"),a.querySelectorAll(\"[name=d]\").length&&o.push(\"name\"+K+\"*[*^$|!~]?=\"),a.querySelectorAll(\":enabled\").length||o.push(\":enabled\",\":disabled\"),a.querySelectorAll(\"*,:x\"),o.push(\",.*:\")})),(c.matchesSelector=Y.test(q=m.webkitMatchesSelector||m.mozMatchesSelector||m.oMatchesSelector||m.msMatchesSelector))&&gb(function(a){c.disconnectedMatch=q.call(a,\"div\"),q.call(a,\"[s!='']:x\"),p.push(\"!=\",O)}),o=o.length&&new RegExp(o.join(\"|\")),p=p.length&&new RegExp(p.join(\"|\")),b=Y.test(m.compareDocumentPosition),r=b||Y.test(m.contains)?function(a,b){var c=9===a.nodeType?a.documentElement:a,d=b&&b.parentNode;return a===d||!(!d||1!==d.nodeType||!(c.contains?c.contains(d):a.compareDocumentPosition&&16&a.compareDocumentPosition(d)))}:function(a,b){if(b)while(b=b.parentNode)if(b===a)return!0;return!1},z=b?function(a,b){if(a===b)return j=!0,0;var d=!a.compareDocumentPosition-!b.compareDocumentPosition;return d?d:(d=(a.ownerDocument||a)===(b.ownerDocument||b)?a.compareDocumentPosition(b):1,1&d||!c.sortDetached&&b.compareDocumentPosition(a)===d?a===e||a.ownerDocument===t&&r(t,a)?-1:b===e||b.ownerDocument===t&&r(t,b)?1:i?I.call(i,a)-I.call(i,b):0:4&d?-1:1)}:function(a,b){if(a===b)return j=!0,0;var c,d=0,f=a.parentNode,g=b.parentNode,h=[a],k=[b];if(!f||!g)return a===e?-1:b===e?1:f?-1:g?1:i?I.call(i,a)-I.call(i,b):0;if(f===g)return ib(a,b);c=a;while(c=c.parentNode)h.unshift(c);c=b;while(c=c.parentNode)k.unshift(c);while(h[d]===k[d])d++;return d?ib(h[d],k[d]):h[d]===t?-1:k[d]===t?1:0},e):l},db.matches=function(a,b){return db(a,null,null,b)},db.matchesSelector=function(a,b){if((a.ownerDocument||a)!==l&&k(a),b=b.replace(S,\"='$1']\"),!(!c.matchesSelector||!n||p&&p.test(b)||o&&o.test(b)))try{var d=q.call(a,b);if(d||c.disconnectedMatch||a.document&&11!==a.document.nodeType)return d}catch(e){}return db(b,l,null,[a]).length>0},db.contains=function(a,b){return(a.ownerDocument||a)!==l&&k(a),r(a,b)},db.attr=function(a,b){(a.ownerDocument||a)!==l&&k(a);var e=d.attrHandle[b.toLowerCase()],f=e&&C.call(d.attrHandle,b.toLowerCase())?e(a,b,!n):void 0;return void 0!==f?f:c.attributes||!n?a.getAttribute(b):(f=a.getAttributeNode(b))&&f.specified?f.value:null},db.error=function(a){throw new Error(\"Syntax error, unrecognized expression: \"+a)},db.uniqueSort=function(a){var b,d=[],e=0,f=0;if(j=!c.detectDuplicates,i=!c.sortStable&&a.slice(0),a.sort(z),j){while(b=a[f++])b===a[f]&&(e=d.push(f));while(e--)a.splice(d[e],1)}return i=null,a},e=db.getText=function(a){var b,c=\"\",d=0,f=a.nodeType;if(f){if(1===f||9===f||11===f){if(\"string\"==typeof a.textContent)return a.textContent;for(a=a.firstChild;a;a=a.nextSibling)c+=e(a)}else if(3===f||4===f)return a.nodeValue}else while(b=a[d++])c+=e(b);return c},d=db.selectors={cacheLength:50,createPseudo:fb,match:V,attrHandle:{},find:{},relative:{\">\":{dir:\"parentNode\",first:!0},\" \":{dir:\"parentNode\"},\"+\":{dir:\"previousSibling\",first:!0},\"~\":{dir:\"previousSibling\"}},preFilter:{ATTR:function(a){return a[1]=a[1].replace(ab,bb),a[3]=(a[4]||a[5]||\"\").replace(ab,bb),\"~=\"===a[2]&&(a[3]=\" \"+a[3]+\" \"),a.slice(0,4)},CHILD:function(a){return a[1]=a[1].toLowerCase(),\"nth\"===a[1].slice(0,3)?(a[3]||db.error(a[0]),a[4]=+(a[4]?a[5]+(a[6]||1):2*(\"even\"===a[3]||\"odd\"===a[3])),a[5]=+(a[7]+a[8]||\"odd\"===a[3])):a[3]&&db.error(a[0]),a},PSEUDO:function(a){var b,c=!a[5]&&a[2];return V.CHILD.test(a[0])?null:(a[3]&&void 0!==a[4]?a[2]=a[4]:c&&T.test(c)&&(b=ob(c,!0))&&(b=c.indexOf(\")\",c.length-b)-c.length)&&(a[0]=a[0].slice(0,b),a[2]=c.slice(0,b)),a.slice(0,3))}},filter:{TAG:function(a){var b=a.replace(ab,bb).toLowerCase();return\"*\"===a?function(){return!0}:function(a){return a.nodeName&&a.nodeName.toLowerCase()===b}},CLASS:function(a){var b=w[a+\" \"];return b||(b=new RegExp(\"(^|\"+K+\")\"+a+\"(\"+K+\"|$)\"))&&w(a,function(a){return b.test(\"string\"==typeof a.className&&a.className||typeof a.getAttribute!==A&&a.getAttribute(\"class\")||\"\")})},ATTR:function(a,b,c){return function(d){var e=db.attr(d,a);return null==e?\"!=\"===b:b?(e+=\"\",\"=\"===b?e===c:\"!=\"===b?e!==c:\"^=\"===b?c&&0===e.indexOf(c):\"*=\"===b?c&&e.indexOf(c)>-1:\"$=\"===b?c&&e.slice(-c.length)===c:\"~=\"===b?(\" \"+e+\" \").indexOf(c)>-1:\"|=\"===b?e===c||e.slice(0,c.length+1)===c+\"-\":!1):!0}},CHILD:function(a,b,c,d,e){var f=\"nth\"!==a.slice(0,3),g=\"last\"!==a.slice(-4),h=\"of-type\"===b;return 1===d&&0===e?function(a){return!!a.parentNode}:function(b,c,i){var j,k,l,m,n,o,p=f!==g?\"nextSibling\":\"previousSibling\",q=b.parentNode,r=h&&b.nodeName.toLowerCase(),t=!i&&!h;if(q){if(f){while(p){l=b;while(l=l[p])if(h?l.nodeName.toLowerCase()===r:1===l.nodeType)return!1;o=p=\"only\"===a&&!o&&\"nextSibling\"}return!0}if(o=[g?q.firstChild:q.lastChild],g&&t){k=q[s]||(q[s]={}),j=k[a]||[],n=j[0]===u&&j[1],m=j[0]===u&&j[2],l=n&&q.childNodes[n];while(l=++n&&l&&l[p]||(m=n=0)||o.pop())if(1===l.nodeType&&++m&&l===b){k[a]=[u,n,m];break}}else if(t&&(j=(b[s]||(b[s]={}))[a])&&j[0]===u)m=j[1];else while(l=++n&&l&&l[p]||(m=n=0)||o.pop())if((h?l.nodeName.toLowerCase()===r:1===l.nodeType)&&++m&&(t&&((l[s]||(l[s]={}))[a]=[u,m]),l===b))break;return m-=e,m===d||m%d===0&&m/d>=0}}},PSEUDO:function(a,b){var c,e=d.pseudos[a]||d.setFilters[a.toLowerCase()]||db.error(\"unsupported pseudo: \"+a);return e[s]?e(b):e.length>1?(c=[a,a,\"\",b],d.setFilters.hasOwnProperty(a.toLowerCase())?fb(function(a,c){var d,f=e(a,b),g=f.length;while(g--)d=I.call(a,f[g]),a[d]=!(c[d]=f[g])}):function(a){return e(a,0,c)}):e}},pseudos:{not:fb(function(a){var b=[],c=[],d=g(a.replace(P,\"$1\"));return d[s]?fb(function(a,b,c,e){var f,g=d(a,null,e,[]),h=a.length;while(h--)(f=g[h])&&(a[h]=!(b[h]=f))}):function(a,e,f){return b[0]=a,d(b,null,f,c),!c.pop()}}),has:fb(function(a){return function(b){return db(a,b).length>0}}),contains:fb(function(a){return function(b){return(b.textContent||b.innerText||e(b)).indexOf(a)>-1}}),lang:fb(function(a){return U.test(a||\"\")||db.error(\"unsupported lang: \"+a),a=a.replace(ab,bb).toLowerCase(),function(b){var c;do if(c=n?b.lang:b.getAttribute(\"xml:lang\")||b.getAttribute(\"lang\"))return c=c.toLowerCase(),c===a||0===c.indexOf(a+\"-\");while((b=b.parentNode)&&1===b.nodeType);return!1}}),target:function(b){var c=a.location&&a.location.hash;return c&&c.slice(1)===b.id},root:function(a){return a===m},focus:function(a){return a===l.activeElement&&(!l.hasFocus||l.hasFocus())&&!!(a.type||a.href||~a.tabIndex)},enabled:function(a){return a.disabled===!1},disabled:function(a){return a.disabled===!0},checked:function(a){var b=a.nodeName.toLowerCase();return\"input\"===b&&!!a.checked||\"option\"===b&&!!a.selected},selected:function(a){return a.parentNode&&a.parentNode.selectedIndex,a.selected===!0},empty:function(a){for(a=a.firstChild;a;a=a.nextSibling)if(a.nodeType<6)return!1;return!0},parent:function(a){return!d.pseudos.empty(a)},header:function(a){return X.test(a.nodeName)},input:function(a){return W.test(a.nodeName)},button:function(a){var b=a.nodeName.toLowerCase();return\"input\"===b&&\"button\"===a.type||\"button\"===b},text:function(a){var b;return\"input\"===a.nodeName.toLowerCase()&&\"text\"===a.type&&(null==(b=a.getAttribute(\"type\"))||\"text\"===b.toLowerCase())},first:lb(function(){return[0]}),last:lb(function(a,b){return[b-1]}),eq:lb(function(a,b,c){return[0>c?c+b:c]}),even:lb(function(a,b){for(var c=0;b>c;c+=2)a.push(c);return a}),odd:lb(function(a,b){for(var c=1;b>c;c+=2)a.push(c);return a}),lt:lb(function(a,b,c){for(var d=0>c?c+b:c;--d>=0;)a.push(d);return a}),gt:lb(function(a,b,c){for(var d=0>c?c+b:c;++d<b;)a.push(d);return a})}},d.pseudos.nth=d.pseudos.eq;for(b in{radio:!0,checkbox:!0,file:!0,password:!0,image:!0})d.pseudos[b]=jb(b);for(b in{submit:!0,reset:!0})d.pseudos[b]=kb(b);function nb(){}nb.prototype=d.filters=d.pseudos,d.setFilters=new nb;function ob(a,b){var c,e,f,g,h,i,j,k=x[a+\" \"];if(k)return b?0:k.slice(0);h=a,i=[],j=d.preFilter;while(h){(!c||(e=Q.exec(h)))&&(e&&(h=h.slice(e[0].length)||h),i.push(f=[])),c=!1,(e=R.exec(h))&&(c=e.shift(),f.push({value:c,type:e[0].replace(P,\" \")}),h=h.slice(c.length));for(g in d.filter)!(e=V[g].exec(h))||j[g]&&!(e=j[g](e))||(c=e.shift(),f.push({value:c,type:g,matches:e}),h=h.slice(c.length));if(!c)break}return b?h.length:h?db.error(a):x(a,i).slice(0)}function pb(a){for(var b=0,c=a.length,d=\"\";c>b;b++)d+=a[b].value;return d}function qb(a,b,c){var d=b.dir,e=c&&\"parentNode\"===d,f=v++;return b.first?function(b,c,f){while(b=b[d])if(1===b.nodeType||e)return a(b,c,f)}:function(b,c,g){var h,i,j=[u,f];if(g){while(b=b[d])if((1===b.nodeType||e)&&a(b,c,g))return!0}else while(b=b[d])if(1===b.nodeType||e){if(i=b[s]||(b[s]={}),(h=i[d])&&h[0]===u&&h[1]===f)return j[2]=h[2];if(i[d]=j,j[2]=a(b,c,g))return!0}}}function rb(a){return a.length>1?function(b,c,d){var e=a.length;while(e--)if(!a[e](b,c,d))return!1;return!0}:a[0]}function sb(a,b,c,d,e){for(var f,g=[],h=0,i=a.length,j=null!=b;i>h;h++)(f=a[h])&&(!c||c(f,d,e))&&(g.push(f),j&&b.push(h));return g}function tb(a,b,c,d,e,f){return d&&!d[s]&&(d=tb(d)),e&&!e[s]&&(e=tb(e,f)),fb(function(f,g,h,i){var j,k,l,m=[],n=[],o=g.length,p=f||wb(b||\"*\",h.nodeType?[h]:h,[]),q=!a||!f&&b?p:sb(p,m,a,h,i),r=c?e||(f?a:o||d)?[]:g:q;if(c&&c(q,r,h,i),d){j=sb(r,n),d(j,[],h,i),k=j.length;while(k--)(l=j[k])&&(r[n[k]]=!(q[n[k]]=l))}if(f){if(e||a){if(e){j=[],k=r.length;while(k--)(l=r[k])&&j.push(q[k]=l);e(null,r=[],j,i)}k=r.length;while(k--)(l=r[k])&&(j=e?I.call(f,l):m[k])>-1&&(f[j]=!(g[j]=l))}}else r=sb(r===g?r.splice(o,r.length):r),e?e(null,g,r,i):G.apply(g,r)})}function ub(a){for(var b,c,e,f=a.length,g=d.relative[a[0].type],i=g||d.relative[\" \"],j=g?1:0,k=qb(function(a){return a===b},i,!0),l=qb(function(a){"
,
"return I.call(b,a)>-1},i,!0),m=[function(a,c,d){return!g&&(d||c!==h)||((b=c).nodeType?k(a,c,d):l(a,c,d))}];f>j;j++)if(c=d.relative[a[j].type])m=[qb(rb(m),c)];else{if(c=d.filter[a[j].type].apply(null,a[j].matches),c[s]){for(e=++j;f>e;e++)if(d.relative[a[e].type])break;return tb(j>1&&rb(m),j>1&&pb(a.slice(0,j-1).concat({value:\" \"===a[j-2].type?\"*\":\"\"})).replace(P,\"$1\"),c,e>j&&ub(a.slice(j,e)),f>e&&ub(a=a.slice(e)),f>e&&pb(a))}m.push(c)}return rb(m)}function vb(a,b){var c=b.length>0,e=a.length>0,f=function(f,g,i,j,k){var m,n,o,p=0,q=\"0\",r=f&&[],s=[],t=h,v=f||e&&d.find.TAG(\"*\",k),w=u+=null==t?1:Math.random()||.1,x=v.length;for(k&&(h=g!==l&&g);q!==x&&null!=(m=v[q]);q++){if(e&&m){n=0;while(o=a[n++])if(o(m,g,i)){j.push(m);break}k&&(u=w)}c&&((m=!o&&m)&&p--,f&&r.push(m))}if(p+=q,c&&q!==p){n=0;while(o=b[n++])o(r,s,g,i);if(f){if(p>0)while(q--)r[q]||s[q]||(s[q]=E.call(j));s=sb(s)}G.apply(j,s),k&&!f&&s.length>0&&p+b.length>1&&db.uniqueSort(j)}return k&&(u=w,h=t),r};return c?fb(f):f}g=db.compile=function(a,b){var c,d=[],e=[],f=y[a+\" \"];if(!f){b||(b=ob(a)),c=b.length;while(c--)f=ub(b[c]),f[s]?d.push(f):e.push(f);f=y(a,vb(e,d))}return f};function wb(a,b,c){for(var d=0,e=b.length;e>d;d++)db(a,b[d],c);return c}function xb(a,b,e,f){var h,i,j,k,l,m=ob(a);if(!f&&1===m.length){if(i=m[0]=m[0].slice(0),i.length>2&&\"ID\"===(j=i[0]).type&&c.getById&&9===b.nodeType&&n&&d.relative[i[1].type]){if(b=(d.find.ID(j.matches[0].replace(ab,bb),b)||[])[0],!b)return e;a=a.slice(i.shift().value.length)}h=V.needsContext.test(a)?0:i.length;while(h--){if(j=i[h],d.relative[k=j.type])break;if((l=d.find[k])&&(f=l(j.matches[0].replace(ab,bb),$.test(i[0].type)&&mb(b.parentNode)||b))){if(i.splice(h,1),a=f.length&&pb(i),!a)return G.apply(e,f),e;break}}}return g(a,m)(f,b,!n,e,$.test(a)&&mb(b.parentNode)||b),e}return c.sortStable=s.split(\"\").sort(z).join(\"\")===s,c.detectDuplicates=!!j,k(),c.sortDetached=gb(function(a){return 1&a.compareDocumentPosition(l.createElement(\"div\"))}),gb(function(a){return a.innerHTML=\"<a href='#'></a>\",\"#\"===a.firstChild.getAttribute(\"href\")})||hb(\"type|href|height|width\",function(a,b,c){return c?void 0:a.getAttribute(b,\"type\"===b.toLowerCase()?1:2)}),c.attributes&&gb(function(a){return a.innerHTML=\"<input/>\",a.firstChild.setAttribute(\"value\",\"\"),\"\"===a.firstChild.getAttribute(\"value\")})||hb(\"value\",function(a,b,c){return c||\"input\"!==a.nodeName.toLowerCase()?void 0:a.defaultValue}),gb(function(a){return null==a.getAttribute(\"disabled\")})||hb(J,function(a,b,c){var d;return c?void 0:a[b]===!0?b.toLowerCase():(d=a.getAttributeNode(b))&&d.specified?d.value:null}),db}(a);n.find=t,n.expr=t.selectors,n.expr[\":\"]=n.expr.pseudos,n.unique=t.uniqueSort,n.text=t.getText,n.isXMLDoc=t.isXML,n.contains=t.contains;var u=n.expr.match.needsContext,v=/^<(\\w+)\\s*\\/?>(?:<\\/\\1>|)$/,w=/^.[^:#\\[\\.,]*$/;function x(a,b,c){if(n.isFunction(b))return n.grep(a,function(a,d){return!!b.call(a,d,a)!==c});if(b.nodeType)return n.grep(a,function(a){return a===b!==c});if(\"string\"==typeof b){if(w.test(b))return n.filter(b,a,c);b=n.filter(b,a)}return n.grep(a,function(a){return n.inArray(a,b)>=0!==c})}n.filter=function(a,b,c){var d=b[0];return c&&(a=\":not(\"+a+\")\"),1===b.length&&1===d.nodeType?n.find.matchesSelector(d,a)?[d]:[]:n.find.matches(a,n.grep(b,function(a){return 1===a.nodeType}))},n.fn.extend({find:function(a){var b,c=[],d=this,e=d.length;if(\"string\"!=typeof a)return this.pushStack(n(a).filter(function(){for(b=0;e>b;b++)if(n.contains(d[b],this))return!0}));for(b=0;e>b;b++)n.find(a,d[b],c);return c=this.pushStack(e>1?n.unique(c):c),c.selector=this.selector?this.selector+\" \"+a:a,c},filter:function(a){return this.pushStack(x(this,a||[],!1))},not:function(a){return this.pushStack(x(this,a||[],!0))},is:function(a){return!!x(this,\"string\"==typeof a&&u.test(a)?n(a):a||[],!1).length}});var y,z=a.document,A=/^(?:\\s*(<[\\w\\W]+>)[^>]*|#([\\w-]*))$/,B=n.fn.init=function(a,b){var c,d;if(!a)return this;if(\"string\"==typeof a){if(c=\"<\"===a.charAt(0)&&\">\"===a.charAt(a.length-1)&&a.length>=3?[null,a,null]:A.exec(a),!c||!c[1]&&b)return!b||b.jquery?(b||y).find(a):this.constructor(b).find(a);if(c[1]){if(b=b instanceof n?b[0]:b,n.merge(this,n.parseHTML(c[1],b&&b.nodeType?b.ownerDocument||b:z,!0)),v.test(c[1])&&n.isPlainObject(b))for(c in b)n.isFunction(this[c])?this[c](b[c]):this.attr(c,b[c]);return this}if(d=z.getElementById(c[2]),d&&d.parentNode){if(d.id!==c[2])return y.find(a);this.length=1,this[0]=d}return this.context=z,this.selector=a,this}return a.nodeType?(this.context=this[0]=a,this.length=1,this):n.isFunction(a)?\"undefined\"!=typeof y.ready?y.ready(a):a(n):(void 0!==a.selector&&(this.selector=a.selector,this.context=a.context),n.makeArray(a,this))};B.prototype=n.fn,y=n(z);var C=/^(?:parents|prev(?:Until|All))/,D={children:!0,contents:!0,next:!0,prev:!0};n.extend({dir:function(a,b,c){var d=[],e=a[b];while(e&&9!==e.nodeType&&(void 0===c||1!==e.nodeType||!n(e).is(c)))1===e.nodeType&&d.push(e),e=e[b];return d},sibling:function(a,b){for(var c=[];a;a=a.nextSibling)1===a.nodeType&&a!==b&&c.push(a);return c}}),n.fn.extend({has:function(a){var b,c=n(a,this),d=c.length;return this.filter(function(){for(b=0;d>b;b++)if(n.contains(this,c[b]))return!0})},closest:function(a,b){for(var c,d=0,e=this.length,f=[],g=u.test(a)||\"string\"!=typeof a?n(a,b||this.context):0;e>d;d++)for(c=this[d];c&&c!==b;c=c.parentNode)if(c.nodeType<11&&(g?g.index(c)>-1:1===c.nodeType&&n.find.matchesSelector(c,a))){f.push(c);break}return this.pushStack(f.length>1?n.unique(f):f)},index:function(a){return a?\"string\"==typeof a?n.inArray(this[0],n(a)):n.inArray(a.jquery?a[0]:a,this):this[0]&&this[0].parentNode?this.first().prevAll().length:-1},add:function(a,b){return this.pushStack(n.unique(n.merge(this.get(),n(a,b))))},addBack:function(a){return this.add(null==a?this.prevObject:this.prevObject.filter(a))}});function E(a,b){do a=a[b];while(a&&1!==a.nodeType);return a}n.each({parent:function(a){var b=a.parentNode;return b&&11!==b.nodeType?b:null},parents:function(a){return n.dir(a,\"parentNode\")},parentsUntil:function(a,b,c){return n.dir(a,\"parentNode\",c)},next:function(a){return E(a,\"nextSibling\")},prev:function(a){return E(a,\"previousSibling\")},nextAll:function(a){return n.dir(a,\"nextSibling\")},prevAll:function(a){return n.dir(a,\"previousSibling\")},nextUntil:function(a,b,c){return n.dir(a,\"nextSibling\",c)},prevUntil:function(a,b,c){return n.dir(a,\"previousSibling\",c)},siblings:function(a){return n.sibling((a.parentNode||{}).firstChild,a)},children:function(a){return n.sibling(a.firstChild)},contents:function(a){return n.nodeName(a,\"iframe\")?a.contentDocument||a.contentWindow.document:n.merge([],a.childNodes)}},function(a,b){n.fn[a]=function(c,d){var e=n.map(this,b,c);return\"Until\"!==a.slice(-5)&&(d=c),d&&\"string\"==typeof d&&(e=n.filter(d,e)),this.length>1&&(D[a]||(e=n.unique(e)),C.test(a)&&(e=e.reverse())),this.pushStack(e)}});var F=/\\S+/g,G={};function H(a){var b=G[a]={};return n.each(a.match(F)||[],function(a,c){b[c]=!0}),b}n.Callbacks=function(a){a=\"string\"==typeof a?G[a]||H(a):n.extend({},a);var b,c,d,e,f,g,h=[],i=!a.once&&[],j=function(l){for(c=a.memory&&l,d=!0,f=g||0,g=0,e=h.length,b=!0;h&&e>f;f++)if(h[f].apply(l[0],l[1])===!1&&a.stopOnFalse){c=!1;break}b=!1,h&&(i?i.length&&j(i.shift()):c?h=[]:k.disable())},k={add:function(){if(h){var d=h.length;!function f(b){n.each(b,function(b,c){var d=n.type(c);\"function\"===d?a.unique&&k.has(c)||h.push(c):c&&c.length&&\"string\"!==d&&f(c)})}(arguments),b?e=h.length:c&&(g=d,j(c))}return this},remove:function(){return h&&n.each(arguments,function(a,c){var d;while((d=n.inArray(c,h,d))>-1)h.splice(d,1),b&&(e>=d&&e--,f>=d&&f--)}),this},has:function(a){return a?n.inArray(a,h)>-1:!(!h||!h.length)},empty:function(){return h=[],e=0,this},disable:function(){return h=i=c=void 0,this},disabled:function(){return!h},lock:function(){return i=void 0,c||k.disable(),this},locked:function(){return!i},fireWith:function(a,c){return!h||d&&!i||(c=c||[],c=[a,c.slice?c.slice():c],b?i.push(c):j(c)),this},fire:function(){return k.fireWith(this,arguments),this},fired:function(){return!!d}};return k},n.extend({Deferred:function(a){var b=[[\"resolve\",\"done\",n.Callbacks(\"once memory\"),\"resolved\"],[\"reject\",\"fail\",n.Callbacks(\"once memory\"),\"rejected\"],[\"notify\",\"progress\",n.Callbacks(\"memory\")]],c=\"pending\",d={state:function(){return c},always:function(){return e.done(arguments).fail(arguments),this},then:function(){var a=arguments;return n.Deferred(function(c){n.each(b,function(b,f){var g=n.isFunction(a[b])&&a[b];e[f[1]](function(){var a=g&&g.apply(this,arguments);a&&n.isFunction(a.promise)?a.promise().done(c.resolve).fail(c.reject).progress(c.notify):c[f[0]+\"With\"](this===d?c.promise():this,g?[a]:arguments)})}),a=null}).promise()},promise:function(a){return null!=a?n.extend(a,d):d}},e={};return d.pipe=d.then,n.each(b,function(a,f){var g=f[2],h=f[3];d[f[1]]=g.add,h&&g.add(function(){c=h},b[1^a][2].disable,b[2][2].lock),e[f[0]]=function(){return e[f[0]+\"With\"](this===e?d:this,arguments),this},e[f[0]+\"With\"]=g.fireWith}),d.promise(e),a&&a.call(e,e),e},when:function(a){var b=0,c=d.call(arguments),e=c.length,f=1!==e||a&&n.isFunction(a.promise)?e:0,g=1===f?a:n.Deferred(),h=function(a,b,c){return function(e){b[a]=this,c[a]=arguments.length>1?d.call(arguments):e,c===i?g.notifyWith(b,c):--f||g.resolveWith(b,c)}},i,j,k;if(e>1)for(i=new Array(e),j=new Array(e),k=new Array(e);e>b;b++)c[b]&&n.isFunction(c[b].promise)?c[b].promise().done(h(b,k,c)).fail(g.reject).progress(h(b,j,i)):--f;return f||g.resolveWith(k,c),g.promise()}});var I;n.fn.ready=function(a){return n.ready.promise().done(a),this},n.extend({isReady:!1,readyWait:1,holdReady:function(a){a?n.readyWait++:n.ready(!0)},ready:function(a){if(a===!0?!--n.readyWait:!n.isReady){if(!z.body)return setTimeout(n.ready);n.isReady=!0,a!==!0&&--n.readyWait>0||(I.resolveWith(z,[n]),n.fn.trigger&&n(z).trigger(\"ready\").off(\"ready\"))}}});function J(){z.addEventListener?(z.removeEventListener(\"DOMContentLoaded\",K,!1),a.removeEventListener(\"load\",K,!1)):(z.detachEvent(\"onreadystatec"
,
"hange\",K),a.detachEvent(\"onload\",K))}function K(){(z.addEventListener||\"load\"===event.type||\"complete\"===z.readyState)&&(J(),n.ready())}n.ready.promise=function(b){if(!I)if(I=n.Deferred(),\"complete\"===z.readyState)setTimeout(n.ready);else if(z.addEventListener)z.addEventListener(\"DOMContentLoaded\",K,!1),a.addEventListener(\"load\",K,!1);else{z.attachEvent(\"onreadystatechange\",K),a.attachEvent(\"onload\",K);var c=!1;try{c=null==a.frameElement&&z.documentElement}catch(d){}c&&c.doScroll&&!function e(){if(!n.isReady){try{c.doScroll(\"left\")}catch(a){return setTimeout(e,50)}J(),n.ready()}}()}return I.promise(b)};var L=\"undefined\",M;for(M in n(l))break;l.ownLast=\"0\"!==M,l.inlineBlockNeedsLayout=!1,n(function(){var a,b,c=z.getElementsByTagName(\"body\")[0];c&&(a=z.createElement(\"div\"),a.style.cssText=\"border:0;width:0;height:0;position:absolute;top:0;left:-9999px;margin-top:1px\",b=z.createElement(\"div\"),c.appendChild(a).appendChild(b),typeof b.style.zoom!==L&&(b.style.cssText=\"border:0;margin:0;width:1px;padding:1px;display:inline;zoom:1\",(l.inlineBlockNeedsLayout=3===b.offsetWidth)&&(c.style.zoom=1)),c.removeChild(a),a=b=null)}),function(){var a=z.createElement(\"div\");if(null==l.deleteExpando){l.deleteExpando=!0;try{delete a.test}catch(b){l.deleteExpando=!1}}a=null}(),n.acceptData=function(a){var b=n.noData[(a.nodeName+\" \").toLowerCase()],c=+a.nodeType||1;return 1!==c&&9!==c?!1:!b||b!==!0&&a.getAttribute(\"classid\")===b};var N=/^(?:\\{[\\w\\W]*\\}|\\[[\\w\\W]*\\])$/,O=/([A-Z])/g;function P(a,b,c){if(void 0===c&&1===a.nodeType){var d=\"data-\"+b.replace(O,\"-$1\").toLowerCase();if(c=a.getAttribute(d),\"string\"==typeof c){try{c=\"true\"===c?!0:\"false\"===c?!1:\"null\"===c?null:+c+\"\"===c?+c:N.test(c)?n.parseJSON(c):c}catch(e){}n.data(a,b,c)}else c=void 0}return c}function Q(a){var b;for(b in a)if((\"data\"!==b||!n.isEmptyObject(a[b]))&&\"toJSON\"!==b)return!1;return!0}function R(a,b,d,e){if(n.acceptData(a)){var f,g,h=n.expando,i=a.nodeType,j=i?n.cache:a,k=i?a[h]:a[h]&&h;if(k&&j[k]&&(e||j[k].data)||void 0!==d||\"string\"!=typeof b)return k||(k=i?a[h]=c.pop()||n.guid++:h),j[k]||(j[k]=i?{}:{toJSON:n.noop}),(\"object\"==typeof b||\"function\"==typeof b)&&(e?j[k]=n.extend(j[k],b):j[k].data=n.extend(j[k].data,b)),g=j[k],e||(g.data||(g.data={}),g=g.data),void 0!==d&&(g[n.camelCase(b)]=d),\"string\"==typeof b?(f=g[b],null==f&&(f=g[n.camelCase(b)])):f=g,f\n"
,
"}}function S(a,b,c){if(n.acceptData(a)){var d,e,f=a.nodeType,g=f?n.cache:a,h=f?a[n.expando]:n.expando;if(g[h]){if(b&&(d=c?g[h]:g[h].data)){n.isArray(b)?b=b.concat(n.map(b,n.camelCase)):b in d?b=[b]:(b=n.camelCase(b),b=b in d?[b]:b.split(\" \")),e=b.length;while(e--)delete d[b[e]];if(c?!Q(d):!n.isEmptyObject(d))return}(c||(delete g[h].data,Q(g[h])))&&(f?n.cleanData([a],!0):l.deleteExpando||g!=g.window?delete g[h]:g[h]=null)}}}n.extend({cache:{},noData:{\"applet \":!0,\"embed \":!0,\"object \":\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\"},hasData:function(a){return a=a.nodeType?n.cache[a[n.expando]]:a[n.expando],!!a&&!Q(a)},data:function(a,b,c){return R(a,b,c)},removeData:function(a,b){return S(a,b)},_data:function(a,b,c){return R(a,b,c,!0)},_removeData:function(a,b){return S(a,b,!0)}}),n.fn.extend({data:function(a,b){var c,d,e,f=this[0],g=f&&f.attributes;if(void 0===a){if(this.length&&(e=n.data(f),1===f.nodeType&&!n._data(f,\"parsedAttrs\"))){c=g.length;while(c--)d=g[c].name,0===d.indexOf(\"data-\")&&(d=n.camelCase(d.slice(5)),P(f,d,e[d]));n._data(f,\"parsedAttrs\",!0)}return e}return\"object\"==typeof a?this.each(function(){n.data(this,a)}):arguments.length>1?this.each(function(){n.data(this,a,b)}):f?P(f,a,n.data(f,a)):void 0},removeData:function(a){return this.each(function(){n.removeData(this,a)})}}),n.extend({queue:function(a,b,c){var d;return a?(b=(b||\"fx\")+\"queue\",d=n._data(a,b),c&&(!d||n.isArray(c)?d=n._data(a,b,n.makeArray(c)):d.push(c)),d||[]):void 0},dequeue:function(a,b){b=b||\"fx\";var c=n.queue(a,b),d=c.length,e=c.shift(),f=n._queueHooks(a,b),g=function(){n.dequeue(a,b)};\"inprogress\"===e&&(e=c.shift(),d--),e&&(\"fx\"===b&&c.unshift(\"inprogress\"),delete f.stop,e.call(a,g,f)),!d&&f&&f.empty.fire()},_queueHooks:function(a,b){var c=b+\"queueHooks\";return n._data(a,c)||n._data(a,c,{empty:n.Callbacks(\"once memory\").add(function(){n._removeData(a,b+\"queue\"),n._removeData(a,c)})})}}),n.fn.extend({queue:function(a,b){var c=2;return\"string\"!=typeof a&&(b=a,a=\"fx\",c--),arguments.length<c?n.queue(this[0],a):void 0===b?this:this.each(function(){var c=n.queue(this,a,b);n._queueHooks(this,a),\"fx\"===a&&\"inprogress\"!==c[0]&&n.dequeue(this,a)})},dequeue:function(a){return this.each(function(){n.dequeue(this,a)})},clearQueue:function(a){return this.queue(a||\"fx\",[])},promise:function(a,b){var c,d=1,e=n.Deferred(),f=this,g=this.length,h=function(){--d||e.resolveWith(f,[f])};\"string\"!=typeof a&&(b=a,a=void 0),a=a||\"fx\";while(g--)c=n._data(f[g],a+\"queueHooks\"),c&&c.empty&&(d++,c.empty.add(h));return h(),e.promise(b)}});var T=/[+-]?(?:\\d*\\.|)\\d+(?:[eE][+-]?\\d+|)/.source,U=[\"Top\",\"Right\",\"Bottom\",\"Left\"],V=function(a,b){return a=b||a,\"none\"===n.css(a,\"display\")||!n.contains(a.ownerDocument,a)},W=n.access=function(a,b,c,d,e,f,g){var h=0,i=a.length,j=null==c;if(\"object\"===n.type(c)){e=!0;for(h in c)n.access(a,b,h,c[h],!0,f,g)}else if(void 0!==d&&(e=!0,n.isFunction(d)||(g=!0),j&&(g?(b.call(a,d),b=null):(j=b,b=function(a,b,c){return j.call(n(a),c)})),b))for(;i>h;h++)b(a[h],c,g?d:d.call(a[h],h,b(a[h],c)));return e?a:j?b.call(a):i?b(a[0],c):f},X=/^(?:checkbox|radio)$/i;!function(){var a=z.createDocumentFragment(),b=z.createElement(\"div\"),c=z.createElement(\"input\");if(b.setAttribute(\"className\",\"t\"),b.innerHTML=\"  <link/><table></table><a href='/a'>a</a>\",l.leadingWhitespace=3===b.firstChild.nodeType,l.tbody=!b.getElementsByTagName(\"tbody\").length,l.htmlSerialize=!!b.getElementsByTagName(\"link\").length,l.html5Clone=\"<:nav></:nav>\"!==z.createElement(\"nav\").cloneNode(!0).outerHTML,c.type=\"checkbox\",c.checked=!0,a.appendChild(c),l.appendChecked=c.checked,b.innerHTML=\"<textarea>x</textarea>\",l.noCloneChecked=!!b.cloneNode(!0).lastChild.defaultValue,a.appendChild(b),b.innerHTML=\"<input type='radio' checked='checked' name='t'/>\",l.checkClone=b.cloneNode(!0).cloneNode(!0).lastChild.checked,l.noCloneEvent=!0,b.attachEvent&&(b.attachEvent(\"onclick\",function(){l.noCloneEvent=!1}),b.cloneNode(!0).click()),null==l.deleteExpando){l.deleteExpando=!0;try{delete b.test}catch(d){l.deleteExpando=!1}}a=b=c=null}(),function(){var b,c,d=z.createElement(\"div\");for(b in{submit:!0,change:!0,focusin:!0})c=\"on\"+b,(l[b+\"Bubbles\"]=c in a)||(d.setAttribute(c,\"t\"),l[b+\"Bubbles\"]=d.attributes[c].expando===!1);d=null}();var Y=/^(?:input|select|textarea)$/i,Z=/^key/,$=/^(?:mouse|contextmenu)|click/,_=/^(?:focusinfocus|focusoutblur)$/,ab=/^([^.]*)(?:\\.(.+)|)$/;function bb(){return!0}function cb(){return!1}function db(){try{return z.activeElement}catch(a){}}n.event={global:{},add:function(a,b,c,d,e){var f,g,h,i,j,k,l,m,o,p,q,r=n._data(a);if(r){c.handler&&(i=c,c=i.handler,e=i.selector),c.guid||(c.guid=n.guid++),(g=r.events)||(g=r.events={}),(k=r.handle)||(k=r.handle=function(a){return typeof n===L||a&&n.event.triggered===a.type?void 0:n.event.dispatch.apply(k.elem,arguments)},k.elem=a),b=(b||\"\").match(F)||[\"\"],h=b.length;while(h--)f=ab.exec(b[h])||[],o=q=f[1],p=(f[2]||\"\").split(\".\").sort(),o&&(j=n.event.special[o]||{},o=(e?j.delegateType:j.bindType)||o,j=n.event.special[o]||{},l=n.extend({type:o,origType:q,data:d,handler:c,guid:c.guid,selector:e,needsContext:e&&n.expr.match.needsContext.test(e),namespace:p.join(\".\")},i),(m=g[o])||(m=g[o]=[],m.delegateCount=0,j.setup&&j.setup.call(a,d,p,k)!==!1||(a.addEventListener?a.addEventListener(o,k,!1):a.attachEvent&&a.attachEvent(\"on\"+o,k))),j.add&&(j.add.call(a,l),l.handler.guid||(l.handler.guid=c.guid)),e?m.splice(m.delegateCount++,0,l):m.push(l),n.event.global[o]=!0);a=null}},remove:function(a,b,c,d,e){var f,g,h,i,j,k,l,m,o,p,q,r=n.hasData(a)&&n._data(a);if(r&&(k=r.events)){b=(b||\"\").match(F)||[\"\"],j=b.length;while(j--)if(h=ab.exec(b[j])||[],o=q=h[1],p=(h[2]||\"\").split(\".\").sort(),o){l=n.event.special[o]||{},o=(d?l.delegateType:l.bindType)||o,m=k[o]||[],h=h[2]&&new RegExp(\"(^|\\\\.)\"+p.join(\"\\\\.(?:.*\\\\.|)\")+\"(\\\\.|$)\"),i=f=m.length;while(f--)g=m[f],!e&&q!==g.origType||c&&c.guid!==g.guid||h&&!h.test(g.namespace)||d&&d!==g.selector&&(\"**\"!==d||!g.selector)||(m.splice(f,1),g.selector&&m.delegateCount--,l.remove&&l.remove.call(a,g));i&&!m.length&&(l.teardown&&l.teardown.call(a,p,r.handle)!==!1||n.removeEvent(a,o,r.handle),delete k[o])}else for(o in k)n.event.remove(a,o+b[j],c,d,!0);n.isEmptyObject(k)&&(delete r.handle,n._removeData(a,\"events\"))}},trigger:function(b,c,d,e){var f,g,h,i,k,l,m,o=[d||z],p=j.call(b,\"type\")?b.type:b,q=j.call(b,\"namespace\")?b.namespace.split(\".\"):[];if(h=l=d=d||z,3!==d.nodeType&&8!==d.nodeType&&!_.test(p+n.event.triggered)&&(p.indexOf(\".\")>=0&&(q=p.split(\".\"),p=q.shift(),q.sort()),g=p.indexOf(\":\")<0&&\"on\"+p,b=b[n.expando]?b:new n.Event(p,\"object\"==typeof b&&b),b.isTrigger=e?2:3,b.namespace=q.join(\".\"),b.namespace_re=b.namespace?new RegExp(\"(^|\\\\.)\"+q.join(\"\\\\.(?:.*\\\\.|)\")+\"(\\\\.|$)\"):null,b.result=void 0,b.target||(b.target=d),c=null==c?[b]:n.makeArray(c,[b]),k=n.event.special[p]||{},e||!k.trigger||k.trigger.apply(d,c)!==!1)){if(!e&&!k.noBubble&&!n.isWindow(d)){for(i=k.delegateType||p,_.test(i+p)||(h=h.parentNode);h;h=h.parentNode)o.push(h),l=h;l===(d.ownerDocument||z)&&o.push(l.defaultView||l.parentWindow||a)}m=0;while((h=o[m++])&&!b.isPropagationStopped())b.type=m>1?i:k.bindType||p,f=(n._data(h,\"events\")||{})[b.type]&&n._data(h,\"handle\"),f&&f.apply(h,c),f=g&&h[g],f&&f.apply&&n.acceptData(h)&&(b.result=f.apply(h,c),b.result===!1&&b.preventDefault());if(b.type=p,!e&&!b.isDefaultPrevented()&&(!k._default||k._default.apply(o.pop(),c)===!1)&&n.acceptData(d)&&g&&d[p]&&!n.isWindow(d)){l=d[g],l&&(d[g]=null),n.event.triggered=p;try{d[p]()}catch(r){}n.event.triggered=void 0,l&&(d[g]=l)}return b.result}},dispatch:function(a){a=n.event.fix(a);var b,c,e,f,g,h=[],i=d.call(arguments),j=(n._data(this,\"events\")||{})[a.type]||[],k=n.event.special[a.type]||{};if(i[0]=a,a.delegateTarget=this,!k.preDispatch||k.preDispatch.call(this,a)!==!1){h=n.event.handlers.call(this,a,j),b=0;while((f=h[b++])&&!a.isPropagationStopped()){a.currentTarget=f.elem,g=0;while((e=f.handlers[g++])&&!a.isImmediatePropagationStopped())(!a.namespace_re||a.namespace_re.test(e.namespace))&&(a.handleObj=e,a.data=e.data,c=((n.event.special[e.origType]||{}).handle||e.handler).apply(f.elem,i),void 0!==c&&(a.result=c)===!1&&(a.preventDefault(),a.stopPropagation()))}return k.postDispatch&&k.postDispatch.call(this,a),a.result}},handlers:function(a,b){var c,d,e,f,g=[],h=b.delegateCount,i=a.target;if(h&&i.nodeType&&(!a.button||\"click\"!==a.type))for(;i!=this;i=i.parentNode||this)if(1===i.nodeType&&(i.disabled!==!0||\"click\"!==a.type)){for(e=[],f=0;h>f;f++)d=b[f],c=d.selector+\" \",void 0===e[c]&&(e[c]=d.needsContext?n(c,this).index(i)>=0:n.find(c,this,null,[i]).length),e[c]&&e.push(d);e.length&&g.push({elem:i,handlers:e})}return h<b.length&&g.push({elem:this,handlers:b.slice(h)}),g},fix:function(a){if(a[n.expando])return a;var b,c,d,e=a.type,f=a,g=this.fixHooks[e];g||(this.fixHooks[e]=g=$.test(e)?this.mouseHooks:Z.test(e)?this.keyHooks:{}),d=g.props?this.props.concat(g.props):this.props,a=new n.Event(f),b=d.length;while(b--)c=d[b],a[c]=f[c];return a.target||(a.target=f.srcElement||z),3===a.target.nodeType&&(a.target=a.target.parentNode),a.metaKey=!!a.metaKey,g.filter?g.filter(a,f):a},props:\"altKey bubbles cancelable ctrlKey currentTarget eventPhase metaKey relatedTarget shiftKey target timeStamp view which\".split(\" \"),fixHooks:{},keyHooks:{props:\"char charCode key keyCode\".split(\" \"),filter:function(a,b){return null==a.which&&(a.which=null!=b.charCode?b.charCode:b.keyCode),a}},mouseHooks:{props:\"button buttons clientX clientY fromElement offsetX offsetY pageX pageY screenX screenY toElement\".split(\" \"),filter:function(a,b){var c,d,e,f=b.button,g=b.fromElement;return null==a.pageX&&null!=b.clientX&&(d=a.target.ownerDocument||z,e=d.documentElement,c=d.body,a.pageX=b.clientX+(e&&e.scrollLeft||c&&c.scrollLeft||0)-(e&&e.clientLeft||c&&c.clientLeft||0),a.pageY=b.clientY+(e&&e.scrollTop||c&&c.scrollTop||0)-(e&&e.clientTop||c&&c.clientTop||0)),!a.relatedTarget&&g&&(a.relatedTarget=g===a.target?b.toElement:g),a.which||void 0===f||(a.which=1&f?1:2&f?3:4&f?2:0),a}},special:{load:{noBubble:!0},f"
,
"ocus:{trigger:function(){if(this!==db()&&this.focus)try{return this.focus(),!1}catch(a){}},delegateType:\"focusin\"},blur:{trigger:function(){return this===db()&&this.blur?(this.blur(),!1):void 0},delegateType:\"focusout\"},click:{trigger:function(){return n.nodeName(this,\"input\")&&\"checkbox\"===this.type&&this.click?(this.click(),!1):void 0},_default:function(a){return n.nodeName(a.target,\"a\")}},beforeunload:{postDispatch:function(a){void 0!==a.result&&(a.originalEvent.returnValue=a.result)}}},simulate:function(a,b,c,d){var e=n.extend(new n.Event,c,{type:a,isSimulated:!0,originalEvent:{}});d?n.event.trigger(e,null,b):n.event.dispatch.call(b,e),e.isDefaultPrevented()&&c.preventDefault()}},n.removeEvent=z.removeEventListener?function(a,b,c){a.removeEventListener&&a.removeEventListener(b,c,!1)}:function(a,b,c){var d=\"on\"+b;a.detachEvent&&(typeof a[d]===L&&(a[d]=null),a.detachEvent(d,c))},n.Event=function(a,b){return this instanceof n.Event?(a&&a.type?(this.originalEvent=a,this.type=a.type,this.isDefaultPrevented=a.defaultPrevented||void 0===a.defaultPrevented&&(a.returnValue===!1||a.getPreventDefault&&a.getPreventDefault())?bb:cb):this.type=a,b&&n.extend(this,b),this.timeStamp=a&&a.timeStamp||n.now(),void(this[n.expando]=!0)):new n.Event(a,b)},n.Event.prototype={isDefaultPrevented:cb,isPropagationStopped:cb,isImmediatePropagationStopped:cb,preventDefault:function(){var a=this.originalEvent;this.isDefaultPrevented=bb,a&&(a.preventDefault?a.preventDefault():a.returnValue=!1)},stopPropagation:function(){var a=this.originalEvent;this.isPropagationStopped=bb,a&&(a.stopPropagation&&a.stopPropagation(),a.cancelBubble=!0)},stopImmediatePropagation:function(){this.isImmediatePropagationStopped=bb,this.stopPropagation()}},n.each({mouseenter:\"mouseover\",mouseleave:\"mouseout\"},function(a,b){n.event.special[a]={delegateType:b,bindType:b,handle:function(a){var c,d=this,e=a.relatedTarget,f=a.handleObj;return(!e||e!==d&&!n.contains(d,e))&&(a.type=f.origType,c=f.handler.apply(this,arguments),a.type=b),c}}}),l.submitBubbles||(n.event.special.submit={setup:function(){return n.nodeName(this,\"form\")?!1:void n.event.add(this,\"click._submit keypress._submit\",function(a){var b=a.target,c=n.nodeName(b,\"input\")||n.nodeName(b,\"button\")?b.form:void 0;c&&!n._data(c,\"submitBubbles\")&&(n.event.add(c,\"submit._submit\",function(a){a._submit_bubble=!0}),n._data(c,\"submitBubbles\",!0))})},postDispatch:function(a){a._submit_bubble&&(delete a._submit_bubble,this.parentNode&&!a.isTrigger&&n.event.simulate(\"submit\",this.parentNode,a,!0))},teardown:function(){return n.nodeName(this,\"form\")?!1:void n.event.remove(this,\"._submit\")}}),l.changeBubbles||(n.event.special.change={setup:function(){return Y.test(this.nodeName)?((\"checkbox\"===this.type||\"radio\"===this.type)&&(n.event.add(this,\"propertychange._change\",function(a){\"checked\"===a.originalEvent.propertyName&&(this._just_changed=!0)}),n.event.add(this,\"click._change\",function(a){this._just_changed&&!a.isTrigger&&(this._just_changed=!1),n.event.simulate(\"change\",this,a,!0)})),!1):void n.event.add(this,\"beforeactivate._change\",function(a){var b=a.target;Y.test(b.nodeName)&&!n._data(b,\"changeBubbles\")&&(n.event.add(b,\"change._change\",function(a){!this.parentNode||a.isSimulated||a.isTrigger||n.event.simulate(\"change\",this.parentNode,a,!0)}),n._data(b,\"changeBubbles\",!0))})},handle:function(a){var b=a.target;return this!==b||a.isSimulated||a.isTrigger||\"radio\"!==b.type&&\"checkbox\"!==b.type?a.handleObj.handler.apply(this,arguments):void 0},teardown:function(){return n.event.remove(this,\"._change\"),!Y.test(this.nodeName)}}),l.focusinBubbles||n.each({focus:\"focusin\",blur:\"focusout\"},function(a,b){var c=function(a){n.event.simulate(b,a.target,n.event.fix(a),!0)};n.event.special[b]={setup:function(){var d=this.ownerDocument||this,e=n._data(d,b);e||d.addEventListener(a,c,!0),n._data(d,b,(e||0)+1)},teardown:function(){var d=this.ownerDocument||this,e=n._data(d,b)-1;e?n._data(d,b,e):(d.removeEventListener(a,c,!0),n._removeData(d,b))}}}),n.fn.extend({on:function(a,b,c,d,e){var f,g;if(\"object\"==typeof a){\"string\"!=typeof b&&(c=c||b,b=void 0);for(f in a)this.on(f,b,c,a[f],e);return this}if(null==c&&null==d?(d=b,c=b=void 0):null==d&&(\"string\"==typeof b?(d=c,c=void 0):(d=c,c=b,b=void 0)),d===!1)d=cb;else if(!d)return this;return 1===e&&(g=d,d=function(a){return n().off(a),g.apply(this,arguments)},d.guid=g.guid||(g.guid=n.guid++)),this.each(function(){n.event.add(this,a,d,c,b)})},one:function(a,b,c,d){return this.on(a,b,c,d,1)},off:function(a,b,c){var d,e;if(a&&a.preventDefault&&a.handleObj)return d=a.handleObj,n(a.delegateTarget).off(d.namespace?d.origType+\".\"+d.namespace:d.origType,d.selector,d.handler),this;if(\"object\"==typeof a){for(e in a)this.off(e,b,a[e]);return this}return(b===!1||\"function\"==typeof b)&&(c=b,b=void 0),c===!1&&(c=cb),this.each(function(){n.event.remove(this,a,c,b)})},trigger:function(a,b){return this.each(function(){n.event.trigger(a,b,this)})},triggerHandler:function(a,b){var c=this[0];return c?n.event.trigger(a,b,c,!0):void 0}});function eb(a){var b=fb.split(\"|\"),c=a.createDocumentFragment();if(c.createElement)while(b.length)c.createElement(b.pop());return c}var fb=\"abbr|article|aside|audio|bdi|canvas|data|datalist|details|figcaption|figure|footer|header|hgroup|mark|meter|nav|output|progress|section|summary|time|video\",gb=/ jQuery\\d+=\"(?:null|\\d+)\"/g,hb=new RegExp(\"<(?:\"+fb+\")[\\\\s/>]\",\"i\"),ib=/^\\s+/,jb=/<(?!area|br|col|embed|hr|img|input|link|meta|param)(([\\w:]+)[^>]*)\\/>/gi,kb=/<([\\w:]+)/,lb=/<tbody/i,mb=/<|&#?\\w+;/,nb=/<(?:script|style|link)/i,ob=/checked\\s*(?:[^=]|=\\s*.checked.)/i,pb=/^$|\\/(?:java|ecma)script/i,qb=/^true\\/(.*)/,rb=/^\\s*<!(?:\\[CDATA\\[|--)|(?:\\]\\]|--)>\\s*$/g,sb={option:[1,\"<select multiple='multiple'>\",\"</select>\"],legend:[1,\"<fieldset>\",\"</fieldset>\"],area:[1,\"<map>\",\"</map>\"],param:[1,\"<object>\",\"</object>\"],thead:[1,\"<table>\",\"</table>\"],tr:[2,\"<table><tbody>\",\"</tbody></table>\"],col:[2,\"<table><tbody></tbody><colgroup>\",\"</colgroup></table>\"],td:[3,\"<table><tbody><tr>\",\"</tr></tbody></table>\"],_default:l.htmlSerialize?[0,\"\",\"\"]:[1,\"X<div>\",\"</div>\"]},tb=eb(z),ub=tb.appendChild(z.createElement(\"div\"));sb.optgroup=sb.option,sb.tbody=sb.tfoot=sb.colgroup=sb.caption=sb.thead,sb.th=sb.td;function vb(a,b){var c,d,e=0,f=typeof a.getElementsByTagName!==L?a.getElementsByTagName(b||\"*\"):typeof a.querySelectorAll!==L?a.querySelectorAll(b||\"*\"):void 0;if(!f)for(f=[],c=a.childNodes||a;null!=(d=c[e]);e++)!b||n.nodeName(d,b)?f.push(d):n.merge(f,vb(d,b));return void 0===b||b&&n.nodeName(a,b)?n.merge([a],f):f}function wb(a){X.test(a.type)&&(a.defaultChecked=a.checked)}function xb(a,b){return n.nodeName(a,\"table\")&&n.nodeName(11!==b.nodeType?b:b.firstChild,\"tr\")?a.getElementsByTagName(\"tbody\")[0]||a.appendChild(a.ownerDocument.createElement(\"tbody\")):a}function yb(a){return a.type=(null!==n.find.attr(a,\"type\"))+\"/\"+a.type,a}function zb(a){var b=qb.exec(a.type);return b?a.type=b[1]:a.removeAttribute(\"type\"),a}function Ab(a,b){for(var c,d=0;null!=(c=a[d]);d++)n._data(c,\"globalEval\",!b||n._data(b[d],\"globalEval\"))}function Bb(a,b){if(1===b.nodeType&&n.hasData(a)){var c,d,e,f=n._data(a),g=n._data(b,f),h=f.events;if(h){delete g.handle,g.events={};for(c in h)for(d=0,e=h[c].length;e>d;d++)n.event.add(b,c,h[c][d])}g.data&&(g.data=n.extend({},g.data))}}function Cb(a,b){var c,d,e;if(1===b.nodeType){if(c=b.nodeName.toLowerCase(),!l.noCloneEvent&&b[n.expando]){e=n._data(b);for(d in e.events)n.removeEvent(b,d,e.handle);b.removeAttribute(n.expando)}\"script\"===c&&b.text!==a.text?(yb(b).text=a.text,zb(b)):\"object\"===c?(b.parentNode&&(b.outerHTML=a.outerHTML),l.html5Clone&&a.innerHTML&&!n.trim(b.innerHTML)&&(b.innerHTML=a.innerHTML)):\"input\"===c&&X.test(a.type)?(b.defaultChecked=b.checked=a.checked,b.value!==a.value&&(b.value=a.value)):\"option\"===c?b.defaultSelected=b.selected=a.defaultSelected:(\"input\"===c||\"textarea\"===c)&&(b.defaultValue=a.defaultValue)}}n.extend({clone:function(a,b,c){var d,e,f,g,h,i=n.contains(a.ownerDocument,a);if(l.html5Clone||n.isXMLDoc(a)||!hb.test(\"<\"+a.nodeName+\">\")?f=a.cloneNode(!0):(ub.innerHTML=a.outerHTML,ub.removeChild(f=ub.firstChild)),!(l.noCloneEvent&&l.noCloneChecked||1!==a.nodeType&&11!==a.nodeType||n.isXMLDoc(a)))for(d=vb(f),h=vb(a),g=0;null!=(e=h[g]);++g)d[g]&&Cb(e,d[g]);if(b)if(c)for(h=h||vb(a),d=d||vb(f),g=0;null!=(e=h[g]);g++)Bb(e,d[g]);else Bb(a,f);return d=vb(f,\"script\"),d.length>0&&Ab(d,!i&&vb(a,\"script\")),d=h=e=null,f},buildFragment:function(a,b,c,d){for(var e,f,g,h,i,j,k,m=a.length,o=eb(b),p=[],q=0;m>q;q++)if(f=a[q],f||0===f)if(\"object\"===n.type(f))n.merge(p,f.nodeType?[f]:f);else if(mb.test(f)){h=h||o.appendChild(b.createElement(\"div\")),i=(kb.exec(f)||[\"\",\"\"])[1].toLowerCase(),k=sb[i]||sb._default,h.innerHTML=k[1]+f.replace(jb,\"<$1></$2>\")+k[2],e=k[0];while(e--)h=h.lastChild;if(!l.leadingWhitespace&&ib.test(f)&&p.push(b.createTextNode(ib.exec(f)[0])),!l.tbody){f=\"table\"!==i||lb.test(f)?\"<table>\"!==k[1]||lb.test(f)?0:h:h.firstChild,e=f&&f.childNodes.length;while(e--)n.nodeName(j=f.childNodes[e],\"tbody\")&&!j.childNodes.length&&f.removeChild(j)}n.merge(p,h.childNodes),h.textContent=\"\";while(h.firstChild)h.removeChild(h.firstChild);h=o.lastChild}else p.push(b.createTextNode(f));h&&o.removeChild(h),l.appendChecked||n.grep(vb(p,\"input\"),wb),q=0;while(f=p[q++])if((!d||-1===n.inArray(f,d))&&(g=n.contains(f.ownerDocument,f),h=vb(o.appendChild(f),\"script\"),g&&Ab(h),c)){e=0;while(f=h[e++])pb.test(f.type||\"\")&&c.push(f)}return h=null,o},cleanData:function(a,b){for(var d,e,f,g,h=0,i=n.expando,j=n.cache,k=l.deleteExpando,m=n.event.special;null!=(d=a[h]);h++)if((b||n.acceptData(d))&&(f=d[i],g=f&&j[f])){if(g.events)for(e in g.events)m[e]?n.event.remove(d,e):n.removeEvent(d,e,g.handle);j[f]&&(delete j[f],k?delete d[i]:typeof d.removeAttribute!==L?d.removeAttribute(i):d[i]=null,c.push(f))}}}),n.fn.extend({text:function(a){return W(this,function(a){return void 0===a?n.text(this):this.empty().append((this[0]&&this[0].ownerDocument||z).createTextNode(a))},null,a,arguments.length)},app"
,
"end:function(){return this.domManip(arguments,function(a){if(1===this.nodeType||11===this.nodeType||9===this.nodeType){var b=xb(this,a);b.appendChild(a)}})},prepend:function(){return this.domManip(arguments,function(a){if(1===this.nodeType||11===this.nodeType||9===this.nodeType){var b=xb(this,a);b.insertBefore(a,b.firstChild)}})},before:function(){return this.domManip(arguments,function(a){this.parentNode&&this.parentNode.insertBefore(a,this)})},after:function(){return this.domManip(arguments,function(a){this.parentNode&&this.parentNode.insertBefore(a,this.nextSibling)})},remove:function(a,b){for(var c,d=a?n.filter(a,this):this,e=0;null!=(c=d[e]);e++)b||1!==c.nodeType||n.cleanData(vb(c)),c.parentNode&&(b&&n.contains(c.ownerDocument,c)&&Ab(vb(c,\"script\")),c.parentNode.removeChild(c));return this},empty:function(){for(var a,b=0;null!=(a=this[b]);b++){1===a.nodeType&&n.cleanData(vb(a,!1));while(a.firstChild)a.removeChild(a.firstChild);a.options&&n.nodeName(a,\"select\")&&(a.options.length=0)}return this},clone:function(a,b){return a=null==a?!1:a,b=null==b?a:b,this.map(function(){return n.clone(this,a,b)})},html:function(a){return W(this,function(a){var b=this[0]||{},c=0,d=this.length;if(void 0===a)return 1===b.nodeType?b.innerHTML.replace(gb,\"\"):void 0;if(!(\"string\"!=typeof a||nb.test(a)||!l.htmlSerialize&&hb.test(a)||!l.leadingWhitespace&&ib.test(a)||sb[(kb.exec(a)||[\"\",\"\"])[1].toLowerCase()])){a=a.replace(jb,\"<$1></$2>\");try{for(;d>c;c++)b=this[c]||{},1===b.nodeType&&(n.cleanData(vb(b,!1)),b.innerHTML=a);b=0}catch(e){}}b&&this.empty().append(a)},null,a,arguments.length)},replaceWith:function(){var a=arguments[0];return this.domManip(arguments,function(b){a=this.parentNode,n.cleanData(vb(this)),a&&a.replaceChild(b,this)}),a&&(a.length||a.nodeType)?this:this.remove()},detach:function(a){return this.remove(a,!0)},domManip:function(a,b){a=e.apply([],a);var c,d,f,g,h,i,j=0,k=this.length,m=this,o=k-1,p=a[0],q=n.isFunction(p);if(q||k>1&&\"string\"==typeof p&&!l.checkClone&&ob.test(p))return this.each(function(c){var d=m.eq(c);q&&(a[0]=p.call(this,c,d.html())),d.domManip(a,b)});if(k&&(i=n.buildFragment(a,this[0].ownerDocument,!1,this),c=i.firstChild,1===i.childNodes.length&&(i=c),c)){for(g=n.map(vb(i,\"script\"),yb),f=g.length;k>j;j++)d=i,j!==o&&(d=n.clone(d,!0,!0),f&&n.merge(g,vb(d,\"script\"))),b.call(this[j],d,j);if(f)for(h=g[g.length-1].ownerDocument,n.map(g,zb),j=0;f>j;j++)d=g[j],pb.test(d.type||\"\")&&!n._data(d,\"globalEval\")&&n.contains(h,d)&&(d.src?n._evalUrl&&n._evalUrl(d.src):n.globalEval((d.text||d.textContent||d.innerHTML||\"\").replace(rb,\"\")));i=c=null}return this}}),n.each({appendTo:\"append\",prependTo:\"prepend\",insertBefore:\"before\",insertAfter:\"after\",replaceAll:\"replaceWith\"},function(a,b){n.fn[a]=function(a){for(var c,d=0,e=[],g=n(a),h=g.length-1;h>=d;d++)c=d===h?this:this.clone(!0),n(g[d])[b](c),f.apply(e,c.get());return this.pushStack(e)}});var Db,Eb={};function Fb(b,c){var d=n(c.createElement(b)).appendTo(c.body),e=a.getDefaultComputedStyle?a.getDefaultComputedStyle(d[0]).display:n.css(d[0],\"display\");return d.detach(),e}function Gb(a){var b=z,c=Eb[a];return c||(c=Fb(a,b),\"none\"!==c&&c||(Db=(Db||n(\"<iframe frameborder='0' width='0' height='0'/>\")).appendTo(b.documentElement),b=(Db[0].contentWindow||Db[0].contentDocument).document,b.write(),b.close(),c=Fb(a,b),Db.detach()),Eb[a]=c),c}!function(){var a,b,c=z.createElement(\"div\"),d=\"-webkit-box-sizing:content-box;-moz-box-sizing:content-box;box-sizing:content-box;display:block;padding:0;margin:0;border:0\";c.innerHTML=\"  <link/><table></table><a href='/a'>a</a><input type='checkbox'/>\",a=c.getElementsByTagName(\"a\")[0],a.style.cssText=\"float:left;opacity:.5\",l.opacity=/^0.5/.test(a.style.opacity),l.cssFloat=!!a.style.cssFloat,c.style.backgroundClip=\"content-box\",c.cloneNode(!0).style.backgroundClip=\"\",l.clearCloneStyle=\"content-box\"===c.style.backgroundClip,a=c=null,l.shrinkWrapBlocks=function(){var a,c,e,f;if(null==b){if(a=z.getElementsByTagName(\"body\")[0],!a)return;f=\"border:0;width:0;height:0;position:absolute;top:0;left:-9999px\",c=z.createElement(\"div\"),e=z.createElement(\"div\"),a.appendChild(c).appendChild(e),b=!1,typeof e.style.zoom!==L&&(e.style.cssText=d+\";width:1px;padding:1px;zoom:1\",e.innerHTML=\"<div></div>\",e.firstChild.style.width=\"5px\",b=3!==e.offsetWidth),a.removeChild(c),a=c=e=null}return b}}();var Hb=/^margin/,Ib=new RegExp(\"^(\"+T+\")(?!px)[a-z%]+$\",\"i\"),Jb,Kb,Lb=/^(top|right|bottom|left)$/;a.getComputedStyle?(Jb=function(a){return a.ownerDocument.defaultView.getComputedStyle(a,null)},Kb=function(a,b,c){var d,e,f,g,h=a.style;return c=c||Jb(a),g=c?c.getPropertyValue(b)||c[b]:void 0,c&&(\"\"!==g||n.contains(a.ownerDocument,a)||(g=n.style(a,b)),Ib.test(g)&&Hb.test(b)&&(d=h.width,e=h.minWidth,f=h.maxWidth,h.minWidth=h.maxWidth=h.width=g,g=c.width,h.width=d,h.minWidth=e,h.maxWidth=f)),void 0===g?g:g+\"\"}):z.documentElement.currentStyle&&(Jb=function(a){return a.currentStyle},Kb=function(a,b,c){var d,e,f,g,h=a.style;return c=c||Jb(a),g=c?c[b]:void 0,null==g&&h&&h[b]&&(g=h[b]),Ib.test(g)&&!Lb.test(b)&&(d=h.left,e=a.runtimeStyle,f=e&&e.left,f&&(e.left=a.currentStyle.left),h.left=\"fontSize\"===b?\"1em\":g,g=h.pixelLeft+\"px\",h.left=d,f&&(e.left=f)),void 0===g?g:g+\"\"||\"auto\"});function Mb(a,b){return{get:function(){var c=a();if(null!=c)return c?void delete this.get:(this.get=b).apply(this,arguments)}}}!function(){var b,c,d,e,f,g,h=z.createElement(\"div\"),i=\"border:0;width:0;height:0;position:absolute;top:0;left:-9999px\",j=\"-webkit-box-sizing:content-box;-moz-box-sizing:content-box;box-sizing:content-box;display:block;padding:0;margin:0;border:0\";h.innerHTML=\"  <link/><table></table><a href='/a'>a</a><input type='checkbox'/>\",b=h.getElementsByTagName(\"a\")[0],b.style.cssText=\"float:left;opacity:.5\",l.opacity=/^0.5/.test(b.style.opacity),l.cssFloat=!!b.style.cssFloat,h.style.backgroundClip=\"content-box\",h.cloneNode(!0).style.backgroundClip=\"\",l.clearCloneStyle=\"content-box\"===h.style.backgroundClip,b=h=null,n.extend(l,{reliableHiddenOffsets:function(){if(null!=c)return c;var a,b,d,e=z.createElement(\"div\"),f=z.getElementsByTagName(\"body\")[0];if(f)return e.setAttribute(\"className\",\"t\"),e.innerHTML=\"  <link/><table></table><a href='/a'>a</a><input type='checkbox'/>\",a=z.createElement(\"div\"),a.style.cssText=i,f.appendChild(a).appendChild(e),e.innerHTML=\"<table><tr><td></td><td>t</td></tr></table>\",b=e.getElementsByTagName(\"td\"),b[0].style.cssText=\"padding:0;margin:0;border:0;display:none\",d=0===b[0].offsetHeight,b[0].style.display=\"\",b[1].style.display=\"none\",c=d&&0===b[0].offsetHeight,f.removeChild(a),e=f=null,c},boxSizing:function(){return null==d&&k(),d},boxSizingReliable:function(){return null==e&&k(),e},pixelPosition:function(){return null==f&&k(),f},reliableMarginRight:function(){var b,c,d,e;if(null==g&&a.getComputedStyle){if(b=z.getElementsByTagName(\"body\")[0],!b)return;c=z.createElement(\"div\"),d=z.createElement(\"div\"),c.style.cssText=i,b.appendChild(c).appendChild(d),e=d.appendChild(z.createElement(\"div\")),e.style.cssText=d.style.cssText=j,e.style.marginRight=e.style.width=\"0\",d.style.width=\"1px\",g=!parseFloat((a.getComputedStyle(e,null)||{}).marginRight),b.removeChild(c)}return g}});function k(){var b,c,h=z.getElementsByTagName(\"body\")[0];h&&(b=z.createElement(\"div\"),c=z.createElement(\"div\"),b.style.cssText=i,h.appendChild(b).appendChild(c),c.style.cssText=\"-webkit-box-sizing:border-box;-moz-box-sizing:border-box;box-sizing:border-box;position:absolute;display:block;padding:1px;border:1px;width:4px;margin-top:1%;top:1%\",n.swap(h,null!=h.style.zoom?{zoom:1}:{},function(){d=4===c.offsetWidth}),e=!0,f=!1,g=!0,a.getComputedStyle&&(f=\"1%\"!==(a.getComputedStyle(c,null)||{}).top,e=\"4px\"===(a.getComputedStyle(c,null)||{width:\"4px\"}).width),h.removeChild(b),c=h=null)}}(),n.swap=function(a,b,c,d){var e,f,g={};for(f in b)g[f]=a.style[f],a.style[f]=b[f];e=c.apply(a,d||[]);for(f in b)a.style[f]=g[f];return e};var Nb=/alpha\\([^)]*\\)/i,Ob=/opacity\\s*=\\s*([^)]*)/,Pb=/^(none|table(?!-c[ea]).+)/,Qb=new RegExp(\"^(\"+T+\")(.*)$\",\"i\"),Rb=new RegExp(\"^([+-])=(\"+T+\")\",\"i\"),Sb={position:\"absolute\",visibility:\"hidden\",display:\"block\"},Tb={letterSpacing:0,fontWeight:400},Ub=[\"Webkit\",\"O\",\"Moz\",\"ms\"];function Vb(a,b){if(b in a)return b;var c=b.charAt(0).toUpperCase()+b.slice(1),d=b,e=Ub.length;while(e--)if(b=Ub[e]+c,b in a)return b;return d}function Wb(a,b){for(var c,d,e,f=[],g=0,h=a.length;h>g;g++)d=a[g],d.style&&(f[g]=n._data(d,\"olddisplay\"),c=d.style.display,b?(f[g]||\"none\"!==c||(d.style.display=\"\"),\"\"===d.style.display&&V(d)&&(f[g]=n._data(d,\"olddisplay\",Gb(d.nodeName)))):f[g]||(e=V(d),(c&&\"none\"!==c||!e)&&n._data(d,\"olddisplay\",e?c:n.css(d,\"display\"))));for(g=0;h>g;g++)d=a[g],d.style&&(b&&\"none\"!==d.style.display&&\"\"!==d.style.display||(d.style.display=b?f[g]||\"\":\"none\"));return a}function Xb(a,b,c){var d=Qb.exec(b);return d?Math.max(0,d[1]-(c||0))+(d[2]||\"px\"):b}function Yb(a,b,c,d,e){for(var f=c===(d?\"border\":\"content\")?4:\"width\"===b?1:0,g=0;4>f;f+=2)\"margin\"===c&&(g+=n.css(a,c+U[f],!0,e)),d?(\"content\"===c&&(g-=n.css(a,\"padding\"+U[f],!0,e)),\"margin\"!==c&&(g-=n.css(a,\"border\"+U[f]+\"Width\",!0,e))):(g+=n.css(a,\"padding\"+U[f],!0,e),\"padding\"!==c&&(g+=n.css(a,\"border\"+U[f]+\"Width\",!0,e)));return g}function Zb(a,b,c){var d=!0,e=\"width\"===b?a.offsetWidth:a.offsetHeight,f=Jb(a),g=l.boxSizing()&&\"border-box\"===n.css(a,\"boxSizing\",!1,f);if(0>=e||null==e){if(e=Kb(a,b,f),(0>e||null==e)&&(e=a.style[b]),Ib.test(e))return e;d=g&&(l.boxSizingReliable()||e===a.style[b]),e=parseFloat(e)||0}return e+Yb(a,b,c||(g?\"border\":\"content\"),d,f)+\"px\"}n.extend({cssHooks:{opacity:{get:function(a,b){if(b){var c=Kb(a,\"opacity\");return\"\"===c?\"1\":c}}}},cssNumber:{columnCount:!0,fillOpacity:!0,fontWeight:!0,lineHeight:!0,opacity:!0,order:!0,orphans:!0,widows:!0,zIndex:!0,zoom:!0},cssProps:{\"float\":l.cssFloat?\"cssFloat\":\"styleFloat\"},style:function(a,b,c,d){if(a&&3!==a.nodeType&&8!==a.nodeType&&a.style){var e,f,g,h=n.camelCase(b),i=a.style;if(b=n.cssProps[h]||(n.cssProps[h]=Vb(i,h)),g=n.cssHooks[b]||n.cssHooks[h],void 0=="
,
"=c)return g&&\"get\"in g&&void 0!==(e=g.get(a,!1,d))?e:i[b];if(f=typeof c,\"string\"===f&&(e=Rb.exec(c))&&(c=(e[1]+1)*e[2]+parseFloat(n.css(a,b)),f=\"number\"),null!=c&&c===c&&(\"number\"!==f||n.cssNumber[h]||(c+=\"px\"),l.clearCloneStyle||\"\"!==c||0!==b.indexOf(\"background\")||(i[b]=\"inherit\"),!(g&&\"set\"in g&&void 0===(c=g.set(a,c,d)))))try{i[b]=\"\",i[b]=c}catch(j){}}},css:function(a,b,c,d){var e,f,g,h=n.camelCase(b);return b=n.cssProps[h]||(n.cssProps[h]=Vb(a.style,h)),g=n.cssHooks[b]||n.cssHooks[h],g&&\"get\"in g&&(f=g.get(a,!0,c)),void 0===f&&(f=Kb(a,b,d)),\"normal\"===f&&b in Tb&&(f=Tb[b]),\"\"===c||c?(e=parseFloat(f),c===!0||n.isNumeric(e)?e||0:f):f}}),n.each([\"height\",\"width\"],function(a,b){n.cssHooks[b]={get:function(a,c,d){return c?0===a.offsetWidth&&Pb.test(n.css(a,\"display\"))?n.swap(a,Sb,function(){return Zb(a,b,d)}):Zb(a,b,d):void 0},set:function(a,c,d){var e=d&&Jb(a);return Xb(a,c,d?Yb(a,b,d,l.boxSizing()&&\"border-box\"===n.css(a,\"boxSizing\",!1,e),e):0)}}}),l.opacity||(n.cssHooks.opacity={get:function(a,b){return Ob.test((b&&a.currentStyle?a.currentStyle.filter:a.style.filter)||\"\")?.01*parseFloat(RegExp.$1)+\"\":b?\"1\":\"\"},set:function(a,b){var c=a.style,d=a.currentStyle,e=n.isNumeric(b)?\"alpha(opacity=\"+100*b+\")\":\"\",f=d&&d.filter||c.filter||\"\";c.zoom=1,(b>=1||\"\"===b)&&\"\"===n.trim(f.replace(Nb,\"\"))&&c.removeAttribute&&(c.removeAttribute(\"filter\"),\"\"===b||d&&!d.filter)||(c.filter=Nb.test(f)?f.replace(Nb,e):f+\" \"+e)}}),n.cssHooks.marginRight=Mb(l.reliableMarginRight,function(a,b){return b?n.swap(a,{display:\"inline-block\"},Kb,[a,\"marginRight\"]):void 0}),n.each({margin:\"\",padding:\"\",border:\"Width\"},function(a,b){n.cssHooks[a+b]={expand:function(c){for(var d=0,e={},f=\"string\"==typeof c?c.split(\" \"):[c];4>d;d++)e[a+U[d]+b]=f[d]||f[d-2]||f[0];return e}},Hb.test(a)||(n.cssHooks[a+b].set=Xb)}),n.fn.extend({css:function(a,b){return W(this,function(a,b,c){var d,e,f={},g=0;if(n.isArray(b)){for(d=Jb(a),e=b.length;e>g;g++)f[b[g]]=n.css(a,b[g],!1,d);return f}return void 0!==c?n.style(a,b,c):n.css(a,b)\n"
,
"},a,b,arguments.length>1)},show:function(){return Wb(this,!0)},hide:function(){return Wb(this)},toggle:function(a){return\"boolean\"==typeof a?a?this.show():this.hide():this.each(function(){V(this)?n(this).show():n(this).hide()})}});function $b(a,b,c,d,e){return new $b.prototype.init(a,b,c,d,e)}n.Tween=$b,$b.prototype={constructor:$b,init:function(a,b,c,d,e,f){this.elem=a,this.prop=c,this.easing=e||\"swing\",this.options=b,this.start=this.now=this.cur(),this.end=d,this.unit=f||(n.cssNumber[c]?\"\":\"px\")},cur:function(){var a=$b.propHooks[this.prop];return a&&a.get?a.get(this):$b.propHooks._default.get(this)},run:function(a){var b,c=$b.propHooks[this.prop];return this.pos=b=this.options.duration?n.easing[this.easing](a,this.options.duration*a,0,1,this.options.duration):a,this.now=(this.end-this.start)*b+this.start,this.options.step&&this.options.step.call(this.elem,this.now,this),c&&c.set?c.set(this):$b.propHooks._default.set(this),this}},$b.prototype.init.prototype=$b.prototype,$b.propHooks={_default:{get:function(a){var b;return null==a.elem[a.prop]||a.elem.style&&null!=a.elem.style[a.prop]?(b=n.css(a.elem,a.prop,\"\"),b&&\"auto\"!==b?b:0):a.elem[a.prop]},set:function(a){n.fx.step[a.prop]?n.fx.step[a.prop](a):a.elem.style&&(null!=a.elem.style[n.cssProps[a.prop]]||n.cssHooks[a.prop])?n.style(a.elem,a.prop,a.now+a.unit):a.elem[a.prop]=a.now}}},$b.propHooks.scrollTop=$b.propHooks.scrollLeft={set:function(a){a.elem.nodeType&&a.elem.parentNode&&(a.elem[a.prop]=a.now)}},n.easing={linear:function(a){return a},swing:function(a){return.5-Math.cos(a*Math.PI)/2}},n.fx=$b.prototype.init,n.fx.step={};var _b,ac,bc=/^(?:toggle|show|hide)$/,cc=new RegExp(\"^(?:([+-])=|)(\"+T+\")([a-z%]*)$\",\"i\"),dc=/queueHooks$/,ec=[jc],fc={\"*\":[function(a,b){var c=this.createTween(a,b),d=c.cur(),e=cc.exec(b),f=e&&e[3]||(n.cssNumber[a]?\"\":\"px\"),g=(n.cssNumber[a]||\"px\"!==f&&+d)&&cc.exec(n.css(c.elem,a)),h=1,i=20;if(g&&g[3]!==f){f=f||g[3],e=e||[],g=+d||1;do h=h||\".5\",g/=h,n.style(c.elem,a,g+f);while(h!==(h=c.cur()/d)&&1!==h&&--i)}return e&&(g=c.start=+g||+d||0,c.unit=f,c.end=e[1]?g+(e[1]+1)*e[2]:+e[2]),c}]};function gc(){return setTimeout(function(){_b=void 0}),_b=n.now()}function hc(a,b){var c,d={height:a},e=0;for(b=b?1:0;4>e;e+=2-b)c=U[e],d[\"margin\"+c]=d[\"padding\"+c]=a;return b&&(d.opacity=d.width=a),d}function ic(a,b,c){for(var d,e=(fc[b]||[]).concat(fc[\"*\"]),f=0,g=e.length;g>f;f++)if(d=e[f].call(c,b,a))return d}function jc(a,b,c){var d,e,f,g,h,i,j,k,m=this,o={},p=a.style,q=a.nodeType&&V(a),r=n._data(a,\"fxshow\");c.queue||(h=n._queueHooks(a,\"fx\"),null==h.unqueued&&(h.unqueued=0,i=h.empty.fire,h.empty.fire=function(){h.unqueued||i()}),h.unqueued++,m.always(function(){m.always(function(){h.unqueued--,n.queue(a,\"fx\").length||h.empty.fire()})})),1===a.nodeType&&(\"height\"in b||\"width\"in b)&&(c.overflow=[p.overflow,p.overflowX,p.overflowY],j=n.css(a,\"display\"),k=Gb(a.nodeName),\"none\"===j&&(j=k),\"inline\"===j&&\"none\"===n.css(a,\"float\")&&(l.inlineBlockNeedsLayout&&\"inline\"!==k?p.zoom=1:p.display=\"inline-block\")),c.overflow&&(p.overflow=\"hidden\",l.shrinkWrapBlocks()||m.always(function(){p.overflow=c.overflow[0],p.overflowX=c.overflow[1],p.overflowY=c.overflow[2]}));for(d in b)if(e=b[d],bc.exec(e)){if(delete b[d],f=f||\"toggle\"===e,e===(q?\"hide\":\"show\")){if(\"show\"!==e||!r||void 0===r[d])continue;q=!0}o[d]=r&&r[d]||n.style(a,d)}if(!n.isEmptyObject(o)){r?\"hidden\"in r&&(q=r.hidden):r=n._data(a,\"fxshow\",{}),f&&(r.hidden=!q),q?n(a).show():m.done(function(){n(a).hide()}),m.done(function(){var b;n._removeData(a,\"fxshow\");for(b in o)n.style(a,b,o[b])});for(d in o)g=ic(q?r[d]:0,d,m),d in r||(r[d]=g.start,q&&(g.end=g.start,g.start=\"width\"===d||\"height\"===d?1:0))}}function kc(a,b){var c,d,e,f,g;for(c in a)if(d=n.camelCase(c),e=b[d],f=a[c],n.isArray(f)&&(e=f[1],f=a[c]=f[0]),c!==d&&(a[d]=f,delete a[c]),g=n.cssHooks[d],g&&\"expand\"in g){f=g.expand(f),delete a[d];for(c in f)c in a||(a[c]=f[c],b[c]=e)}else b[d]=e}function lc(a,b,c){var d,e,f=0,g=ec.length,h=n.Deferred().always(function(){delete i.elem}),i=function(){if(e)return!1;for(var b=_b||gc(),c=Math.max(0,j.startTime+j.duration-b),d=c/j.duration||0,f=1-d,g=0,i=j.tweens.length;i>g;g++)j.tweens[g].run(f);return h.notifyWith(a,[j,f,c]),1>f&&i?c:(h.resolveWith(a,[j]),!1)},j=h.promise({elem:a,props:n.extend({},b),opts:n.extend(!0,{specialEasing:{}},c),originalProperties:b,originalOptions:c,startTime:_b||gc(),duration:c.duration,tweens:[],createTween:function(b,c){var d=n.Tween(a,j.opts,b,c,j.opts.specialEasing[b]||j.opts.easing);return j.tweens.push(d),d},stop:function(b){var c=0,d=b?j.tweens.length:0;if(e)return this;for(e=!0;d>c;c++)j.tweens[c].run(1);return b?h.resolveWith(a,[j,b]):h.rejectWith(a,[j,b]),this}}),k=j.props;for(kc(k,j.opts.specialEasing);g>f;f++)if(d=ec[f].call(j,a,k,j.opts))return d;return n.map(k,ic,j),n.isFunction(j.opts.start)&&j.opts.start.call(a,j),n.fx.timer(n.extend(i,{elem:a,anim:j,queue:j.opts.queue})),j.progress(j.opts.progress).done(j.opts.done,j.opts.complete).fail(j.opts.fail).always(j.opts.always)}n.Animation=n.extend(lc,{tweener:function(a,b){n.isFunction(a)?(b=a,a=[\"*\"]):a=a.split(\" \");for(var c,d=0,e=a.length;e>d;d++)c=a[d],fc[c]=fc[c]||[],fc[c].unshift(b)},prefilter:function(a,b){b?ec.unshift(a):ec.push(a)}}),n.speed=function(a,b,c){var d=a&&\"object\"==typeof a?n.extend({},a):{complete:c||!c&&b||n.isFunction(a)&&a,duration:a,easing:c&&b||b&&!n.isFunction(b)&&b};return d.duration=n.fx.off?0:\"number\"==typeof d.duration?d.duration:d.duration in n.fx.speeds?n.fx.speeds[d.duration]:n.fx.speeds._default,(null==d.queue||d.queue===!0)&&(d.queue=\"fx\"),d.old=d.complete,d.complete=function(){n.isFunction(d.old)&&d.old.call(this),d.queue&&n.dequeue(this,d.queue)},d},n.fn.extend({fadeTo:function(a,b,c,d){return this.filter(V).css(\"opacity\",0).show().end().animate({opacity:b},a,c,d)},animate:function(a,b,c,d){var e=n.isEmptyObject(a),f=n.speed(b,c,d),g=function(){var b=lc(this,n.extend({},a),f);(e||n._data(this,\"finish\"))&&b.stop(!0)};return g.finish=g,e||f.queue===!1?this.each(g):this.queue(f.queue,g)},stop:function(a,b,c){var d=function(a){var b=a.stop;delete a.stop,b(c)};return\"string\"!=typeof a&&(c=b,b=a,a=void 0),b&&a!==!1&&this.queue(a||\"fx\",[]),this.each(function(){var b=!0,e=null!=a&&a+\"queueHooks\",f=n.timers,g=n._data(this);if(e)g[e]&&g[e].stop&&d(g[e]);else for(e in g)g[e]&&g[e].stop&&dc.test(e)&&d(g[e]);for(e=f.length;e--;)f[e].elem!==this||null!=a&&f[e].queue!==a||(f[e].anim.stop(c),b=!1,f.splice(e,1));(b||!c)&&n.dequeue(this,a)})},finish:function(a){return a!==!1&&(a=a||\"fx\"),this.each(function(){var b,c=n._data(this),d=c[a+\"queue\"],e=c[a+\"queueHooks\"],f=n.timers,g=d?d.length:0;for(c.finish=!0,n.queue(this,a,[]),e&&e.stop&&e.stop.call(this,!0),b=f.length;b--;)f[b].elem===this&&f[b].queue===a&&(f[b].anim.stop(!0),f.splice(b,1));for(b=0;g>b;b++)d[b]&&d[b].finish&&d[b].finish.call(this);delete c.finish})}}),n.each([\"toggle\",\"show\",\"hide\"],function(a,b){var c=n.fn[b];n.fn[b]=function(a,d,e){return null==a||\"boolean\"==typeof a?c.apply(this,arguments):this.animate(hc(b,!0),a,d,e)}}),n.each({slideDown:hc(\"show\"),slideUp:hc(\"hide\"),slideToggle:hc(\"toggle\"),fadeIn:{opacity:\"show\"},fadeOut:{opacity:\"hide\"},fadeToggle:{opacity:\"toggle\"}},function(a,b){n.fn[a]=function(a,c,d){return this.animate(b,a,c,d)}}),n.timers=[],n.fx.tick=function(){var a,b=n.timers,c=0;for(_b=n.now();c<b.length;c++)a=b[c],a()||b[c]!==a||b.splice(c--,1);b.length||n.fx.stop(),_b=void 0},n.fx.timer=function(a){n.timers.push(a),a()?n.fx.start():n.timers.pop()},n.fx.interval=13,n.fx.start=function(){ac||(ac=setInterval(n.fx.tick,n.fx.interval))},n.fx.stop=function(){clearInterval(ac),ac=null},n.fx.speeds={slow:600,fast:200,_default:400},n.fn.delay=function(a,b){return a=n.fx?n.fx.speeds[a]||a:a,b=b||\"fx\",this.queue(b,function(b,c){var d=setTimeout(b,a);c.stop=function(){clearTimeout(d)}})},function(){var a,b,c,d,e=z.createElement(\"div\");e.setAttribute(\"className\",\"t\"),e.innerHTML=\"  <link/><table></table><a href='/a'>a</a><input type='checkbox'/>\",a=e.getElementsByTagName(\"a\")[0],c=z.createElement(\"select\"),d=c.appendChild(z.createElement(\"option\")),b=e.getElementsByTagName(\"input\")[0],a.style.cssText=\"top:1px\",l.getSetAttribute=\"t\"!==e.className,l.style=/top/.test(a.getAttribute(\"style\")),l.hrefNormalized=\"/a\"===a.getAttribute(\"href\"),l.checkOn=!!b.value,l.optSelected=d.selected,l.enctype=!!z.createElement(\"form\").enctype,c.disabled=!0,l.optDisabled=!d.disabled,b=z.createElement(\"input\"),b.setAttribute(\"value\",\"\"),l.input=\"\"===b.getAttribute(\"value\"),b.value=\"t\",b.setAttribute(\"type\",\"radio\"),l.radioValue=\"t\"===b.value,a=b=c=d=e=null}();var mc=/\\r/g;n.fn.extend({val:function(a){var b,c,d,e=this[0];{if(arguments.length)return d=n.isFunction(a),this.each(function(c){var e;1===this.nodeType&&(e=d?a.call(this,c,n(this).val()):a,null==e?e=\"\":\"number\"==typeof e?e+=\"\":n.isArray(e)&&(e=n.map(e,function(a){return null==a?\"\":a+\"\"})),b=n.valHooks[this.type]||n.valHooks[this.nodeName.toLowerCase()],b&&\"set\"in b&&void 0!==b.set(this,e,\"value\")||(this.value=e))});if(e)return b=n.valHooks[e.type]||n.valHooks[e.nodeName.toLowerCase()],b&&\"get\"in b&&void 0!==(c=b.get(e,\"value\"))?c:(c=e.value,\"string\"==typeof c?c.replace(mc,\"\"):null==c?\"\":c)}}}),n.extend({valHooks:{option:{get:function(a){var b=n.find.attr(a,\"value\");return null!=b?b:n.text(a)}},select:{get:function(a){for(var b,c,d=a.options,e=a.selectedIndex,f=\"select-one\"===a.type||0>e,g=f?null:[],h=f?e+1:d.length,i=0>e?h:f?e:0;h>i;i++)if(c=d[i],!(!c.selected&&i!==e||(l.optDisabled?c.disabled:null!==c.getAttribute(\"disabled\"))||c.parentNode.disabled&&n.nodeName(c.parentNode,\"optgroup\"))){if(b=n(c).val(),f)return b;g.push(b)}return g},set:function(a,b){var c,d,e=a.options,f=n.makeArray(b),g=e.length;while(g--)if(d=e[g],n.inArray(n.valHooks.option.get(d),f)>=0)try{d.selected=c=!0}catch(h){d.scrollHeight}else d.selected=!1;return c||(a.selectedIndex=-1),e}}}}),n.each([\"radio\",\"checkbox\"],function(){n.valHooks[this]={set:function(a,b){return n.isArray(b)?a.checked=n.inArray(n(a).val(),b)>=0:void 0}},l.checkO"
,
"n||(n.valHooks[this].get=function(a){return null===a.getAttribute(\"value\")?\"on\":a.value})});var nc,oc,pc=n.expr.attrHandle,qc=/^(?:checked|selected)$/i,rc=l.getSetAttribute,sc=l.input;n.fn.extend({attr:function(a,b){return W(this,n.attr,a,b,arguments.length>1)},removeAttr:function(a){return this.each(function(){n.removeAttr(this,a)})}}),n.extend({attr:function(a,b,c){var d,e,f=a.nodeType;if(a&&3!==f&&8!==f&&2!==f)return typeof a.getAttribute===L?n.prop(a,b,c):(1===f&&n.isXMLDoc(a)||(b=b.toLowerCase(),d=n.attrHooks[b]||(n.expr.match.bool.test(b)?oc:nc)),void 0===c?d&&\"get\"in d&&null!==(e=d.get(a,b))?e:(e=n.find.attr(a,b),null==e?void 0:e):null!==c?d&&\"set\"in d&&void 0!==(e=d.set(a,c,b))?e:(a.setAttribute(b,c+\"\"),c):void n.removeAttr(a,b))},removeAttr:function(a,b){var c,d,e=0,f=b&&b.match(F);if(f&&1===a.nodeType)while(c=f[e++])d=n.propFix[c]||c,n.expr.match.bool.test(c)?sc&&rc||!qc.test(c)?a[d]=!1:a[n.camelCase(\"default-\"+c)]=a[d]=!1:n.attr(a,c,\"\"),a.removeAttribute(rc?c:d)},attrHooks:{type:{set:function(a,b){if(!l.radioValue&&\"radio\"===b&&n.nodeName(a,\"input\")){var c=a.value;return a.setAttribute(\"type\",b),c&&(a.value=c),b}}}}}),oc={set:function(a,b,c){return b===!1?n.removeAttr(a,c):sc&&rc||!qc.test(c)?a.setAttribute(!rc&&n.propFix[c]||c,c):a[n.camelCase(\"default-\"+c)]=a[c]=!0,c}},n.each(n.expr.match.bool.source.match(/\\w+/g),function(a,b){var c=pc[b]||n.find.attr;pc[b]=sc&&rc||!qc.test(b)?function(a,b,d){var e,f;return d||(f=pc[b],pc[b]=e,e=null!=c(a,b,d)?b.toLowerCase():null,pc[b]=f),e}:function(a,b,c){return c?void 0:a[n.camelCase(\"default-\"+b)]?b.toLowerCase():null}}),sc&&rc||(n.attrHooks.value={set:function(a,b,c){return n.nodeName(a,\"input\")?void(a.defaultValue=b):nc&&nc.set(a,b,c)}}),rc||(nc={set:function(a,b,c){var d=a.getAttributeNode(c);return d||a.setAttributeNode(d=a.ownerDocument.createAttribute(c)),d.value=b+=\"\",\"value\"===c||b===a.getAttribute(c)?b:void 0}},pc.id=pc.name=pc.coords=function(a,b,c){var d;return c?void 0:(d=a.getAttributeNode(b))&&\"\"!==d.value?d.value:null},n.valHooks.button={get:function(a,b){var c=a.getAttributeNode(b);return c&&c.specified?c.value:void 0},set:nc.set},n.attrHooks.contenteditable={set:function(a,b,c){nc.set(a,\"\"===b?!1:b,c)}},n.each([\"width\",\"height\"],function(a,b){n.attrHooks[b]={set:function(a,c){return\"\"===c?(a.setAttribute(b,\"auto\"),c):void 0}}})),l.style||(n.attrHooks.style={get:function(a){return a.style.cssText||void 0},set:function(a,b){return a.style.cssText=b+\"\"}});var tc=/^(?:input|select|textarea|button|object)$/i,uc=/^(?:a|area)$/i;n.fn.extend({prop:function(a,b){return W(this,n.prop,a,b,arguments.length>1)},removeProp:function(a){return a=n.propFix[a]||a,this.each(function(){try{this[a]=void 0,delete this[a]}catch(b){}})}}),n.extend({propFix:{\"for\":\"htmlFor\",\"class\":\"className\"},prop:function(a,b,c){var d,e,f,g=a.nodeType;if(a&&3!==g&&8!==g&&2!==g)return f=1!==g||!n.isXMLDoc(a),f&&(b=n.propFix[b]||b,e=n.propHooks[b]),void 0!==c?e&&\"set\"in e&&void 0!==(d=e.set(a,c,b))?d:a[b]=c:e&&\"get\"in e&&null!==(d=e.get(a,b))?d:a[b]},propHooks:{tabIndex:{get:function(a){var b=n.find.attr(a,\"tabindex\");return b?parseInt(b,10):tc.test(a.nodeName)||uc.test(a.nodeName)&&a.href?0:-1}}}}),l.hrefNormalized||n.each([\"href\",\"src\"],function(a,b){n.propHooks[b]={get:function(a){return a.getAttribute(b,4)}}}),l.optSelected||(n.propHooks.selected={get:function(a){var b=a.parentNode;return b&&(b.selectedIndex,b.parentNode&&b.parentNode.selectedIndex),null}}),n.each([\"tabIndex\",\"readOnly\",\"maxLength\",\"cellSpacing\",\"cellPadding\",\"rowSpan\",\"colSpan\",\"useMap\",\"frameBorder\",\"contentEditable\"],function(){n.propFix[this.toLowerCase()]=this}),l.enctype||(n.propFix.enctype=\"encoding\");var vc=/[\\t\\r\\n\\f]/g;n.fn.extend({addClass:function(a){var b,c,d,e,f,g,h=0,i=this.length,j=\"string\"==typeof a&&a;if(n.isFunction(a))return this.each(function(b){n(this).addClass(a.call(this,b,this.className))});if(j)for(b=(a||\"\").match(F)||[];i>h;h++)if(c=this[h],d=1===c.nodeType&&(c.className?(\" \"+c.className+\" \").replace(vc,\" \"):\" \")){f=0;while(e=b[f++])d.indexOf(\" \"+e+\" \")<0&&(d+=e+\" \");g=n.trim(d),c.className!==g&&(c.className=g)}return this},removeClass:function(a){var b,c,d,e,f,g,h=0,i=this.length,j=0===arguments.length||\"string\"==typeof a&&a;if(n.isFunction(a))return this.each(function(b){n(this).removeClass(a.call(this,b,this.className))});if(j)for(b=(a||\"\").match(F)||[];i>h;h++)if(c=this[h],d=1===c.nodeType&&(c.className?(\" \"+c.className+\" \").replace(vc,\" \"):\"\")){f=0;while(e=b[f++])while(d.indexOf(\" \"+e+\" \")>=0)d=d.replace(\" \"+e+\" \",\" \");g=a?n.trim(d):\"\",c.className!==g&&(c.className=g)}return this},toggleClass:function(a,b){var c=typeof a;return\"boolean\"==typeof b&&\"string\"===c?b?this.addClass(a):this.removeClass(a):this.each(n.isFunction(a)?function(c){n(this).toggleClass(a.call(this,c,this.className,b),b)}:function(){if(\"string\"===c){var b,d=0,e=n(this),f=a.match(F)||[];while(b=f[d++])e.hasClass(b)?e.removeClass(b):e.addClass(b)}else(c===L||\"boolean\"===c)&&(this.className&&n._data(this,\"__className__\",this.className),this.className=this.className||a===!1?\"\":n._data(this,\"__className__\")||\"\")})},hasClass:function(a){for(var b=\" \"+a+\" \",c=0,d=this.length;d>c;c++)if(1===this[c].nodeType&&(\" \"+this[c].className+\" \").replace(vc,\" \").indexOf(b)>=0)return!0;return!1}}),n.each(\"blur focus focusin focusout load resize scroll unload click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave change select submit keydown keypress keyup error contextmenu\".split(\" \"),function(a,b){n.fn[b]=function(a,c){return arguments.length>0?this.on(b,null,a,c):this.trigger(b)}}),n.fn.extend({hover:function(a,b){return this.mouseenter(a).mouseleave(b||a)},bind:function(a,b,c){return this.on(a,null,b,c)},unbind:function(a,b){return this.off(a,null,b)},delegate:function(a,b,c,d){return this.on(b,a,c,d)},undelegate:function(a,b,c){return 1===arguments.length?this.off(a,\"**\"):this.off(b,a||\"**\",c)}});var wc=n.now(),xc=/\\?/,yc=/(,)|(\\[|{)|(}|])|\"(?:[^\"\\\\\\r\\n]|\\\\[\"\\\\\\/bfnrt]|\\\\u[\\da-fA-F]{4})*\"\\s*:?|true|false|null|-?(?!0\\d)\\d+(?:\\.\\d+|)(?:[eE][+-]?\\d+|)/g;n.parseJSON=function(b){if(a.JSON&&a.JSON.parse)return a.JSON.parse(b+\"\");var c,d=null,e=n.trim(b+\"\");return e&&!n.trim(e.replace(yc,function(a,b,e,f){return c&&b&&(d=0),0===d?a:(c=e||b,d+=!f-!e,\"\")}))?Function(\"return \"+e)():n.error(\"Invalid JSON: \"+b)},n.parseXML=function(b){var c,d;if(!b||\"string\"!=typeof b)return null;try{a.DOMParser?(d=new DOMParser,c=d.parseFromString(b,\"text/xml\")):(c=new ActiveXObject(\"Microsoft.XMLDOM\"),c.async=\"false\",c.loadXML(b))}catch(e){c=void 0}return c&&c.documentElement&&!c.getElementsByTagName(\"parsererror\").length||n.error(\"Invalid XML: \"+b),c};var zc,Ac,Bc=/#.*$/,Cc=/([?&])_=[^&]*/,Dc=/^(.*?):[ \\t]*([^\\r\\n]*)\\r?$/gm,Ec=/^(?:about|app|app-storage|.+-extension|file|res|widget):$/,Fc=/^(?:GET|HEAD)$/,Gc=/^\\/\\//,Hc=/^([\\w.+-]+:)(?:\\/\\/(?:[^\\/?#]*@|)([^\\/?#:]*)(?::(\\d+)|)|)/,Ic={},Jc={},Kc=\"*/\".concat(\"*\");try{Ac=location.href}catch(Lc){Ac=z.createElement(\"a\"),Ac.href=\"\",Ac=Ac.href}zc=Hc.exec(Ac.toLowerCase())||[];function Mc(a){return function(b,c){\"string\"!=typeof b&&(c=b,b=\"*\");var d,e=0,f=b.toLowerCase().match(F)||[];if(n.isFunction(c))while(d=f[e++])\"+\"===d.charAt(0)?(d=d.slice(1)||\"*\",(a[d]=a[d]||[]).unshift(c)):(a[d]=a[d]||[]).push(c)}}function Nc(a,b,c,d){var e={},f=a===Jc;function g(h){var i;return e[h]=!0,n.each(a[h]||[],function(a,h){var j=h(b,c,d);return\"string\"!=typeof j||f||e[j]?f?!(i=j):void 0:(b.dataTypes.unshift(j),g(j),!1)}),i}return g(b.dataTypes[0])||!e[\"*\"]&&g(\"*\")}function Oc(a,b){var c,d,e=n.ajaxSettings.flatOptions||{};for(d in b)void 0!==b[d]&&((e[d]?a:c||(c={}))[d]=b[d]);return c&&n.extend(!0,a,c),a}function Pc(a,b,c){var d,e,f,g,h=a.contents,i=a.dataTypes;while(\"*\"===i[0])i.shift(),void 0===e&&(e=a.mimeType||b.getResponseHeader(\"Content-Type\"));if(e)for(g in h)if(h[g]&&h[g].test(e)){i.unshift(g);break}if(i[0]in c)f=i[0];else{for(g in c){if(!i[0]||a.converters[g+\" \"+i[0]]){f=g;break}d||(d=g)}f=f||d}return f?(f!==i[0]&&i.unshift(f),c[f]):void 0}function Qc(a,b,c,d){var e,f,g,h,i,j={},k=a.dataTypes.slice();if(k[1])for(g in a.converters)j[g.toLowerCase()]=a.converters[g];f=k.shift();while(f)if(a.responseFields[f]&&(c[a.responseFields[f]]=b),!i&&d&&a.dataFilter&&(b=a.dataFilter(b,a.dataType)),i=f,f=k.shift())if(\"*\"===f)f=i;else if(\"*\"!==i&&i!==f){if(g=j[i+\" \"+f]||j[\"* \"+f],!g)for(e in j)if(h=e.split(\" \"),h[1]===f&&(g=j[i+\" \"+h[0]]||j[\"* \"+h[0]])){g===!0?g=j[e]:j[e]!==!0&&(f=h[0],k.unshift(h[1]));break}if(g!==!0)if(g&&a[\"throws\"])b=g(b);else try{b=g(b)}catch(l){return{state:\"parsererror\",error:g?l:\"No conversion from \"+i+\" to \"+f}}}return{state:\"success\",data:b}}n.extend({active:0,lastModified:{},etag:{},ajaxSettings:{url:Ac,type:\"GET\",isLocal:Ec.test(zc[1]),global:!0,processData:!0,async:!0,contentType:\"application/x-www-form-urlencoded; charset=UTF-8\",accepts:{\"*\":Kc,text:\"text/plain\",html:\"text/html\",xml:\"application/xml, text/xml\",json:\"application/json, text/javascript\"},contents:{xml:/xml/,html:/html/,json:/json/},responseFields:{xml:\"responseXML\",text:\"responseText\",json:\"responseJSON\"},converters:{\"* text\":String,\"text html\":!0,\"text json\":n.parseJSON,\"text xml\":n.parseXML},flatOptions:{url:!0,context:!0}},ajaxSetup:function(a,b){return b?Oc(Oc(a,n.ajaxSettings),b):Oc(n.ajaxSettings,a)},ajaxPrefilter:Mc(Ic),ajaxTransport:Mc(Jc),ajax:function(a,b){\"object\"==typeof a&&(b=a,a=void 0),b=b||{};var c,d,e,f,g,h,i,j,k=n.ajaxSetup({},b),l=k.context||k,m=k.context&&(l.nodeType||l.jquery)?n(l):n.event,o=n.Deferred(),p=n.Callbacks(\"once memory\"),q=k.statusCode||{},r={},s={},t=0,u=\"canceled\",v={readyState:0,getResponseHeader:function(a){var b;if(2===t){if(!j){j={};while(b=Dc.exec(f))j[b[1].toLowerCase()]=b[2]}b=j[a.toLowerCase()]}return null==b?null:b},getAllResponseHeaders:function(){return 2===t?f:null},setRequestHeader:function(a,b){var c=a.toLowerCase();return t||(a=s[c]=s[c]||a,r[a]=b),this},overrideMimeType:function(a){return t||(k.mimeType=a),this},statusCode:function(a){var b;if(a)if(2>t)for(b in"
,
" a)q[b]=[q[b],a[b]];else v.always(a[v.status]);return this},abort:function(a){var b=a||u;return i&&i.abort(b),x(0,b),this}};if(o.promise(v).complete=p.add,v.success=v.done,v.error=v.fail,k.url=((a||k.url||Ac)+\"\").replace(Bc,\"\").replace(Gc,zc[1]+\"//\"),k.type=b.method||b.type||k.method||k.type,k.dataTypes=n.trim(k.dataType||\"*\").toLowerCase().match(F)||[\"\"],null==k.crossDomain&&(c=Hc.exec(k.url.toLowerCase()),k.crossDomain=!(!c||c[1]===zc[1]&&c[2]===zc[2]&&(c[3]||(\"http:\"===c[1]?\"80\":\"443\"))===(zc[3]||(\"http:\"===zc[1]?\"80\":\"443\")))),k.data&&k.processData&&\"string\"!=typeof k.data&&(k.data=n.param(k.data,k.traditional)),Nc(Ic,k,b,v),2===t)return v;h=k.global,h&&0===n.active++&&n.event.trigger(\"ajaxStart\"),k.type=k.type.toUpperCase(),k.hasContent=!Fc.test(k.type),e=k.url,k.hasContent||(k.data&&(e=k.url+=(xc.test(e)?\"&\":\"?\")+k.data,delete k.data),k.cache===!1&&(k.url=Cc.test(e)?e.replace(Cc,\"$1_=\"+wc++):e+(xc.test(e)?\"&\":\"?\")+\"_=\"+wc++)),k.ifModified&&(n.lastModified[e]&&v.setRequestHeader(\"If-Modified-Since\",n.lastModified[e]),n.etag[e]&&v.setRequestHeader(\"If-None-Match\",n.etag[e])),(k.data&&k.hasContent&&k.contentType!==!1||b.contentType)&&v.setRequestHeader(\"Content-Type\",k.contentType),v.setRequestHeader(\"Accept\",k.dataTypes[0]&&k.accepts[k.dataTypes[0]]?k.accepts[k.dataTypes[0]]+(\"*\"!==k.dataTypes[0]?\", \"+Kc+\"; q=0.01\":\"\"):k.accepts[\"*\"]);for(d in k.headers)v.setRequestHeader(d,k.headers[d]);if(k.beforeSend&&(k.beforeSend.call(l,v,k)===!1||2===t))return v.abort();u=\"abort\";for(d in{success:1,error:1,complete:1})v[d](k[d]);if(i=Nc(Jc,k,b,v)){v.readyState=1,h&&m.trigger(\"ajaxSend\",[v,k]),k.async&&k.timeout>0&&(g=setTimeout(function(){v.abort(\"timeout\")},k.timeout));try{t=1,i.send(r,x)}catch(w){if(!(2>t))throw w;x(-1,w)}}else x(-1,\"No Transport\");function x(a,b,c,d){var j,r,s,u,w,x=b;2!==t&&(t=2,g&&clearTimeout(g),i=void 0,f=d||\"\",v.readyState=a>0?4:0,j=a>=200&&300>a||304===a,c&&(u=Pc(k,v,c)),u=Qc(k,u,v,j),j?(k.ifModified&&(w=v.getResponseHeader(\"Last-Modified\"),w&&(n.lastModified[e]=w),w=v.getResponseHeader(\"etag\"),w&&(n.etag[e]=w)),204===a||\"HEAD\"===k.type?x=\"nocontent\":304===a?x=\"notmodified\":(x=u.state,r=u.data,s=u.error,j=!s)):(s=x,(a||!x)&&(x=\"error\",0>a&&(a=0))),v.status=a,v.statusText=(b||x)+\"\",j?o.resolveWith(l,[r,x,v]):o.rejectWith(l,[v,x,s]),v.statusCode(q),q=void 0,h&&m.trigger(j?\"ajaxSuccess\":\"ajaxError\",[v,k,j?r:s]),p.fireWith(l,[v,x]),h&&(m.trigger(\"ajaxComplete\",[v,k]),--n.active||n.event.trigger(\"ajaxStop\")))}return v},getJSON:function(a,b,c){return n.get(a,b,c,\"json\")},getScript:function(a,b){return n.get(a,void 0,b,\"script\")}}),n.each([\"get\",\"post\"],function(a,b){n[b]=function(a,c,d,e){return n.isFunction(c)&&(e=e||d,d=c,c=void 0),n.ajax({url:a,type:b,dataType:e,data:c,success:d})}}),n.each([\"ajaxStart\",\"ajaxStop\",\"ajaxComplete\",\"ajaxError\",\"ajaxSuccess\",\"ajaxSend\"],function(a,b){n.fn[b]=function(a){return this.on(b,a)}}),n._evalUrl=function(a){return n.ajax({url:a,type:\"GET\",dataType:\"script\",async:!1,global:!1,\"throws\":!0})},n.fn.extend({wrapAll:function(a){if(n.isFunction(a))return this.each(function(b){n(this).wrapAll(a.call(this,b))});if(this[0]){var b=n(a,this[0].ownerDocument).eq(0).clone(!0);this[0].parentNode&&b.insertBefore(this[0]),b.map(function(){var a=this;while(a.firstChild&&1===a.firstChild.nodeType)a=a.firstChild;return a}).append(this)}return this},wrapInner:function(a){return this.each(n.isFunction(a)?function(b){n(this).wrapInner(a.call(this,b))}:function(){var b=n(this),c=b.contents();c.length?c.wrapAll(a):b.append(a)})},wrap:function(a){var b=n.isFunction(a);return this.each(function(c){n(this).wrapAll(b?a.call(this,c):a)})},unwrap:function(){return this.parent().each(function(){n.nodeName(this,\"body\")||n(this).replaceWith(this.childNodes)}).end()}}),n.expr.filters.hidden=function(a){return a.offsetWidth<=0&&a.offsetHeight<=0||!l.reliableHiddenOffsets()&&\"none\"===(a.style&&a.style.display||n.css(a,\"display\"))},n.expr.filters.visible=function(a){return!n.expr.filters.hidden(a)};var Rc=/%20/g,Sc=/\\[\\]$/,Tc=/\\r?\\n/g,Uc=/^(?:submit|button|image|reset|file)$/i,Vc=/^(?:input|select|textarea|keygen)/i;function Wc(a,b,c,d){var e;if(n.isArray(b))n.each(b,function(b,e){c||Sc.test(a)?d(a,e):Wc(a+\"[\"+(\"object\"==typeof e?b:\"\")+\"]\",e,c,d)});else if(c||\"object\"!==n.type(b))d(a,b);else for(e in b)Wc(a+\"[\"+e+\"]\",b[e],c,d)}n.param=function(a,b){var c,d=[],e=function(a,b){b=n.isFunction(b)?b():null==b?\"\":b,d[d.length]=encodeURIComponent(a)+\"=\"+encodeURIComponent(b)};if(void 0===b&&(b=n.ajaxSettings&&n.ajaxSettings.traditional),n.isArray(a)||a.jquery&&!n.isPlainObject(a))n.each(a,function(){e(this.name,this.value)});else for(c in a)Wc(c,a[c],b,e);return d.join(\"&\").replace(Rc,\"+\")},n.fn.extend({serialize:function(){return n.param(this.serializeArray())},serializeArray:function(){return this.map(function(){var a=n.prop(this,\"elements\");return a?n.makeArray(a):this}).filter(function(){var a=this.type;return this.name&&!n(this).is(\":disabled\")&&Vc.test(this.nodeName)&&!Uc.test(a)&&(this.checked||!X.test(a))}).map(function(a,b){var c=n(this).val();return null==c?null:n.isArray(c)?n.map(c,function(a){return{name:b.name,value:a.replace(Tc,\"\\r\\n\")}}):{name:b.name,value:c.replace(Tc,\"\\r\\n\")}}).get()}}),n.ajaxSettings.xhr=void 0!==a.ActiveXObject?function(){return!this.isLocal&&/^(get|post|head|put|delete|options)$/i.test(this.type)&&$c()||_c()}:$c;var Xc=0,Yc={},Zc=n.ajaxSettings.xhr();a.ActiveXObject&&n(a).on(\"unload\",function(){for(var a in Yc)Yc[a](void 0,!0)}),l.cors=!!Zc&&\"withCredentials\"in Zc,Zc=l.ajax=!!Zc,Zc&&n.ajaxTransport(function(a){if(!a.crossDomain||l.cors){var b;return{send:function(c,d){var e,f=a.xhr(),g=++Xc;if(f.open(a.type,a.url,a.async,a.username,a.password),a.xhrFields)for(e in a.xhrFields)f[e]=a.xhrFields[e];a.mimeType&&f.overrideMimeType&&f.overrideMimeType(a.mimeType),a.crossDomain||c[\"X-Requested-With\"]||(c[\"X-Requested-With\"]=\"XMLHttpRequest\");for(e in c)void 0!==c[e]&&f.setRequestHeader(e,c[e]+\"\");f.send(a.hasContent&&a.data||null),b=function(c,e){var h,i,j;if(b&&(e||4===f.readyState))if(delete Yc[g],b=void 0,f.onreadystatechange=n.noop,e)4!==f.readyState&&f.abort();else{j={},h=f.status,\"string\"==typeof f.responseText&&(j.text=f.responseText);try{i=f.statusText}catch(k){i=\"\"}h||!a.isLocal||a.crossDomain?1223===h&&(h=204):h=j.text?200:404}j&&d(h,i,j,f.getAllResponseHeaders())},a.async?4===f.readyState?setTimeout(b):f.onreadystatechange=Yc[g]=b:b()},abort:function(){b&&b(void 0,!0)}}}});function $c(){try{return new a.XMLHttpRequest}catch(b){}}function _c(){try{return new a.ActiveXObject(\"Microsoft.XMLHTTP\")}catch(b){}}n.ajaxSetup({accepts:{script:\"text/javascript, application/javascript, application/ecmascript, application/x-ecmascript\"},contents:{script:/(?:java|ecma)script/},converters:{\"text script\":function(a){return n.globalEval(a),a}}}),n.ajaxPrefilter(\"script\",function(a){void 0===a.cache&&(a.cache=!1),a.crossDomain&&(a.type=\"GET\",a.global=!1)}),n.ajaxTransport(\"script\",function(a){if(a.crossDomain){var b,c=z.head||n(\"head\")[0]||z.documentElement;return{send:function(d,e){b=z.createElement(\"script\"),b.async=!0,a.scriptCharset&&(b.charset=a.scriptCharset),b.src=a.url,b.onload=b.onreadystatechange=function(a,c){(c||!b.readyState||/loaded|complete/.test(b.readyState))&&(b.onload=b.onreadystatechange=null,b.parentNode&&b.parentNode.removeChild(b),b=null,c||e(200,\"success\"))},c.insertBefore(b,c.firstChild)},abort:function(){b&&b.onload(void 0,!0)}}}});var ad=[],bd=/(=)\\?(?=&|$)|\\?\\?/;n.ajaxSetup({jsonp:\"callback\",jsonpCallback:function(){var a=ad.pop()||n.expando+\"_\"+wc++;return this[a]=!0,a}}),n.ajaxPrefilter(\"json jsonp\",function(b,c,d){var e,f,g,h=b.jsonp!==!1&&(bd.test(b.url)?\"url\":\"string\"==typeof b.data&&!(b.contentType||\"\").indexOf(\"application/x-www-form-urlencoded\")&&bd.test(b.data)&&\"data\");return h||\"jsonp\"===b.dataTypes[0]?(e=b.jsonpCallback=n.isFunction(b.jsonpCallback)?b.jsonpCallback():b.jsonpCallback,h?b[h]=b[h].replace(bd,\"$1\"+e):b.jsonp!==!1&&(b.url+=(xc.test(b.url)?\"&\":\"?\")+b.jsonp+\"=\"+e),b.converters[\"script json\"]=function(){return g||n.error(e+\" was not called\"),g[0]},b.dataTypes[0]=\"json\",f=a[e],a[e]=function(){g=arguments},d.always(function(){a[e]=f,b[e]&&(b.jsonpCallback=c.jsonpCallback,ad.push(e)),g&&n.isFunction(f)&&f(g[0]),g=f=void 0}),\"script\"):void 0}),n.parseHTML=function(a,b,c){if(!a||\"string\"!=typeof a)return null;\"boolean\"==typeof b&&(c=b,b=!1),b=b||z;var d=v.exec(a),e=!c&&[];return d?[b.createElement(d[1])]:(d=n.buildFragment([a],b,e),e&&e.length&&n(e).remove(),n.merge([],d.childNodes))};var cd=n.fn.load;n.fn.load=function(a,b,c){if(\"string\"!=typeof a&&cd)return cd.apply(this,arguments);var d,e,f,g=this,h=a.indexOf(\" \");return h>=0&&(d=a.slice(h,a.length),a=a.slice(0,h)),n.isFunction(b)?(c=b,b=void 0):b&&\"object\"==typeof b&&(f=\"POST\"),g.length>0&&n.ajax({url:a,type:f,dataType:\"html\",data:b}).done(function(a){e=arguments,g.html(d?n(\"<div>\").append(n.parseHTML(a)).find(d):a)}).complete(c&&function(a,b){g.each(c,e||[a.responseText,b,a])}),this},n.expr.filters.animated=function(a){return n.grep(n.timers,function(b){return a===b.elem}).length};var dd=a.document.documentElement;function ed(a){return n.isWindow(a)?a:9===a.nodeType?a.defaultView||a.parentWindow:!1}n.offset={setOffset:function(a,b,c){var d,e,f,g,h,i,j,k=n.css(a,\"position\"),l=n(a),m={};\"static\"===k&&(a.style.position=\"relative\"),h=l.offset(),f=n.css(a,\"top\"),i=n.css(a,\"left\"),j=(\"absolute\"===k||\"fixed\"===k)&&n.inArray(\"auto\",[f,i])>-1,j?(d=l.position(),g=d.top,e=d.left):(g=parseFloat(f)||0,e=parseFloat(i)||0),n.isFunction(b)&&(b=b.call(a,c,h)),null!=b.top&&(m.top=b.top-h.top+g),null!=b.left&&(m.left=b.left-h.left+e),\"using\"in b?b.using.call(a,m):l.css(m)}},n.fn.extend({offset:function(a){if(arguments.length)return void 0===a?this:this.each(function(b){n.offset.setOffset(this,a,b)});var b,c,d={top:0,left:0},e=this[0],f=e&&e.ownerDocument;if(f)return b=f.documentElement,n.contains(b,e)?(typeof e.getBoundingClientRect!==L&&(d=e.getBoundingClientRect()),c=ed(f),{top:d.top+(c.pageYOffs"
,
"et||b.scrollTop)-(b.clientTop||0),left:d.left+(c.pageXOffset||b.scrollLeft)-(b.clientLeft||0)}):d},position:function(){if(this[0]){var a,b,c={top:0,left:0},d=this[0];return\"fixed\"===n.css(d,\"position\")?b=d.getBoundingClientRect():(a=this.offsetParent(),b=this.offset(),n.nodeName(a[0],\"html\")||(c=a.offset()),c.top+=n.css(a[0],\"borderTopWidth\",!0),c.left+=n.css(a[0],\"borderLeftWidth\",!0)),{top:b.top-c.top-n.css(d,\"marginTop\",!0),left:b.left-c.left-n.css(d,\"marginLeft\",!0)}}},offsetParent:function(){return this.map(function(){var a=this.offsetParent||dd;while(a&&!n.nodeName(a,\"html\")&&\"static\"===n.css(a,\"position\"))a=a.offsetParent;return a||dd})}}),n.each({scrollLeft:\"pageXOffset\",scrollTop:\"pageYOffset\"},function(a,b){var c=/Y/.test(b);n.fn[a]=function(d){return W(this,function(a,d,e){var f=ed(a);return void 0===e?f?b in f?f[b]:f.document.documentElement[d]:a[d]:void(f?f.scrollTo(c?n(f).scrollLeft():e,c?e:n(f).scrollTop()):a[d]=e)},a,d,arguments.length,null)}}),n.each([\"top\",\"left\"],function(a,b){n.cssHooks[b]=Mb(l.pixelPosition,function(a,c){return c?(c=Kb(a,b),Ib.test(c)?n(a).position()[b]+\"px\":c):void 0})}),n.each({Height:\"height\",Width:\"width\"},function(a,b){n.each({padding:\"inner\"+a,content:b,\"\":\"outer\"+a},function(c,d){n.fn[d]=function(d,e){var f=arguments.length&&(c||\"boolean\"!=typeof d),g=c||(d===!0||e===!0?\"margin\":\"border\");return W(this,function(b,c,d){var e;return n.isWindow(b)?b.document.documentElement[\"client\"+a]:9===b.nodeType?(e=b.documentElement,Math.max(b.body[\"scroll\"+a],e[\"scroll\"+a],b.body[\"offset\"+a],e[\"offset\"+a],e[\"client\"+a])):void 0===d?n.css(b,c,g):n.style(b,c,d,g)},b,f?d:void 0,f,null)}})}),n.fn.size=function(){return this.length},n.fn.andSelf=n.fn.addBack,\"function\"==typeof define&&define.amd&&define(\"jquery\",[],function(){return n});var fd=a.jQuery,gd=a.$;return n.noConflict=function(b){return a.$===n&&(a.$=gd),b&&a.jQuery===n&&(a.jQuery=fd),n},typeof b===L&&(a.jQuery=a.$=n),n});\n"
"  </script>\n"
"  <script type=\"text/javascript\">\n"
"/*\n"
" Highcharts JS v4.1.8 (2015-08-20)\n"
"\n"
" (c) 2009-2014 Torstein Honsi\n"
"\n"
" License: www.highcharts.com/license\n"
"*/\n"
"(function(){function A(){var a,b=arguments,c,d={},e=function(a,b){var c,d;typeof a!==\"object\"&&(a={});for(d in b)b.hasOwnProperty(d)&&(c=b[d],a[d]=c&&typeof c===\"object\"&&Object.prototype.toString.call(c)!==\"[object Array]\"&&d!==\"renderTo\"&&typeof c.nodeType!==\"number\"?e(a[d]||{},c):b[d]);return a};b[0]===!0&&(d=b[1],b=Array.prototype.slice.call(b,2));c=b.length;for(a=0;a<c;a++)d=e(d,b[a]);return d}function F(a,b){return parseInt(a,b||10)}function Aa(a){return typeof a===\"string\"}function da(a){return a&&\n"
"typeof a===\"object\"}function Fa(a){return Object.prototype.toString.call(a)===\"[object Array]\"}function qa(a){return typeof a===\"number\"}function Ba(a){return W.log(a)/W.LN10}function ia(a){return W.pow(10,a)}function ja(a,b){for(var c=a.length;c--;)if(a[c]===b){a.splice(c,1);break}}function s(a){return a!==v&&a!==null}function J(a,b,c){var d,e;if(Aa(b))s(c)?a.setAttribute(b,c):a&&a.getAttribute&&(e=a.getAttribute(b));else if(s(b)&&da(b))for(d in b)a.setAttribute(d,b[d]);return e}function ra(a){return Fa(a)?\n"
"a:[a]}function L(a,b){if(xa&&!ca&&b&&b.opacity!==v)b.filter=\"alpha(opacity=\"+b.opacity*100+\")\";r(a.style,b)}function $(a,b,c,d,e){a=C.createElement(a);b&&r(a,b);e&&L(a,{padding:0,border:O,margin:0});c&&L(a,c);d&&d.appendChild(a);return a}function ka(a,b){var c=function(){return v};c.prototype=new a;r(c.prototype,b);return c}function Ga(a,b){return Array((b||2)+1-String(a).length).join(0)+a}function Va(a){return(bb&&bb(a)||mb||0)*6E4}function Ha(a,b){for(var c=\"{\",d=!1,e,f,g,h,i,j=[];(c=a.indexOf(c))!==\n"
"-1;){e=a.slice(0,c);if(d){f=e.split(\":\");g=f.shift().split(\".\");i=g.length;e=b;for(h=0;h<i;h++)e=e[g[h]];if(f.length)f=f.join(\":\"),g=/\\.([0-9])/,h=S.lang,i=void 0,/f$/.test(f)?(i=(i=f.match(g))?i[1]:-1,e!==null&&(e=B.numberFormat(e,i,h.decimalPoint,f.indexOf(\",\")>-1?h.thousandsSep:\"\"))):e=Ma(f,e)}j.push(e);a=a.slice(c+1);c=(d=!d)?\"}\":\"{\"}j.push(a);return j.join(\"\")}function nb(a){return W.pow(10,V(W.log(a)/W.LN10))}function ob(a,b,c,d,e){var f,g=a,c=p(c,1);f=a/c;b||(b=[1,2,2.5,5,10],d===!1&&(c===\n"
"1?b=[1,2,5,10]:c<=0.1&&(b=[1/c])));for(d=0;d<b.length;d++)if(g=b[d],e&&g*c>=a||!e&&f<=(b[d]+(b[d+1]||b[d]))/2)break;g*=c;return g}function pb(a,b){var c=a.length,d,e;for(e=0;e<c;e++)a[e].ss_i=e;a.sort(function(a,c){d=b(a,c);return d===0?a.ss_i-c.ss_i:d});for(e=0;e<c;e++)delete a[e].ss_i}function Na(a){for(var b=a.length,c=a[0];b--;)a[b]<c&&(c=a[b]);return c}function Ca(a){for(var b=a.length,c=a[0];b--;)a[b]>c&&(c=a[b]);return c}function Oa(a,b){for(var c in a)a[c]&&a[c]!==b&&a[c].destroy&&a[c].destroy(),\n"
"delete a[c]}function Pa(a){cb||(cb=$(Ia));a&&cb.appendChild(a);cb.innerHTML=\"\"}function la(a,b){var c=\"Highcharts error #\"+a+\": www.highcharts.com/errors/\"+a;if(b)throw c;K.console&&console.log(c)}function ea(a,b){return parseFloat(a.toPrecision(b||14))}function Qa(a,b){b.renderer.globalAnimation=p(a,b.animation)}function Cb(){var a=S.global,b=a.useUTC,c=b?\"getUTC\":\"get\",d=b?\"setUTC\":\"set\";ya=a.Date||window.Date;mb=b&&a.timezoneOffset;bb=b&&a.getTimezoneOffset;db=function(a,c,d,h,i,j){var k;b?(k=\n"
"ya.UTC.apply(0,arguments),k+=Va(k)):k=(new ya(a,c,p(d,1),p(h,0),p(i,0),p(j,0))).getTime();return k};qb=c+\"Minutes\";rb=c+\"Hours\";sb=c+\"Day\";Wa=c+\"Date\";Xa=c+\"Month\";Ya=c+\"FullYear\";Db=d+\"Milliseconds\";Eb=d+\"Seconds\";Fb=d+\"Minutes\";Gb=d+\"Hours\";tb=d+\"Date\";ub=d+\"Month\";vb=d+\"FullYear\"}function P(){}function Ra(a,b,c,d){this.axis=a;this.pos=b;this.type=c||\"\";this.isNew=!0;!c&&!d&&this.addLabel()}function Hb(a,b,c,d,e){var f=a.chart.inverted;this.axis=a;this.isNegative=c;this.options=b;this.x=d;this.total=\n"
"null;this.points={};this.stack=e;this.alignOptions={align:b.align||(f?c?\"left\":\"right\":\"center\"),verticalAlign:b.verticalAlign||(f?\"middle\":c?\"bottom\":\"top\"),y:p(b.y,f?4:c?14:-6),x:p(b.x,f?c?-6:6:0)};this.textAlign=b.textAlign||(f?c?\"right\":\"left\":\"center\")}var v,C=document,K=window,W=Math,w=W.round,V=W.floor,sa=W.ceil,t=W.max,z=W.min,N=W.abs,X=W.cos,aa=W.sin,ma=W.PI,ga=ma*2/360,za=navigator.userAgent,Ib=K.opera,xa=/(msie|trident)/i.test(za)&&!Ib,eb=C.documentMode===8,fb=/AppleWebKit/.test(za),Ja=\n"
"/Firefox/.test(za),Jb=/(Mobile|Android|Windows Phone)/.test(za),Da=\"http://www.w3.org/2000/svg\",ca=!!C.createElementNS&&!!C.createElementNS(Da,\"svg\").createSVGRect,Nb=Ja&&parseInt(za.split(\"Firefox/\")[1],10)<4,fa=!ca&&!xa&&!!C.createElement(\"canvas\").getContext,Za,$a,Kb={},wb=0,cb,S,Ma,xb,E,ta=function(){return v},Y=[],ab=0,Ia=\"div\",O=\"none\",Ob=/^[0-9]+$/,gb=[\"plotTop\",\"marginRight\",\"marginBottom\",\"plotLeft\"],Pb=\"stroke-width\",ya,db,mb,bb,qb,rb,sb,Wa,Xa,Ya,Db,Eb,Fb,Gb,tb,ub,vb,M={},B;B=K.Highcharts=\n"
"K.Highcharts?la(16,!0):{};B.seriesTypes=M;var r=B.extend=function(a,b){var c;a||(a={});for(c in b)a[c]=b[c];return a},p=B.pick=function(){var a=arguments,b,c,d=a.length;for(b=0;b<d;b++)if(c=a[b],c!==v&&c!==null)return c},Sa=B.wrap=function(a,b,c){var d=a[b];a[b]=function(){var a=Array.prototype.slice.call(arguments);a.unshift(d);return c.apply(this,a)}};Ma=function(a,b,c){if(!s(b)||isNaN(b))return S.lang.invalidDate||\"\";var a=p(a,\"%Y-%m-%d %H:%M:%S\"),d=new ya(b-Va(b)),e,f=d[rb](),g=d[sb](),h=d[Wa](),\n"
"i=d[Xa](),j=d[Ya](),k=S.lang,m=k.weekdays,d=r({a:m[g].substr(0,3),A:m[g],d:Ga(h),e:h,w:g,b:k.shortMonths[i],B:k.months[i],m:Ga(i+1),y:j.toString().substr(2,2),Y:j,H:Ga(f),k:f,I:Ga(f%12||12),l:f%12||12,M:Ga(d[qb]()),p:f<12?\"AM\":\"PM\",P:f<12?\"am\":\"pm\",S:Ga(d.getSeconds()),L:Ga(w(b%1E3),3)},B.dateFormats);for(e in d)for(;a.indexOf(\"%\"+e)!==-1;)a=a.replace(\"%\"+e,typeof d[e]===\"function\"?d[e](b):d[e]);return c?a.substr(0,1).toUpperCase()+a.substr(1):a};E={millisecond:1,second:1E3,minute:6E4,hour:36E5,day:864E5,\n"
"week:6048E5,month:24192E5,year:314496E5};B.numberFormat=function(a,b,c,d){var e=S.lang,a=+a||0,f=b===-1?z((a.toString().split(\".\")[1]||\"\").length,20):isNaN(b=N(b))?2:b,b=c===void 0?e.decimalPoint:c,d=d===void 0?e.thousandsSep:d,e=a<0?\"-\":\"\",c=String(F(a=N(a).toFixed(f))),g=c.length>3?c.length%3:0;return e+(g?c.substr(0,g)+d:\"\")+c.substr(g).replace(/(\\d{3})(?=\\d)/g,\"$1\"+d)+(f?b+N(a-c).toFixed(f).slice(2):\"\")};xb={init:function(a,b,c){var b=b||\"\",d=a.shift,e=b.indexOf(\"C\")>-1,f=e?7:3,g,b=b.split(\" \"),\n"
"c=[].concat(c),h,i,j=function(a){for(g=a.length;g--;)a[g]===\"M\"&&a.splice(g+1,0,a[g+1],a[g+2],a[g+1],a[g+2])};e&&(j(b),j(c));a.isArea&&(h=b.splice(b.length-6,6),i=c.splice(c.length-6,6));if(d<=c.length/f&&b.length===c.length)for(;d--;)c=[].concat(c).splice(0,f).concat(c);a.shift=0;if(b.length)for(a=c.length;b.length<a;)d=[].concat(b).splice(b.length-f,f),e&&(d[f-6]=d[f-2],d[f-5]=d[f-1]),b=b.concat(d);h&&(b=b.concat(h),c=c.concat(i));return[b,c]},step:function(a,b,c,d){var e=[],f=a.length;if(c===1)e=\n"
"d;else if(f===b.length&&c<1)for(;f--;)d=parseFloat(a[f]),e[f]=isNaN(d)?a[f]:c*parseFloat(b[f]-d)+d;else e=b;return e}};(function(a){K.HighchartsAdapter=K.HighchartsAdapter||a&&{init:function(b){var c=a.fx;a.extend(a.easing,{easeOutQuad:function(a,b,c,g,h){return-g*(b/=h)*(b-2)+c}});a.each([\"cur\",\"_default\",\"width\",\"height\",\"opacity\"],function(b,e){var f=c.step,g;e===\"cur\"?f=c.prototype:e===\"_default\"&&a.Tween&&(f=a.Tween.propHooks[e],e=\"set\");(g=f[e])&&(f[e]=function(a){var c,a=b?a:this;if(a.prop!==\n"
,
"\"align\")return c=a.elem,c.attr?c.attr(a.prop,e===\"cur\"?v:a.now):g.apply(this,arguments)})});Sa(a.cssHooks.opacity,\"get\",function(a,b,c){return b.attr?b.opacity||0:a.call(this,b,c)});this.addAnimSetter(\"d\",function(a){var c=a.elem,f;if(!a.started)f=b.init(c,c.d,c.toD),a.start=f[0],a.end=f[1],a.started=!0;c.attr(\"d\",b.step(a.start,a.end,a.pos,c.toD))});this.each=Array.prototype.forEach?function(a,b){return Array.prototype.forEach.call(a,b)}:function(a,b){var c,g=a.length;for(c=0;c<g;c++)if(b.call(a[c],\n"
"a[c],c,a)===!1)return c};a.fn.highcharts=function(){var a=\"Chart\",b=arguments,c,g;if(this[0]){Aa(b[0])&&(a=b[0],b=Array.prototype.slice.call(b,1));c=b[0];if(c!==v)c.chart=c.chart||{},c.chart.renderTo=this[0],new B[a](c,b[1]),g=this;c===v&&(g=Y[J(this[0],\"data-highcharts-chart\")])}return g}},addAnimSetter:function(b,c){a.Tween?a.Tween.propHooks[b]={set:c}:a.fx.step[b]=c},getScript:a.getScript,inArray:a.inArray,adapterRun:function(b,c){return a(b)[c]()},grep:a.grep,map:function(a,c){for(var d=[],e=\n"
"0,f=a.length;e<f;e++)d[e]=c.call(a[e],a[e],e,a);return d},offset:function(b){return a(b).offset()},addEvent:function(b,c,d){a(b).bind(c,d)},removeEvent:function(b,c,d){var e=C.removeEventListener?\"removeEventListener\":\"detachEvent\";C[e]&&b&&!b[e]&&(b[e]=function(){});a(b).unbind(c,d)},fireEvent:function(b,c,d,e){var f=a.Event(c),g=\"detached\"+c,h;!xa&&d&&(delete d.layerX,delete d.layerY,delete d.returnValue);r(f,d);b[c]&&(b[g]=b[c],b[c]=null);a.each([\"preventDefault\",\"stopPropagation\"],function(a,\n"
"b){var c=f[b];f[b]=function(){try{c.call(f)}catch(a){b===\"preventDefault\"&&(h=!0)}}});a(b).trigger(f);b[g]&&(b[c]=b[g],b[g]=null);e&&!f.isDefaultPrevented()&&!h&&e(f)},washMouseEvent:function(a){var c=a.originalEvent||a;if(c.pageX===v)c.pageX=a.pageX,c.pageY=a.pageY;return c},animate:function(b,c,d){var e=a(b);if(!b.style)b.style={};if(c.d)b.toD=c.d,c.d=1;e.stop();c.opacity!==v&&b.attr&&(c.opacity+=\"px\");b.hasAnim=1;e.animate(c,d)},stop:function(b){b.hasAnim&&a(b).stop()}}})(K.jQuery);var U=K.HighchartsAdapter,\n"
"D=U||{};U&&U.init.call(U,xb);var hb=D.adapterRun,Qb=D.getScript,Ka=D.inArray,o=B.each=D.each,ib=D.grep,Rb=D.offset,Ta=D.map,H=D.addEvent,Z=D.removeEvent,I=D.fireEvent,Sb=D.washMouseEvent,jb=D.animate,kb=D.stop;S={colors:\"#7cb5ec,#434348,#90ed7d,#f7a35c,#8085e9,#f15c80,#e4d354,#2b908f,#f45b5b,#91e8e1\".split(\",\"),symbols:[\"circle\",\"diamond\",\"square\",\"triangle\",\"triangle-down\"],lang:{loading:\"Loading...\",months:\"January,February,March,April,May,June,July,August,September,October,November,December\".split(\",\"),\n"
"shortMonths:\"Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec\".split(\",\"),weekdays:\"Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday\".split(\",\"),decimalPoint:\".\",numericSymbols:\"k,M,G,T,P,E\".split(\",\"),resetZoom:\"Reset zoom\",resetZoomTitle:\"Reset zoom level 1:1\",thousandsSep:\" \"},global:{useUTC:!0,canvasToolsURL:\"http://code.highcharts.com/4.1.8/modules/canvas-tools.js\",VMLRadialGradientURL:\"http://code.highcharts.com/4.1.8/gfx/vml-radial-gradient.png\"},chart:{borderColor:\"#4572A7\",borderRadius:0,\n"
"defaultSeriesType:\"line\",ignoreHiddenSeries:!0,spacing:[10,10,15,10],backgroundColor:\"#FFFFFF\",plotBorderColor:\"#C0C0C0\",resetZoomButton:{theme:{zIndex:20},position:{align:\"right\",x:-10,y:10}}},title:{text:\"Chart title\",align:\"center\",margin:15,style:{color:\"#333333\",fontSize:\"18px\"}},subtitle:{text:\"\",align:\"center\",style:{color:\"#555555\"}},plotOptions:{line:{allowPointSelect:!1,showCheckbox:!1,animation:{duration:1E3},events:{},lineWidth:2,marker:{lineWidth:0,radius:4,lineColor:\"#FFFFFF\",states:{hover:{enabled:!0,\n"
"lineWidthPlus:1,radiusPlus:2},select:{fillColor:\"#FFFFFF\",lineColor:\"#000000\",lineWidth:2}}},point:{events:{}},dataLabels:{align:\"center\",formatter:function(){return this.y===null?\"\":B.numberFormat(this.y,-1)},style:{color:\"contrast\",fontSize:\"11px\",fontWeight:\"bold\",textShadow:\"0 0 6px contrast, 0 0 3px contrast\"},verticalAlign:\"bottom\",x:0,y:0,padding:5},cropThreshold:300,pointRange:0,states:{hover:{lineWidthPlus:1,marker:{},halo:{size:10,opacity:0.25}},select:{marker:{}}},stickyTracking:!0,turboThreshold:1E3}},\n"
"labels:{style:{position:\"absolute\",color:\"#3E576F\"}},legend:{enabled:!0,align:\"center\",layout:\"horizontal\",labelFormatter:function(){return this.name},borderColor:\"#909090\",borderRadius:0,navigation:{activeColor:\"#274b6d\",inactiveColor:\"#CCC\"},shadow:!1,itemStyle:{color:\"#333333\",fontSize:\"12px\",fontWeight:\"bold\"},itemHoverStyle:{color:\"#000\"},itemHiddenStyle:{color:\"#CCC\"},itemCheckboxStyle:{position:\"absolute\",width:\"13px\",height:\"13px\"},symbolPadding:5,verticalAlign:\"bottom\",x:0,y:0,title:{style:{fontWeight:\"bold\"}}},\n"
"loading:{labelStyle:{fontWeight:\"bold\",position:\"relative\",top:\"45%\"},style:{position:\"absolute\",backgroundColor:\"white\",opacity:0.5,textAlign:\"center\"}},tooltip:{enabled:!0,animation:ca,backgroundColor:\"rgba(249, 249, 249, .85)\",borderWidth:1,borderRadius:3,dateTimeLabelFormats:{millisecond:\"%A, %b %e, %H:%M:%S.%L\",second:\"%A, %b %e, %H:%M:%S\",minute:\"%A, %b %e, %H:%M\",hour:\"%A, %b %e, %H:%M\",day:\"%A, %b %e, %Y\",week:\"Week from %A, %b %e, %Y\",month:\"%B %Y\",year:\"%Y\"},footerFormat:\"\",headerFormat:'<span style=\"font-size: 10px\">{point.key}</span><br/>',\n"
"pointFormat:'<span style=\"color:{point.color}\">\\u25cf</span> {series.name}: <b>{point.y}</b><br/>',shadow:!0,snap:Jb?25:10,style:{color:\"#333333\",cursor:\"default\",fontSize:\"12px\",padding:\"8px\",whiteSpace:\"nowrap\"}},credits:{enabled:!0,text:\"Highcharts.com\",href:\"http://www.highcharts.com\",position:{align:\"right\",x:-10,verticalAlign:\"bottom\",y:-5},style:{cursor:\"pointer\",color:\"#909090\",fontSize:\"9px\"}}};var ba=S.plotOptions,U=ba.line;Cb();var Tb=/rgba\\(\\s*([0-9]{1,3})\\s*,\\s*([0-9]{1,3})\\s*,\\s*([0-9]{1,3})\\s*,\\s*([0-9]?(?:\\.[0-9]+)?)\\s*\\)/,\n"
"Ub=/#([a-fA-F0-9]{2})([a-fA-F0-9]{2})([a-fA-F0-9]{2})/,Vb=/rgb\\(\\s*([0-9]{1,3})\\s*,\\s*([0-9]{1,3})\\s*,\\s*([0-9]{1,3})\\s*\\)/,na=function(a){var b=[],c,d;(function(a){a&&a.stops?d=Ta(a.stops,function(a){return na(a[1])}):(c=Tb.exec(a))?b=[F(c[1]),F(c[2]),F(c[3]),parseFloat(c[4],10)]:(c=Ub.exec(a))?b=[F(c[1],16),F(c[2],16),F(c[3],16),1]:(c=Vb.exec(a))&&(b=[F(c[1]),F(c[2]),F(c[3]),1])})(a);return{get:function(c){var f;d?(f=A(a),f.stops=[].concat(f.stops),o(d,function(a,b){f.stops[b]=[f.stops[b][0],a.get(c)]})):\n"
"f=b&&!isNaN(b[0])?c===\"rgb\"?\"rgb(\"+b[0]+\",\"+b[1]+\",\"+b[2]+\")\":c===\"a\"?b[3]:\"rgba(\"+b.join(\",\")+\")\":a;return f},brighten:function(a){if(d)o(d,function(b){b.brighten(a)});else if(qa(a)&&a!==0){var c;for(c=0;c<3;c++)b[c]+=F(a*255),b[c]<0&&(b[c]=0),b[c]>255&&(b[c]=255)}return this},rgba:b,setOpacity:function(a){b[3]=a;return this},raw:a}};P.prototype={opacity:1,textProps:\"fontSize,fontWeight,fontFamily,fontStyle,color,lineHeight,width,textDecoration,textOverflow,textShadow\".split(\",\"),init:function(a,\n"
"b){this.element=b===\"span\"?$(b):C.createElementNS(Da,b);this.renderer=a},animate:function(a,b,c){b=p(b,this.renderer.globalAnimation,!0);kb(this);if(b){b=A(b,{});if(c)b.complete=c;jb(this,a,b)}else this.attr(a,null,c);return this},colorGradient:function(a,b,c){var d=this.renderer,e,f,g,h,i,j,k,m,l,n,q=[];a.linearGradient?f=\"linearGradient\":a.radialGradient&&(f=\"radialGradient\");if(f){g=a[f];h=d.gradients;j=a.stops;l=c.radialReference;Fa(g)&&(a[f]=g={x1:g[0],y1:g[1],x2:g[2],y2:g[3],gradientUnits:\"userSpaceOnUse\"});\n"
"f===\"radialGradient\"&&l&&!s(g.gradientUnits)&&(g=A(g,{cx:l[0]-l[2]/2+g.cx*l[2],cy:l[1]-l[2]/2+g.cy*l[2],r:g.r*l[2],gradientUnits:\"userSpaceOnUse\"}));for(n in g)n!==\"id\"&&q.push(n,g[n]);for(n in j)q.push(j[n]);q=q.join(\",\");h[q]?a=h[q].attr(\"id\"):(g.id=a=\"highcharts-\"+wb++,h[q]=i=d.createElement(f).attr(g).add(d.defs),i.stops=[],o(j,function(a){a[1].indexOf(\"rgba\")===0?(e=na(a[1]),k=e.get(\"rgb\"),m=e.get(\"a\")):(k=a[1],m=1);a=d.createElement(\"stop\").attr({offset:a[0],\"stop-color\":k,\"stop-opacity\":m}).add(i);\n"
"i.stops.push(a)}));c.setAttribute(b,\"url(\"+d.url+\"#\"+a+\")\")}},applyTextShadow:function(a){var b=this.element,c,d=a.indexOf(\"contrast\")!==-1,e={},f=this.renderer.forExport||b.style.textShadow!==v&&!xa;if(d)e.textShadow=a=a.replace(/contrast/g,this.renderer.getContrast(b.style.fill));if(fb)e.textRendering=\"geometricPrecision\";f?L(b,e):(this.fakeTS=!0,this.ySetter=this.xSetter,c=[].slice.call(b.getElementsByTagName(\"tspan\")),o(a.split(/\\s?,\\s?/g),function(a){var d=b.firstChild,e,f,a=a.split(\" \");e=a[a.length-\n"
"1];(f=a[a.length-2])&&o(c,function(a,c){var g;c===0&&(a.setAttribute(\"x\",b.getAttribute(\"x\")),c=b.getAttribute(\"y\"),a.setAttribute(\"y\",c||0),c===null&&b.setAttribute(\"y\",0));g=a.cloneNode(1);J(g,{\"class\":\"highcharts-text-shadow\",fill:e,stroke:e,\"stroke-opacity\":1/t(F(f),3),\"stroke-width\":f,\"stroke-linejoin\":\"round\"});b.insertBefore(g,d)})}))},attr:function(a,b,c){var d,e=this.element,f,g=this,h;typeof a===\"string\"&&b!==v&&(d=a,a={},a[d]=b);if(typeof a===\"string\")g=(this[a+\"Getter\"]||this._defaultGetter).call(this,\n"
"a,e);else{for(d in a){b=a[d];h=!1;this.symbolName&&/^(x|y|width|height|r|start|end|innerR|anchorX|anchorY)/.test(d)&&(f||(this.symbolAttr(a),f=!0),h=!0);if(this.rotation&&(d===\"x\"||d===\"y\"))this.doTransform=!0;h||(this[d+\"Setter\"]||this._defaultSetter).call(this,b,d,e);this.shadows&&/^(width|height|visibility|x|y|d|transform|cx|cy|r)$/.test(d)&&this.updateShadows(d,b)}if(this.doTransform)this.updateTransform(),this.doTransform=!1}c&&c();return g},updateShadows:function(a,b){for(var c=this.shadows,\n"
,
"d=c.length;d--;)c[d].setAttribute(a,a===\"height\"?t(b-(c[d].cutHeight||0),0):a===\"d\"?this.d:b)},addClass:function(a){var b=this.element,c=J(b,\"class\")||\"\";c.indexOf(a)===-1&&J(b,\"class\",c+\" \"+a);return this},symbolAttr:function(a){var b=this;o(\"x,y,r,start,end,width,height,innerR,anchorX,anchorY\".split(\",\"),function(c){b[c]=p(a[c],b[c])});b.attr({d:b.renderer.symbols[b.symbolName](b.x,b.y,b.width,b.height,b)})},clip:function(a){return this.attr(\"clip-path\",a?\"url(\"+this.renderer.url+\"#\"+a.id+\")\":O)},\n"
"crisp:function(a){var b,c={},d,e=a.strokeWidth||this.strokeWidth||0;d=w(e)%2/2;a.x=V(a.x||this.x||0)+d;a.y=V(a.y||this.y||0)+d;a.width=V((a.width||this.width||0)-2*d);a.height=V((a.height||this.height||0)-2*d);a.strokeWidth=e;for(b in a)this[b]!==a[b]&&(this[b]=c[b]=a[b]);return c},css:function(a){var b=this.styles,c={},d=this.element,e,f,g=\"\";e=!b;if(a&&a.color)a.fill=a.color;if(b)for(f in a)a[f]!==b[f]&&(c[f]=a[f],e=!0);if(e){e=this.textWidth=a&&a.width&&d.nodeName.toLowerCase()===\"text\"&&F(a.width)||\n"
"this.textWidth;b&&(a=r(b,c));this.styles=a;e&&(fa||!ca&&this.renderer.forExport)&&delete a.width;if(xa&&!ca)L(this.element,a);else{b=function(a,b){return\"-\"+b.toLowerCase()};for(f in a)g+=f.replace(/([A-Z])/g,b)+\":\"+a[f]+\";\";J(d,\"style\",g)}e&&this.added&&this.renderer.buildText(this)}return this},on:function(a,b){var c=this,d=c.element;$a&&a===\"click\"?(d.ontouchstart=function(a){c.touchEventFired=ya.now();a.preventDefault();b.call(d,a)},d.onclick=function(a){(za.indexOf(\"Android\")===-1||ya.now()-\n"
"(c.touchEventFired||0)>1100)&&b.call(d,a)}):d[\"on\"+a]=b;return this},setRadialReference:function(a){this.element.radialReference=a;return this},translate:function(a,b){return this.attr({translateX:a,translateY:b})},invert:function(){this.inverted=!0;this.updateTransform();return this},updateTransform:function(){var a=this.translateX||0,b=this.translateY||0,c=this.scaleX,d=this.scaleY,e=this.inverted,f=this.rotation,g=this.element;e&&(a+=this.attr(\"width\"),b+=this.attr(\"height\"));a=[\"translate(\"+a+\n"
"\",\"+b+\")\"];e?a.push(\"rotate(90) scale(-1,1)\"):f&&a.push(\"rotate(\"+f+\" \"+(g.getAttribute(\"x\")||0)+\" \"+(g.getAttribute(\"y\")||0)+\")\");(s(c)||s(d))&&a.push(\"scale(\"+p(c,1)+\" \"+p(d,1)+\")\");a.length&&g.setAttribute(\"transform\",a.join(\" \"))},toFront:function(){var a=this.element;a.parentNode.appendChild(a);return this},align:function(a,b,c){var d,e,f,g,h={};e=this.renderer;f=e.alignedObjects;if(a){if(this.alignOptions=a,this.alignByTranslate=b,!c||Aa(c))this.alignTo=d=c||\"renderer\",ja(f,this),f.push(this),\n"
"c=null}else a=this.alignOptions,b=this.alignByTranslate,d=this.alignTo;c=p(c,e[d],e);d=a.align;e=a.verticalAlign;f=(c.x||0)+(a.x||0);g=(c.y||0)+(a.y||0);if(d===\"right\"||d===\"center\")f+=(c.width-(a.width||0))/{right:1,center:2}[d];h[b?\"translateX\":\"x\"]=w(f);if(e===\"bottom\"||e===\"middle\")g+=(c.height-(a.height||0))/({bottom:1,middle:2}[e]||1);h[b?\"translateY\":\"y\"]=w(g);this[this.placed?\"animate\":\"attr\"](h);this.placed=!0;this.alignAttr=h;return this},getBBox:function(a){var b,c=this.renderer,d,e=this.rotation,\n"
"f=this.element,g=this.styles,h=e*ga;d=this.textStr;var i,j=f.style,k,m;d!==v&&(m=[\"\",e||0,g&&g.fontSize,f.style.width].join(\",\"),m=d===\"\"||Ob.test(d)?\"num:\"+d.toString().length+m:d+m);m&&!a&&(b=c.cache[m]);if(!b){if(f.namespaceURI===Da||c.forExport){try{k=this.fakeTS&&function(a){o(f.querySelectorAll(\".highcharts-text-shadow\"),function(b){b.style.display=a})},Ja&&j.textShadow?(i=j.textShadow,j.textShadow=\"\"):k&&k(O),b=f.getBBox?r({},f.getBBox()):{width:f.offsetWidth,height:f.offsetHeight},i?j.textShadow=\n"
"i:k&&k(\"\")}catch(l){}if(!b||b.width<0)b={width:0,height:0}}else b=this.htmlGetBBox();if(c.isSVG){a=b.width;d=b.height;if(xa&&g&&g.fontSize===\"11px\"&&d.toPrecision(3)===\"16.9\")b.height=d=14;if(e)b.width=N(d*aa(h))+N(a*X(h)),b.height=N(d*X(h))+N(a*aa(h))}m&&(c.cache[m]=b)}return b},show:function(a){return this.attr({visibility:a?\"inherit\":\"visible\"})},hide:function(){return this.attr({visibility:\"hidden\"})},fadeOut:function(a){var b=this;b.animate({opacity:0},{duration:a||150,complete:function(){b.attr({y:-9999})}})},\n"
"add:function(a){var b=this.renderer,c=this.element,d;if(a)this.parentGroup=a;this.parentInverted=a&&a.inverted;this.textStr!==void 0&&b.buildText(this);this.added=!0;if(!a||a.handleZ||this.zIndex)d=this.zIndexSetter();d||(a?a.element:b.box).appendChild(c);if(this.onAdd)this.onAdd();return this},safeRemoveChild:function(a){var b=a.parentNode;b&&b.removeChild(a)},destroy:function(){var a=this,b=a.element||{},c=a.shadows,d=a.renderer.isSVG&&b.nodeName===\"SPAN\"&&a.parentGroup,e,f;b.onclick=b.onmouseout=\n"
"b.onmouseover=b.onmousemove=b.point=null;kb(a);if(a.clipPath)a.clipPath=a.clipPath.destroy();if(a.stops){for(f=0;f<a.stops.length;f++)a.stops[f]=a.stops[f].destroy();a.stops=null}a.safeRemoveChild(b);for(c&&o(c,function(b){a.safeRemoveChild(b)});d&&d.div&&d.div.childNodes.length===0;)b=d.parentGroup,a.safeRemoveChild(d.div),delete d.div,d=b;a.alignTo&&ja(a.renderer.alignedObjects,a);for(e in a)delete a[e];return null},shadow:function(a,b,c){var d=[],e,f,g=this.element,h,i,j,k;if(a){i=p(a.width,3);\n"
"j=(a.opacity||0.15)/i;k=this.parentInverted?\"(-1,-1)\":\"(\"+p(a.offsetX,1)+\", \"+p(a.offsetY,1)+\")\";for(e=1;e<=i;e++){f=g.cloneNode(0);h=i*2+1-2*e;J(f,{isShadow:\"true\",stroke:a.color||\"black\",\"stroke-opacity\":j*e,\"stroke-width\":h,transform:\"translate\"+k,fill:O});if(c)J(f,\"height\",t(J(f,\"height\")-h,0)),f.cutHeight=h;b?b.element.appendChild(f):g.parentNode.insertBefore(f,g);d.push(f)}this.shadows=d}return this},xGetter:function(a){this.element.nodeName===\"circle\"&&(a={x:\"cx\",y:\"cy\"}[a]||a);return this._defaultGetter(a)},\n"
"_defaultGetter:function(a){a=p(this[a],this.element?this.element.getAttribute(a):null,0);/^[\\-0-9\\.]+$/.test(a)&&(a=parseFloat(a));return a},dSetter:function(a,b,c){a&&a.join&&(a=a.join(\" \"));/(NaN| {2}|^$)/.test(a)&&(a=\"M 0 0\");c.setAttribute(b,a);this[b]=a},dashstyleSetter:function(a){var b;if(a=a&&a.toLowerCase()){a=a.replace(\"shortdashdotdot\",\"3,1,1,1,1,1,\").replace(\"shortdashdot\",\"3,1,1,1\").replace(\"shortdot\",\"1,1,\").replace(\"shortdash\",\"3,1,\").replace(\"longdash\",\"8,3,\").replace(/dot/g,\"1,3,\").replace(\"dash\",\n"
"\"4,3,\").replace(/,$/,\"\").split(\",\");for(b=a.length;b--;)a[b]=F(a[b])*this[\"stroke-width\"];a=a.join(\",\").replace(\"NaN\",\"none\");this.element.setAttribute(\"stroke-dasharray\",a)}},alignSetter:function(a){this.element.setAttribute(\"text-anchor\",{left:\"start\",center:\"middle\",right:\"end\"}[a])},opacitySetter:function(a,b,c){this[b]=a;c.setAttribute(b,a)},titleSetter:function(a){var b=this.element.getElementsByTagName(\"title\")[0];b||(b=C.createElementNS(Da,\"title\"),this.element.appendChild(b));b.appendChild(C.createTextNode(String(p(a),\n"
"\"\").replace(/<[^>]*>/g,\"\")))},textSetter:function(a){if(a!==this.textStr)delete this.bBox,this.textStr=a,this.added&&this.renderer.buildText(this)},fillSetter:function(a,b,c){typeof a===\"string\"?c.setAttribute(b,a):a&&this.colorGradient(a,b,c)},visibilitySetter:function(a,b,c){a===\"inherit\"?c.removeAttribute(b):c.setAttribute(b,a)},zIndexSetter:function(a,b){var c=this.renderer,d=this.parentGroup,c=(d||c).element||c.box,e,f,g=this.element,h;e=this.added;var i;s(a)&&(g.setAttribute(b,a),a=+a,this[b]===\n"
"a&&(e=!1),this[b]=a);if(e){if((a=this.zIndex)&&d)d.handleZ=!0;d=c.childNodes;for(i=0;i<d.length&&!h;i++)if(e=d[i],f=J(e,\"zIndex\"),e!==g&&(F(f)>a||!s(a)&&s(f)))c.insertBefore(g,e),h=!0;h||c.appendChild(g)}return h},_defaultSetter:function(a,b,c){c.setAttribute(b,a)}};P.prototype.yGetter=P.prototype.xGetter;P.prototype.translateXSetter=P.prototype.translateYSetter=P.prototype.rotationSetter=P.prototype.verticalAlignSetter=P.prototype.scaleXSetter=P.prototype.scaleYSetter=function(a,b){this[b]=a;this.doTransform=\n"
"!0};P.prototype[\"stroke-widthSetter\"]=P.prototype.strokeSetter=function(a,b,c){this[b]=a;if(this.stroke&&this[\"stroke-width\"])this.strokeWidth=this[\"stroke-width\"],P.prototype.fillSetter.call(this,this.stroke,\"stroke\",c),c.setAttribute(\"stroke-width\",this[\"stroke-width\"]),this.hasStroke=!0;else if(b===\"stroke-width\"&&a===0&&this.hasStroke)c.removeAttribute(\"stroke\"),this.hasStroke=!1};var ua=function(){this.init.apply(this,arguments)};ua.prototype={Element:P,init:function(a,b,c,d,e){var f=location,\n"
"g,d=this.createElement(\"svg\").attr({version:\"1.1\"}).css(this.getStyle(d));g=d.element;a.appendChild(g);a.innerHTML.indexOf(\"xmlns\")===-1&&J(g,\"xmlns\",Da);this.isSVG=!0;this.box=g;this.boxWrapper=d;this.alignedObjects=[];this.url=(Ja||fb)&&C.getElementsByTagName(\"base\").length?f.href.replace(/#.*?$/,\"\").replace(/([\\('\\)])/g,\"\\\\$1\").replace(/ /g,\"%20\"):\"\";this.createElement(\"desc\").add().element.appendChild(C.createTextNode(\"Created with Highcharts 4.1.8\"));this.defs=this.createElement(\"defs\").add();\n"
"this.forExport=e;this.gradients={};this.cache={};this.setSize(b,c,!1);var h;if(Ja&&a.getBoundingClientRect)this.subPixelFix=b=function(){L(a,{left:0,top:0});h=a.getBoundingClientRect();L(a,{left:sa(h.left)-h.left+\"px\",top:sa(h.top)-h.top+\"px\"})},b(),H(K,\"resize\",b)},getStyle:function(a){return this.style=r({fontFamily:'\"Lucida Grande\", \"Lucida Sans Unicode\", Arial, Helvetica, sans-serif',fontSize:\"12px\"},a)},isHidden:function(){return!this.boxWrapper.getBBox().width},destroy:function(){var a=this.defs;\n"
,
"this.box=null;this.boxWrapper=this.boxWrapper.destroy();Oa(this.gradients||{});this.gradients=null;if(a)this.defs=a.destroy();this.subPixelFix&&Z(K,\"resize\",this.subPixelFix);return this.alignedObjects=null},createElement:function(a){var b=new this.Element;b.init(this,a);return b},draw:function(){},buildText:function(a){for(var b=a.element,c=this,d=c.forExport,e=p(a.textStr,\"\").toString(),f=e.indexOf(\"<\")!==-1,g=b.childNodes,h,i,j=J(b,\"x\"),k=a.styles,m=a.textWidth,l=k&&k.lineHeight,n=k&&k.textShadow,\n"
"q=k&&k.textOverflow===\"ellipsis\",x=g.length,T=m&&!a.added&&this.box,Q=function(a){return l?F(l):c.fontMetrics(/(px|em)$/.test(a&&a.style.fontSize)?a.style.fontSize:k&&k.fontSize||c.style.fontSize||12,a).h},u=function(a){return a.replace(/&lt;/g,\"<\").replace(/&gt;/g,\">\")};x--;)b.removeChild(g[x]);!f&&!n&&!q&&e.indexOf(\" \")===-1?b.appendChild(C.createTextNode(u(e))):(h=/<.*style=\"([^\"]+)\".*>/,i=/<.*href=\"(http[^\"]+)\".*>/,T&&T.appendChild(b),e=f?e.replace(/<(b|strong)>/g,'<span style=\"font-weight:bold\">').replace(/<(i|em)>/g,\n"
"'<span style=\"font-style:italic\">').replace(/<a/g,\"<span\").replace(/<\\/(b|strong|i|em|a)>/g,\"</span>\").split(/<br.*?>/g):[e],e[e.length-1]===\"\"&&e.pop(),o(e,function(e,f){var g,l=0,e=e.replace(/<span/g,\"|||<span\").replace(/<\\/span>/g,\"</span>|||\");g=e.split(\"|||\");o(g,function(e){if(e!==\"\"||g.length===1){var n={},x=C.createElementNS(Da,\"tspan\"),p;h.test(e)&&(p=e.match(h)[1].replace(/(;| |^)color([ :])/,\"$1fill$2\"),J(x,\"style\",p));i.test(e)&&!d&&(J(x,\"onclick\",'location.href=\"'+e.match(i)[1]+'\"'),\n"
"L(x,{cursor:\"pointer\"}));e=u(e.replace(/<(.|\\n)*?>/g,\"\")||\" \");if(e!==\" \"){x.appendChild(C.createTextNode(e));if(l)n.dx=0;else if(f&&j!==null)n.x=j;J(x,n);b.appendChild(x);!l&&f&&(!ca&&d&&L(x,{display:\"block\"}),J(x,\"dy\",Q(x)));if(m){for(var n=e.replace(/([^\\^])-/g,\"$1- \").split(\" \"),o=g.length>1||f||n.length>1&&k.whiteSpace!==\"nowrap\",T,y,s,t=[],v=Q(x),w=1,r=a.rotation,z=e,A=z.length;(o||q)&&(n.length||t.length);)a.rotation=0,T=a.getBBox(!0),s=T.width,!ca&&c.forExport&&(s=c.measureSpanWidth(x.firstChild.data,\n"
"a.styles)),T=s>m,y===void 0&&(y=T),q&&y?(A/=2,z===\"\"||!T&&A<0.5?n=[]:(T&&(y=!0),z=e.substring(0,z.length+(T?-1:1)*sa(A)),n=[z+(m>3?\"\\u2026\":\"\")],x.removeChild(x.firstChild))):!T||n.length===1?(n=t,t=[],n.length&&(w++,x=C.createElementNS(Da,\"tspan\"),J(x,{dy:v,x:j}),p&&J(x,\"style\",p),b.appendChild(x)),s>m&&(m=s)):(x.removeChild(x.firstChild),t.unshift(n.pop())),n.length&&x.appendChild(C.createTextNode(n.join(\" \").replace(/- /g,\"-\")));y&&a.attr(\"title\",a.textStr);a.rotation=r}l++}}})}),T&&T.removeChild(b),\n"
"n&&a.applyTextShadow&&a.applyTextShadow(n))},getContrast:function(a){a=na(a).rgba;return a[0]+a[1]+a[2]>384?\"#000000\":\"#FFFFFF\"},button:function(a,b,c,d,e,f,g,h,i){var j=this.label(a,b,c,i,null,null,null,null,\"button\"),k=0,m,l,n,q,x,p,a={x1:0,y1:0,x2:0,y2:1},e=A({\"stroke-width\":1,stroke:\"#CCCCCC\",fill:{linearGradient:a,stops:[[0,\"#FEFEFE\"],[1,\"#F6F6F6\"]]},r:2,padding:5,style:{color:\"black\"}},e);n=e.style;delete e.style;f=A(e,{stroke:\"#68A\",fill:{linearGradient:a,stops:[[0,\"#FFF\"],[1,\"#ACF\"]]}},f);\n"
"q=f.style;delete f.style;g=A(e,{stroke:\"#68A\",fill:{linearGradient:a,stops:[[0,\"#9BD\"],[1,\"#CDF\"]]}},g);x=g.style;delete g.style;h=A(e,{style:{color:\"#CCC\"}},h);p=h.style;delete h.style;H(j.element,xa?\"mouseover\":\"mouseenter\",function(){k!==3&&j.attr(f).css(q)});H(j.element,xa?\"mouseout\":\"mouseleave\",function(){k!==3&&(m=[e,f,g][k],l=[n,q,x][k],j.attr(m).css(l))});j.setState=function(a){(j.state=k=a)?a===2?j.attr(g).css(x):a===3&&j.attr(h).css(p):j.attr(e).css(n)};return j.on(\"click\",function(a){k!==\n"
"3&&d.call(j,a)}).attr(e).css(r({cursor:\"default\"},n))},crispLine:function(a,b){a[1]===a[4]&&(a[1]=a[4]=w(a[1])-b%2/2);a[2]===a[5]&&(a[2]=a[5]=w(a[2])+b%2/2);return a},path:function(a){var b={fill:O};Fa(a)?b.d=a:da(a)&&r(b,a);return this.createElement(\"path\").attr(b)},circle:function(a,b,c){a=da(a)?a:{x:a,y:b,r:c};b=this.createElement(\"circle\");b.xSetter=function(a){this.element.setAttribute(\"cx\",a)};b.ySetter=function(a){this.element.setAttribute(\"cy\",a)};return b.attr(a)},arc:function(a,b,c,d,e,\n"
"f){if(da(a))b=a.y,c=a.r,d=a.innerR,e=a.start,f=a.end,a=a.x;a=this.symbol(\"arc\",a||0,b||0,c||0,c||0,{innerR:d||0,start:e||0,end:f||0});a.r=c;return a},rect:function(a,b,c,d,e,f){var e=da(a)?a.r:e,g=this.createElement(\"rect\"),a=da(a)?a:a===v?{}:{x:a,y:b,width:t(c,0),height:t(d,0)};if(f!==v)a.strokeWidth=f,a=g.crisp(a);if(e)a.r=e;g.rSetter=function(a){J(this.element,{rx:a,ry:a})};return g.attr(a)},setSize:function(a,b,c){var d=this.alignedObjects,e=d.length;this.width=a;this.height=b;for(this.boxWrapper[p(c,\n"
"!0)?\"animate\":\"attr\"]({width:a,height:b});e--;)d[e].align()},g:function(a){var b=this.createElement(\"g\");return s(a)?b.attr({\"class\":\"highcharts-\"+a}):b},image:function(a,b,c,d,e){var f={preserveAspectRatio:O};arguments.length>1&&r(f,{x:b,y:c,width:d,height:e});f=this.createElement(\"image\").attr(f);f.element.setAttributeNS?f.element.setAttributeNS(\"http://www.w3.org/1999/xlink\",\"href\",a):f.element.setAttribute(\"hc-svg-href\",a);return f},symbol:function(a,b,c,d,e,f){var g,h=this.symbols[a],h=h&&h(w(b),\n"
"w(c),d,e,f),i=/^url\\((.*?)\\)$/,j,k;if(h)g=this.path(h),r(g,{symbolName:a,x:b,y:c,width:d,height:e}),f&&r(g,f);else if(i.test(a))k=function(a,b){a.element&&(a.attr({width:b[0],height:b[1]}),a.alignByTranslate||a.translate(w((d-b[0])/2),w((e-b[1])/2)))},j=a.match(i)[1],a=Kb[j]||f&&f.width&&f.height&&[f.width,f.height],g=this.image(j).attr({x:b,y:c}),g.isImg=!0,a?k(g,a):(g.attr({width:0,height:0}),$(\"img\",{onload:function(){k(g,Kb[j]=[this.width,this.height])},src:j}));return g},symbols:{circle:function(a,\n"
"b,c,d){var e=0.166*c;return[\"M\",a+c/2,b,\"C\",a+c+e,b,a+c+e,b+d,a+c/2,b+d,\"C\",a-e,b+d,a-e,b,a+c/2,b,\"Z\"]},square:function(a,b,c,d){return[\"M\",a,b,\"L\",a+c,b,a+c,b+d,a,b+d,\"Z\"]},triangle:function(a,b,c,d){return[\"M\",a+c/2,b,\"L\",a+c,b+d,a,b+d,\"Z\"]},\"triangle-down\":function(a,b,c,d){return[\"M\",a,b,\"L\",a+c,b,a+c/2,b+d,\"Z\"]},diamond:function(a,b,c,d){return[\"M\",a+c/2,b,\"L\",a+c,b+d/2,a+c/2,b+d,a,b+d/2,\"Z\"]},arc:function(a,b,c,d,e){var f=e.start,c=e.r||c||d,g=e.end-0.001,d=e.innerR,h=e.open,i=X(f),j=aa(f),\n"
"k=X(g),g=aa(g),e=e.end-f<ma?0:1;return[\"M\",a+c*i,b+c*j,\"A\",c,c,0,e,1,a+c*k,b+c*g,h?\"M\":\"L\",a+d*k,b+d*g,\"A\",d,d,0,e,0,a+d*i,b+d*j,h?\"\":\"Z\"]},callout:function(a,b,c,d,e){var f=z(e&&e.r||0,c,d),g=f+6,h=e&&e.anchorX,e=e&&e.anchorY,i;i=[\"M\",a+f,b,\"L\",a+c-f,b,\"C\",a+c,b,a+c,b,a+c,b+f,\"L\",a+c,b+d-f,\"C\",a+c,b+d,a+c,b+d,a+c-f,b+d,\"L\",a+f,b+d,\"C\",a,b+d,a,b+d,a,b+d-f,\"L\",a,b+f,\"C\",a,b,a,b,a+f,b];h&&h>c&&e>b+g&&e<b+d-g?i.splice(13,3,\"L\",a+c,e-6,a+c+6,e,a+c,e+6,a+c,b+d-f):h&&h<0&&e>b+g&&e<b+d-g?i.splice(33,3,\"L\",\n"
"a,e+6,a-6,e,a,e-6,a,b+f):e&&e>d&&h>a+g&&h<a+c-g?i.splice(23,3,\"L\",h+6,b+d,h,b+d+6,h-6,b+d,a+f,b+d):e&&e<0&&h>a+g&&h<a+c-g&&i.splice(3,3,\"L\",h-6,b,h,b-6,h+6,b,c-f,b);return i}},clipRect:function(a,b,c,d){var e=\"highcharts-\"+wb++,f=this.createElement(\"clipPath\").attr({id:e}).add(this.defs),a=this.rect(a,b,c,d,0).add(f);a.id=e;a.clipPath=f;a.count=0;return a},text:function(a,b,c,d){var e=fa||!ca&&this.forExport,f={};if(d&&!this.forExport)return this.html(a,b,c);f.x=Math.round(b||0);if(c)f.y=Math.round(c);\n"
"if(a||a===0)f.text=a;a=this.createElement(\"text\").attr(f);e&&a.css({position:\"absolute\"});if(!d)a.xSetter=function(a,b,c){var d=c.getElementsByTagName(\"tspan\"),e,f=c.getAttribute(b),l;for(l=0;l<d.length;l++)e=d[l],e.getAttribute(b)===f&&e.setAttribute(b,a);c.setAttribute(b,a)};return a},fontMetrics:function(a,b){var c,d,a=a||this.style.fontSize;b&&K.getComputedStyle&&(b=b.element||b,a=(c=K.getComputedStyle(b,\"\"))&&c.fontSize);a=/px/.test(a)?F(a):/em/.test(a)?parseFloat(a)*12:12;c=a<24?a+3:w(a*1.2);\n"
"d=w(c*0.8);return{h:c,b:d,f:a}},rotCorr:function(a,b,c){var d=a;b&&c&&(d=t(d*X(b*ga),4));return{x:-a/3*aa(b*ga),y:d}},label:function(a,b,c,d,e,f,g,h,i){function j(){var a,b;a=q.element.style;p=(t===void 0||yb===void 0||n.styles.textAlign)&&s(q.textStr)&&q.getBBox();n.width=(t||p.width||0)+2*u+y;n.height=(yb||p.height||0)+2*u;B=u+l.fontMetrics(a&&a.fontSize,q).b;if(D){if(!x)a=w(-Q*u)+C,b=(h?-B:0)+C,n.box=x=d?l.symbol(d,a,b,n.width,n.height,G):l.rect(a,b,n.width,n.height,0,G[Pb]),x.isImg||x.attr(\"fill\",\n"
"O),x.add(n);x.isImg||x.attr(r({width:w(n.width),height:w(n.height)},G));G=null}}function k(){var a=n.styles,a=a&&a.textAlign,b=y+u*(1-Q),c;c=h?0:B;if(s(t)&&p&&(a===\"center\"||a===\"right\"))b+={center:0.5,right:1}[a]*(t-p.width);if(b!==q.x||c!==q.y)q.attr(\"x\",b),c!==v&&q.attr(\"y\",c);q.x=b;q.y=c}function m(a,b){x?x.attr(a,b):G[a]=b}var l=this,n=l.g(i),q=l.text(\"\",0,0,g).attr({zIndex:1}),x,p,Q=0,u=3,y=0,t,yb,zb,z,C=0,G={},B,D;n.onAdd=function(){q.add(n);n.attr({text:a||a===0?a:\"\",x:b,y:c});x&&s(e)&&n.attr({anchorX:e,\n"
"anchorY:f})};n.widthSetter=function(a){t=a};n.heightSetter=function(a){yb=a};n.paddingSetter=function(a){if(s(a)&&a!==u)u=n.padding=a,k()};n.paddingLeftSetter=function(a){s(a)&&a!==y&&(y=a,k())};n.alignSetter=function(a){Q={left:0,center:0.5,right:1}[a]};n.textSetter=function(a){a!==v&&q.textSetter(a);j();k()};n[\"stroke-widthSetter\"]=function(a,b){a&&(D=!0);C=a%2/2;m(b,a)};n.strokeSetter=n.fillSetter=n.rSetter=function(a,b){b===\"fill\"&&a&&(D=!0);m(b,a)};n.anchorXSetter=function(a,b){e=a;m(b,w(a)-\n"
,
"C-zb)};n.anchorYSetter=function(a,b){f=a;m(b,a-z)};n.xSetter=function(a){n.x=a;Q&&(a-=Q*((t||p.width)+u));zb=w(a);n.attr(\"translateX\",zb)};n.ySetter=function(a){z=n.y=w(a);n.attr(\"translateY\",z)};var F=n.css;return r(n,{css:function(a){if(a){var b={},a=A(a);o(n.textProps,function(c){a[c]!==v&&(b[c]=a[c],delete a[c])});q.css(b)}return F.call(n,a)},getBBox:function(){return{width:p.width+2*u,height:p.height+2*u,x:p.x-u,y:p.y-u}},shadow:function(a){x&&x.shadow(a);return n},destroy:function(){Z(n.element,\n"
"\"mouseenter\");Z(n.element,\"mouseleave\");q&&(q=q.destroy());x&&(x=x.destroy());P.prototype.destroy.call(n);n=l=j=k=m=null}})}};Za=ua;r(P.prototype,{htmlCss:function(a){var b=this.element;if(b=a&&b.tagName===\"SPAN\"&&a.width)delete a.width,this.textWidth=b,this.updateTransform();if(a&&a.textOverflow===\"ellipsis\")a.whiteSpace=\"nowrap\",a.overflow=\"hidden\";this.styles=r(this.styles,a);L(this.element,a);return this},htmlGetBBox:function(){var a=this.element;if(a.nodeName===\"text\")a.style.position=\"absolute\";\n"
"return{x:a.offsetLeft,y:a.offsetTop,width:a.offsetWidth,height:a.offsetHeight}},htmlUpdateTransform:function(){if(this.added){var a=this.renderer,b=this.element,c=this.translateX||0,d=this.translateY||0,e=this.x||0,f=this.y||0,g=this.textAlign||\"left\",h={left:0,center:0.5,right:1}[g],i=this.shadows,j=this.styles;L(b,{marginLeft:c,marginTop:d});i&&o(i,function(a){L(a,{marginLeft:c+1,marginTop:d+1})});this.inverted&&o(b.childNodes,function(c){a.invertChild(c,b)});if(b.tagName===\"SPAN\"){var k=this.rotation,\n"
"m,l=F(this.textWidth),n=[k,g,b.innerHTML,this.textWidth,this.textAlign].join(\",\");if(n!==this.cTT){m=a.fontMetrics(b.style.fontSize).b;s(k)&&this.setSpanRotation(k,h,m);i=p(this.elemWidth,b.offsetWidth);if(i>l&&/[ \\-]/.test(b.textContent||b.innerText))L(b,{width:l+\"px\",display:\"block\",whiteSpace:j&&j.whiteSpace||\"normal\"}),i=l;this.getSpanCorrection(i,m,h,k,g)}L(b,{left:e+(this.xCorr||0)+\"px\",top:f+(this.yCorr||0)+\"px\"});if(fb)m=b.offsetHeight;this.cTT=n}}else this.alignOnAdd=!0},setSpanRotation:function(a,\n"
"b,c){var d={},e=xa?\"-ms-transform\":fb?\"-webkit-transform\":Ja?\"MozTransform\":Ib?\"-o-transform\":\"\";d[e]=d.transform=\"rotate(\"+a+\"deg)\";d[e+(Ja?\"Origin\":\"-origin\")]=d.transformOrigin=b*100+\"% \"+c+\"px\";L(this.element,d)},getSpanCorrection:function(a,b,c){this.xCorr=-a*c;this.yCorr=-b}});r(ua.prototype,{html:function(a,b,c){var d=this.createElement(\"span\"),e=d.element,f=d.renderer;d.textSetter=function(a){a!==e.innerHTML&&delete this.bBox;e.innerHTML=this.textStr=a;d.htmlUpdateTransform()};d.xSetter=d.ySetter=\n"
"d.alignSetter=d.rotationSetter=function(a,b){b===\"align\"&&(b=\"textAlign\");d[b]=a;d.htmlUpdateTransform()};d.attr({text:a,x:w(b),y:w(c)}).css({position:\"absolute\",fontFamily:this.style.fontFamily,fontSize:this.style.fontSize});e.style.whiteSpace=\"nowrap\";d.css=d.htmlCss;if(f.isSVG)d.add=function(a){var b,c=f.box.parentNode,j=[];if(this.parentGroup=a){if(b=a.div,!b){for(;a;)j.push(a),a=a.parentGroup;o(j.reverse(),function(a){var d,e=J(a.element,\"class\");e&&(e={className:e});b=a.div=a.div||$(Ia,e,{position:\"absolute\",\n"
"left:(a.translateX||0)+\"px\",top:(a.translateY||0)+\"px\"},b||c);d=b.style;r(a,{translateXSetter:function(b,c){d.left=b+\"px\";a[c]=b;a.doTransform=!0},translateYSetter:function(b,c){d.top=b+\"px\";a[c]=b;a.doTransform=!0}});Sa(a,\"visibilitySetter\",function(a,b,c,e){a.call(this,b,c,e);d[c]=b})})}}else b=c;b.appendChild(e);d.added=!0;d.alignOnAdd&&d.htmlUpdateTransform();return d};return d}});if(!ca&&!fa){D={init:function(a,b){var c=[\"<\",b,' filled=\"f\" stroked=\"f\"'],d=[\"position: \",\"absolute\",\";\"],e=b===\n"
"Ia;(b===\"shape\"||e)&&d.push(\"left:0;top:0;width:1px;height:1px;\");d.push(\"visibility: \",e?\"hidden\":\"visible\");c.push(' style=\"',d.join(\"\"),'\"/>');if(b)c=e||b===\"span\"||b===\"img\"?c.join(\"\"):a.prepVML(c),this.element=$(c);this.renderer=a},add:function(a){var b=this.renderer,c=this.element,d=b.box,d=a?a.element||a:d;a&&a.inverted&&b.invertChild(c,d);d.appendChild(c);this.added=!0;this.alignOnAdd&&!this.deferUpdateTransform&&this.updateTransform();if(this.onAdd)this.onAdd();return this},updateTransform:P.prototype.htmlUpdateTransform,\n"
"setSpanRotation:function(){var a=this.rotation,b=X(a*ga),c=aa(a*ga);L(this.element,{filter:a?[\"progid:DXImageTransform.Microsoft.Matrix(M11=\",b,\", M12=\",-c,\", M21=\",c,\", M22=\",b,\", sizingMethod='auto expand')\"].join(\"\"):O})},getSpanCorrection:function(a,b,c,d,e){var f=d?X(d*ga):1,g=d?aa(d*ga):0,h=p(this.elemHeight,this.element.offsetHeight),i;this.xCorr=f<0&&-a;this.yCorr=g<0&&-h;i=f*g<0;this.xCorr+=g*b*(i?1-c:c);this.yCorr-=f*b*(d?i?c:1-c:1);e&&e!==\"left\"&&(this.xCorr-=a*c*(f<0?-1:1),d&&(this.yCorr-=\n"
"h*c*(g<0?-1:1)),L(this.element,{textAlign:e}))},pathToVML:function(a){for(var b=a.length,c=[];b--;)if(qa(a[b]))c[b]=w(a[b]*10)-5;else if(a[b]===\"Z\")c[b]=\"x\";else if(c[b]=a[b],a.isArc&&(a[b]===\"wa\"||a[b]===\"at\"))c[b+5]===c[b+7]&&(c[b+7]+=a[b+7]>a[b+5]?1:-1),c[b+6]===c[b+8]&&(c[b+8]+=a[b+8]>a[b+6]?1:-1);return c.join(\" \")||\"x\"},clip:function(a){var b=this,c;a?(c=a.members,ja(c,b),c.push(b),b.destroyClip=function(){ja(c,b)},a=a.getCSS(b)):(b.destroyClip&&b.destroyClip(),a={clip:eb?\"inherit\":\"rect(auto)\"});\n"
"return b.css(a)},css:P.prototype.htmlCss,safeRemoveChild:function(a){a.parentNode&&Pa(a)},destroy:function(){this.destroyClip&&this.destroyClip();return P.prototype.destroy.apply(this)},on:function(a,b){this.element[\"on\"+a]=function(){var a=K.event;a.target=a.srcElement;b(a)};return this},cutOffPath:function(a,b){var c,a=a.split(/[ ,]/);c=a.length;if(c===9||c===11)a[c-4]=a[c-2]=F(a[c-2])-10*b;return a.join(\" \")},shadow:function(a,b,c){var d=[],e,f=this.element,g=this.renderer,h,i=f.style,j,k=f.path,\n"
"m,l,n,q;k&&typeof k.value!==\"string\"&&(k=\"x\");l=k;if(a){n=p(a.width,3);q=(a.opacity||0.15)/n;for(e=1;e<=3;e++){m=n*2+1-2*e;c&&(l=this.cutOffPath(k.value,m+0.5));j=['<shape isShadow=\"true\" strokeweight=\"',m,'\" filled=\"false\" path=\"',l,'\" coordsize=\"10 10\" style=\"',f.style.cssText,'\" />'];h=$(g.prepVML(j),null,{left:F(i.left)+p(a.offsetX,1),top:F(i.top)+p(a.offsetY,1)});if(c)h.cutOff=m+1;j=['<stroke color=\"',a.color||\"black\",'\" opacity=\"',q*e,'\"/>'];$(g.prepVML(j),null,null,h);b?b.element.appendChild(h):\n"
"f.parentNode.insertBefore(h,f);d.push(h)}this.shadows=d}return this},updateShadows:ta,setAttr:function(a,b){eb?this.element[a]=b:this.element.setAttribute(a,b)},classSetter:function(a){this.element.className=a},dashstyleSetter:function(a,b,c){(c.getElementsByTagName(\"stroke\")[0]||$(this.renderer.prepVML([\"<stroke/>\"]),null,null,c))[b]=a||\"solid\";this[b]=a},dSetter:function(a,b,c){var d=this.shadows,a=a||[];this.d=a.join&&a.join(\" \");c.path=a=this.pathToVML(a);if(d)for(c=d.length;c--;)d[c].path=d[c].cutOff?\n"
"this.cutOffPath(a,d[c].cutOff):a;this.setAttr(b,a)},fillSetter:function(a,b,c){var d=c.nodeName;if(d===\"SPAN\")c.style.color=a;else if(d!==\"IMG\")c.filled=a!==O,this.setAttr(\"fillcolor\",this.renderer.color(a,c,b,this))},opacitySetter:ta,rotationSetter:function(a,b,c){c=c.style;this[b]=c[b]=a;c.left=-w(aa(a*ga)+1)+\"px\";c.top=w(X(a*ga))+\"px\"},strokeSetter:function(a,b,c){this.setAttr(\"strokecolor\",this.renderer.color(a,c,b))},\"stroke-widthSetter\":function(a,b,c){c.stroked=!!a;this[b]=a;qa(a)&&(a+=\"px\");\n"
"this.setAttr(\"strokeweight\",a)},titleSetter:function(a,b){this.setAttr(b,a)},visibilitySetter:function(a,b,c){a===\"inherit\"&&(a=\"visible\");this.shadows&&o(this.shadows,function(c){c.style[b]=a});c.nodeName===\"DIV\"&&(a=a===\"hidden\"?\"-999em\":0,eb||(c.style[b]=a?\"visible\":\"hidden\"),b=\"top\");c.style[b]=a},xSetter:function(a,b,c){this[b]=a;b===\"x\"?b=\"left\":b===\"y\"&&(b=\"top\");this.updateClipping?(this[b]=a,this.updateClipping()):c.style[b]=a},zIndexSetter:function(a,b,c){c.style[b]=a}};B.VMLElement=D=ka(P,\n"
"D);D.prototype.ySetter=D.prototype.widthSetter=D.prototype.heightSetter=D.prototype.xSetter;var La={Element:D,isIE8:za.indexOf(\"MSIE 8.0\")>-1,init:function(a,b,c,d){var e;this.alignedObjects=[];d=this.createElement(Ia).css(r(this.getStyle(d),{position:\"relative\"}));e=d.element;a.appendChild(d.element);this.isVML=!0;this.box=e;this.boxWrapper=d;this.cache={};this.setSize(b,c,!1);if(!C.namespaces.hcv){C.namespaces.add(\"hcv\",\"urn:schemas-microsoft-com:vml\");try{C.createStyleSheet().cssText=\"hcv\\\\:fill, hcv\\\\:path, hcv\\\\:shape, hcv\\\\:stroke{ behavior:url(#default#VML); display: inline-block; } \"}catch(f){C.styleSheets[0].cssText+=\n"
"\"hcv\\\\:fill, hcv\\\\:path, hcv\\\\:shape, hcv\\\\:stroke{ behavior:url(#default#VML); display: inline-block; } \"}}},isHidden:function(){return!this.box.offsetWidth},clipRect:function(a,b,c,d){var e=this.createElement(),f=da(a);return r(e,{members:[],count:0,left:(f?a.x:a)+1,top:(f?a.y:b)+1,width:(f?a.width:c)-1,height:(f?a.height:d)-1,getCSS:function(a){var b=a.element,c=b.nodeName,a=a.inverted,d=this.top-(c===\"shape\"?b.offsetTop:0),e=this.left,b=e+this.width,f=d+this.height,d={clip:\"rect(\"+w(a?e:d)+\"px,\"+\n"
"w(a?f:b)+\"px,\"+w(a?b:f)+\"px,\"+w(a?d:e)+\"px)\"};!a&&eb&&c===\"DIV\"&&r(d,{width:b+\"px\",height:f+\"px\"});return d},updateClipping:function(){o(e.members,function(a){a.element&&a.css(e.getCSS(a))})}})},color:function(a,b,c,d){var e=this,f,g=/^rgba/,h,i,j=O;a&&a.linearGradient?i=\"gradient\":a&&a.radialGradient&&(i=\"pattern\");if(i){var k,m,l=a.linearGradient||a.radialGradient,n,q,x,p,Q,u=\"\",a=a.stops,y,t=[],s=function(){h=['<fill colors=\"'+t.join(\",\")+'\" opacity=\"',x,'\" o:opacity2=\"',q,'\" type=\"',i,'\" ',u,\n"
,
"'focus=\"100%\" method=\"any\" />'];$(e.prepVML(h),null,null,b)};n=a[0];y=a[a.length-1];n[0]>0&&a.unshift([0,n[1]]);y[0]<1&&a.push([1,y[1]]);o(a,function(a,b){g.test(a[1])?(f=na(a[1]),k=f.get(\"rgb\"),m=f.get(\"a\")):(k=a[1],m=1);t.push(a[0]*100+\"% \"+k);b?(x=m,p=k):(q=m,Q=k)});if(c===\"fill\")if(i===\"gradient\")c=l.x1||l[0]||0,a=l.y1||l[1]||0,n=l.x2||l[2]||0,l=l.y2||l[3]||0,u='angle=\"'+(90-W.atan((l-a)/(n-c))*180/ma)+'\"',s();else{var j=l.r,v=j*2,w=j*2,r=l.cx,z=l.cy,A=b.radialReference,C,j=function(){A&&(C=d.getBBox(),\n"
"r+=(A[0]-C.x)/C.width-0.5,z+=(A[1]-C.y)/C.height-0.5,v*=A[2]/C.width,w*=A[2]/C.height);u='src=\"'+S.global.VMLRadialGradientURL+'\" size=\"'+v+\",\"+w+'\" origin=\"0.5,0.5\" position=\"'+r+\",\"+z+'\" color2=\"'+Q+'\" ';s()};d.added?j():d.onAdd=j;j=p}else j=k}else if(g.test(a)&&b.tagName!==\"IMG\")f=na(a),h=[\"<\",c,' opacity=\"',f.get(\"a\"),'\"/>'],$(this.prepVML(h),null,null,b),j=f.get(\"rgb\");else{j=b.getElementsByTagName(c);if(j.length)j[0].opacity=1,j[0].type=\"solid\";j=a}return j},prepVML:function(a){var b=this.isIE8,\n"
"a=a.join(\"\");b?(a=a.replace(\"/>\",' xmlns=\"urn:schemas-microsoft-com:vml\" />'),a=a.indexOf('style=\"')===-1?a.replace(\"/>\",' style=\"display:inline-block;behavior:url(#default#VML);\" />'):a.replace('style=\"','style=\"display:inline-block;behavior:url(#default#VML);')):a=a.replace(\"<\",\"<hcv:\");return a},text:ua.prototype.html,path:function(a){var b={coordsize:\"10 10\"};Fa(a)?b.d=a:da(a)&&r(b,a);return this.createElement(\"shape\").attr(b)},circle:function(a,b,c){var d=this.symbol(\"circle\");if(da(a))c=a.r,\n"
"b=a.y,a=a.x;d.isCircle=!0;d.r=c;return d.attr({x:a,y:b})},g:function(a){var b;a&&(b={className:\"highcharts-\"+a,\"class\":\"highcharts-\"+a});return this.createElement(Ia).attr(b)},image:function(a,b,c,d,e){var f=this.createElement(\"img\").attr({src:a});arguments.length>1&&f.attr({x:b,y:c,width:d,height:e});return f},createElement:function(a){return a===\"rect\"?this.symbol(a):ua.prototype.createElement.call(this,a)},invertChild:function(a,b){var c=this,d=b.style,e=a.tagName===\"IMG\"&&a.style;L(a,{flip:\"x\",\n"
"left:F(d.width)-(e?F(e.top):1),top:F(d.height)-(e?F(e.left):1),rotation:-90});o(a.childNodes,function(b){c.invertChild(b,a)})},symbols:{arc:function(a,b,c,d,e){var f=e.start,g=e.end,h=e.r||c||d,c=e.innerR,d=X(f),i=aa(f),j=X(g),k=aa(g);if(g-f===0)return[\"x\"];f=[\"wa\",a-h,b-h,a+h,b+h,a+h*d,b+h*i,a+h*j,b+h*k];e.open&&!c&&f.push(\"e\",\"M\",a,b);f.push(\"at\",a-c,b-c,a+c,b+c,a+c*j,b+c*k,a+c*d,b+c*i,\"x\",\"e\");f.isArc=!0;return f},circle:function(a,b,c,d,e){e&&(c=d=2*e.r);e&&e.isCircle&&(a-=c/2,b-=d/2);return[\"wa\",\n"
"a,b,a+c,b+d,a+c,b+d/2,a+c,b+d/2,\"e\"]},rect:function(a,b,c,d,e){return ua.prototype.symbols[!s(e)||!e.r?\"square\":\"callout\"].call(0,a,b,c,d,e)}}};B.VMLRenderer=D=function(){this.init.apply(this,arguments)};D.prototype=A(ua.prototype,La);Za=D}ua.prototype.measureSpanWidth=function(a,b){var c=C.createElement(\"span\"),d;d=C.createTextNode(a);c.appendChild(d);L(c,b);this.box.appendChild(c);d=c.offsetWidth;Pa(c);return d};var Lb;if(fa)B.CanVGRenderer=D=function(){Da=\"http://www.w3.org/1999/xhtml\"},D.prototype.symbols=\n"
"{},Lb=function(){function a(){var a=b.length,d;for(d=0;d<a;d++)b[d]();b=[]}var b=[];return{push:function(c,d){b.length===0&&Qb(d,a);b.push(c)}}}(),Za=D;Ra.prototype={addLabel:function(){var a=this.axis,b=a.options,c=a.chart,d=a.categories,e=a.names,f=this.pos,g=b.labels,h=a.tickPositions,i=f===h[0],j=f===h[h.length-1],e=d?p(d[f],e[f],f):f,d=this.label,h=h.info,k;a.isDatetimeAxis&&h&&(k=b.dateTimeLabelFormats[h.higherRanks[f]||h.unitName]);this.isFirst=i;this.isLast=j;b=a.labelFormatter.call({axis:a,\n"
"chart:c,isFirst:i,isLast:j,dateTimeLabelFormat:k,value:a.isLog?ea(ia(e)):e});s(d)?d&&d.attr({text:b}):(this.labelLength=(this.label=d=s(b)&&g.enabled?c.renderer.text(b,0,0,g.useHTML).css(A(g.style)).add(a.labelGroup):null)&&d.getBBox().width,this.rotation=0)},getLabelSize:function(){return this.label?this.label.getBBox()[this.axis.horiz?\"height\":\"width\"]:0},handleOverflow:function(a){var b=this.axis,c=a.x,d=b.chart.chartWidth,e=b.chart.spacing,f=p(b.labelLeft,z(b.pos,e[3])),e=p(b.labelRight,t(b.pos+\n"
"b.len,d-e[1])),g=this.label,h=this.rotation,i={left:0,center:0.5,right:1}[b.labelAlign],j=g.getBBox().width,k=b.slotWidth,m=1,l,n={};if(h)h<0&&c-i*j<f?l=w(c/X(h*ga)-f):h>0&&c+i*j>e&&(l=w((d-c)/X(h*ga)));else if(d=c+(1-i)*j,c-i*j<f?k=a.x+k*(1-i)-f:d>e&&(k=e-a.x+k*i,m=-1),k=z(b.slotWidth,k),k<b.slotWidth&&b.labelAlign===\"center\"&&(a.x+=m*(b.slotWidth-k-i*(b.slotWidth-z(j,k)))),j>k||b.autoRotation&&g.styles.width)l=k;if(l){n.width=l;if(!b.options.labels.style.textOverflow)n.textOverflow=\"ellipsis\";g.css(n)}},\n"
"getPosition:function(a,b,c,d){var e=this.axis,f=e.chart,g=d&&f.oldChartHeight||f.chartHeight;return{x:a?e.translate(b+c,null,null,d)+e.transB:e.left+e.offset+(e.opposite?(d&&f.oldChartWidth||f.chartWidth)-e.right-e.left:0),y:a?g-e.bottom+e.offset-(e.opposite?e.height:0):g-e.translate(b+c,null,null,d)-e.transB}},getLabelPosition:function(a,b,c,d,e,f,g,h){var i=this.axis,j=i.transA,k=i.reversed,m=i.staggerLines,l=i.tickRotCorr||{x:0,y:0},c=p(e.y,l.y+(i.side===2?8:-(c.getBBox().height/2))),a=a+e.x+l.x-\n"
"(f&&d?f*j*(k?-1:1):0),b=b+c-(f&&!d?f*j*(k?1:-1):0);m&&(b+=g/(h||1)%m*(i.labelOffset/m));return{x:a,y:w(b)}},getMarkPath:function(a,b,c,d,e,f){return f.crispLine([\"M\",a,b,\"L\",a+(e?0:-c),b+(e?c:0)],d)},render:function(a,b,c){var d=this.axis,e=d.options,f=d.chart.renderer,g=d.horiz,h=this.type,i=this.label,j=this.pos,k=e.labels,m=this.gridLine,l=h?h+\"Grid\":\"grid\",n=h?h+\"Tick\":\"tick\",q=e[l+\"LineWidth\"],x=e[l+\"LineColor\"],o=e[l+\"LineDashStyle\"],Q=e[n+\"Length\"],l=p(e[n+\"Width\"],!h&&d.isXAxis?1:0),u=e[n+\n"
"\"Color\"],y=e[n+\"Position\"],n=this.mark,t=k.step,s=!0,w=d.tickmarkOffset,r=this.getPosition(g,j,w,b),z=r.x,r=r.y,C=g&&z===d.pos+d.len||!g&&r===d.pos?-1:1,c=p(c,1);this.isActive=!0;if(q){j=d.getPlotLinePath(j+w,q*C,b,!0);if(m===v){m={stroke:x,\"stroke-width\":q};if(o)m.dashstyle=o;if(!h)m.zIndex=1;if(b)m.opacity=0;this.gridLine=m=q?f.path(j).attr(m).add(d.gridGroup):null}if(!b&&m&&j)m[this.isNew?\"attr\":\"animate\"]({d:j,opacity:c})}if(l&&Q)y===\"inside\"&&(Q=-Q),d.opposite&&(Q=-Q),h=this.getMarkPath(z,r,\n"
"Q,l*C,g,f),n?n.animate({d:h,opacity:c}):this.mark=f.path(h).attr({stroke:u,\"stroke-width\":l,opacity:c}).add(d.axisGroup);if(i&&!isNaN(z))i.xy=r=this.getLabelPosition(z,r,i,g,k,w,a,t),this.isFirst&&!this.isLast&&!p(e.showFirstLabel,1)||this.isLast&&!this.isFirst&&!p(e.showLastLabel,1)?s=!1:g&&!d.isRadial&&!k.step&&!k.rotation&&!b&&c!==0&&this.handleOverflow(r),t&&a%t&&(s=!1),s&&!isNaN(r.y)?(r.opacity=c,i[this.isNew?\"attr\":\"animate\"](r),this.isNew=!1):i.attr(\"y\",-9999)},destroy:function(){Oa(this,this.axis)}};\n"
"B.PlotLineOrBand=function(a,b){this.axis=a;if(b)this.options=b,this.id=b.id};B.PlotLineOrBand.prototype={render:function(){var a=this,b=a.axis,c=b.horiz,d=a.options,e=d.label,f=a.label,g=d.width,h=d.to,i=d.from,j=s(i)&&s(h),k=d.value,m=d.dashStyle,l=a.svgElem,n=[],q,x=d.color,p=d.zIndex,o=d.events,u={},y=b.chart.renderer;b.isLog&&(i=Ba(i),h=Ba(h),k=Ba(k));if(g){if(n=b.getPlotLinePath(k,g),u={stroke:x,\"stroke-width\":g},m)u.dashstyle=m}else if(j){n=b.getPlotBandPath(i,h,d);if(x)u.fill=x;if(d.borderWidth)u.stroke=\n"
"d.borderColor,u[\"stroke-width\"]=d.borderWidth}else return;if(s(p))u.zIndex=p;if(l)if(n)l.animate({d:n},null,l.onGetPath);else{if(l.hide(),l.onGetPath=function(){l.show()},f)a.label=f=f.destroy()}else if(n&&n.length&&(a.svgElem=l=y.path(n).attr(u).add(),o))for(q in d=function(b){l.on(b,function(c){o[b].apply(a,[c])})},o)d(q);if(e&&s(e.text)&&n&&n.length&&b.width>0&&b.height>0){e=A({align:c&&j&&\"center\",x:c?!j&&4:10,verticalAlign:!c&&j&&\"middle\",y:c?j?16:10:j?6:-4,rotation:c&&!j&&90},e);if(!f){u={align:e.textAlign||\n"
"e.align,rotation:e.rotation};if(s(p))u.zIndex=p;a.label=f=y.text(e.text,0,0,e.useHTML).attr(u).css(e.style).add()}b=[n[1],n[4],j?n[6]:n[1]];j=[n[2],n[5],j?n[7]:n[2]];n=Na(b);c=Na(j);f.align(e,!1,{x:n,y:c,width:Ca(b)-n,height:Ca(j)-c});f.show()}else f&&f.hide();return a},destroy:function(){ja(this.axis.plotLinesAndBands,this);delete this.axis;Oa(this)}};var ha=B.Axis=function(){this.init.apply(this,arguments)};ha.prototype={defaultOptions:{dateTimeLabelFormats:{millisecond:\"%H:%M:%S.%L\",second:\"%H:%M:%S\",\n"
"minute:\"%H:%M\",hour:\"%H:%M\",day:\"%e. %b\",week:\"%e. %b\",month:\"%b '%y\",year:\"%Y\"},endOnTick:!1,gridLineColor:\"#D8D8D8\",labels:{enabled:!0,style:{color:\"#606060\",cursor:\"default\",fontSize:\"11px\"},x:0,y:15},lineColor:\"#C0D0E0\",lineWidth:1,minPadding:0.01,maxPadding:0.01,minorGridLineColor:\"#E0E0E0\",minorGridLineWidth:1,minorTickColor:\"#A0A0A0\",minorTickLength:2,minorTickPosition:\"outside\",startOfWeek:1,startOnTick:!1,tickColor:\"#C0D0E0\",tickLength:10,tickmarkPlacement:\"between\",tickPixelInterval:100,\n"
"tickPosition:\"outside\",title:{align:\"middle\",style:{color:\"#707070\"}},type:\"linear\"},defaultYAxisOptions:{endOnTick:!0,gridLineWidth:1,tickPixelInterval:72,showLastLabel:!0,labels:{x:-8,y:3},lineWidth:0,maxPadding:0.05,minPadding:0.05,startOnTick:!0,title:{rotation:270,text:\"Values\"},stackLabels:{enabled:!1,formatter:function(){return B.numberFormat(this.total,-1)},style:A(ba.line.dataLabels.style,{color:\"#000000\"})}},defaultLeftAxisOptions:{labels:{x:-15,y:null},title:{rotation:270}},defaultRightAxisOptions:{labels:{x:15,\n"
"y:null},title:{rotation:90}},defaultBottomAxisOptions:{labels:{autoRotation:[-45],x:0,y:null},title:{rotation:0}},defaultTopAxisOptions:{labels:{autoRotation:[-45],x:0,y:-15},title:{rotation:0}},init:function(a,b){var c=b.isX;this.chart=a;this.horiz=a.inverted?!c:c;this.coll=(this.isXAxis=c)?\"xAxis\":\"yAxis\";this.opposite=b.opposite;this.side=b.side||(this.horiz?this.opposite?0:2:this.opposite?1:3);this.setOptions(b);var d=this.options,e=d.type;this.labelFormatter=d.labels.formatter||this.defaultLabelFormatter;\n"
,
"this.userOptions=b;this.minPixelPadding=0;this.reversed=d.reversed;this.zoomEnabled=d.zoomEnabled!==!1;this.categories=d.categories||e===\"category\";this.names=this.names||[];this.isLog=e===\"logarithmic\";this.isDatetimeAxis=e===\"datetime\";this.isLinked=s(d.linkedTo);this.ticks={};this.labelEdge=[];this.minorTicks={};this.plotLinesAndBands=[];this.alternateBands={};this.len=0;this.minRange=this.userMinRange=d.minRange||d.maxZoom;this.range=d.range;this.offset=d.offset||0;this.stacks={};this.oldStacks=\n"
"{};this.stacksTouched=0;this.min=this.max=null;this.crosshair=p(d.crosshair,ra(a.options.tooltip.crosshairs)[c?0:1],!1);var f,d=this.options.events;Ka(this,a.axes)===-1&&(c&&!this.isColorAxis?a.axes.splice(a.xAxis.length,0,this):a.axes.push(this),a[this.coll].push(this));this.series=this.series||[];if(a.inverted&&c&&this.reversed===v)this.reversed=!0;this.removePlotLine=this.removePlotBand=this.removePlotBandOrLine;for(f in d)H(this,f,d[f]);if(this.isLog)this.val2lin=Ba,this.lin2val=ia},setOptions:function(a){this.options=\n"
"A(this.defaultOptions,this.isXAxis?{}:this.defaultYAxisOptions,[this.defaultTopAxisOptions,this.defaultRightAxisOptions,this.defaultBottomAxisOptions,this.defaultLeftAxisOptions][this.side],A(S[this.coll],a))},defaultLabelFormatter:function(){var a=this.axis,b=this.value,c=a.categories,d=this.dateTimeLabelFormat,e=S.lang.numericSymbols,f=e&&e.length,g,h=a.options.labels.format,a=a.isLog?b:a.tickInterval;if(h)g=Ha(h,this);else if(c)g=b;else if(d)g=Ma(d,b);else if(f&&a>=1E3)for(;f--&&g===v;)c=Math.pow(1E3,\n"
"f+1),a>=c&&b*10%c===0&&e[f]!==null&&(g=B.numberFormat(b/c,-1)+e[f]);g===v&&(g=N(b)>=1E4?B.numberFormat(b,-1):B.numberFormat(b,-1,v,\"\"));return g},getSeriesExtremes:function(){var a=this,b=a.chart;a.hasVisibleSeries=!1;a.dataMin=a.dataMax=a.ignoreMinPadding=a.ignoreMaxPadding=null;a.buildStacks&&a.buildStacks();o(a.series,function(c){if(c.visible||!b.options.chart.ignoreHiddenSeries){var d;d=c.options.threshold;var e;a.hasVisibleSeries=!0;a.isLog&&d<=0&&(d=null);if(a.isXAxis){if(d=c.xData,d.length)a.dataMin=\n"
"z(p(a.dataMin,d[0]),Na(d)),a.dataMax=t(p(a.dataMax,d[0]),Ca(d))}else{c.getExtremes();e=c.dataMax;c=c.dataMin;if(s(c)&&s(e))a.dataMin=z(p(a.dataMin,c),c),a.dataMax=t(p(a.dataMax,e),e);if(s(d))if(a.dataMin>=d)a.dataMin=d,a.ignoreMinPadding=!0;else if(a.dataMax<d)a.dataMax=d,a.ignoreMaxPadding=!0}}})},translate:function(a,b,c,d,e,f){var g=this.linkedParent||this,h=1,i=0,j=d?g.oldTransA:g.transA,d=d?g.oldMin:g.min,k=g.minPixelPadding,e=(g.doPostTranslate||g.isLog&&e)&&g.lin2val;if(!j)j=g.transA;if(c)h*=\n"
"-1,i=g.len;g.reversed&&(h*=-1,i-=h*(g.sector||g.len));b?(a=a*h+i,a-=k,a=a/j+d,e&&(a=g.lin2val(a))):(e&&(a=g.val2lin(a)),f===\"between\"&&(f=0.5),a=h*(a-d)*j+i+h*k+(qa(f)?j*f*g.pointRange:0));return a},toPixels:function(a,b){return this.translate(a,!1,!this.horiz,null,!0)+(b?0:this.pos)},toValue:function(a,b){return this.translate(a-(b?0:this.pos),!0,!this.horiz,null,!0)},getPlotLinePath:function(a,b,c,d,e){var f=this.chart,g=this.left,h=this.top,i,j,k=c&&f.oldChartHeight||f.chartHeight,m=c&&f.oldChartWidth||\n"
"f.chartWidth,l;i=this.transB;var n=function(a,b,c){if(a<b||a>c)d?a=z(t(b,a),c):l=!0;return a},e=p(e,this.translate(a,null,null,c)),a=c=w(e+i);i=j=w(k-e-i);isNaN(e)?l=!0:this.horiz?(i=h,j=k-this.bottom,a=c=n(a,g,g+this.width)):(a=g,c=m-this.right,i=j=n(i,h,h+this.height));return l&&!d?null:f.renderer.crispLine([\"M\",a,i,\"L\",c,j],b||1)},getLinearTickPositions:function(a,b,c){var d,e=ea(V(b/a)*a),f=ea(sa(c/a)*a),g=[];if(b===c&&qa(b))return[b];for(b=e;b<=f;){g.push(b);b=ea(b+a);if(b===d)break;d=b}return g},\n"
"getMinorTickPositions:function(){var a=this.options,b=this.tickPositions,c=this.minorTickInterval,d=[],e,f=this.pointRangePadding||0;e=this.min-f;var f=this.max+f,g=f-e;if(g&&g/c<this.len/3)if(this.isLog){f=b.length;for(e=1;e<f;e++)d=d.concat(this.getLogTickPositions(c,b[e-1],b[e],!0))}else if(this.isDatetimeAxis&&a.minorTickInterval===\"auto\")d=d.concat(this.getTimeTicks(this.normalizeTimeTickInterval(c),e,f,a.startOfWeek));else for(b=e+(b[0]-e)%c;b<=f;b+=c)d.push(b);d.length!==0&&this.trimTicks(d,\n"
"a.startOnTick,a.endOnTick);return d},adjustForMinRange:function(){var a=this.options,b=this.min,c=this.max,d,e=this.dataMax-this.dataMin>=this.minRange,f,g,h,i,j;if(this.isXAxis&&this.minRange===v&&!this.isLog)s(a.min)||s(a.max)?this.minRange=null:(o(this.series,function(a){i=a.xData;for(g=j=a.xIncrement?1:i.length-1;g>0;g--)if(h=i[g]-i[g-1],f===v||h<f)f=h}),this.minRange=z(f*5,this.dataMax-this.dataMin));if(c-b<this.minRange){var k=this.minRange;d=(k-c+b)/2;d=[b-d,p(a.min,b-d)];if(e)d[2]=this.dataMin;\n"
"b=Ca(d);c=[b+k,p(a.max,b+k)];if(e)c[2]=this.dataMax;c=Na(c);c-b<k&&(d[0]=c-k,d[1]=p(a.min,c-k),b=Ca(d))}this.min=b;this.max=c},setAxisTranslation:function(a){var b=this,c=b.max-b.min,d=b.axisPointRange||0,e,f=0,g=0,h=b.linkedParent,i=!!b.categories,j=b.transA,k=b.isXAxis;if(k||i||d)if(h?(f=h.minPointOffset,g=h.pointRangePadding):o(b.series,function(a){var h=i?1:k?a.pointRange:b.axisPointRange||0,j=a.options.pointPlacement,q=a.closestPointRange;h>c&&(h=0);d=t(d,h);b.single||(f=t(f,Aa(j)?0:h/2),g=t(g,\n"
"j===\"on\"?0:h));!a.noSharedTooltip&&s(q)&&(e=s(e)?z(e,q):q)}),h=b.ordinalSlope&&e?b.ordinalSlope/e:1,b.minPointOffset=f*=h,b.pointRangePadding=g*=h,b.pointRange=z(d,c),k)b.closestPointRange=e;if(a)b.oldTransA=j;b.translationSlope=b.transA=j=b.len/(c+g||1);b.transB=b.horiz?b.left:b.bottom;b.minPixelPadding=j*f},minFromRange:function(){return this.max-this.range},setTickInterval:function(a){var b=this,c=b.chart,d=b.options,e=b.isLog,f=b.isDatetimeAxis,g=b.isXAxis,h=b.isLinked,i=d.maxPadding,j=d.minPadding,\n"
"k=d.tickInterval,m=d.tickPixelInterval,l=b.categories;!f&&!l&&!h&&this.getTickAmount();h?(b.linkedParent=c[b.coll][d.linkedTo],c=b.linkedParent.getExtremes(),b.min=p(c.min,c.dataMin),b.max=p(c.max,c.dataMax),d.type!==b.linkedParent.options.type&&la(11,1)):(b.min=p(b.userMin,d.min,b.dataMin),b.max=p(b.userMax,d.max,b.dataMax));if(e)!a&&z(b.min,p(b.dataMin,b.min))<=0&&la(10,1),b.min=ea(Ba(b.min),15),b.max=ea(Ba(b.max),15);if(b.range&&s(b.max))b.userMin=b.min=t(b.min,b.minFromRange()),b.userMax=b.max,\n"
"b.range=null;b.beforePadding&&b.beforePadding();b.adjustForMinRange();if(!l&&!b.axisPointRange&&!b.usePercentage&&!h&&s(b.min)&&s(b.max)&&(c=b.max-b.min)){if(!s(d.min)&&!s(b.userMin)&&j&&(b.dataMin<0||!b.ignoreMinPadding))b.min-=c*j;if(!s(d.max)&&!s(b.userMax)&&i&&(b.dataMax>0||!b.ignoreMaxPadding))b.max+=c*i}if(qa(d.floor))b.min=t(b.min,d.floor);if(qa(d.ceiling))b.max=z(b.max,d.ceiling);b.tickInterval=b.min===b.max||b.min===void 0||b.max===void 0?1:h&&!k&&m===b.linkedParent.options.tickPixelInterval?\n"
"k=b.linkedParent.tickInterval:p(k,this.tickAmount?(b.max-b.min)/t(this.tickAmount-1,1):void 0,l?1:(b.max-b.min)*m/t(b.len,m));g&&!a&&o(b.series,function(a){a.processData(b.min!==b.oldMin||b.max!==b.oldMax)});b.setAxisTranslation(!0);b.beforeSetTickPositions&&b.beforeSetTickPositions();if(b.postProcessTickInterval)b.tickInterval=b.postProcessTickInterval(b.tickInterval);if(b.pointRange)b.tickInterval=t(b.pointRange,b.tickInterval);a=p(d.minTickInterval,b.isDatetimeAxis&&b.closestPointRange);if(!k&&\n"
"b.tickInterval<a)b.tickInterval=a;if(!f&&!e&&!k)b.tickInterval=ob(b.tickInterval,null,nb(b.tickInterval),p(d.allowDecimals,!(b.tickInterval>0.5&&b.tickInterval<5&&b.max>1E3&&b.max<9999)),!!this.tickAmount);if(!this.tickAmount&&this.len)b.tickInterval=b.unsquish();this.setTickPositions()},setTickPositions:function(){var a=this.options,b,c=a.tickPositions,d=a.tickPositioner,e=a.startOnTick,f=a.endOnTick,g;this.tickmarkOffset=this.categories&&a.tickmarkPlacement===\"between\"&&this.tickInterval===1?0.5:\n"
"0;this.minorTickInterval=a.minorTickInterval===\"auto\"&&this.tickInterval?this.tickInterval/5:a.minorTickInterval;this.tickPositions=b=c&&c.slice();if(!b&&(b=this.isDatetimeAxis?this.getTimeTicks(this.normalizeTimeTickInterval(this.tickInterval,a.units),this.min,this.max,a.startOfWeek,this.ordinalPositions,this.closestPointRange,!0):this.isLog?this.getLogTickPositions(this.tickInterval,this.min,this.max):this.getLinearTickPositions(this.tickInterval,this.min,this.max),b.length>this.len&&(b=[b[0],b.pop()]),\n"
"this.tickPositions=b,d&&(d=d.apply(this,[this.min,this.max]))))this.tickPositions=b=d;if(!this.isLinked)this.trimTicks(b,e,f),this.min===this.max&&s(this.min)&&!this.tickAmount&&(g=!0,this.min-=0.5,this.max+=0.5),this.single=g,!c&&!d&&this.adjustTickAmount()},trimTicks:function(a,b,c){var d=a[0],e=a[a.length-1],f=this.minPointOffset||0;b?this.min=d:this.min-f>d&&a.shift();c?this.max=e:this.max+f<e&&a.pop();a.length===0&&s(d)&&a.push((e+d)/2)},getTickAmount:function(){var a={},b,c=this.options,d=c.tickAmount,\n"
"e=c.tickPixelInterval;!s(c.tickInterval)&&this.len<e&&!this.isRadial&&!this.isLog&&c.startOnTick&&c.endOnTick&&(d=2);!d&&this.chart.options.chart.alignTicks!==!1&&c.alignTicks!==!1&&(o(this.chart[this.coll],function(c){var d=c.options,e=c.horiz,d=[e?d.left:d.top,e?d.width:d.height,d.pane].join(\",\");a[d]?c.series.length&&(b=!0):a[d]=1}),b&&(d=sa(this.len/e)+1));if(d<4)this.finalTickAmt=d,d=5;this.tickAmount=d},adjustTickAmount:function(){var a=this.tickInterval,b=this.tickPositions,c=this.tickAmount,\n"
"d=this.finalTickAmt,e=b&&b.length;if(e<c){for(;b.length<c;)b.push(ea(b[b.length-1]+a));this.transA*=(e-1)/(c-1);this.max=b[b.length-1]}else e>c&&(this.tickInterval*=2,this.setTickPositions());if(s(d)){for(a=c=b.length;a--;)(d===3&&a%2===1||d<=2&&a>0&&a<c-1)&&b.splice(a,1);this.finalTickAmt=v}},setScale:function(){var a,b;this.oldMin=this.min;this.oldMax=this.max;this.oldAxisLength=this.len;this.setAxisSize();b=this.len!==this.oldAxisLength;o(this.series,function(b){if(b.isDirtyData||b.isDirty||b.xAxis.isDirty)a=\n"
,
"!0});if(b||a||this.isLinked||this.forceRedraw||this.userMin!==this.oldUserMin||this.userMax!==this.oldUserMax){if(this.resetStacks&&this.resetStacks(),this.forceRedraw=!1,this.getSeriesExtremes(),this.setTickInterval(),this.oldUserMin=this.userMin,this.oldUserMax=this.userMax,!this.isDirty)this.isDirty=b||this.min!==this.oldMin||this.max!==this.oldMax}else this.cleanStacks&&this.cleanStacks()},setExtremes:function(a,b,c,d,e){var f=this,g=f.chart,c=p(c,!0);o(f.series,function(a){delete a.kdTree});\n"
"e=r(e,{min:a,max:b});I(f,\"setExtremes\",e,function(){f.userMin=a;f.userMax=b;f.eventArgs=e;c&&g.redraw(d)})},zoom:function(a,b){var c=this.dataMin,d=this.dataMax,e=this.options,f=z(c,p(e.min,c)),e=t(d,p(e.max,d));this.allowZoomOutside||(s(c)&&a<=f&&(a=f),s(d)&&b>=e&&(b=e));this.displayBtn=a!==v||b!==v;this.setExtremes(a,b,!1,v,{trigger:\"zoom\"});return!0},setAxisSize:function(){var a=this.chart,b=this.options,c=b.offsetLeft||0,d=this.horiz,e=p(b.width,a.plotWidth-c+(b.offsetRight||0)),f=p(b.height,\n"
"a.plotHeight),g=p(b.top,a.plotTop),b=p(b.left,a.plotLeft+c),c=/%$/;c.test(f)&&(f=parseFloat(f)/100*a.plotHeight);c.test(g)&&(g=parseFloat(g)/100*a.plotHeight+a.plotTop);this.left=b;this.top=g;this.width=e;this.height=f;this.bottom=a.chartHeight-f-g;this.right=a.chartWidth-e-b;this.len=t(d?e:f,0);this.pos=d?b:g},getExtremes:function(){var a=this.isLog;return{min:a?ea(ia(this.min)):this.min,max:a?ea(ia(this.max)):this.max,dataMin:this.dataMin,dataMax:this.dataMax,userMin:this.userMin,userMax:this.userMax}},\n"
"getThreshold:function(a){var b=this.isLog,c=b?ia(this.min):this.min,b=b?ia(this.max):this.max;a===null?a=b<0?b:c:c>a?a=c:b<a&&(a=b);return this.translate(a,0,1,0,1)},autoLabelAlign:function(a){a=(p(a,0)-this.side*90+720)%360;return a>15&&a<165?\"right\":a>195&&a<345?\"left\":\"center\"},unsquish:function(){var a=this.ticks,b=this.options.labels,c=this.horiz,d=this.tickInterval,e=d,f=this.len/(((this.categories?1:0)+this.max-this.min)/d),g,h=b.rotation,i=this.chart.renderer.fontMetrics(b.style.fontSize,\n"
"a[0]&&a[0].label),j,k=Number.MAX_VALUE,m,l=function(a){a/=f||1;a=a>1?sa(a):1;return a*d};c?(m=s(h)?[h]:f<p(b.autoRotationLimit,80)&&!b.staggerLines&&!b.step&&b.autoRotation)&&o(m,function(a){var b;if(a===h||a&&a>=-90&&a<=90)j=l(N(i.h/aa(ga*a))),b=j+N(a/360),b<k&&(k=b,g=a,e=j)}):b.step||(e=l(i.h));this.autoRotation=m;this.labelRotation=g;return e},renderUnsquish:function(){var a=this.chart,b=a.renderer,c=this.tickPositions,d=this.ticks,e=this.options.labels,f=this.horiz,g=a.margin,h=this.categories?\n"
"c.length:c.length-1,i=this.slotWidth=f&&!e.step&&!e.rotation&&(this.staggerLines||1)*a.plotWidth/h||!f&&(g[3]&&g[3]-a.spacing[3]||a.chartWidth*0.33),j=t(1,w(i-2*(e.padding||5))),k={},g=b.fontMetrics(e.style.fontSize,d[0]&&d[0].label),h=e.style.textOverflow,m,l=0;if(!Aa(e.rotation))k.rotation=e.rotation||0;if(this.autoRotation)o(c,function(a){if((a=d[a])&&a.labelLength>l)l=a.labelLength}),l>j&&l>g.h?k.rotation=this.labelRotation:this.labelRotation=0;else if(i&&(m={width:j+\"px\"},!h)){m.textOverflow=\n"
"\"clip\";for(i=c.length;!f&&i--;)if(j=c[i],j=d[j].label)if(j.styles.textOverflow===\"ellipsis\"&&j.css({textOverflow:\"clip\"}),j.getBBox().height>this.len/c.length-(g.h-g.f))j.specCss={textOverflow:\"ellipsis\"}}if(k.rotation&&(m={width:(l>a.chartHeight*0.5?a.chartHeight*0.33:a.chartHeight)+\"px\"},!h))m.textOverflow=\"ellipsis\";this.labelAlign=k.align=e.align||this.autoLabelAlign(this.labelRotation);o(c,function(a){var b=(a=d[a])&&a.label;if(b)m&&b.css(A(m,b.specCss)),delete b.specCss,b.attr(k),a.rotation=\n"
"k.rotation});this.tickRotCorr=b.rotCorr(g.b,this.labelRotation||0,this.side===2)},hasData:function(){return this.hasVisibleSeries||s(this.min)&&s(this.max)&&!!this.tickPositions},getOffset:function(){var a=this,b=a.chart,c=b.renderer,d=a.options,e=a.tickPositions,f=a.ticks,g=a.horiz,h=a.side,i=b.inverted?[1,0,3,2][h]:h,j,k,m=0,l,n=0,q=d.title,x=d.labels,T=0,Q=b.axisOffset,b=b.clipOffset,u=[-1,1,1,-1][h],y;j=a.hasData();a.showAxis=k=j||p(d.showEmpty,!0);a.staggerLines=a.horiz&&x.staggerLines;if(!a.axisGroup)a.gridGroup=\n"
"c.g(\"grid\").attr({zIndex:d.gridZIndex||1}).add(),a.axisGroup=c.g(\"axis\").attr({zIndex:d.zIndex||2}).add(),a.labelGroup=c.g(\"axis-labels\").attr({zIndex:x.zIndex||7}).addClass(\"highcharts-\"+a.coll.toLowerCase()+\"-labels\").add();if(j||a.isLinked){if(o(e,function(b){f[b]?f[b].addLabel():f[b]=new Ra(a,b)}),a.renderUnsquish(),o(e,function(b){if(h===0||h===2||{1:\"left\",3:\"right\"}[h]===a.labelAlign)T=t(f[b].getLabelSize(),T)}),a.staggerLines)T*=a.staggerLines,a.labelOffset=T}else for(y in f)f[y].destroy(),\n"
"delete f[y];if(q&&q.text&&q.enabled!==!1){if(!a.axisTitle)a.axisTitle=c.text(q.text,0,0,q.useHTML).attr({zIndex:7,rotation:q.rotation||0,align:q.textAlign||{low:\"left\",middle:\"center\",high:\"right\"}[q.align]}).addClass(\"highcharts-\"+this.coll.toLowerCase()+\"-title\").css(q.style).add(a.axisGroup),a.axisTitle.isNew=!0;if(k)m=a.axisTitle.getBBox()[g?\"height\":\"width\"],l=q.offset,n=s(l)?0:p(q.margin,g?5:10);a.axisTitle[k?\"show\":\"hide\"]()}a.offset=u*p(d.offset,Q[h]);a.tickRotCorr=a.tickRotCorr||{x:0,y:0};\n"
"c=h===2?a.tickRotCorr.y:0;g=T+n+(T&&u*(g?p(x.y,a.tickRotCorr.y+8):x.x)-c);a.axisTitleMargin=p(l,g);Q[h]=t(Q[h],a.axisTitleMargin+m+u*a.offset,g);d=d.offset?0:V(d.lineWidth/2)*2;b[i]=t(b[i],d)},getLinePath:function(a){var b=this.chart,c=this.opposite,d=this.offset,e=this.horiz,f=this.left+(c?this.width:0)+d,d=b.chartHeight-this.bottom-(c?this.height:0)+d;c&&(a*=-1);return b.renderer.crispLine([\"M\",e?this.left:f,e?d:this.top,\"L\",e?b.chartWidth-this.right:f,e?d:b.chartHeight-this.bottom],a)},getTitlePosition:function(){var a=\n"
"this.horiz,b=this.left,c=this.top,d=this.len,e=this.options.title,f=a?b:c,g=this.opposite,h=this.offset,i=e.x||0,j=e.y||0,k=F(e.style.fontSize||12),d={low:f+(a?0:d),middle:f+d/2,high:f+(a?d:0)}[e.align],b=(a?c+this.height:b)+(a?1:-1)*(g?-1:1)*this.axisTitleMargin+(this.side===2?k:0);return{x:a?d+i:b+(g?this.width:0)+h+i,y:a?b+j-(g?this.height:0)+h:d+j}},render:function(){var a=this,b=a.chart,c=b.renderer,d=a.options,e=a.isLog,f=a.isLinked,g=a.tickPositions,h=a.axisTitle,i=a.ticks,j=a.minorTicks,k=\n"
"a.alternateBands,m=d.stackLabels,l=d.alternateGridColor,n=a.tickmarkOffset,q=d.lineWidth,x,p=b.hasRendered&&s(a.oldMin)&&!isNaN(a.oldMin),Q=a.showAxis,u=c.globalAnimation,y,t;a.labelEdge.length=0;a.overlap=!1;o([i,j,k],function(a){for(var b in a)a[b].isActive=!1});if(a.hasData()||f){a.minorTickInterval&&!a.categories&&o(a.getMinorTickPositions(),function(b){j[b]||(j[b]=new Ra(a,b,\"minor\"));p&&j[b].isNew&&j[b].render(null,!0);j[b].render(null,!1,1)});if(g.length&&(o(g,function(b,c){if(!f||b>=a.min&&\n"
"b<=a.max)i[b]||(i[b]=new Ra(a,b)),p&&i[b].isNew&&i[b].render(c,!0,0.1),i[b].render(c)}),n&&(a.min===0||a.single)))i[-1]||(i[-1]=new Ra(a,-1,null,!0)),i[-1].render(-1);l&&o(g,function(b,c){if(c%2===0&&b<a.max)k[b]||(k[b]=new B.PlotLineOrBand(a)),y=b+n,t=g[c+1]!==v?g[c+1]+n:a.max,k[b].options={from:e?ia(y):y,to:e?ia(t):t,color:l},k[b].render(),k[b].isActive=!0});if(!a._addedPlotLB)o((d.plotLines||[]).concat(d.plotBands||[]),function(b){a.addPlotBandOrLine(b)}),a._addedPlotLB=!0}o([i,j,k],function(a){var c,\n"
"d,e=[],f=u?u.duration||500:0,g=function(){for(d=e.length;d--;)a[e[d]]&&!a[e[d]].isActive&&(a[e[d]].destroy(),delete a[e[d]])};for(c in a)if(!a[c].isActive)a[c].render(c,!1,0),a[c].isActive=!1,e.push(c);a===k||!b.hasRendered||!f?g():f&&setTimeout(g,f)});if(q)x=a.getLinePath(q),a.axisLine?a.axisLine.animate({d:x}):a.axisLine=c.path(x).attr({stroke:d.lineColor,\"stroke-width\":q,zIndex:7}).add(a.axisGroup),a.axisLine[Q?\"show\":\"hide\"]();if(h&&Q)h[h.isNew?\"attr\":\"animate\"](a.getTitlePosition()),h.isNew=\n"
"!1;m&&m.enabled&&a.renderStackTotals();a.isDirty=!1},redraw:function(){this.render();o(this.plotLinesAndBands,function(a){a.render()});o(this.series,function(a){a.isDirty=!0})},destroy:function(a){var b=this,c=b.stacks,d,e=b.plotLinesAndBands;a||Z(b);for(d in c)Oa(c[d]),c[d]=null;o([b.ticks,b.minorTicks,b.alternateBands],function(a){Oa(a)});for(a=e.length;a--;)e[a].destroy();o(\"stackTotalGroup,axisLine,axisTitle,axisGroup,cross,gridGroup,labelGroup\".split(\",\"),function(a){b[a]&&(b[a]=b[a].destroy())});\n"
"this.cross&&this.cross.destroy()},drawCrosshair:function(a,b){var c,d=this.crosshair,e=d.animation;if(!this.crosshair||(s(b)||!p(this.crosshair.snap,!0))===!1||b&&b.series&&b.series[this.coll]!==this)this.hideCrosshair();else if(p(d.snap,!0)?s(b)&&(c=this.isXAxis?b.plotX:this.len-b.plotY):c=this.horiz?a.chartX-this.pos:this.len-a.chartY+this.pos,c=this.isRadial?this.getPlotLinePath(this.isXAxis?b.x:p(b.stackY,b.y))||null:this.getPlotLinePath(null,null,null,null,c)||null,c===null)this.hideCrosshair();\n"
"else if(this.cross)this.cross.attr({visibility:\"visible\"})[e?\"animate\":\"attr\"]({d:c},e);else{e=this.categories&&!this.isRadial;e={\"stroke-width\":d.width||(e?this.transA:1),stroke:d.color||(e?\"rgba(155,200,255,0.2)\":\"#C0C0C0\"),zIndex:d.zIndex||2};if(d.dashStyle)e.dashstyle=d.dashStyle;this.cross=this.chart.renderer.path(c).attr(e).add()}},hideCrosshair:function(){this.cross&&this.cross.hide()}};r(ha.prototype,{getPlotBandPath:function(a,b){var c=this.getPlotLinePath(b,null,null,!0),d=this.getPlotLinePath(a,\n"
"null,null,!0);d&&c&&d.toString()!==c.toString()?d.push(c[4],c[5],c[1],c[2]):d=null;return d},addPlotBand:function(a){return this.addPlotBandOrLine(a,\"plotBands\")},addPlotLine:function(a){return this.addPlotBandOrLine(a,\"plotLines\")},addPlotBandOrLine:function(a,b){var c=(new B.PlotLineOrBand(this,a)).render(),d=this.userOptions;c&&(b&&(d[b]=d[b]||[],d[b].push(a)),this.plotLinesAndBands.push(c));return c},removePlotBandOrLine:function(a){for(var b=this.plotLinesAndBands,c=this.options,d=this.userOptions,\n"
,
"e=b.length;e--;)b[e].id===a&&b[e].destroy();o([c.plotLines||[],d.plotLines||[],c.plotBands||[],d.plotBands||[]],function(b){for(e=b.length;e--;)b[e].id===a&&ja(b,b[e])})}});ha.prototype.getTimeTicks=function(a,b,c,d){var e=[],f={},g=S.global.useUTC,h,i=new ya(b-Va(b)),j=a.unitRange,k=a.count;if(s(b)){i[Db](j>=E.second?0:k*V(i.getMilliseconds()/k));if(j>=E.second)i[Eb](j>=E.minute?0:k*V(i.getSeconds()/k));if(j>=E.minute)i[Fb](j>=E.hour?0:k*V(i[qb]()/k));if(j>=E.hour)i[Gb](j>=E.day?0:k*V(i[rb]()/k));\n"
"if(j>=E.day)i[tb](j>=E.month?1:k*V(i[Wa]()/k));j>=E.month&&(i[ub](j>=E.year?0:k*V(i[Xa]()/k)),h=i[Ya]());j>=E.year&&(h-=h%k,i[vb](h));if(j===E.week)i[tb](i[Wa]()-i[sb]()+p(d,1));b=1;if(mb||bb)i=i.getTime(),i=new ya(i+Va(i));h=i[Ya]();for(var d=i.getTime(),m=i[Xa](),l=i[Wa](),n=(E.day+(g?Va(i):i.getTimezoneOffset()*6E4))%E.day;d<c;)e.push(d),j===E.year?d=db(h+b*k,0):j===E.month?d=db(h,m+b*k):!g&&(j===E.day||j===E.week)?d=db(h,m,l+b*k*(j===E.day?1:7)):d+=j*k,b++;e.push(d);o(ib(e,function(a){return j<=\n"
"E.hour&&a%E.day===n}),function(a){f[a]=\"day\"})}e.info=r(a,{higherRanks:f,totalRange:j*k});return e};ha.prototype.normalizeTimeTickInterval=function(a,b){var c=b||[[\"millisecond\",[1,2,5,10,20,25,50,100,200,500]],[\"second\",[1,2,5,10,15,30]],[\"minute\",[1,2,5,10,15,30]],[\"hour\",[1,2,3,4,6,8,12]],[\"day\",[1,2]],[\"week\",[1,2]],[\"month\",[1,2,3,4,6]],[\"year\",null]],d=c[c.length-1],e=E[d[0]],f=d[1],g;for(g=0;g<c.length;g++)if(d=c[g],e=E[d[0]],f=d[1],c[g+1]&&a<=(e*f[f.length-1]+E[c[g+1][0]])/2)break;e===E.year&&\n"
"a<5*e&&(f=[1,2,5]);c=ob(a/e,f,d[0]===\"year\"?t(nb(a/e),1):1);return{unitRange:e,count:c,unitName:d[0]}};ha.prototype.getLogTickPositions=function(a,b,c,d){var e=this.options,f=this.len,g=[];if(!d)this._minorAutoInterval=null;if(a>=0.5)a=w(a),g=this.getLinearTickPositions(a,b,c);else if(a>=0.08)for(var f=V(b),h,i,j,k,m,e=a>0.3?[1,2,4]:a>0.15?[1,2,4,6,8]:[1,2,3,4,5,6,7,8,9];f<c+1&&!m;f++){i=e.length;for(h=0;h<i&&!m;h++)j=Ba(ia(f)*e[h]),j>b&&(!d||k<=c)&&k!==v&&g.push(k),k>c&&(m=!0),k=j}else if(b=ia(b),\n"
"c=ia(c),a=e[d?\"minorTickInterval\":\"tickInterval\"],a=p(a===\"auto\"?null:a,this._minorAutoInterval,(c-b)*(e.tickPixelInterval/(d?5:1))/((d?f/this.tickPositions.length:f)||1)),a=ob(a,null,nb(a)),g=Ta(this.getLinearTickPositions(a,b,c),Ba),!d)this._minorAutoInterval=a/5;if(!d)this.tickInterval=a;return g};var Mb=B.Tooltip=function(){this.init.apply(this,arguments)};Mb.prototype={init:function(a,b){var c=b.borderWidth,d=b.style,e=F(d.padding);this.chart=a;this.options=b;this.crosshairs=[];this.now={x:0,\n"
"y:0};this.isHidden=!0;this.label=a.renderer.label(\"\",0,0,b.shape||\"callout\",null,null,b.useHTML,null,\"tooltip\").attr({padding:e,fill:b.backgroundColor,\"stroke-width\":c,r:b.borderRadius,zIndex:8}).css(d).css({padding:0}).add().attr({y:-9999});fa||this.label.shadow(b.shadow);this.shared=b.shared},destroy:function(){if(this.label)this.label=this.label.destroy();clearTimeout(this.hideTimer);clearTimeout(this.tooltipTimeout)},move:function(a,b,c,d){var e=this,f=e.now,g=e.options.animation!==!1&&!e.isHidden&&\n"
"(N(a-f.x)>1||N(b-f.y)>1),h=e.followPointer||e.len>1;r(f,{x:g?(2*f.x+a)/3:a,y:g?(f.y+b)/2:b,anchorX:h?v:g?(2*f.anchorX+c)/3:c,anchorY:h?v:g?(f.anchorY+d)/2:d});e.label.attr(f);if(g)clearTimeout(this.tooltipTimeout),this.tooltipTimeout=setTimeout(function(){e&&e.move(a,b,c,d)},32)},hide:function(a){var b=this;clearTimeout(this.hideTimer);if(!this.isHidden)this.hideTimer=setTimeout(function(){b.label.fadeOut();b.isHidden=!0},p(a,this.options.hideDelay,500))},getAnchor:function(a,b){var c,d=this.chart,\n"
"e=d.inverted,f=d.plotTop,g=d.plotLeft,h=0,i=0,j,k,a=ra(a);c=a[0].tooltipPos;this.followPointer&&b&&(b.chartX===v&&(b=d.pointer.normalize(b)),c=[b.chartX-d.plotLeft,b.chartY-f]);c||(o(a,function(a){j=a.series.yAxis;k=a.series.xAxis;h+=a.plotX+(!e&&k?k.left-g:0);i+=(a.plotLow?(a.plotLow+a.plotHigh)/2:a.plotY)+(!e&&j?j.top-f:0)}),h/=a.length,i/=a.length,c=[e?d.plotWidth-i:h,this.shared&&!e&&a.length>1&&b?b.chartY-f:e?d.plotHeight-h:i]);return Ta(c,w)},getPosition:function(a,b,c){var d=this.chart,e=this.distance,\n"
"f={},g=c.h||0,h,i=[\"y\",d.chartHeight,b,c.plotY+d.plotTop,d.plotTop,d.plotTop+d.plotHeight],j=[\"x\",d.chartWidth,a,c.plotX+d.plotLeft,d.plotLeft,d.plotLeft+d.plotWidth],k=p(c.ttBelow,d.inverted&&!c.negative||!d.inverted&&c.negative),m=function(a,b,c,d,h,i){var j=c<d-e,l=d+e+c<b,m=d-e-c;d+=e;if(k&&l)f[a]=d;else if(!k&&j)f[a]=m;else if(j)f[a]=z(i-c,m-g<0?m:m-g);else if(l)f[a]=t(h,d+g+c>b?d:d+g);else return!1},l=function(a,b,c,d){if(d<e||d>b-e)return!1;else f[a]=d<c/2?1:d>b-c/2?b-c-2:d-c/2},n=function(a){var b=\n"
"i;i=j;j=b;h=a},q=function(){m.apply(0,i)!==!1?l.apply(0,j)===!1&&!h&&(n(!0),q()):h?f.x=f.y=0:(n(!0),q())};(d.inverted||this.len>1)&&n();q();return f},defaultFormatter:function(a){var b=this.points||ra(this),c;c=[a.tooltipFooterHeaderFormatter(b[0])];c=c.concat(a.bodyFormatter(b));c.push(a.tooltipFooterHeaderFormatter(b[0],!0));return c.join(\"\")},refresh:function(a,b){var c=this.chart,d=this.label,e=this.options,f,g,h,i={},j,k=[];j=e.formatter||this.defaultFormatter;var i=c.hoverPoints,m,l=this.shared;\n"
"clearTimeout(this.hideTimer);this.followPointer=ra(a)[0].series.tooltipOptions.followPointer;h=this.getAnchor(a,b);f=h[0];g=h[1];l&&(!a.series||!a.series.noSharedTooltip)?(c.hoverPoints=a,i&&o(i,function(a){a.setState()}),o(a,function(a){a.setState(\"hover\");k.push(a.getLabelConfig())}),i={x:a[0].category,y:a[0].y},i.points=k,this.len=k.length,a=a[0]):i=a.getLabelConfig();j=j.call(i,this);i=a.series;this.distance=p(i.tooltipOptions.distance,16);j===!1?this.hide():(this.isHidden&&(kb(d),d.attr(\"opacity\",\n"
"1).show()),d.attr({text:j}),m=e.borderColor||a.color||i.color||\"#606060\",d.attr({stroke:m}),this.updatePosition({plotX:f,plotY:g,negative:a.negative,ttBelow:a.ttBelow,h:h[2]||0}),this.isHidden=!1);I(c,\"tooltipRefresh\",{text:j,x:f+c.plotLeft,y:g+c.plotTop,borderColor:m})},updatePosition:function(a){var b=this.chart,c=this.label,c=(this.options.positioner||this.getPosition).call(this,c.width,c.height,a);this.move(w(c.x),w(c.y||0),a.plotX+b.plotLeft,a.plotY+b.plotTop)},getXDateFormat:function(a,b,c){var d,\n"
"b=b.dateTimeLabelFormats,e=c&&c.closestPointRange,f,g={millisecond:15,second:12,minute:9,hour:6,day:3},h,i=\"millisecond\";if(e){h=Ma(\"%m-%d %H:%M:%S.%L\",a.x);for(f in E){if(e===E.week&&+Ma(\"%w\",a.x)===c.options.startOfWeek&&h.substr(6)===\"00:00:00.000\"){f=\"week\";break}else if(E[f]>e){f=i;break}else if(g[f]&&h.substr(g[f])!==\"01-01 00:00:00.000\".substr(g[f]))break;f!==\"week\"&&(i=f)}f&&(d=b[f])}else d=b.day;return d||b.year},tooltipFooterHeaderFormatter:function(a,b){var c=b?\"footer\":\"header\",d=a.series,\n"
"e=d.tooltipOptions,f=e.xDateFormat,g=d.xAxis,h=g&&g.options.type===\"datetime\"&&qa(a.key),c=e[c+\"Format\"];h&&!f&&(f=this.getXDateFormat(a,e,g));h&&f&&(c=c.replace(\"{point.key}\",\"{point.key:\"+f+\"}\"));return Ha(c,{point:a,series:d})},bodyFormatter:function(a){return Ta(a,function(a){var c=a.series.tooltipOptions;return(c.pointFormatter||a.point.tooltipFormatter).call(a.point,c.pointFormat)})}};var oa;$a=C.documentElement.ontouchstart!==v;var Ua=B.Pointer=function(a,b){this.init(a,b)};Ua.prototype={init:function(a,\n"
"b){var c=b.chart,d=c.events,e=fa?\"\":c.zoomType,c=a.inverted,f;this.options=b;this.chart=a;this.zoomX=f=/x/.test(e);this.zoomY=e=/y/.test(e);this.zoomHor=f&&!c||e&&c;this.zoomVert=e&&!c||f&&c;this.hasZoom=f||e;this.runChartClick=d&&!!d.click;this.pinchDown=[];this.lastValidTouch={};if(B.Tooltip&&b.tooltip.enabled)a.tooltip=new Mb(a,b.tooltip),this.followTouchMove=p(b.tooltip.followTouchMove,!0);this.setDOMEvents()},normalize:function(a,b){var c,d,a=a||window.event,a=Sb(a);if(!a.target)a.target=a.srcElement;\n"
"d=a.touches?a.touches.length?a.touches.item(0):a.changedTouches[0]:a;if(!b)this.chartPosition=b=Rb(this.chart.container);d.pageX===v?(c=t(a.x,a.clientX-b.left),d=a.y):(c=d.pageX-b.left,d=d.pageY-b.top);return r(a,{chartX:w(c),chartY:w(d)})},getCoordinates:function(a){var b={xAxis:[],yAxis:[]};o(this.chart.axes,function(c){b[c.isXAxis?\"xAxis\":\"yAxis\"].push({axis:c,value:c.toValue(a[c.horiz?\"chartX\":\"chartY\"])})});return b},runPointActions:function(a){var b=this.chart,c=b.series,d=b.tooltip,e=d?d.shared:\n"
"!1,f=b.hoverPoint,g=b.hoverSeries,h,i=b.chartWidth,j,k,m=[],l,n;if(!e&&!g)for(h=0;h<c.length;h++)if(c[h].directTouch||!c[h].options.stickyTracking)c=[];!e&&g&&g.directTouch&&f?l=f:(o(c,function(b){j=b.noSharedTooltip&&e;k=!e&&b.directTouch;b.visible&&!j&&!k&&p(b.options.enableMouseTracking,!0)&&(n=b.searchPoint(a,!j&&b.kdDimensions===1))&&m.push(n)}),o(m,function(a){if(a&&typeof a.dist===\"number\"&&a.dist<i)i=a.dist,l=a}));if(l&&(l!==this.prevKDPoint||d&&d.isHidden)){if(e&&!l.series.noSharedTooltip){for(h=\n"
"m.length;h--;)(m[h].clientX!==l.clientX||m[h].series.noSharedTooltip)&&m.splice(h,1);m.length&&d&&d.refresh(m,a);o(m,function(b){b.onMouseOver(a,b!==(g&&g.directTouch&&f||l))})}else if(d&&d.refresh(l,a),!g||!g.directTouch)l.onMouseOver(a);this.prevKDPoint=l}else c=g&&g.tooltipOptions.followPointer,d&&c&&!d.isHidden&&(c=d.getAnchor([{}],a),d.updatePosition({plotX:c[0],plotY:c[1]}));if(d&&!this._onDocumentMouseMove)this._onDocumentMouseMove=function(a){if(Y[oa])Y[oa].pointer.onDocumentMouseMove(a)},\n"
"H(C,\"mousemove\",this._onDocumentMouseMove);o(b.axes,function(b){b.drawCrosshair(a,p(l,f))})},reset:function(a,b){var c=this.chart,d=c.hoverSeries,e=c.hoverPoint,f=c.hoverPoints,g=c.tooltip,h=g&&g.shared?f:e;(a=a&&g&&h)&&ra(h)[0].plotX===v&&(a=!1);if(a)g.refresh(h),e&&(e.setState(e.state,!0),o(c.axes,function(a){p(a.options.crosshair&&a.options.crosshair.snap,!0)?a.drawCrosshair(null,e):a.hideCrosshair()}));else{if(e)e.onMouseOut();f&&o(f,function(a){a.setState()});if(d)d.onMouseOut();g&&g.hide(b);\n"
,
"if(this._onDocumentMouseMove)Z(C,\"mousemove\",this._onDocumentMouseMove),this._onDocumentMouseMove=null;o(c.axes,function(a){a.hideCrosshair()});this.hoverX=c.hoverPoints=c.hoverPoint=null}},scaleGroups:function(a,b){var c=this.chart,d;o(c.series,function(e){d=a||e.getPlotBox();e.xAxis&&e.xAxis.zoomEnabled&&(e.group.attr(d),e.markerGroup&&(e.markerGroup.attr(d),e.markerGroup.clip(b?c.clipRect:null)),e.dataLabelsGroup&&e.dataLabelsGroup.attr(d))});c.clipRect.attr(b||c.clipBox)},dragStart:function(a){var b=\n"
"this.chart;b.mouseIsDown=a.type;b.cancelClick=!1;b.mouseDownX=this.mouseDownX=a.chartX;b.mouseDownY=this.mouseDownY=a.chartY},drag:function(a){var b=this.chart,c=b.options.chart,d=a.chartX,e=a.chartY,f=this.zoomHor,g=this.zoomVert,h=b.plotLeft,i=b.plotTop,j=b.plotWidth,k=b.plotHeight,m,l=this.selectionMarker,n=this.mouseDownX,q=this.mouseDownY,x=c.panKey&&a[c.panKey+\"Key\"];if(!l||!l.touch)if(d<h?d=h:d>h+j&&(d=h+j),e<i?e=i:e>i+k&&(e=i+k),this.hasDragged=Math.sqrt(Math.pow(n-d,2)+Math.pow(q-e,2)),this.hasDragged>\n"
"10){m=b.isInsidePlot(n-h,q-i);if(b.hasCartesianSeries&&(this.zoomX||this.zoomY)&&m&&!x&&!l)this.selectionMarker=l=b.renderer.rect(h,i,f?1:j,g?1:k,0).attr({fill:c.selectionMarkerFill||\"rgba(69,114,167,0.25)\",zIndex:7}).add();l&&f&&(d-=n,l.attr({width:N(d),x:(d>0?0:d)+n}));l&&g&&(d=e-q,l.attr({height:N(d),y:(d>0?0:d)+q}));m&&!l&&c.panning&&b.pan(a,c.panning)}},drop:function(a){var b=this,c=this.chart,d=this.hasPinched;if(this.selectionMarker){var e={xAxis:[],yAxis:[],originalEvent:a.originalEvent||\n"
"a},f=this.selectionMarker,g=f.attr?f.attr(\"x\"):f.x,h=f.attr?f.attr(\"y\"):f.y,i=f.attr?f.attr(\"width\"):f.width,j=f.attr?f.attr(\"height\"):f.height,k;if(this.hasDragged||d)o(c.axes,function(c){if(c.zoomEnabled&&s(c.min)&&(d||b[{xAxis:\"zoomX\",yAxis:\"zoomY\"}[c.coll]])){var f=c.horiz,n=a.type===\"touchend\"?c.minPixelPadding:0,q=c.toValue((f?g:h)+n),f=c.toValue((f?g+i:h+j)-n);e[c.coll].push({axis:c,min:z(q,f),max:t(q,f)});k=!0}}),k&&I(c,\"selection\",e,function(a){c.zoom(r(a,d?{animation:!1}:null))});this.selectionMarker=\n"
"this.selectionMarker.destroy();d&&this.scaleGroups()}if(c)L(c.container,{cursor:c._cursor}),c.cancelClick=this.hasDragged>10,c.mouseIsDown=this.hasDragged=this.hasPinched=!1,this.pinchDown=[]},onContainerMouseDown:function(a){a=this.normalize(a);a.preventDefault&&a.preventDefault();this.dragStart(a)},onDocumentMouseUp:function(a){Y[oa]&&Y[oa].pointer.drop(a)},onDocumentMouseMove:function(a){var b=this.chart,c=this.chartPosition,a=this.normalize(a,c);c&&!this.inClass(a.target,\"highcharts-tracker\")&&\n"
"!b.isInsidePlot(a.chartX-b.plotLeft,a.chartY-b.plotTop)&&this.reset()},onContainerMouseLeave:function(){var a=Y[oa];if(a)a.pointer.reset(),a.pointer.chartPosition=null},onContainerMouseMove:function(a){var b=this.chart;oa=b.index;a=this.normalize(a);a.returnValue=!1;b.mouseIsDown===\"mousedown\"&&this.drag(a);(this.inClass(a.target,\"highcharts-tracker\")||b.isInsidePlot(a.chartX-b.plotLeft,a.chartY-b.plotTop))&&!b.openMenu&&this.runPointActions(a)},inClass:function(a,b){for(var c;a;){if(c=J(a,\"class\"))if(c.indexOf(b)!==\n"
"-1)return!0;else if(c.indexOf(\"highcharts-container\")!==-1)return!1;a=a.parentNode}},onTrackerMouseOut:function(a){var b=this.chart.hoverSeries,c=(a=a.relatedTarget||a.toElement)&&a.point&&a.point.series;if(b&&!b.options.stickyTracking&&!this.inClass(a,\"highcharts-tooltip\")&&c!==b)b.onMouseOut()},onContainerClick:function(a){var b=this.chart,c=b.hoverPoint,d=b.plotLeft,e=b.plotTop,a=this.normalize(a);a.originalEvent=a;b.cancelClick||(c&&this.inClass(a.target,\"highcharts-tracker\")?(I(c.series,\"click\",\n"
"r(a,{point:c})),b.hoverPoint&&c.firePointEvent(\"click\",a)):(r(a,this.getCoordinates(a)),b.isInsidePlot(a.chartX-d,a.chartY-e)&&I(b,\"click\",a)))},setDOMEvents:function(){var a=this,b=a.chart.container;b.onmousedown=function(b){a.onContainerMouseDown(b)};b.onmousemove=function(b){a.onContainerMouseMove(b)};b.onclick=function(b){a.onContainerClick(b)};H(b,\"mouseleave\",a.onContainerMouseLeave);ab===1&&H(C,\"mouseup\",a.onDocumentMouseUp);if($a)b.ontouchstart=function(b){a.onContainerTouchStart(b)},b.ontouchmove=\n"
"function(b){a.onContainerTouchMove(b)},ab===1&&H(C,\"touchend\",a.onDocumentTouchEnd)},destroy:function(){var a;Z(this.chart.container,\"mouseleave\",this.onContainerMouseLeave);ab||(Z(C,\"mouseup\",this.onDocumentMouseUp),Z(C,\"touchend\",this.onDocumentTouchEnd));clearInterval(this.tooltipTimeout);for(a in this)this[a]=null}};r(B.Pointer.prototype,{pinchTranslate:function(a,b,c,d,e,f){(this.zoomHor||this.pinchHor)&&this.pinchTranslateDirection(!0,a,b,c,d,e,f);(this.zoomVert||this.pinchVert)&&this.pinchTranslateDirection(!1,\n"
"a,b,c,d,e,f)},pinchTranslateDirection:function(a,b,c,d,e,f,g,h){var i=this.chart,j=a?\"x\":\"y\",k=a?\"X\":\"Y\",m=\"chart\"+k,l=a?\"width\":\"height\",n=i[\"plot\"+(a?\"Left\":\"Top\")],q,x,p=h||1,o=i.inverted,u=i.bounds[a?\"h\":\"v\"],t=b.length===1,s=b[0][m],r=c[0][m],w=!t&&b[1][m],v=!t&&c[1][m],z,c=function(){!t&&N(s-w)>20&&(p=h||N(r-v)/N(s-w));x=(n-r)/p+s;q=i[\"plot\"+(a?\"Width\":\"Height\")]/p};c();b=x;b<u.min?(b=u.min,z=!0):b+q>u.max&&(b=u.max-q,z=!0);z?(r-=0.8*(r-g[j][0]),t||(v-=0.8*(v-g[j][1])),c()):g[j]=[r,v];o||(f[j]=\n"
"x-n,f[l]=q);f=o?1/p:p;e[l]=q;e[j]=b;d[o?a?\"scaleY\":\"scaleX\":\"scale\"+k]=p;d[\"translate\"+k]=f*n+(r-f*s)},pinch:function(a){var b=this,c=b.chart,d=b.pinchDown,e=a.touches,f=e.length,g=b.lastValidTouch,h=b.hasZoom,i=b.selectionMarker,j={},k=f===1&&(b.inClass(a.target,\"highcharts-tracker\")&&c.runTrackerClick||b.runChartClick),m={};if(f>1)b.initiated=!0;h&&b.initiated&&!k&&a.preventDefault();Ta(e,function(a){return b.normalize(a)});if(a.type===\"touchstart\")o(e,function(a,b){d[b]={chartX:a.chartX,chartY:a.chartY}}),\n"
"g.x=[d[0].chartX,d[1]&&d[1].chartX],g.y=[d[0].chartY,d[1]&&d[1].chartY],o(c.axes,function(a){if(a.zoomEnabled){var b=c.bounds[a.horiz?\"h\":\"v\"],d=a.minPixelPadding,e=a.toPixels(p(a.options.min,a.dataMin)),f=a.toPixels(p(a.options.max,a.dataMax)),g=z(e,f),e=t(e,f);b.min=z(a.pos,g-d);b.max=t(a.pos+a.len,e+d)}}),b.res=!0;else if(d.length){if(!i)b.selectionMarker=i=r({destroy:ta,touch:!0},c.plotBox);b.pinchTranslate(d,e,j,i,m,g);b.hasPinched=h;b.scaleGroups(j,m);if(!h&&b.followTouchMove&&f===1)this.runPointActions(b.normalize(a));\n"
"else if(b.res)b.res=!1,this.reset(!1,0)}},touch:function(a,b){var c=this.chart;oa=c.index;a.touches.length===1?(a=this.normalize(a),c.isInsidePlot(a.chartX-c.plotLeft,a.chartY-c.plotTop)&&!c.openMenu?(b&&this.runPointActions(a),this.pinch(a)):b&&this.reset()):a.touches.length===2&&this.pinch(a)},onContainerTouchStart:function(a){this.touch(a,!0)},onContainerTouchMove:function(a){this.touch(a)},onDocumentTouchEnd:function(a){Y[oa]&&Y[oa].pointer.drop(a)}});if(K.PointerEvent||K.MSPointerEvent){var va=\n"
"{},Ab=!!K.PointerEvent,Wb=function(){var a,b=[];b.item=function(a){return this[a]};for(a in va)va.hasOwnProperty(a)&&b.push({pageX:va[a].pageX,pageY:va[a].pageY,target:va[a].target});return b},Bb=function(a,b,c,d){a=a.originalEvent||a;if((a.pointerType===\"touch\"||a.pointerType===a.MSPOINTER_TYPE_TOUCH)&&Y[oa])d(a),d=Y[oa].pointer,d[b]({type:c,target:a.currentTarget,preventDefault:ta,touches:Wb()})};r(Ua.prototype,{onContainerPointerDown:function(a){Bb(a,\"onContainerTouchStart\",\"touchstart\",function(a){va[a.pointerId]=\n"
"{pageX:a.pageX,pageY:a.pageY,target:a.currentTarget}})},onContainerPointerMove:function(a){Bb(a,\"onContainerTouchMove\",\"touchmove\",function(a){va[a.pointerId]={pageX:a.pageX,pageY:a.pageY};if(!va[a.pointerId].target)va[a.pointerId].target=a.currentTarget})},onDocumentPointerUp:function(a){Bb(a,\"onDocumentTouchEnd\",\"touchend\",function(a){delete va[a.pointerId]})},batchMSEvents:function(a){a(this.chart.container,Ab?\"pointerdown\":\"MSPointerDown\",this.onContainerPointerDown);a(this.chart.container,Ab?\n"
"\"pointermove\":\"MSPointerMove\",this.onContainerPointerMove);a(C,Ab?\"pointerup\":\"MSPointerUp\",this.onDocumentPointerUp)}});Sa(Ua.prototype,\"init\",function(a,b,c){a.call(this,b,c);this.hasZoom&&L(b.container,{\"-ms-touch-action\":O,\"touch-action\":O})});Sa(Ua.prototype,\"setDOMEvents\",function(a){a.apply(this);(this.hasZoom||this.followTouchMove)&&this.batchMSEvents(H)});Sa(Ua.prototype,\"destroy\",function(a){this.batchMSEvents(Z);a.call(this)})}var lb=B.Legend=function(a,b){this.init(a,b)};lb.prototype=\n"
"{init:function(a,b){var c=this,d=b.itemStyle,e=b.itemMarginTop||0;this.options=b;if(b.enabled)c.itemStyle=d,c.itemHiddenStyle=A(d,b.itemHiddenStyle),c.itemMarginTop=e,c.padding=d=p(b.padding,8),c.initialItemX=d,c.initialItemY=d-5,c.maxItemWidth=0,c.chart=a,c.itemHeight=0,c.symbolWidth=p(b.symbolWidth,16),c.pages=[],c.render(),H(c.chart,\"endResize\",function(){c.positionCheckboxes()})},colorizeItem:function(a,b){var c=this.options,d=a.legendItem,e=a.legendLine,f=a.legendSymbol,g=this.itemHiddenStyle.color,\n"
"c=b?c.itemStyle.color:g,h=b?a.legendColor||a.color||\"#CCC\":g,g=a.options&&a.options.marker,i={fill:h},j;d&&d.css({fill:c,color:c});e&&e.attr({stroke:h});if(f){if(g&&f.isMarker)for(j in i.stroke=h,g=a.convertAttribs(g),g)d=g[j],d!==v&&(i[j]=d);f.attr(i)}},positionItem:function(a){var b=this.options,c=b.symbolPadding,b=!b.rtl,d=a._legendItemPos,e=d[0],d=d[1],f=a.checkbox;(a=a.legendGroup)&&a.element&&a.translate(b?e:this.legendWidth-e-2*c-4,d);if(f)f.x=e,f.y=d},destroyItem:function(a){var b=a.checkbox;\n"
"o([\"legendItem\",\"legendLine\",\"legendSymbol\",\"legendGroup\"],function(b){a[b]&&(a[b]=a[b].destroy())});b&&Pa(a.checkbox)},destroy:function(){var a=this.group,b=this.box;if(b)this.box=b.destroy();if(a)this.group=a.destroy()},positionCheckboxes:function(a){var b=this.group.alignAttr,c,d=this.clipHeight||this.legendHeight;if(b)c=b.translateY,o(this.allItems,function(e){var f=e.checkbox,g;f&&(g=c+f.y+(a||0)+3,L(f,{left:b.translateX+e.checkboxOffset+f.x-20+\"px\",top:g+\"px\",display:g>c-6&&g<c+d-6?\"\":O}))})},\n"
,
"renderTitle:function(){var a=this.padding,b=this.options.title,c=0;if(b.text){if(!this.title)this.title=this.chart.renderer.label(b.text,a-3,a-4,null,null,null,null,null,\"legend-title\").attr({zIndex:1}).css(b.style).add(this.group);a=this.title.getBBox();c=a.height;this.offsetWidth=a.width;this.contentGroup.attr({translateY:c})}this.titleHeight=c},setText:function(a){var b=this.options;a.legendItem.attr({text:b.labelFormat?Ha(b.labelFormat,a):b.labelFormatter.call(a)})},renderItem:function(a){var b=\n"
"this.chart,c=b.renderer,d=this.options,e=d.layout===\"horizontal\",f=this.symbolWidth,g=d.symbolPadding,h=this.itemStyle,i=this.itemHiddenStyle,j=this.padding,k=e?p(d.itemDistance,20):0,m=!d.rtl,l=d.width,n=d.itemMarginBottom||0,q=this.itemMarginTop,x=this.initialItemX,o=a.legendItem,s=a.series&&a.series.drawLegendSymbol?a.series:a,u=s.options,u=this.createCheckboxForItem&&u&&u.showCheckbox,y=d.useHTML;if(!o){a.legendGroup=c.g(\"legend-item\").attr({zIndex:1}).add(this.scrollGroup);a.legendItem=o=c.text(\"\",\n"
"m?f+g:-g,this.baseline||0,y).css(A(a.visible?h:i)).attr({align:m?\"left\":\"right\",zIndex:2}).add(a.legendGroup);if(!this.baseline)this.fontMetrics=c.fontMetrics(h.fontSize,o),this.baseline=this.fontMetrics.f+3+q,o.attr(\"y\",this.baseline);s.drawLegendSymbol(this,a);this.setItemEvents&&this.setItemEvents(a,o,y,h,i);this.colorizeItem(a,a.visible);u&&this.createCheckboxForItem(a)}this.setText(a);c=o.getBBox();f=a.checkboxOffset=d.itemWidth||a.legendItemWidth||f+g+c.width+k+(u?20:0);this.itemHeight=g=w(a.legendItemHeight||\n"
"c.height);if(e&&this.itemX-x+f>(l||b.chartWidth-2*j-x-d.x))this.itemX=x,this.itemY+=q+this.lastLineHeight+n,this.lastLineHeight=0;this.maxItemWidth=t(this.maxItemWidth,f);this.lastItemY=q+this.itemY+n;this.lastLineHeight=t(g,this.lastLineHeight);a._legendItemPos=[this.itemX,this.itemY];e?this.itemX+=f:(this.itemY+=q+g+n,this.lastLineHeight=g);this.offsetWidth=l||t((e?this.itemX-x-k:f)+j,this.offsetWidth)},getAllItems:function(){var a=[];o(this.chart.series,function(b){var c=b.options;if(p(c.showInLegend,\n"
"!s(c.linkedTo)?v:!1,!0))a=a.concat(b.legendItems||(c.legendType===\"point\"?b.data:b))});return a},adjustMargins:function(a,b){var c=this.chart,d=this.options,e=d.align.charAt(0)+d.verticalAlign.charAt(0)+d.layout.charAt(0);this.display&&!d.floating&&o([/(lth|ct|rth)/,/(rtv|rm|rbv)/,/(rbh|cb|lbh)/,/(lbv|lm|ltv)/],function(f,g){f.test(e)&&!s(a[g])&&(c[gb[g]]=t(c[gb[g]],c.legend[(g+1)%2?\"legendHeight\":\"legendWidth\"]+[1,-1,-1,1][g]*d[g%2?\"x\":\"y\"]+p(d.margin,12)+b[g]))})},render:function(){var a=this,b=\n"
"a.chart,c=b.renderer,d=a.group,e,f,g,h,i=a.box,j=a.options,k=a.padding,m=j.borderWidth,l=j.backgroundColor;a.itemX=a.initialItemX;a.itemY=a.initialItemY;a.offsetWidth=0;a.lastItemY=0;if(!d)a.group=d=c.g(\"legend\").attr({zIndex:7}).add(),a.contentGroup=c.g().attr({zIndex:1}).add(d),a.scrollGroup=c.g().add(a.contentGroup);a.renderTitle();e=a.getAllItems();pb(e,function(a,b){return(a.options&&a.options.legendIndex||0)-(b.options&&b.options.legendIndex||0)});j.reversed&&e.reverse();a.allItems=e;a.display=\n"
"f=!!e.length;a.lastLineHeight=0;o(e,function(b){a.renderItem(b)});g=(j.width||a.offsetWidth)+k;h=a.lastItemY+a.lastLineHeight+a.titleHeight;h=a.handleOverflow(h);h+=k;if(m||l){if(i){if(g>0&&h>0)i[i.isNew?\"attr\":\"animate\"](i.crisp({width:g,height:h})),i.isNew=!1}else a.box=i=c.rect(0,0,g,h,j.borderRadius,m||0).attr({stroke:j.borderColor,\"stroke-width\":m||0,fill:l||O}).add(d).shadow(j.shadow),i.isNew=!0;i[f?\"show\":\"hide\"]()}a.legendWidth=g;a.legendHeight=h;o(e,function(b){a.positionItem(b)});f&&d.align(r({width:g,\n"
"height:h},j),!0,\"spacingBox\");b.isResizing||this.positionCheckboxes()},handleOverflow:function(a){var b=this,c=this.chart,d=c.renderer,e=this.options,f=e.y,f=c.spacingBox.height+(e.verticalAlign===\"top\"?-f:f)-this.padding,g=e.maxHeight,h,i=this.clipRect,j=e.navigation,k=p(j.animation,!0),m=j.arrowSize||12,l=this.nav,n=this.pages,q=this.padding,x,s=this.allItems,r=function(a){i.attr({height:a});if(b.contentGroup.div)b.contentGroup.div.style.clip=\"rect(\"+q+\"px,9999px,\"+(q+a)+\"px,0)\"};e.layout===\"horizontal\"&&\n"
"(f/=2);g&&(f=z(f,g));n.length=0;if(a>f){this.clipHeight=h=t(f-20-this.titleHeight-q,0);this.currentPage=p(this.currentPage,1);this.fullHeight=a;o(s,function(a,b){var c=a._legendItemPos[1],d=w(a.legendItem.getBBox().height),e=n.length;if(!e||c-n[e-1]>h&&(x||c)!==n[e-1])n.push(x||c),e++;b===s.length-1&&c+d-n[e-1]>h&&n.push(c);c!==x&&(x=c)});if(!i)i=b.clipRect=d.clipRect(0,q,9999,0),b.contentGroup.clip(i);r(h);if(!l)this.nav=l=d.g().attr({zIndex:1}).add(this.group),this.up=d.symbol(\"triangle\",0,0,m,\n"
"m).on(\"click\",function(){b.scroll(-1,k)}).add(l),this.pager=d.text(\"\",15,10).css(j.style).add(l),this.down=d.symbol(\"triangle-down\",0,0,m,m).on(\"click\",function(){b.scroll(1,k)}).add(l);b.scroll(0);a=f}else if(l)r(c.chartHeight),l.hide(),this.scrollGroup.attr({translateY:1}),this.clipHeight=0;return a},scroll:function(a,b){var c=this.pages,d=c.length,e=this.currentPage+a,f=this.clipHeight,g=this.options.navigation,h=g.activeColor,g=g.inactiveColor,i=this.pager,j=this.padding;e>d&&(e=d);if(e>0)b!==\n"
"v&&Qa(b,this.chart),this.nav.attr({translateX:j,translateY:f+this.padding+7+this.titleHeight,visibility:\"visible\"}),this.up.attr({fill:e===1?g:h}).css({cursor:e===1?\"default\":\"pointer\"}),i.attr({text:e+\"/\"+d}),this.down.attr({x:18+this.pager.getBBox().width,fill:e===d?g:h}).css({cursor:e===d?\"default\":\"pointer\"}),c=-c[e-1]+this.initialItemY,this.scrollGroup.animate({translateY:c}),this.currentPage=e,this.positionCheckboxes(c)}};La=B.LegendSymbolMixin={drawRectangle:function(a,b){var c=a.options.symbolHeight||\n"
"a.fontMetrics.f;b.legendSymbol=this.chart.renderer.rect(0,a.baseline-c+1,a.symbolWidth,c,a.options.symbolRadius||0).attr({zIndex:3}).add(b.legendGroup)},drawLineMarker:function(a){var b=this.options,c=b.marker,d;d=a.symbolWidth;var e=this.chart.renderer,f=this.legendGroup,a=a.baseline-w(a.fontMetrics.b*0.3),g;if(b.lineWidth){g={\"stroke-width\":b.lineWidth};if(b.dashStyle)g.dashstyle=b.dashStyle;this.legendLine=e.path([\"M\",0,a,\"L\",d,a]).attr(g).add(f)}if(c&&c.enabled!==!1)b=c.radius,this.legendSymbol=\n"
"d=e.symbol(this.symbol,d/2-b,a-b,2*b,2*b).add(f),d.isMarker=!0}};(/Trident\\/7\\.0/.test(za)||Ja)&&Sa(lb.prototype,\"positionItem\",function(a,b){var c=this,d=function(){b._legendItemPos&&a.call(c,b)};d();setTimeout(d)});D=B.Chart=function(){this.init.apply(this,arguments)};D.prototype={callbacks:[],init:function(a,b){var c,d=a.series;a.series=null;c=A(S,a);c.series=a.series=d;this.userOptions=a;d=c.chart;this.margin=this.splashArray(\"margin\",d);this.spacing=this.splashArray(\"spacing\",d);var e=d.events;\n"
"this.bounds={h:{},v:{}};this.callback=b;this.isResizing=0;this.options=c;this.axes=[];this.series=[];this.hasCartesianSeries=d.showAxes;var f=this,g;f.index=Y.length;Y.push(f);ab++;d.reflow!==!1&&H(f,\"load\",function(){f.initReflow()});if(e)for(g in e)H(f,g,e[g]);f.xAxis=[];f.yAxis=[];f.animation=fa?!1:p(d.animation,!0);f.pointCount=f.colorCounter=f.symbolCounter=0;f.firstRender()},initSeries:function(a){var b=this.options.chart;(b=M[a.type||b.type||b.defaultSeriesType])||la(17,!0);b=new b;b.init(this,\n"
"a);return b},isInsidePlot:function(a,b,c){var d=c?b:a,a=c?a:b;return d>=0&&d<=this.plotWidth&&a>=0&&a<=this.plotHeight},redraw:function(a){var b=this.axes,c=this.series,d=this.pointer,e=this.legend,f=this.isDirtyLegend,g,h,i=this.hasCartesianSeries,j=this.isDirtyBox,k=c.length,m=k,l=this.renderer,n=l.isHidden(),q=[];Qa(a,this);n&&this.cloneRenderTo();for(this.layOutTitles();m--;)if(a=c[m],a.options.stacking&&(g=!0,a.isDirty)){h=!0;break}if(h)for(m=k;m--;)if(a=c[m],a.options.stacking)a.isDirty=!0;\n"
"o(c,function(a){a.isDirty&&a.options.legendType===\"point\"&&(a.updateTotals&&a.updateTotals(),f=!0)});if(f&&e.options.enabled)e.render(),this.isDirtyLegend=!1;g&&this.getStacks();if(i&&!this.isResizing)this.maxTicks=null,o(b,function(a){a.setScale()});this.getMargins();i&&(o(b,function(a){a.isDirty&&(j=!0)}),o(b,function(a){var b=a.min+\",\"+a.max;if(a.extKey!==b)a.extKey=b,q.push(function(){I(a,\"afterSetExtremes\",r(a.eventArgs,a.getExtremes()));delete a.eventArgs});(j||g)&&a.redraw()}));j&&this.drawChartBox();\n"
"o(c,function(a){a.isDirty&&a.visible&&(!a.isCartesian||a.xAxis)&&a.redraw()});d&&d.reset(!0);l.draw();I(this,\"redraw\");n&&this.cloneRenderTo(!0);o(q,function(a){a.call()})},get:function(a){var b=this.axes,c=this.series,d,e;for(d=0;d<b.length;d++)if(b[d].options.id===a)return b[d];for(d=0;d<c.length;d++)if(c[d].options.id===a)return c[d];for(d=0;d<c.length;d++){e=c[d].points||[];for(b=0;b<e.length;b++)if(e[b].id===a)return e[b]}return null},getAxes:function(){var a=this,b=this.options,c=b.xAxis=ra(b.xAxis||\n"
"{}),b=b.yAxis=ra(b.yAxis||{});o(c,function(a,b){a.index=b;a.isX=!0});o(b,function(a,b){a.index=b});c=c.concat(b);o(c,function(b){new ha(a,b)})},getSelectedPoints:function(){var a=[];o(this.series,function(b){a=a.concat(ib(b.points||[],function(a){return a.selected}))});return a},getSelectedSeries:function(){return ib(this.series,function(a){return a.selected})},setTitle:function(a,b,c){var g;var d=this,e=d.options,f;f=e.title=A(e.title,a);g=e.subtitle=A(e.subtitle,b),e=g;o([[\"title\",a,f],[\"subtitle\",\n"
"b,e]],function(a){var b=a[0],c=d[b],e=a[1],a=a[2];c&&e&&(d[b]=c=c.destroy());a&&a.text&&!c&&(d[b]=d.renderer.text(a.text,0,0,a.useHTML).attr({align:a.align,\"class\":\"highcharts-\"+b,zIndex:a.zIndex||4}).css(a.style).add())});d.layOutTitles(c)},layOutTitles:function(a){var b=0,c=this.title,d=this.subtitle,e=this.options,f=e.title,e=e.subtitle,g=this.renderer,h=this.spacingBox.width-44;if(c&&(c.css({width:(f.width||h)+\"px\"}).align(r({y:g.fontMetrics(f.style.fontSize,c).b-3},f),!1,\"spacingBox\"),!f.floating&&\n"
,
"!f.verticalAlign))b=c.getBBox().height;d&&(d.css({width:(e.width||h)+\"px\"}).align(r({y:b+(f.margin-13)+g.fontMetrics(f.style.fontSize,d).b},e),!1,\"spacingBox\"),!e.floating&&!e.verticalAlign&&(b=sa(b+d.getBBox().height)));c=this.titleOffset!==b;this.titleOffset=b;if(!this.isDirtyBox&&c)this.isDirtyBox=c,this.hasRendered&&p(a,!0)&&this.isDirtyBox&&this.redraw()},getChartSize:function(){var a=this.options.chart,b=a.width,a=a.height,c=this.renderToClone||this.renderTo;if(!s(b))this.containerWidth=hb(c,\n"
"\"width\");if(!s(a))this.containerHeight=hb(c,\"height\");this.chartWidth=t(0,b||this.containerWidth||600);this.chartHeight=t(0,p(a,this.containerHeight>19?this.containerHeight:400))},cloneRenderTo:function(a){var b=this.renderToClone,c=this.container;a?b&&(this.renderTo.appendChild(c),Pa(b),delete this.renderToClone):(c&&c.parentNode===this.renderTo&&this.renderTo.removeChild(c),this.renderToClone=b=this.renderTo.cloneNode(0),L(b,{position:\"absolute\",top:\"-9999px\",display:\"block\"}),b.style.setProperty&&\n"
"b.style.setProperty(\"display\",\"block\",\"important\"),C.body.appendChild(b),c&&b.appendChild(c))},getContainer:function(){var a,b=this.options.chart,c,d,e;this.renderTo=a=b.renderTo;e=\"highcharts-\"+wb++;if(Aa(a))this.renderTo=a=C.getElementById(a);a||la(13,!0);c=F(J(a,\"data-highcharts-chart\"));!isNaN(c)&&Y[c]&&Y[c].hasRendered&&Y[c].destroy();J(a,\"data-highcharts-chart\",this.index);a.innerHTML=\"\";!b.skipClone&&!a.offsetWidth&&this.cloneRenderTo();this.getChartSize();c=this.chartWidth;d=this.chartHeight;\n"
"this.container=a=$(Ia,{className:\"highcharts-container\"+(b.className?\" \"+b.className:\"\"),id:e},r({position:\"relative\",overflow:\"hidden\",width:c+\"px\",height:d+\"px\",textAlign:\"left\",lineHeight:\"normal\",zIndex:0,\"-webkit-tap-highlight-color\":\"rgba(0,0,0,0)\"},b.style),this.renderToClone||a);this._cursor=a.style.cursor;this.renderer=b.forExport?new ua(a,c,d,b.style,!0):new Za(a,c,d,b.style);fa&&this.renderer.create(this,a,c,d);this.renderer.chartIndex=this.index},getMargins:function(a){var b=this.spacing,\n"
"c=this.margin,d=this.titleOffset;this.resetMargins();if(d&&!s(c[0]))this.plotTop=t(this.plotTop,d+this.options.title.margin+b[0]);this.legend.adjustMargins(c,b);this.extraBottomMargin&&(this.marginBottom+=this.extraBottomMargin);this.extraTopMargin&&(this.plotTop+=this.extraTopMargin);a||this.getAxisMargins()},getAxisMargins:function(){var a=this,b=a.axisOffset=[0,0,0,0],c=a.margin;a.hasCartesianSeries&&o(a.axes,function(a){a.getOffset()});o(gb,function(d,e){s(c[e])||(a[d]+=b[e])});a.setChartSize()},\n"
"reflow:function(a){var b=this,c=b.options.chart,d=b.renderTo,e=c.width||hb(d,\"width\"),f=c.height||hb(d,\"height\"),c=a?a.target:K,d=function(){if(b.container)b.setSize(e,f,!1),b.hasUserSize=null};if(!b.hasUserSize&&!b.isPrinting&&e&&f&&(c===K||c===C)){if(e!==b.containerWidth||f!==b.containerHeight)clearTimeout(b.reflowTimeout),a?b.reflowTimeout=setTimeout(d,100):d();b.containerWidth=e;b.containerHeight=f}},initReflow:function(){var a=this,b=function(b){a.reflow(b)};H(K,\"resize\",b);H(a,\"destroy\",function(){Z(K,\n"
"\"resize\",b)})},setSize:function(a,b,c){var d=this,e,f,g,h=d.renderer,i=h.globalAnimation;d.isResizing+=1;g=function(){d&&I(d,\"endResize\",null,function(){d.isResizing-=1})};Qa(c,d);d.oldChartHeight=d.chartHeight;d.oldChartWidth=d.chartWidth;if(s(a))d.chartWidth=e=t(0,w(a)),d.hasUserSize=!!e;if(s(b))d.chartHeight=f=t(0,w(b));(i?jb:L)(d.container,{width:e+\"px\",height:f+\"px\"},i);d.setChartSize(!0);h.setSize(e,f,c);d.maxTicks=null;o(d.axes,function(a){a.isDirty=!0;a.setScale()});o(d.series,function(a){a.isDirty=\n"
"!0});d.isDirtyLegend=!0;d.isDirtyBox=!0;d.layOutTitles();d.getMargins();d.redraw(c);d.oldChartHeight=null;I(d,\"resize\");i===!1?g():setTimeout(g,i&&i.duration||500)},setChartSize:function(a){var b=this.inverted,c=this.renderer,d=this.chartWidth,e=this.chartHeight,f=this.options.chart,g=this.spacing,h=this.clipOffset,i,j,k,m;this.plotLeft=i=w(this.plotLeft);this.plotTop=j=w(this.plotTop);this.plotWidth=k=t(0,w(d-i-this.marginRight));this.plotHeight=m=t(0,w(e-j-this.marginBottom));this.plotSizeX=b?m:\n"
"k;this.plotSizeY=b?k:m;this.plotBorderWidth=f.plotBorderWidth||0;this.spacingBox=c.spacingBox={x:g[3],y:g[0],width:d-g[3]-g[1],height:e-g[0]-g[2]};this.plotBox=c.plotBox={x:i,y:j,width:k,height:m};d=2*V(this.plotBorderWidth/2);b=sa(t(d,h[3])/2);c=sa(t(d,h[0])/2);this.clipBox={x:b,y:c,width:V(this.plotSizeX-t(d,h[1])/2-b),height:t(0,V(this.plotSizeY-t(d,h[2])/2-c))};a||o(this.axes,function(a){a.setAxisSize();a.setAxisTranslation()})},resetMargins:function(){var a=this;o(gb,function(b,c){a[b]=p(a.margin[c],\n"
"a.spacing[c])});a.axisOffset=[0,0,0,0];a.clipOffset=[0,0,0,0]},drawChartBox:function(){var a=this.options.chart,b=this.renderer,c=this.chartWidth,d=this.chartHeight,e=this.chartBackground,f=this.plotBackground,g=this.plotBorder,h=this.plotBGImage,i=a.borderWidth||0,j=a.backgroundColor,k=a.plotBackgroundColor,m=a.plotBackgroundImage,l=a.plotBorderWidth||0,n,q=this.plotLeft,p=this.plotTop,o=this.plotWidth,t=this.plotHeight,u=this.plotBox,s=this.clipRect,r=this.clipBox;n=i+(a.shadow?8:0);if(i||j)if(e)e.animate(e.crisp({width:c-\n"
"n,height:d-n}));else{e={fill:j||O};if(i)e.stroke=a.borderColor,e[\"stroke-width\"]=i;this.chartBackground=b.rect(n/2,n/2,c-n,d-n,a.borderRadius,i).attr(e).addClass(\"highcharts-background\").add().shadow(a.shadow)}if(k)f?f.animate(u):this.plotBackground=b.rect(q,p,o,t,0).attr({fill:k}).add().shadow(a.plotShadow);if(m)h?h.animate(u):this.plotBGImage=b.image(m,q,p,o,t).add();s?s.animate({width:r.width,height:r.height}):this.clipRect=b.clipRect(r);if(l)g?g.animate(g.crisp({x:q,y:p,width:o,height:t,strokeWidth:-l})):\n"
"this.plotBorder=b.rect(q,p,o,t,0,-l).attr({stroke:a.plotBorderColor,\"stroke-width\":l,fill:O,zIndex:1}).add();this.isDirtyBox=!1},propFromSeries:function(){var a=this,b=a.options.chart,c,d=a.options.series,e,f;o([\"inverted\",\"angular\",\"polar\"],function(g){c=M[b.type||b.defaultSeriesType];f=a[g]||b[g]||c&&c.prototype[g];for(e=d&&d.length;!f&&e--;)(c=M[d[e].type])&&c.prototype[g]&&(f=!0);a[g]=f})},linkSeries:function(){var a=this,b=a.series;o(b,function(a){a.linkedSeries.length=0});o(b,function(b){var d=\n"
"b.options.linkedTo;if(Aa(d)&&(d=d===\":previous\"?a.series[b.index-1]:a.get(d)))d.linkedSeries.push(b),b.linkedParent=d,b.visible=p(b.options.visible,d.options.visible,b.visible)})},renderSeries:function(){o(this.series,function(a){a.translate();a.render()})},renderLabels:function(){var a=this,b=a.options.labels;b.items&&o(b.items,function(c){var d=r(b.style,c.style),e=F(d.left)+a.plotLeft,f=F(d.top)+a.plotTop+12;delete d.left;delete d.top;a.renderer.text(c.html,e,f).attr({zIndex:2}).css(d).add()})},\n"
"render:function(){var a=this.axes,b=this.renderer,c=this.options,d,e,f,g;this.setTitle();this.legend=new lb(this,c.legend);this.getStacks&&this.getStacks();this.getMargins(!0);this.setChartSize();d=this.plotWidth;e=this.plotHeight-=13;o(a,function(a){a.setScale()});this.getAxisMargins();f=d/this.plotWidth>1.1;g=e/this.plotHeight>1.1;if(f||g)this.maxTicks=null,o(a,function(a){(a.horiz&&f||!a.horiz&&g)&&a.setTickInterval(!0)}),this.getMargins();this.drawChartBox();this.hasCartesianSeries&&o(a,function(a){a.render()});\n"
"if(!this.seriesGroup)this.seriesGroup=b.g(\"series-group\").attr({zIndex:3}).add();this.renderSeries();this.renderLabels();this.showCredits(c.credits);this.hasRendered=!0},showCredits:function(a){if(a.enabled&&!this.credits)this.credits=this.renderer.text(a.text,0,0).on(\"click\",function(){if(a.href)location.href=a.href}).attr({align:a.position.align,zIndex:8}).css(a.style).add().align(a.position)},destroy:function(){var a=this,b=a.axes,c=a.series,d=a.container,e,f=d&&d.parentNode;I(a,\"destroy\");Y[a.index]=\n"
"v;ab--;a.renderTo.removeAttribute(\"data-highcharts-chart\");Z(a);for(e=b.length;e--;)b[e]=b[e].destroy();for(e=c.length;e--;)c[e]=c[e].destroy();o(\"title,subtitle,chartBackground,plotBackground,plotBGImage,plotBorder,seriesGroup,clipRect,credits,pointer,scroller,rangeSelector,legend,resetZoomButton,tooltip,renderer\".split(\",\"),function(b){var c=a[b];c&&c.destroy&&(a[b]=c.destroy())});if(d)d.innerHTML=\"\",Z(d),f&&Pa(d);for(e in a)delete a[e]},isReadyToRender:function(){var a=this;return!ca&&K==K.top&&\n"
"C.readyState!==\"complete\"||fa&&!K.canvg?(fa?Lb.push(function(){a.firstRender()},a.options.global.canvasToolsURL):C.attachEvent(\"onreadystatechange\",function(){C.detachEvent(\"onreadystatechange\",a.firstRender);C.readyState===\"complete\"&&a.firstRender()}),!1):!0},firstRender:function(){var a=this,b=a.options,c=a.callback;if(a.isReadyToRender()){a.getContainer();I(a,\"init\");a.resetMargins();a.setChartSize();a.propFromSeries();a.getAxes();o(b.series||[],function(b){a.initSeries(b)});a.linkSeries();I(a,\n"
"\"beforeRender\");if(B.Pointer)a.pointer=new Ua(a,b);a.render();a.renderer.draw();c&&c.apply(a,[a]);o(a.callbacks,function(b){a.index!==v&&b.apply(a,[a])});I(a,\"load\");a.cloneRenderTo(!0)}},splashArray:function(a,b){var c=b[a],c=da(c)?c:[c,c,c,c];return[p(b[a+\"Top\"],c[0]),p(b[a+\"Right\"],c[1]),p(b[a+\"Bottom\"],c[2]),p(b[a+\"Left\"],c[3])]}};var Xb=B.CenteredSeriesMixin={getCenter:function(){var a=this.options,b=this.chart,c=2*(a.slicedOffset||0),d=b.plotWidth-2*c,b=b.plotHeight-2*c,e=a.center,e=[p(e[0],\n"
"\"50%\"),p(e[1],\"50%\"),a.size||\"100%\",a.innerSize||0],f=z(d,b),g,h;for(g=0;g<4;++g)h=e[g],a=g<2||g===2&&/%$/.test(h),e[g]=(/%$/.test(h)?[d,b,f,e[2]][g]*parseFloat(h)/100:parseFloat(h))+(a?c:0);return e}},Ea=function(){};Ea.prototype={init:function(a,b,c){this.series=a;this.color=a.color;this.applyOptions(b,c);this.pointAttr={};if(a.options.colorByPoint&&(b=a.options.colors||a.chart.options.colors,this.color=this.color||b[a.colorCounter++],a.colorCounter===b.length))a.colorCounter=0;a.chart.pointCount++;\n"
,
"return this},applyOptions:function(a,b){var c=this.series,d=c.options.pointValKey||c.pointValKey,a=Ea.prototype.optionsToObject.call(this,a);r(this,a);this.options=this.options?r(this.options,a):a;if(d)this.y=this[d];if(this.x===v&&c)this.x=b===v?c.autoIncrement():b;return this},optionsToObject:function(a){var b={},c=this.series,d=c.options.keys,e=d||c.pointArrayMap||[\"y\"],f=e.length,g=0,h=0;if(typeof a===\"number\"||a===null)b[e[0]]=a;else if(Fa(a)){if(!d&&a.length>f){c=typeof a[0];if(c===\"string\")b.name=\n"
"a[0];else if(c===\"number\")b.x=a[0];g++}for(;h<f;)b[e[h++]]=a[g++]}else if(typeof a===\"object\"){b=a;if(a.dataLabels)c._hasPointLabels=!0;if(a.marker)c._hasPointMarkers=!0}return b},destroy:function(){var a=this.series.chart,b=a.hoverPoints,c;a.pointCount--;if(b&&(this.setState(),ja(b,this),!b.length))a.hoverPoints=null;if(this===a.hoverPoint)this.onMouseOut();if(this.graphic||this.dataLabel)Z(this),this.destroyElements();this.legendItem&&a.legend.destroyItem(this);for(c in this)this[c]=null},destroyElements:function(){for(var a=\n"
"\"graphic,dataLabel,dataLabelUpper,group,connector,shadowGroup\".split(\",\"),b,c=6;c--;)b=a[c],this[b]&&(this[b]=this[b].destroy())},getLabelConfig:function(){return{x:this.category,y:this.y,color:this.color,key:this.name||this.category,series:this.series,point:this,percentage:this.percentage,total:this.total||this.stackTotal}},tooltipFormatter:function(a){var b=this.series,c=b.tooltipOptions,d=p(c.valueDecimals,\"\"),e=c.valuePrefix||\"\",f=c.valueSuffix||\"\";o(b.pointArrayMap||[\"y\"],function(b){b=\"{point.\"+\n"
"b;if(e||f)a=a.replace(b+\"}\",e+b+\"}\"+f);a=a.replace(b+\"}\",b+\":,.\"+d+\"f}\")});return Ha(a,{point:this,series:this.series})},firePointEvent:function(a,b,c){var d=this,e=this.series.options;(e.point.events[a]||d.options&&d.options.events&&d.options.events[a])&&this.importEvents();a===\"click\"&&e.allowPointSelect&&(c=function(a){d.select&&d.select(null,a.ctrlKey||a.metaKey||a.shiftKey)});I(this,a,b,c)}};var R=B.Series=function(){};R.prototype={isCartesian:!0,type:\"line\",pointClass:Ea,sorted:!0,requireSorting:!0,\n"
"pointAttrToOptions:{stroke:\"lineColor\",\"stroke-width\":\"lineWidth\",fill:\"fillColor\",r:\"radius\"},axisTypes:[\"xAxis\",\"yAxis\"],colorCounter:0,parallelArrays:[\"x\",\"y\"],init:function(a,b){var c=this,d,e,f=a.series,g=function(a,b){return p(a.options.index,a._i)-p(b.options.index,b._i)};c.chart=a;c.options=b=c.setOptions(b);c.linkedSeries=[];c.bindAxes();r(c,{name:b.name,state:\"\",pointAttr:{},visible:b.visible!==!1,selected:b.selected===!0});if(fa)b.animation=!1;e=b.events;for(d in e)H(c,d,e[d]);if(e&&e.click||\n"
"b.point&&b.point.events&&b.point.events.click||b.allowPointSelect)a.runTrackerClick=!0;c.getColor();c.getSymbol();o(c.parallelArrays,function(a){c[a+\"Data\"]=[]});c.setData(b.data,!1);if(c.isCartesian)a.hasCartesianSeries=!0;f.push(c);c._i=f.length-1;pb(f,g);this.yAxis&&pb(this.yAxis.series,g);o(f,function(a,b){a.index=b;a.name=a.name||\"Series \"+(b+1)})},bindAxes:function(){var a=this,b=a.options,c=a.chart,d;o(a.axisTypes||[],function(e){o(c[e],function(c){d=c.options;if(b[e]===d.index||b[e]!==v&&\n"
"b[e]===d.id||b[e]===v&&d.index===0)c.series.push(a),a[e]=c,c.isDirty=!0});!a[e]&&a.optionalAxis!==e&&la(18,!0)})},updateParallelArrays:function(a,b){var c=a.series,d=arguments;o(c.parallelArrays,typeof b===\"number\"?function(d){var f=d===\"y\"&&c.toYData?c.toYData(a):a[d];c[d+\"Data\"][b]=f}:function(a){Array.prototype[b].apply(c[a+\"Data\"],Array.prototype.slice.call(d,2))})},autoIncrement:function(){var a=this.options,b=this.xIncrement,c,d=a.pointIntervalUnit,b=p(b,a.pointStart,0);this.pointInterval=c=\n"
"p(this.pointInterval,a.pointInterval,1);if(d===\"month\"||d===\"year\")a=new ya(b),a=d===\"month\"?+a[ub](a[Xa]()+c):+a[vb](a[Ya]()+c),c=a-b;this.xIncrement=b+c;return b},getSegments:function(){var a=-1,b=[],c,d=this.points,e=d.length;if(e)if(this.options.connectNulls){for(c=e;c--;)d[c].y===null&&d.splice(c,1);d.length&&(b=[d])}else o(d,function(c,g){c.y===null?(g>a+1&&b.push(d.slice(a+1,g)),a=g):g===e-1&&b.push(d.slice(a+1,g+1))});this.segments=b},setOptions:function(a){var b=this.chart,c=b.options.plotOptions,\n"
"b=b.userOptions||{},d=b.plotOptions||{},e=c[this.type];this.userOptions=a;c=A(e,c.series,a);this.tooltipOptions=A(S.tooltip,S.plotOptions[this.type].tooltip,b.tooltip,d.series&&d.series.tooltip,d[this.type]&&d[this.type].tooltip,a.tooltip);e.marker===null&&delete c.marker;this.zoneAxis=c.zoneAxis;a=this.zones=(c.zones||[]).slice();if((c.negativeColor||c.negativeFillColor)&&!c.zones)a.push({value:c[this.zoneAxis+\"Threshold\"]||c.threshold||0,color:c.negativeColor,fillColor:c.negativeFillColor});a.length&&\n"
"s(a[a.length-1].value)&&a.push({color:this.color,fillColor:this.fillColor});return c},getCyclic:function(a,b,c){var d=this.userOptions,e=\"_\"+a+\"Index\",f=a+\"Counter\";b||(s(d[e])?b=d[e]:(d[e]=b=this.chart[f]%c.length,this.chart[f]+=1),b=c[b]);this[a]=b},getColor:function(){this.options.colorByPoint?this.options.color=null:this.getCyclic(\"color\",this.options.color||ba[this.type].color,this.chart.options.colors)},getSymbol:function(){var a=this.options.marker;this.getCyclic(\"symbol\",a.symbol,this.chart.options.symbols);\n"
"if(/^url/.test(this.symbol))a.radius=0},drawLegendSymbol:La.drawLineMarker,setData:function(a,b,c,d){var e=this,f=e.points,g=f&&f.length||0,h,i=e.options,j=e.chart,k=null,m=e.xAxis,l=m&&!!m.categories,n=i.turboThreshold,q=this.xData,x=this.yData,t=(h=e.pointArrayMap)&&h.length,a=a||[];h=a.length;b=p(b,!0);if(d!==!1&&h&&g===h&&!e.cropped&&!e.hasGroupedData&&e.visible)o(a,function(a,b){f[b].update&&f[b].update(a,!1,null,!1)});else{e.xIncrement=null;e.pointRange=l?1:i.pointRange;e.colorCounter=0;o(this.parallelArrays,\n"
"function(a){e[a+\"Data\"].length=0});if(n&&h>n){for(c=0;k===null&&c<h;)k=a[c],c++;if(qa(k)){l=p(i.pointStart,0);i=p(i.pointInterval,1);for(c=0;c<h;c++)q[c]=l,x[c]=a[c],l+=i;e.xIncrement=l}else if(Fa(k))if(t)for(c=0;c<h;c++)i=a[c],q[c]=i[0],x[c]=i.slice(1,t+1);else for(c=0;c<h;c++)i=a[c],q[c]=i[0],x[c]=i[1];else la(12)}else for(c=0;c<h;c++)if(a[c]!==v&&(i={series:e},e.pointClass.prototype.applyOptions.apply(i,[a[c]]),e.updateParallelArrays(i,c),l&&s(i.name)))m.names[i.x]=i.name;Aa(x[0])&&la(14,!0);e.data=\n"
"[];e.options.data=a;for(c=g;c--;)f[c]&&f[c].destroy&&f[c].destroy();if(m)m.minRange=m.userMinRange;e.isDirty=e.isDirtyData=j.isDirtyBox=!0;c=!1}b&&j.redraw(c)},processData:function(a){var b=this.xData,c=this.yData,d=b.length,e;e=0;var f,g,h=this.xAxis,i,j=this.options;i=j.cropThreshold;var k=this.isCartesian,m,l;if(k&&!this.isDirty&&!h.isDirty&&!this.yAxis.isDirty&&!a)return!1;if(h)a=h.getExtremes(),m=a.min,l=a.max;if(k&&this.sorted&&(!i||d>i||this.forceCrop))if(b[d-1]<m||b[0]>l)b=[],c=[];else if(b[0]<\n"
"m||b[d-1]>l)e=this.cropData(this.xData,this.yData,m,l),b=e.xData,c=e.yData,e=e.start,f=!0;for(i=b.length-1;i>=0;i--)d=b[i]-b[i-1],d>0&&(g===v||d<g)?g=d:d<0&&this.requireSorting&&la(15);this.cropped=f;this.cropStart=e;this.processedXData=b;this.processedYData=c;if(j.pointRange===null)this.pointRange=g||1;this.closestPointRange=g},cropData:function(a,b,c,d){var e=a.length,f=0,g=e,h=p(this.cropShoulder,1),i;for(i=0;i<e;i++)if(a[i]>=c){f=t(0,i-h);break}for(;i<e;i++)if(a[i]>d){g=i+h;break}return{xData:a.slice(f,\n"
"g),yData:b.slice(f,g),start:f,end:g}},generatePoints:function(){var a=this.options.data,b=this.data,c,d=this.processedXData,e=this.processedYData,f=this.pointClass,g=d.length,h=this.cropStart||0,i,j=this.hasGroupedData,k,m=[],l;if(!b&&!j)b=[],b.length=a.length,b=this.data=b;for(l=0;l<g;l++)i=h+l,j?m[l]=(new f).init(this,[d[l]].concat(ra(e[l]))):(b[i]?k=b[i]:a[i]!==v&&(b[i]=k=(new f).init(this,a[i],d[l])),m[l]=k),m[l].index=i;if(b&&(g!==(c=b.length)||j))for(l=0;l<c;l++)if(l===h&&!j&&(l+=g),b[l])b[l].destroyElements(),\n"
"b[l].plotX=v;this.data=b;this.points=m},getExtremes:function(a){var b=this.yAxis,c=this.processedXData,d,e=[],f=0;d=this.xAxis.getExtremes();var g=d.min,h=d.max,i,j,k,m,a=a||this.stackedYData||this.processedYData;d=a.length;for(m=0;m<d;m++)if(j=c[m],k=a[m],i=k!==null&&k!==v&&(!b.isLog||k.length||k>0),j=this.getExtremesFromAll||this.options.getExtremesFromAll||this.cropped||(c[m+1]||j)>=g&&(c[m-1]||j)<=h,i&&j)if(i=k.length)for(;i--;)k[i]!==null&&(e[f++]=k[i]);else e[f++]=k;this.dataMin=Na(e);this.dataMax=\n"
"Ca(e)},translate:function(){this.processedXData||this.processData();this.generatePoints();for(var a=this.options,b=a.stacking,c=this.xAxis,d=c.categories,e=this.yAxis,f=this.points,g=f.length,h=!!this.modifyValue,i=a.pointPlacement,j=i===\"between\"||qa(i),k=a.threshold,m=a.startFromThreshold?k:0,l,n,q,o=Number.MAX_VALUE,a=0;a<g;a++){var r=f[a],w=r.x,u=r.y;n=r.low;var y=b&&e.stacks[(this.negStacks&&u<(m?0:k)?\"-\":\"\")+this.stackKey];if(e.isLog&&u!==null&&u<=0)r.y=u=null,la(10);r.plotX=l=z(t(-1E5,c.translate(w,\n"
"0,0,0,1,i,this.type===\"flags\")),1E5);if(b&&this.visible&&y&&y[w])y=y[w],u=y.points[this.index+\",\"+a],n=u[0],u=u[1],n===m&&(n=p(k,e.min)),e.isLog&&n<=0&&(n=null),r.total=r.stackTotal=y.total,r.percentage=y.total&&r.y/y.total*100,r.stackY=u,y.setOffset(this.pointXOffset||0,this.barW||0);r.yBottom=s(n)?e.translate(n,0,1,0,1):null;h&&(u=this.modifyValue(u,r));r.plotY=n=typeof u===\"number\"&&u!==Infinity?z(t(-1E5,e.translate(u,0,1,0,1)),1E5):v;r.isInside=n!==v&&n>=0&&n<=e.len&&l>=0&&l<=c.len;r.clientX=\n"
"j?c.translate(w,0,0,0,1):l;r.negative=r.y<(k||0);r.category=d&&d[r.x]!==v?d[r.x]:r.x;a&&(o=z(o,N(l-q)));q=l}this.closestPointRangePx=o;this.getSegments()},setClip:function(a){var b=this.chart,c=b.renderer,d=b.inverted,e=this.clipBox,f=e||b.clipBox,g=this.sharedClipKey||[\"_sharedClip\",a&&a.duration,a&&a.easing,f.height].join(\",\"),h=b[g],i=b[g+\"m\"];if(!h){if(a)f.width=0,b[g+\"m\"]=i=c.clipRect(-99,d?-b.plotLeft:-b.plotTop,99,d?b.chartWidth:b.chartHeight);b[g]=h=c.clipRect(f)}a&&(h.count+=1);if(this.options.clip!==\n"
,
"!1)this.group.clip(a||e?h:b.clipRect),this.markerGroup.clip(i),this.sharedClipKey=g;a||(h.count-=1,h.count<=0&&g&&b[g]&&(e||(b[g]=b[g].destroy()),b[g+\"m\"]&&(b[g+\"m\"]=b[g+\"m\"].destroy())))},animate:function(a){var b=this.chart,c=this.options.animation,d;if(c&&!da(c))c=ba[this.type].animation;a?this.setClip(c):(d=this.sharedClipKey,(a=b[d])&&a.animate({width:b.plotSizeX},c),b[d+\"m\"]&&b[d+\"m\"].animate({width:b.plotSizeX+99},c),this.animate=null)},afterAnimate:function(){this.setClip();I(this,\"afterAnimate\")},\n"
"drawPoints:function(){var a,b=this.points,c=this.chart,d,e,f,g,h,i,j,k,m=this.options.marker,l=this.pointAttr[\"\"],n,q,o,t=this.markerGroup,s=p(m.enabled,this.xAxis.isRadial,this.closestPointRangePx>2*m.radius);if(m.enabled!==!1||this._hasPointMarkers)for(f=b.length;f--;)if(g=b[f],d=V(g.plotX),e=g.plotY,k=g.graphic,n=g.marker||{},q=!!g.marker,a=s&&n.enabled===v||n.enabled,o=g.isInside,a&&e!==v&&!isNaN(e)&&g.y!==null)if(a=g.pointAttr[g.selected?\"select\":\"\"]||l,h=a.r,i=p(n.symbol,this.symbol),j=i.indexOf(\"url\")===\n"
"0,k)k[o?\"show\":\"hide\"](!0).animate(r({x:d-h,y:e-h},k.symbolName?{width:2*h,height:2*h}:{}));else{if(o&&(h>0||j))g.graphic=c.renderer.symbol(i,d-h,e-h,2*h,2*h,q?n:m).attr(a).add(t)}else if(k)g.graphic=k.destroy()},convertAttribs:function(a,b,c,d){var e=this.pointAttrToOptions,f,g,h={},a=a||{},b=b||{},c=c||{},d=d||{};for(f in e)g=e[f],h[f]=p(a[g],b[f],c[f],d[f]);return h},getAttribs:function(){var a=this,b=a.options,c=ba[a.type].marker?b.marker:b,d=c.states,e=d.hover,f,g=a.color,h=a.options.negativeColor;\n"
"f={stroke:g,fill:g};var i=a.points||[],j,k,m=[],l=a.pointAttrToOptions;j=a.hasPointSpecificOptions;var n=c.lineColor,q=c.fillColor;k=b.turboThreshold;var p=a.zones,t=a.zoneAxis||\"y\",w;b.marker?(e.radius=e.radius||c.radius+e.radiusPlus,e.lineWidth=e.lineWidth||c.lineWidth+e.lineWidthPlus):(e.color=e.color||na(e.color||g).brighten(e.brightness).get(),e.negativeColor=e.negativeColor||na(e.negativeColor||h).brighten(e.brightness).get());m[\"\"]=a.convertAttribs(c,f);o([\"hover\",\"select\"],function(b){m[b]=\n"
"a.convertAttribs(d[b],m[\"\"])});a.pointAttr=m;g=i.length;if(!k||g<k||j)for(;g--;){k=i[g];if((c=k.options&&k.options.marker||k.options)&&c.enabled===!1)c.radius=0;if(p.length){j=0;for(f=p[j];k[t]>=f.value;)f=p[++j];if(f.color)k.color=k.fillColor=f.color}j=b.colorByPoint||k.color;if(k.options)for(w in l)s(c[l[w]])&&(j=!0);if(j){c=c||{};j=[];d=c.states||{};f=d.hover=d.hover||{};if(!b.marker)f.color=f.color||!k.options.color&&e[k.negative&&h?\"negativeColor\":\"color\"]||na(k.color).brighten(f.brightness||\n"
"e.brightness).get();f={color:k.color};if(!q)f.fillColor=k.color;if(!n)f.lineColor=k.color;c.hasOwnProperty(\"color\")&&!c.color&&delete c.color;j[\"\"]=a.convertAttribs(r(f,c),m[\"\"]);j.hover=a.convertAttribs(d.hover,m.hover,j[\"\"]);j.select=a.convertAttribs(d.select,m.select,j[\"\"])}else j=m;k.pointAttr=j}},destroy:function(){var a=this,b=a.chart,c=/AppleWebKit\\/533/.test(za),d,e=a.data||[],f,g,h;I(a,\"destroy\");Z(a);o(a.axisTypes||[],function(b){if(h=a[b])ja(h.series,a),h.isDirty=h.forceRedraw=!0});a.legendItem&&\n"
"a.chart.legend.destroyItem(a);for(d=e.length;d--;)(f=e[d])&&f.destroy&&f.destroy();a.points=null;clearTimeout(a.animationTimeout);for(g in a)a[g]instanceof P&&!a[g].survive&&(d=c&&g===\"group\"?\"hide\":\"destroy\",a[g][d]());if(b.hoverSeries===a)b.hoverSeries=null;ja(b.series,a);for(g in a)delete a[g]},getSegmentPath:function(a){var b=this,c=[],d=b.options.step;o(a,function(e,f){var g=e.plotX,h=e.plotY,i;b.getPointSpline?c.push.apply(c,b.getPointSpline(a,e,f)):(c.push(f?\"L\":\"M\"),d&&f&&(i=a[f-1],d===\"right\"?\n"
"c.push(i.plotX,h):d===\"center\"?c.push((i.plotX+g)/2,i.plotY,(i.plotX+g)/2,h):c.push(g,i.plotY)),c.push(e.plotX,e.plotY))});return c},getGraphPath:function(){var a=this,b=[],c,d=[];o(a.segments,function(e){c=a.getSegmentPath(e);e.length>1?b=b.concat(c):d.push(e[0])});a.singlePoints=d;return a.graphPath=b},drawGraph:function(){var a=this,b=this.options,c=[[\"graph\",b.lineColor||this.color,b.dashStyle]],d=b.lineWidth,e=b.linecap!==\"square\",f=this.getGraphPath(),g=this.fillGraph&&this.color||O;o(this.zones,\n"
"function(d,e){c.push([\"zoneGraph\"+e,d.color||a.color,d.dashStyle||b.dashStyle])});o(c,function(c,i){var j=c[0],k=a[j];if(k)k.animate({d:f});else if((d||g)&&f.length)k={stroke:c[1],\"stroke-width\":d,fill:g,zIndex:1},c[2]?k.dashstyle=c[2]:e&&(k[\"stroke-linecap\"]=k[\"stroke-linejoin\"]=\"round\"),a[j]=a.chart.renderer.path(f).attr(k).add(a.group).shadow(i<2&&b.shadow)})},applyZones:function(){var a=this,b=this.chart,c=b.renderer,d=this.zones,e,f,g=this.clips||[],h,i=this.graph,j=this.area,k=t(b.chartWidth,\n"
"b.chartHeight),m=this[(this.zoneAxis||\"y\")+\"Axis\"],l,n=m.reversed,q=b.inverted,x=m.horiz,r,s,u,y=!1;if(d.length&&(i||j)&&m.min!==v)i&&i.hide(),j&&j.hide(),l=m.getExtremes(),o(d,function(d,o){e=n?x?b.plotWidth:0:x?0:m.toPixels(l.min);e=z(t(p(f,e),0),k);f=z(t(w(m.toPixels(p(d.value,l.max),!0)),0),k);y&&(e=f=m.toPixels(l.max));r=Math.abs(e-f);s=z(e,f);u=t(e,f);if(m.isXAxis){if(h={x:q?u:s,y:0,width:r,height:k},!x)h.x=b.plotHeight-h.x}else if(h={x:0,y:q?u:s,width:k,height:r},x)h.y=b.plotWidth-h.y;b.inverted&&\n"
"c.isVML&&(h=m.isXAxis?{x:0,y:n?s:u,height:h.width,width:b.chartWidth}:{x:h.y-b.plotLeft-b.spacingBox.x,y:0,width:h.height,height:b.chartHeight});g[o]?g[o].animate(h):(g[o]=c.clipRect(h),i&&a[\"zoneGraph\"+o].clip(g[o]),j&&a[\"zoneArea\"+o].clip(g[o]));y=d.value>l.max}),this.clips=g},invertGroups:function(){function a(){var a={width:b.yAxis.len,height:b.xAxis.len};o([\"group\",\"markerGroup\"],function(c){b[c]&&b[c].attr(a).invert()})}var b=this,c=b.chart;if(b.xAxis)H(c,\"resize\",a),H(b,\"destroy\",function(){Z(c,\n"
"\"resize\",a)}),a(),b.invertGroups=a},plotGroup:function(a,b,c,d,e){var f=this[a],g=!f;g&&(this[a]=f=this.chart.renderer.g(b).attr({visibility:c,zIndex:d||0.1}).add(e));f[g?\"attr\":\"animate\"](this.getPlotBox());return f},getPlotBox:function(){var a=this.chart,b=this.xAxis,c=this.yAxis;if(a.inverted)b=c,c=this.xAxis;return{translateX:b?b.left:a.plotLeft,translateY:c?c.top:a.plotTop,scaleX:1,scaleY:1}},render:function(){var a=this,b=a.chart,c,d=a.options,e=(c=d.animation)&&!!a.animate&&b.renderer.isSVG&&\n"
"p(c.duration,500)||0,f=a.visible?\"visible\":\"hidden\",g=d.zIndex,h=a.hasRendered,i=b.seriesGroup;c=a.plotGroup(\"group\",\"series\",f,g,i);a.markerGroup=a.plotGroup(\"markerGroup\",\"markers\",f,g,i);e&&a.animate(!0);a.getAttribs();c.inverted=a.isCartesian?b.inverted:!1;a.drawGraph&&(a.drawGraph(),a.applyZones());o(a.points,function(a){a.redraw&&a.redraw()});a.drawDataLabels&&a.drawDataLabels();a.visible&&a.drawPoints();a.drawTracker&&a.options.enableMouseTracking!==!1&&a.drawTracker();b.inverted&&a.invertGroups();\n"
"d.clip!==!1&&!a.sharedClipKey&&!h&&c.clip(b.clipRect);e&&a.animate();if(!h)e?a.animationTimeout=setTimeout(function(){a.afterAnimate()},e):a.afterAnimate();a.isDirty=a.isDirtyData=!1;a.hasRendered=!0},redraw:function(){var a=this.chart,b=this.isDirtyData,c=this.isDirty,d=this.group,e=this.xAxis,f=this.yAxis;d&&(a.inverted&&d.attr({width:a.plotWidth,height:a.plotHeight}),d.animate({translateX:p(e&&e.left,a.plotLeft),translateY:p(f&&f.top,a.plotTop)}));this.translate();this.render();b&&I(this,\"updatedData\");\n"
"(c||b)&&delete this.kdTree},kdDimensions:1,kdAxisArray:[\"clientX\",\"plotY\"],searchPoint:function(a,b){var c=this.xAxis,d=this.yAxis,e=this.chart.inverted;return this.searchKDTree({clientX:e?c.len-a.chartY+c.pos:a.chartX-c.pos,plotY:e?d.len-a.chartX+d.pos:a.chartY-d.pos},b)},buildKDTree:function(){function a(b,d,g){var h,i;if(i=b&&b.length)return h=c.kdAxisArray[d%g],b.sort(function(a,b){return a[h]-b[h]}),i=Math.floor(i/2),{point:b[i],left:a(b.slice(0,i),d+1,g),right:a(b.slice(i+1),d+1,g)}}function b(){var b=\n"
"ib(c.points||[],function(a){return a.y!==null});c.kdTree=a(b,d,d)}var c=this,d=c.kdDimensions;delete c.kdTree;c.options.kdSync?b():setTimeout(b)},searchKDTree:function(a,b){function c(a,b,j,k){var m=b.point,l=d.kdAxisArray[j%k],n,q,p=m;q=s(a[e])&&s(m[e])?Math.pow(a[e]-m[e],2):null;n=s(a[f])&&s(m[f])?Math.pow(a[f]-m[f],2):null;n=(q||0)+(n||0);m.dist=s(n)?Math.sqrt(n):Number.MAX_VALUE;m.distX=s(q)?Math.sqrt(q):Number.MAX_VALUE;l=a[l]-m[l];n=l<0?\"left\":\"right\";q=l<0?\"right\":\"left\";b[n]&&(n=c(a,b[n],\n"
"j+1,k),p=n[g]<p[g]?n:m);b[q]&&Math.sqrt(l*l)<p[g]&&(a=c(a,b[q],j+1,k),p=a[g]<p[g]?a:p);return p}var d=this,e=this.kdAxisArray[0],f=this.kdAxisArray[1],g=b?\"distX\":\"dist\";this.kdTree||this.buildKDTree();if(this.kdTree)return c(a,this.kdTree,this.kdDimensions,this.kdDimensions)}};Hb.prototype={destroy:function(){Oa(this,this.axis)},render:function(a){var b=this.options,c=b.format,c=c?Ha(c,this):b.formatter.call(this);this.label?this.label.attr({text:c,visibility:\"hidden\"}):this.label=this.axis.chart.renderer.text(c,\n"
"null,null,b.useHTML).css(b.style).attr({align:this.textAlign,rotation:b.rotation,visibility:\"hidden\"}).add(a)},setOffset:function(a,b){var c=this.axis,d=c.chart,e=d.inverted,f=c.reversed,f=this.isNegative&&!f||!this.isNegative&&f,g=c.translate(c.usePercentage?100:this.total,0,0,0,1),c=c.translate(0),c=N(g-c),h=d.xAxis[0].translate(this.x)+a,i=d.plotHeight,f={x:e?f?g:g-c:h,y:e?i-h-b:f?i-g-c:i-g,width:e?c:b,height:e?b:c};if(e=this.label)e.align(this.alignOptions,null,f),f=e.alignAttr,e[this.options.crop===\n"
"!1||d.isInsidePlot(f.x,f.y)?\"show\":\"hide\"](!0)}};D.prototype.getStacks=function(){var a=this;o(a.yAxis,function(a){if(a.stacks&&a.hasVisibleSeries)a.oldStacks=a.stacks});o(a.series,function(b){if(b.options.stacking&&(b.visible===!0||a.options.chart.ignoreHiddenSeries===!1))b.stackKey=b.type+p(b.options.stack,\"\")})};ha.prototype.buildStacks=function(){var a=this.series,b=p(this.options.reversedStacks,!0),c=a.length;if(!this.isXAxis){for(this.usePercentage=!1;c--;)a[b?c:a.length-c-1].setStackedPoints();\n"
,
"if(this.usePercentage)for(c=0;c<a.length;c++)a[c].setPercentStacks()}};ha.prototype.renderStackTotals=function(){var a=this.chart,b=a.renderer,c=this.stacks,d,e,f=this.stackTotalGroup;if(!f)this.stackTotalGroup=f=b.g(\"stack-labels\").attr({visibility:\"visible\",zIndex:6}).add();f.translate(a.plotLeft,a.plotTop);for(d in c)for(e in a=c[d],a)a[e].render(f)};ha.prototype.resetStacks=function(){var a=this.stacks,b,c;if(!this.isXAxis)for(b in a)for(c in a[b])a[b][c].touched<this.stacksTouched?(a[b][c].destroy(),\n"
"delete a[b][c]):(a[b][c].total=null,a[b][c].cum=0)};ha.prototype.cleanStacks=function(){var a,b,c;if(!this.isXAxis){if(this.oldStacks)a=this.stacks=this.oldStacks;for(b in a)for(c in a[b])a[b][c].cum=a[b][c].total}};R.prototype.setStackedPoints=function(){if(this.options.stacking&&!(this.visible!==!0&&this.chart.options.chart.ignoreHiddenSeries!==!1)){var a=this.processedXData,b=this.processedYData,c=[],d=b.length,e=this.options,f=e.threshold,g=e.startFromThreshold?f:0,h=e.stack,e=e.stacking,i=this.stackKey,\n"
"j=\"-\"+i,k=this.negStacks,m=this.yAxis,l=m.stacks,n=m.oldStacks,q,o,r,s,u,y;m.stacksTouched+=1;for(s=0;s<d;s++){u=a[s];y=b[s];r=this.index+\",\"+s;o=(q=k&&y<(g?0:f))?j:i;l[o]||(l[o]={});if(!l[o][u])n[o]&&n[o][u]?(l[o][u]=n[o][u],l[o][u].total=null):l[o][u]=new Hb(m,m.options.stackLabels,q,u,h);o=l[o][u];o.points[r]=[p(o.cum,g)];o.touched=m.stacksTouched;e===\"percent\"?(q=q?i:j,k&&l[q]&&l[q][u]?(q=l[q][u],o.total=q.total=t(q.total,o.total)+N(y)||0):o.total=ea(o.total+(N(y)||0))):o.total=ea(o.total+(y||\n"
"0));o.cum=p(o.cum,g)+(y||0);o.points[r].push(o.cum);c[s]=o.cum}if(e===\"percent\")m.usePercentage=!0;this.stackedYData=c;m.oldStacks={}}};R.prototype.setPercentStacks=function(){var a=this,b=a.stackKey,c=a.yAxis.stacks,d=a.processedXData;o([b,\"-\"+b],function(b){var e;for(var f=d.length,g,h;f--;)if(g=d[f],e=(h=c[b]&&c[b][g])&&h.points[a.index+\",\"+f],g=e)h=h.total?100/h.total:0,g[0]=ea(g[0]*h),g[1]=ea(g[1]*h),a.stackedYData[f]=g[1]})};r(D.prototype,{addSeries:function(a,b,c){var d,e=this;a&&(b=p(b,!0),\n"
"I(e,\"addSeries\",{options:a},function(){d=e.initSeries(a);e.isDirtyLegend=!0;e.linkSeries();b&&e.redraw(c)}));return d},addAxis:function(a,b,c,d){var e=b?\"xAxis\":\"yAxis\",f=this.options;new ha(this,A(a,{index:this[e].length,isX:b}));f[e]=ra(f[e]||{});f[e].push(a);p(c,!0)&&this.redraw(d)},showLoading:function(a){var b=this,c=b.options,d=b.loadingDiv,e=c.loading,f=function(){d&&L(d,{left:b.plotLeft+\"px\",top:b.plotTop+\"px\",width:b.plotWidth+\"px\",height:b.plotHeight+\"px\"})};if(!d)b.loadingDiv=d=$(Ia,{className:\"highcharts-loading\"},\n"
"r(e.style,{zIndex:10,display:O}),b.container),b.loadingSpan=$(\"span\",null,e.labelStyle,d),H(b,\"redraw\",f);b.loadingSpan.innerHTML=a||c.lang.loading;if(!b.loadingShown)L(d,{opacity:0,display:\"\"}),jb(d,{opacity:e.style.opacity},{duration:e.showDuration||0}),b.loadingShown=!0;f()},hideLoading:function(){var a=this.options,b=this.loadingDiv;b&&jb(b,{opacity:0},{duration:a.loading.hideDuration||100,complete:function(){L(b,{display:O})}});this.loadingShown=!1}});r(Ea.prototype,{update:function(a,b,c,d){function e(){f.applyOptions(a);\n"
"if(f.y===null&&h)f.graphic=h.destroy();if(da(a)&&!Fa(a))f.redraw=function(){if(h&&h.element&&a&&a.marker&&a.marker.symbol)f.graphic=h.destroy();if(a&&a.dataLabels&&f.dataLabel)f.dataLabel=f.dataLabel.destroy();f.redraw=null};i=f.index;g.updateParallelArrays(f,i);if(m&&f.name)m[f.x]=f.name;k.data[i]=f.options;g.isDirty=g.isDirtyData=!0;if(!g.fixedBox&&g.hasCartesianSeries)j.isDirtyBox=!0;if(k.legendType===\"point\")j.isDirtyLegend=!0;b&&j.redraw(c)}var f=this,g=f.series,h=f.graphic,i,j=g.chart,k=g.options,\n"
"m=g.xAxis&&g.xAxis.names,b=p(b,!0);d===!1?e():f.firePointEvent(\"update\",{options:a},e)},remove:function(a,b){this.series.removePoint(Ka(this,this.series.data),a,b)}});r(R.prototype,{addPoint:function(a,b,c,d){var e=this,f=e.options,g=e.data,h=e.graph,i=e.area,j=e.chart,k=e.xAxis&&e.xAxis.names,m=h&&h.shift||0,l=[\"graph\",\"area\"],h=f.data,n,q=e.xData;Qa(d,j);if(c){for(d=e.zones.length;d--;)l.push(\"zoneGraph\"+d,\"zoneArea\"+d);o(l,function(a){if(e[a])e[a].shift=m+1})}if(i)i.isArea=!0;b=p(b,!0);i={series:e};\n"
"e.pointClass.prototype.applyOptions.apply(i,[a]);l=i.x;d=q.length;if(e.requireSorting&&l<q[d-1])for(n=!0;d&&q[d-1]>l;)d--;e.updateParallelArrays(i,\"splice\",d,0,0);e.updateParallelArrays(i,d);if(k&&i.name)k[l]=i.name;h.splice(d,0,a);n&&(e.data.splice(d,0,null),e.processData());f.legendType===\"point\"&&e.generatePoints();c&&(g[0]&&g[0].remove?g[0].remove(!1):(g.shift(),e.updateParallelArrays(i,\"shift\"),h.shift()));e.isDirty=!0;e.isDirtyData=!0;b&&(e.getAttribs(),j.redraw())},removePoint:function(a,b,\n"
"c){var d=this,e=d.data,f=e[a],g=d.points,h=d.chart,i=function(){e.length===g.length&&g.splice(a,1);e.splice(a,1);d.options.data.splice(a,1);d.updateParallelArrays(f||{series:d},\"splice\",a,1);f&&f.destroy();d.isDirty=!0;d.isDirtyData=!0;b&&h.redraw()};Qa(c,h);b=p(b,!0);f?f.firePointEvent(\"remove\",null,i):i()},remove:function(a,b){var c=this,d=c.chart,a=p(a,!0);if(!c.isRemoving)c.isRemoving=!0,I(c,\"remove\",null,function(){c.destroy();d.isDirtyLegend=d.isDirtyBox=!0;d.linkSeries();a&&d.redraw(b)});c.isRemoving=\n"
"!1},update:function(a,b){var c=this,d=this.chart,e=this.userOptions,f=this.type,g=M[f].prototype,h=[\"group\",\"markerGroup\",\"dataLabelsGroup\"],i;if(a.type&&a.type!==f||a.zIndex!==void 0)h.length=0;o(h,function(a){h[a]=c[a];delete c[a]});a=A(e,{animation:!1,index:this.index,pointStart:this.xData[0]},{data:this.options.data},a);this.remove(!1);for(i in g)this[i]=v;r(this,M[a.type||f].prototype);o(h,function(a){c[a]=h[a]});this.init(d,a);d.linkSeries();p(b,!0)&&d.redraw(!1)}});r(ha.prototype,{update:function(a,\n"
"b){var c=this.chart,a=c.options[this.coll][this.options.index]=A(this.userOptions,a);this.destroy(!0);this._addedPlotLB=this.chart._labelPanes=v;this.init(c,r(a,{events:v}));c.isDirtyBox=!0;p(b,!0)&&c.redraw()},remove:function(a){for(var b=this.chart,c=this.coll,d=this.series,e=d.length;e--;)d[e]&&d[e].remove(!1);ja(b.axes,this);ja(b[c],this);b.options[c].splice(this.options.index,1);o(b[c],function(a,b){a.options.index=b});this.destroy();b.isDirtyBox=!0;p(a,!0)&&b.redraw()},setTitle:function(a,b){this.update({title:a},\n"
"b)},setCategories:function(a,b){this.update({categories:a},b)}});var wa=ka(R);M.line=wa;ba.area=A(U,{threshold:0});var pa=ka(R,{type:\"area\",getSegments:function(){var a=this,b=[],c=[],d=[],e=this.xAxis,f=this.yAxis,g=f.stacks[this.stackKey],h={},i,j,k=this.points,m=this.options.connectNulls,l,n;if(this.options.stacking&&!this.cropped){for(l=0;l<k.length;l++)h[k[l].x]=k[l];for(n in g)g[n].total!==null&&d.push(+n);d.sort(function(a,b){return a-b});o(d,function(b){var d=0,k;if(!m||h[b]&&h[b].y!==null)if(h[b])c.push(h[b]);\n"
"else{for(l=a.index;l<=f.series.length;l++)if(k=g[b].points[l+\",\"+b]){d=k[1];break}i=e.translate(b);j=f.toPixels(d,!0);c.push({y:null,plotX:i,clientX:i,plotY:j,yBottom:j,onMouseOver:ta})}});c.length&&b.push(c)}else R.prototype.getSegments.call(this),b=this.segments;this.segments=b},getSegmentPath:function(a){var b=R.prototype.getSegmentPath.call(this,a),c=[].concat(b),d,e=this.options;d=b.length;var f=this.yAxis.getThreshold(e.threshold),g;d===3&&c.push(\"L\",b[1],b[2]);if(e.stacking&&!this.closedStacks)for(d=\n"
"a.length-1;d>=0;d--)g=p(a[d].yBottom,f),d<a.length-1&&e.step&&c.push(a[d+1].plotX,g),c.push(a[d].plotX,g);else this.closeSegment(c,a,f);this.areaPath=this.areaPath.concat(c);return b},closeSegment:function(a,b,c){a.push(\"L\",b[b.length-1].plotX,c,\"L\",b[0].plotX,c)},drawGraph:function(){this.areaPath=[];R.prototype.drawGraph.apply(this);var a=this,b=this.areaPath,c=this.options,d=[[\"area\",this.color,c.fillColor]];o(this.zones,function(b,f){d.push([\"zoneArea\"+f,b.color||a.color,b.fillColor||c.fillColor])});\n"
"o(d,function(d){var f=d[0],g=a[f];g?g.animate({d:b}):a[f]=a.chart.renderer.path(b).attr({fill:p(d[2],na(d[1]).setOpacity(p(c.fillOpacity,0.75)).get()),zIndex:0}).add(a.group)})},drawLegendSymbol:La.drawRectangle});M.area=pa;ba.spline=A(U);wa=ka(R,{type:\"spline\",getPointSpline:function(a,b,c){var d=b.plotX,e=b.plotY,f=a[c-1],g=a[c+1],h,i,j,k;if(f&&g){a=f.plotY;j=g.plotX;var g=g.plotY,m;h=(1.5*d+f.plotX)/2.5;i=(1.5*e+a)/2.5;j=(1.5*d+j)/2.5;k=(1.5*e+g)/2.5;m=(k-i)*(j-d)/(j-h)+e-k;i+=m;k+=m;i>a&&i>e?\n"
"(i=t(a,e),k=2*e-i):i<a&&i<e&&(i=z(a,e),k=2*e-i);k>g&&k>e?(k=t(g,e),i=2*e-k):k<g&&k<e&&(k=z(g,e),i=2*e-k);b.rightContX=j;b.rightContY=k}c?(b=[\"C\",f.rightContX||f.plotX,f.rightContY||f.plotY,h||d,i||e,d,e],f.rightContX=f.rightContY=null):b=[\"M\",d,e];return b}});M.spline=wa;ba.areaspline=A(ba.area);pa=pa.prototype;wa=ka(wa,{type:\"areaspline\",closedStacks:!0,getSegmentPath:pa.getSegmentPath,closeSegment:pa.closeSegment,drawGraph:pa.drawGraph,drawLegendSymbol:La.drawRectangle});M.areaspline=wa;ba.column=\n"
"A(U,{borderColor:\"#FFFFFF\",borderRadius:0,groupPadding:0.2,marker:null,pointPadding:0.1,minPointLength:0,cropThreshold:50,pointRange:null,states:{hover:{brightness:0.1,shadow:!1,halo:!1},select:{color:\"#C0C0C0\",borderColor:\"#000000\",shadow:!1}},dataLabels:{align:null,verticalAlign:null,y:null},startFromThreshold:!0,stickyTracking:!1,tooltip:{distance:6},threshold:0});wa=ka(R,{type:\"column\",pointAttrToOptions:{stroke:\"borderColor\",fill:\"color\",r:\"borderRadius\"},cropShoulder:0,directTouch:!0,trackerGroups:[\"group\",\n"
"\"dataLabelsGroup\"],negStacks:!0,init:function(){R.prototype.init.apply(this,arguments);var a=this,b=a.chart;b.hasRendered&&o(b.series,function(b){if(b.type===a.type)b.isDirty=!0})},getColumnMetrics:function(){var a=this,b=a.options,c=a.xAxis,d=a.yAxis,e=c.reversed,f,g={},h,i=0;b.grouping===!1?i=1:o(a.chart.series,function(b){var c=b.options,e=b.yAxis;if(b.type===a.type&&b.visible&&d.len===e.len&&d.pos===e.pos)c.stacking?(f=b.stackKey,g[f]===v&&(g[f]=i++),h=g[f]):c.grouping!==!1&&(h=i++),b.columnIndex=\n"
,
"h});var j=z(N(c.transA)*(c.ordinalSlope||b.pointRange||c.closestPointRange||c.tickInterval||1),c.len),k=j*b.groupPadding,m=(j-2*k)/i,b=z(b.maxPointWidth||c.len,p(b.pointWidth,m*(1-2*b.pointPadding)));return a.columnMetrics={width:b,offset:(m-b)/2+(k+((e?i-(a.columnIndex||0):a.columnIndex)||0)*m-j/2)*(e?-1:1)}},translate:function(){var a=this,b=a.chart,c=a.options,d=a.borderWidth=p(c.borderWidth,a.closestPointRange*a.xAxis.transA<2?0:1),e=a.yAxis,f=a.translatedThreshold=e.getThreshold(c.threshold),\n"
"g=p(c.minPointLength,5),h=a.getColumnMetrics(),i=h.width,j=a.barW=t(i,1+2*d),k=a.pointXOffset=h.offset,m=-(d%2?0.5:0),l=d%2?0.5:1;b.inverted&&(f-=0.5,b.renderer.isVML&&(l+=1));c.pointPadding&&(j=sa(j));R.prototype.translate.apply(a);o(a.points,function(c){var d=p(c.yBottom,f),h=999+N(d),h=z(t(-h,c.plotY),e.len+h),o=c.plotX+k,r=j,s=z(h,d),y,v;y=t(h,d)-s;N(y)<g&&g&&(y=g,v=!e.reversed&&!c.negative||e.reversed&&c.negative,s=w(N(s-f)>g?d-g:f-(v?g:0)));c.barX=o;c.pointWidth=i;r=w(o+r)+m;o=w(o)+m;r-=o;d=\n"
"N(s)<0.5;y=z(w(s+y)+l,9E4);s=w(s)+l;y-=s;d&&(s-=1,y+=1);c.tooltipPos=b.inverted?[e.len+e.pos-b.plotLeft-h,a.xAxis.len-o-r/2,y]:[o+r/2,h+e.pos-b.plotTop,y];c.shapeType=\"rect\";c.shapeArgs={x:o,y:s,width:r,height:y}})},getSymbol:ta,drawLegendSymbol:La.drawRectangle,drawGraph:ta,drawPoints:function(){var a=this,b=this.chart,c=a.options,d=b.renderer,e=c.animationLimit||250,f,g;o(a.points,function(h){var i=h.plotY,j=h.graphic;if(i!==v&&!isNaN(i)&&h.y!==null)f=h.shapeArgs,i=s(a.borderWidth)?{\"stroke-width\":a.borderWidth}:\n"
"{},g=h.pointAttr[h.selected?\"select\":\"\"]||a.pointAttr[\"\"],j?(kb(j),j.attr(i)[b.pointCount<e?\"animate\":\"attr\"](A(f))):h.graphic=d[h.shapeType](f).attr(i).attr(g).add(a.group).shadow(c.shadow,null,c.stacking&&!c.borderRadius);else if(j)h.graphic=j.destroy()})},animate:function(a){var b=this.yAxis,c=this.options,d=this.chart.inverted,e={};if(ca)a?(e.scaleY=0.001,a=z(b.pos+b.len,t(b.pos,b.toPixels(c.threshold))),d?e.translateX=a-b.len:e.translateY=a,this.group.attr(e)):(e.scaleY=1,e[d?\"translateX\":\"translateY\"]=\n"
"b.pos,this.group.animate(e,this.options.animation),this.animate=null)},remove:function(){var a=this,b=a.chart;b.hasRendered&&o(b.series,function(b){if(b.type===a.type)b.isDirty=!0});R.prototype.remove.apply(a,arguments)}});M.column=wa;ba.bar=A(ba.column);pa=ka(wa,{type:\"bar\",inverted:!0});M.bar=pa;ba.scatter=A(U,{lineWidth:0,marker:{enabled:!0},tooltip:{headerFormat:'<span style=\"color:{point.color}\">\\u25cf</span> <span style=\"font-size: 10px;\"> {series.name}</span><br/>',pointFormat:\"x: <b>{point.x}</b><br/>y: <b>{point.y}</b><br/>\"}});\n"
"pa=ka(R,{type:\"scatter\",sorted:!1,requireSorting:!1,noSharedTooltip:!0,trackerGroups:[\"group\",\"markerGroup\",\"dataLabelsGroup\"],takeOrdinalPosition:!1,kdDimensions:2,drawGraph:function(){this.options.lineWidth&&R.prototype.drawGraph.call(this)}});M.scatter=pa;ba.pie=A(U,{borderColor:\"#FFFFFF\",borderWidth:1,center:[null,null],clip:!1,colorByPoint:!0,dataLabels:{distance:30,enabled:!0,formatter:function(){return this.y===null?void 0:this.point.name},x:0},ignoreHiddenPoint:!0,legendType:\"point\",marker:null,\n"
"size:null,showInLegend:!1,slicedOffset:10,states:{hover:{brightness:0.1,shadow:!1}},stickyTracking:!1,tooltip:{followPointer:!0}});U={type:\"pie\",isCartesian:!1,pointClass:ka(Ea,{init:function(){Ea.prototype.init.apply(this,arguments);var a=this,b;r(a,{visible:a.visible!==!1,name:p(a.name,\"Slice\")});b=function(b){a.slice(b.type===\"select\")};H(a,\"select\",b);H(a,\"unselect\",b);return a},setVisible:function(a,b){var c=this,d=c.series,e=d.chart,f=d.options.ignoreHiddenPoint,b=p(b,f);if(a!==c.visible){c.visible=\n"
"c.options.visible=a=a===v?!c.visible:a;d.options.data[Ka(c,d.data)]=c.options;o([\"graphic\",\"dataLabel\",\"connector\",\"shadowGroup\"],function(b){if(c[b])c[b][a?\"show\":\"hide\"](!0)});c.legendItem&&e.legend.colorizeItem(c,a);!a&&c.state===\"hover\"&&c.setState(\"\");if(f)d.isDirty=!0;b&&e.redraw()}},slice:function(a,b,c){var d=this.series;Qa(c,d.chart);p(b,!0);this.sliced=this.options.sliced=a=s(a)?a:!this.sliced;d.options.data[Ka(this,d.data)]=this.options;a=a?this.slicedTranslation:{translateX:0,translateY:0};\n"
"this.graphic.animate(a);this.shadowGroup&&this.shadowGroup.animate(a)},haloPath:function(a){var b=this.shapeArgs,c=this.series.chart;return this.sliced||!this.visible?[]:this.series.chart.renderer.symbols.arc(c.plotLeft+b.x,c.plotTop+b.y,b.r+a,b.r+a,{innerR:this.shapeArgs.r,start:b.start,end:b.end})}}),requireSorting:!1,directTouch:!0,noSharedTooltip:!0,trackerGroups:[\"group\",\"dataLabelsGroup\"],axisTypes:[],pointAttrToOptions:{stroke:\"borderColor\",\"stroke-width\":\"borderWidth\",fill:\"color\"},animate:function(a){var b=\n"
"this,c=b.points,d=b.startAngleRad;if(!a)o(c,function(a){var c=a.graphic,g=a.shapeArgs;c&&(c.attr({r:a.startR||b.center[3]/2,start:d,end:d}),c.animate({r:g.r,start:g.start,end:g.end},b.options.animation))}),b.animate=null},setData:function(a,b,c,d){R.prototype.setData.call(this,a,!1,c,d);this.processData();this.generatePoints();p(b,!0)&&this.chart.redraw(c)},updateTotals:function(){var a,b=0,c=this.points,d=c.length,e,f=this.options.ignoreHiddenPoint;for(a=0;a<d;a++)e=c[a],b+=f&&!e.visible?0:e.y;this.total=\n"
"b;for(a=0;a<d;a++)e=c[a],e.percentage=b>0&&(e.visible||!f)?e.y/b*100:0,e.total=b},generatePoints:function(){R.prototype.generatePoints.call(this);this.updateTotals()},translate:function(a){this.generatePoints();var b=0,c=this.options,d=c.slicedOffset,e=d+c.borderWidth,f,g,h,i=c.startAngle||0,j=this.startAngleRad=ma/180*(i-90),i=(this.endAngleRad=ma/180*(p(c.endAngle,i+360)-90))-j,k=this.points,m=c.dataLabels.distance,c=c.ignoreHiddenPoint,l,n=k.length,o;if(!a)this.center=a=this.getCenter();this.getX=\n"
"function(b,c){h=W.asin(z((b-a[1])/(a[2]/2+m),1));return a[0]+(c?-1:1)*X(h)*(a[2]/2+m)};for(l=0;l<n;l++){o=k[l];f=j+b*i;if(!c||o.visible)b+=o.percentage/100;g=j+b*i;o.shapeType=\"arc\";o.shapeArgs={x:a[0],y:a[1],r:a[2]/2,innerR:a[3]/2,start:w(f*1E3)/1E3,end:w(g*1E3)/1E3};h=(g+f)/2;h>1.5*ma?h-=2*ma:h<-ma/2&&(h+=2*ma);o.slicedTranslation={translateX:w(X(h)*d),translateY:w(aa(h)*d)};f=X(h)*a[2]/2;g=aa(h)*a[2]/2;o.tooltipPos=[a[0]+f*0.7,a[1]+g*0.7];o.half=h<-ma/2||h>ma/2?1:0;o.angle=h;e=z(e,m/2);o.labelPos=\n"
"[a[0]+f+X(h)*m,a[1]+g+aa(h)*m,a[0]+f+X(h)*e,a[1]+g+aa(h)*e,a[0]+f,a[1]+g,m<0?\"center\":o.half?\"right\":\"left\",h]}},drawGraph:null,drawPoints:function(){var a=this,b=a.chart.renderer,c,d,e=a.options.shadow,f,g,h;if(e&&!a.shadowGroup)a.shadowGroup=b.g(\"shadow\").add(a.group);o(a.points,function(i){if(i.y!==null){d=i.graphic;g=i.shapeArgs;f=i.shadowGroup;if(e&&!f)f=i.shadowGroup=b.g(\"shadow\").add(a.shadowGroup);c=i.sliced?i.slicedTranslation:{translateX:0,translateY:0};f&&f.attr(c);if(d)d.animate(r(g,c));\n"
"else{h={\"stroke-linejoin\":\"round\"};if(!i.visible)h.visibility=\"hidden\";i.graphic=d=b[i.shapeType](g).setRadialReference(a.center).attr(i.pointAttr[i.selected?\"select\":\"\"]).attr(h).attr(c).add(a.group).shadow(e,f)}}})},searchPoint:ta,sortByAngle:function(a,b){a.sort(function(a,d){return a.angle!==void 0&&(d.angle-a.angle)*b})},drawLegendSymbol:La.drawRectangle,getCenter:Xb.getCenter,getSymbol:ta};U=ka(R,U);M.pie=U;R.prototype.drawDataLabels=function(){var a=this,b=a.options,c=b.cursor,d=b.dataLabels,\n"
"e=a.points,f,g,h=a.hasRendered||0,i,j,k=a.chart.renderer;if(d.enabled||a._hasPointLabels)a.dlProcessOptions&&a.dlProcessOptions(d),j=a.plotGroup(\"dataLabelsGroup\",\"data-labels\",d.defer?\"hidden\":\"visible\",d.zIndex||6),p(d.defer,!0)&&(j.attr({opacity:+h}),h||H(a,\"afterAnimate\",function(){a.visible&&j.show();j[b.animation?\"animate\":\"attr\"]({opacity:1},{duration:200})})),g=d,o(e,function(e){var h,n=e.dataLabel,o,t,w=e.connector,z=!0,u,y={};f=e.dlOptions||e.options&&e.options.dataLabels;h=p(f&&f.enabled,\n"
"g.enabled);if(n&&!h)e.dataLabel=n.destroy();else if(h){d=A(g,f);u=d.style;h=d.rotation;o=e.getLabelConfig();i=d.format?Ha(d.format,o):d.formatter.call(o,d);u.color=p(d.color,u.color,a.color,\"black\");if(n)if(s(i))n.attr({text:i}),z=!1;else{if(e.dataLabel=n=n.destroy(),w)e.connector=w.destroy()}else if(s(i)){n={fill:d.backgroundColor,stroke:d.borderColor,\"stroke-width\":d.borderWidth,r:d.borderRadius||0,rotation:h,padding:d.padding,zIndex:1};if(u.color===\"contrast\")y.color=d.inside||d.distance<0||b.stacking?\n"
"k.getContrast(e.color||a.color):\"#000000\";if(c)y.cursor=c;for(t in n)n[t]===v&&delete n[t];n=e.dataLabel=k[h?\"text\":\"label\"](i,0,-999,d.shape,null,null,d.useHTML).attr(n).css(r(u,y)).add(j).shadow(d.shadow)}n&&a.alignDataLabel(e,n,d,null,z)}})};R.prototype.alignDataLabel=function(a,b,c,d,e){var f=this.chart,g=f.inverted,h=p(a.plotX,-999),i=p(a.plotY,-999),j=b.getBBox(),k=f.renderer.fontMetrics(c.style.fontSize).b,m=this.visible&&(a.series.forceDL||f.isInsidePlot(h,w(i),g)||d&&f.isInsidePlot(h,g?d.x+\n"
"1:d.y+d.height-1,g));if(m)d=r({x:g?f.plotWidth-i:h,y:w(g?f.plotHeight-h:i),width:0,height:0},d),r(c,{width:j.width,height:j.height}),c.rotation?(a=f.renderer.rotCorr(k,c.rotation),b[e?\"attr\":\"animate\"]({x:d.x+c.x+d.width/2+a.x,y:d.y+c.y+d.height/2}).attr({align:c.align})):(b.align(c,null,d),g=b.alignAttr,p(c.overflow,\"justify\")===\"justify\"?this.justifyDataLabel(b,c,g,j,d,e):p(c.crop,!0)&&(m=f.isInsidePlot(g.x,g.y)&&f.isInsidePlot(g.x+j.width,g.y+j.height)),c.shape&&b.attr({anchorX:a.plotX,anchorY:a.plotY}));\n"
"if(!m)b.attr({y:-999}),b.placed=!1};R.prototype.justifyDataLabel=function(a,b,c,d,e,f){var g=this.chart,h=b.align,i=b.verticalAlign,j,k,m=a.box?0:a.padding||0;j=c.x+m;if(j<0)h===\"right\"?b.align=\"left\":b.x=-j,k=!0;j=c.x+d.width-m;if(j>g.plotWidth)h===\"left\"?b.align=\"right\":b.x=g.plotWidth-j,k=!0;j=c.y+m;if(j<0)i===\"bottom\"?b.verticalAlign=\"top\":b.y=-j,k=!0;j=c.y+d.height-m;if(j>g.plotHeight)i===\"top\"?b.verticalAlign=\"bottom\":b.y=g.plotHeight-j,k=!0;if(k)a.placed=!f,a.align(b,null,e)};if(M.pie)M.pie.prototype.drawDataLabels=\n"
,
"function(){var a=this,b=a.data,c,d=a.chart,e=a.options.dataLabels,f=p(e.connectorPadding,10),g=p(e.connectorWidth,1),h=d.plotWidth,i=d.plotHeight,j,k,m=p(e.softConnector,!0),l=e.distance,n=a.center,q=n[2]/2,r=n[1],s=l>0,v,u,y,C=[[],[]],A,B,D,F,G,E=[0,0,0,0],L=function(a,b){return b.y-a.y};if(a.visible&&(e.enabled||a._hasPointLabels)){R.prototype.drawDataLabels.apply(a);o(b,function(a){a.dataLabel&&a.visible&&C[a.half].push(a)});for(F=2;F--;){var I=[],M=[],H=C[F],K=H.length,J;if(K){a.sortByAngle(H,\n"
"F-0.5);for(G=b=0;!b&&H[G];)b=H[G]&&H[G].dataLabel&&(H[G].dataLabel.getBBox().height||21),G++;if(l>0){u=z(r+q+l,d.plotHeight);for(G=t(0,r-q-l);G<=u;G+=b)I.push(G);u=I.length;if(K>u){c=[].concat(H);c.sort(L);for(G=K;G--;)c[G].rank=G;for(G=K;G--;)H[G].rank>=u&&H.splice(G,1);K=H.length}for(G=0;G<K;G++){c=H[G];y=c.labelPos;c=9999;var P,O;for(O=0;O<u;O++)P=N(I[O]-y[1]),P<c&&(c=P,J=O);if(J<G&&I[G]!==null)J=G;else for(u<K-G+J&&I[G]!==null&&(J=u-K+G);I[J]===null;)J++;M.push({i:J,y:I[J]});I[J]=null}M.sort(L)}for(G=\n"
"0;G<K;G++){c=H[G];y=c.labelPos;v=c.dataLabel;D=c.visible===!1?\"hidden\":\"inherit\";c=y[1];if(l>0){if(u=M.pop(),J=u.i,B=u.y,c>B&&I[J+1]!==null||c<B&&I[J-1]!==null)B=z(t(0,c),d.plotHeight)}else B=c;A=e.justify?n[0]+(F?-1:1)*(q+l):a.getX(B===r-q-l||B===r+q+l?c:B,F);v._attr={visibility:D,align:y[6]};v._pos={x:A+e.x+({left:f,right:-f}[y[6]]||0),y:B+e.y-10};v.connX=A;v.connY=B;if(this.options.size===null)u=v.width,A-u<f?E[3]=t(w(u-A+f),E[3]):A+u>h-f&&(E[1]=t(w(A+u-h+f),E[1])),B-b/2<0?E[0]=t(w(-B+b/2),E[0]):\n"
"B+b/2>i&&(E[2]=t(w(B+b/2-i),E[2]))}}}if(Ca(E)===0||this.verifyDataLabelOverflow(E))this.placeDataLabels(),s&&g&&o(this.points,function(b){j=b.connector;y=b.labelPos;if((v=b.dataLabel)&&v._pos&&b.visible)D=v._attr.visibility,A=v.connX,B=v.connY,k=m?[\"M\",A+(y[6]===\"left\"?5:-5),B,\"C\",A,B,2*y[2]-y[4],2*y[3]-y[5],y[2],y[3],\"L\",y[4],y[5]]:[\"M\",A+(y[6]===\"left\"?5:-5),B,\"L\",y[2],y[3],\"L\",y[4],y[5]],j?(j.animate({d:k}),j.attr(\"visibility\",D)):b.connector=j=a.chart.renderer.path(k).attr({\"stroke-width\":g,stroke:e.connectorColor||\n"
"b.color||\"#606060\",visibility:D}).add(a.dataLabelsGroup);else if(j)b.connector=j.destroy()})}},M.pie.prototype.placeDataLabels=function(){o(this.points,function(a){var b=a.dataLabel;if(b&&a.visible)(a=b._pos)?(b.attr(b._attr),b[b.moved?\"animate\":\"attr\"](a),b.moved=!0):b&&b.attr({y:-999})})},M.pie.prototype.alignDataLabel=ta,M.pie.prototype.verifyDataLabelOverflow=function(a){var b=this.center,c=this.options,d=c.center,e=c.minSize||80,f=e,g;d[0]!==null?f=t(b[2]-t(a[1],a[3]),e):(f=t(b[2]-a[1]-a[3],\n"
"e),b[0]+=(a[3]-a[1])/2);d[1]!==null?f=t(z(f,b[2]-t(a[0],a[2])),e):(f=t(z(f,b[2]-a[0]-a[2]),e),b[1]+=(a[0]-a[2])/2);f<b[2]?(b[2]=f,b[3]=/%$/.test(c.innerSize||0)?f*parseFloat(c.innerSize||0)/100:parseFloat(c.innerSize||0),this.translate(b),o(this.points,function(a){if(a.dataLabel)a.dataLabel._pos=null}),this.drawDataLabels&&this.drawDataLabels()):g=!0;return g};if(M.column)M.column.prototype.alignDataLabel=function(a,b,c,d,e){var f=this.chart.inverted,g=a.series,h=a.dlBox||a.shapeArgs,i=p(a.below,\n"
"a.plotY>p(this.translatedThreshold,g.yAxis.len)),j=p(c.inside,!!this.options.stacking);if(h&&(d=A(h),f&&(d={x:g.yAxis.len-d.y-d.height,y:g.xAxis.len-d.x-d.width,width:d.height,height:d.width}),!j))f?(d.x+=i?0:d.width,d.width=0):(d.y+=i?d.height:0,d.height=0);c.align=p(c.align,!f||j?\"center\":i?\"right\":\"left\");c.verticalAlign=p(c.verticalAlign,f||j?\"middle\":i?\"top\":\"bottom\");R.prototype.alignDataLabel.call(this,a,b,c,d,e)};(function(a){var b=a.Chart,c=a.each,d=a.pick,e=HighchartsAdapter.addEvent;b.prototype.callbacks.push(function(a){function b(){var e=\n"
"[];c(a.series,function(a){var b=a.options.dataLabels,f=a.dataLabelCollections||[\"dataLabel\"];(b.enabled||a._hasPointLabels)&&!b.allowOverlap&&a.visible&&c(f,function(b){c(a.points,function(a){if(a[b])a[b].labelrank=d(a.labelrank,a.shapeArgs&&a.shapeArgs.height),e.push(a[b])})})});a.hideOverlappingLabels(e)}b();e(a,\"redraw\",b)});b.prototype.hideOverlappingLabels=function(a){var b=a.length,d,e,j,k,m,l,n;for(e=0;e<b;e++)if(d=a[e])d.oldOpacity=d.opacity,d.newOpacity=1;a.sort(function(a,b){return(b.labelrank||\n"
"0)-(a.labelrank||0)});for(e=0;e<b;e++){j=a[e];for(d=e+1;d<b;++d)if(k=a[d],j&&k&&j.placed&&k.placed&&j.newOpacity!==0&&k.newOpacity!==0&&(m=j.alignAttr,l=k.alignAttr,n=2*(j.box?0:j.padding),m=!(l.x>m.x+(j.width-n)||l.x+(k.width-n)<m.x||l.y>m.y+(j.height-n)||l.y+(k.height-n)<m.y)))(j.labelrank<k.labelrank?j:k).newOpacity=0}c(a,function(a){var b,c;if(a){c=a.newOpacity;if(a.oldOpacity!==c&&a.placed)c?a.show(!0):b=function(){a.hide()},a.alignAttr.opacity=c,a[a.isOld?\"animate\":\"attr\"](a.alignAttr,null,\n"
"b);a.isOld=!0}})}})(B);U=B.TrackerMixin={drawTrackerPoint:function(){var a=this,b=a.chart,c=b.pointer,d=a.options.cursor,e=d&&{cursor:d},f=function(a){for(var c=a.target,d;c&&!d;)d=c.point,c=c.parentNode;if(d!==v&&d!==b.hoverPoint)d.onMouseOver(a)};o(a.points,function(a){if(a.graphic)a.graphic.element.point=a;if(a.dataLabel)a.dataLabel.element.point=a});if(!a._hasTracking)o(a.trackerGroups,function(b){if(a[b]&&(a[b].addClass(\"highcharts-tracker\").on(\"mouseover\",f).on(\"mouseout\",function(a){c.onTrackerMouseOut(a)}).css(e),\n"
"$a))a[b].on(\"touchstart\",f)}),a._hasTracking=!0},drawTrackerGraph:function(){var a=this,b=a.options,c=b.trackByArea,d=[].concat(c?a.areaPath:a.graphPath),e=d.length,f=a.chart,g=f.pointer,h=f.renderer,i=f.options.tooltip.snap,j=a.tracker,k=b.cursor,m=k&&{cursor:k},k=a.singlePoints,l,n=function(){if(f.hoverSeries!==a)a.onMouseOver()},p=\"rgba(192,192,192,\"+(ca?1.0E-4:0.002)+\")\";if(e&&!c)for(l=e+1;l--;)d[l]===\"M\"&&d.splice(l+1,0,d[l+1]-i,d[l+2],\"L\"),(l&&d[l]===\"M\"||l===e)&&d.splice(l,0,\"L\",d[l-2]+i,d[l-\n"
"1]);for(l=0;l<k.length;l++)e=k[l],d.push(\"M\",e.plotX-i,e.plotY,\"L\",e.plotX+i,e.plotY);j?j.attr({d:d}):(a.tracker=h.path(d).attr({\"stroke-linejoin\":\"round\",visibility:a.visible?\"visible\":\"hidden\",stroke:p,fill:c?p:O,\"stroke-width\":b.lineWidth+(c?0:2*i),zIndex:2}).add(a.group),o([a.tracker,a.markerGroup],function(a){a.addClass(\"highcharts-tracker\").on(\"mouseover\",n).on(\"mouseout\",function(a){g.onTrackerMouseOut(a)}).css(m);if($a)a.on(\"touchstart\",n)}))}};if(M.column)wa.prototype.drawTracker=U.drawTrackerPoint;\n"
"if(M.pie)M.pie.prototype.drawTracker=U.drawTrackerPoint;if(M.scatter)pa.prototype.drawTracker=U.drawTrackerPoint;r(lb.prototype,{setItemEvents:function(a,b,c,d,e){var f=this;(c?b:a.legendGroup).on(\"mouseover\",function(){a.setState(\"hover\");b.css(f.options.itemHoverStyle)}).on(\"mouseout\",function(){b.css(a.visible?d:e);a.setState()}).on(\"click\",function(b){var c=function(){a.setVisible()},b={browserEvent:b};a.firePointEvent?a.firePointEvent(\"legendItemClick\",b,c):I(a,\"legendItemClick\",b,c)})},createCheckboxForItem:function(a){a.checkbox=\n"
"$(\"input\",{type:\"checkbox\",checked:a.selected,defaultChecked:a.selected},this.options.itemCheckboxStyle,this.chart.container);H(a.checkbox,\"click\",function(b){I(a.series||a,\"checkboxClick\",{checked:b.target.checked,item:a},function(){a.select()})})}});S.legend.itemStyle.cursor=\"pointer\";r(D.prototype,{showResetZoom:function(){var a=this,b=S.lang,c=a.options.chart.resetZoomButton,d=c.theme,e=d.states,f=c.relativeTo===\"chart\"?null:\"plotBox\";this.resetZoomButton=a.renderer.button(b.resetZoom,null,null,\n"
"function(){a.zoomOut()},d,e&&e.hover).attr({align:c.position.align,title:b.resetZoomTitle}).add().align(c.position,!1,f)},zoomOut:function(){var a=this;I(a,\"selection\",{resetSelection:!0},function(){a.zoom()})},zoom:function(a){var b,c=this.pointer,d=!1,e;!a||a.resetSelection?o(this.axes,function(a){b=a.zoom()}):o(a.xAxis.concat(a.yAxis),function(a){var e=a.axis,h=e.isXAxis;if(c[h?\"zoomX\":\"zoomY\"]||c[h?\"pinchX\":\"pinchY\"])b=e.zoom(a.min,a.max),e.displayBtn&&(d=!0)});e=this.resetZoomButton;if(d&&!e)this.showResetZoom();\n"
"else if(!d&&da(e))this.resetZoomButton=e.destroy();b&&this.redraw(p(this.options.chart.animation,a&&a.animation,this.pointCount<100))},pan:function(a,b){var c=this,d=c.hoverPoints,e;d&&o(d,function(a){a.setState()});o(b===\"xy\"?[1,0]:[1],function(b){var d=a[b?\"chartX\":\"chartY\"],h=c[b?\"xAxis\":\"yAxis\"][0],i=c[b?\"mouseDownX\":\"mouseDownY\"],j=(h.pointRange||0)/2,k=h.getExtremes(),m=h.toValue(i-d,!0)+j,j=h.toValue(i+c[b?\"plotWidth\":\"plotHeight\"]-d,!0)-j,i=i>d;if(h.series.length&&(i||m>z(k.dataMin,k.min))&&\n"
"(!i||j<t(k.dataMax,k.max)))h.setExtremes(m,j,!1,!1,{trigger:\"pan\"}),e=!0;c[b?\"mouseDownX\":\"mouseDownY\"]=d});e&&c.redraw(!1);L(c.container,{cursor:\"move\"})}});r(Ea.prototype,{select:function(a,b){var c=this,d=c.series,e=d.chart,a=p(a,!c.selected);c.firePointEvent(a?\"select\":\"unselect\",{accumulate:b},function(){c.selected=c.options.selected=a;d.options.data[Ka(c,d.data)]=c.options;c.setState(a&&\"select\");b||o(e.getSelectedPoints(),function(a){if(a.selected&&a!==c)a.selected=a.options.selected=!1,d.options.data[Ka(a,\n"
"d.data)]=a.options,a.setState(\"\"),a.firePointEvent(\"unselect\")})})},onMouseOver:function(a,b){var c=this.series,d=c.chart,e=d.tooltip,f=d.hoverPoint;if(d.hoverSeries!==c)c.onMouseOver();if(f&&f!==this)f.onMouseOut();if(this.series&&(this.firePointEvent(\"mouseOver\"),e&&(!e.shared||c.noSharedTooltip)&&e.refresh(this,a),this.setState(\"hover\"),!b))d.hoverPoint=this},onMouseOut:function(){var a=this.series.chart,b=a.hoverPoints;this.firePointEvent(\"mouseOut\");if(!b||Ka(this,b)===-1)this.setState(),a.hoverPoint=\n"
,
"null},importEvents:function(){if(!this.hasImportedEvents){var a=A(this.series.options.point,this.options).events,b;this.events=a;for(b in a)H(this,b,a[b]);this.hasImportedEvents=!0}},setState:function(a,b){var c=this.plotX,d=this.plotY,e=this.series,f=e.options.states,g=ba[e.type].marker&&e.options.marker,h=g&&!g.enabled,i=g&&g.states[a],j=i&&i.enabled===!1,k=e.stateMarkerGraphic,m=this.marker||{},l=e.chart,n=e.halo,o,a=a||\"\";o=this.pointAttr[a]||e.pointAttr[a];if(!(a===this.state&&!b||this.selected&&\n"
"a!==\"select\"||f[a]&&f[a].enabled===!1||a&&(j||h&&i.enabled===!1)||a&&m.states&&m.states[a]&&m.states[a].enabled===!1)){if(this.graphic)g=g&&this.graphic.symbolName&&o.r,this.graphic.attr(A(o,g?{x:c-g,y:d-g,width:2*g,height:2*g}:{})),k&&k.hide();else{if(a&&i)if(g=i.radius,m=m.symbol||e.symbol,k&&k.currentSymbol!==m&&(k=k.destroy()),k)k[b?\"animate\":\"attr\"]({x:c-g,y:d-g});else if(m)e.stateMarkerGraphic=k=l.renderer.symbol(m,c-g,d-g,2*g,2*g).attr(o).add(e.markerGroup),k.currentSymbol=m;if(k)k[a&&l.isInsidePlot(c,\n"
"d,l.inverted)?\"show\":\"hide\"](),k.element.point=this}if((c=f[a]&&f[a].halo)&&c.size){if(!n)e.halo=n=l.renderer.path().add(l.seriesGroup);n.attr(r({fill:na(this.color||e.color).setOpacity(c.opacity).get()},c.attributes))[b?\"animate\":\"attr\"]({d:this.haloPath(c.size)})}else n&&n.attr({d:[]});this.state=a}},haloPath:function(a){var b=this.series,c=b.chart,d=b.getPlotBox(),e=c.inverted;return c.renderer.symbols.circle(d.translateX+(e?b.yAxis.len-this.plotY:this.plotX)-a,d.translateY+(e?b.xAxis.len-this.plotX:\n"
"this.plotY)-a,a*2,a*2)}});r(R.prototype,{onMouseOver:function(){var a=this.chart,b=a.hoverSeries;if(b&&b!==this)b.onMouseOut();this.options.events.mouseOver&&I(this,\"mouseOver\");this.setState(\"hover\");a.hoverSeries=this},onMouseOut:function(){var a=this.options,b=this.chart,c=b.tooltip,d=b.hoverPoint;b.hoverSeries=null;if(d)d.onMouseOut();this&&a.events.mouseOut&&I(this,\"mouseOut\");c&&!a.stickyTracking&&(!c.shared||this.noSharedTooltip)&&c.hide();this.setState()},setState:function(a){var b=this.options,\n"
"c=this.graph,d=b.states,e=b.lineWidth,b=0,a=a||\"\";if(this.state!==a&&(this.state=a,!(d[a]&&d[a].enabled===!1)&&(a&&(e=d[a].lineWidth||e+(d[a].lineWidthPlus||0)),c&&!c.dashstyle))){a={\"stroke-width\":e};for(c.attr(a);this[\"zoneGraph\"+b];)this[\"zoneGraph\"+b].attr(a),b+=1}},setVisible:function(a,b){var c=this,d=c.chart,e=c.legendItem,f,g=d.options.chart.ignoreHiddenSeries,h=c.visible;f=(c.visible=a=c.userOptions.visible=a===v?!h:a)?\"show\":\"hide\";o([\"group\",\"dataLabelsGroup\",\"markerGroup\",\"tracker\"],function(a){if(c[a])c[a][f]()});\n"
"if(d.hoverSeries===c||(d.hoverPoint&&d.hoverPoint.series)===c)c.onMouseOut();e&&d.legend.colorizeItem(c,a);c.isDirty=!0;c.options.stacking&&o(d.series,function(a){if(a.options.stacking&&a.visible)a.isDirty=!0});o(c.linkedSeries,function(b){b.setVisible(a,!1)});if(g)d.isDirtyBox=!0;b!==!1&&d.redraw();I(c,f)},show:function(){this.setVisible(!0)},hide:function(){this.setVisible(!1)},select:function(a){this.selected=a=a===v?!this.selected:a;if(this.checkbox)this.checkbox.checked=a;I(this,a?\"select\":\"unselect\")},\n"
"drawTracker:U.drawTrackerGraph});r(B,{Color:na,Point:Ea,Tick:Ra,Renderer:Za,SVGElement:P,SVGRenderer:ua,arrayMin:Na,arrayMax:Ca,charts:Y,dateFormat:Ma,error:la,format:Ha,pathAnim:xb,getOptions:function(){return S},hasBidiBug:Nb,isTouchDevice:Jb,setOptions:function(a){S=A(!0,S,a);Cb();return S},addEvent:H,removeEvent:Z,createElement:$,discardElement:Pa,css:L,each:o,map:Ta,merge:A,splat:ra,extendClass:ka,pInt:F,svg:ca,canvas:fa,vml:!ca&&!fa,product:\"Highcharts\",version:\"4.1.8\"})})();\n"
"  </script>\n"
"\n"
"  <script type=\"text/javascript\">\n"
"$(function () {\n"
"        $('#all-together').highcharts({\n"
"            chart: {\n"
"                type: 'scatter',\n"
"                zoomType: 'y'\n"
"            },\n"
"            title: {\n"
"                text: '{$title}'\n"
"            },\n"
"            subtitle: {\n"
"                text: 'generated with <a href=\"http://flamingdangerzone.com/nonius\">nonius</a>'\n"
"            },\n"
"            yAxis: {\n"
"                title: { text: 'Time ({$units})' },\n"
"                startOnTick: false,\n"
"                endOnTick: true,\n"
"                showLastLabel: true,\n"
"            },\n"
"            xAxis: {\n"
"                title: { text: null },\n"
"                startOnTick: true,\n"
"                endOnTick: true,\n"
"                showLastLabel: true,\n"
"                minPadding: 0,\n"
"                maxPadding: 0\n"
"            },\n"
"            legend: {\n"
"                layout: 'vertical',\n"
"                align: 'right',\n"
"                verticalAlign: 'top',\n"
"                x: 0,\n"
"                y: 100,\n"
"                backgroundColor: '#FFFFFF',\n"
"                borderWidth: 1\n"
"            },\n"
"            plotOptions: {\n"
"                scatter: {\n"
"                    marker: {\n"
"                        radius: 5,\n"
"                        states: {\n"
"                            hover: {\n"
"                                enabled: true,\n"
"                                lineColor: 'rgb(100,100,100)'\n"
"                            }\n"
"                        }\n"
"                    },\n"
"                    states: {\n"
"                        hover: {\n"
"                            marker: {\n"
"                                enabled: false\n"
"                            }\n"
"                        }\n"
"                    },\n"
"                    tooltip: {\n"
"                        headerFormat: '<b>{series.name}</b><br>',\n"
"                        pointFormat: 'Run #{point.x}: <b>{point.y} {$units}</b>'\n"
"                    }\n"
"                }\n"
"            },\n"
"            series: [{% for benchmark in benchmarks %}{\n"
"                        name: '{$benchmark.name}',\n"
"                        data: [{% for time in benchmark.times %}[{$time}],{% endfor %}]\n"
"                    },\n"
"                    {% endfor %}\n"
"            ]\n"
"        });\n"
"    });\n"
"  </script>\n"
"\n"
" </head>\n"
"\n"
" <body>\n"
"  <div id=\"all-together\"></div>\n"
" </body>\n"
"</html>\n"
"\n"
            };
            static std::string const the_template = []() -> std::string {
                std::string s;
                for(auto part : template_parts) {
                    s += part;
                }
                return s;
            }();
            return the_template;
        }

        std::string description() override {
            return "outputs an HTML file with a single interactive chart of all benchmarks";
        }

        void do_configure(configuration& cfg) override {
            cfg.no_analysis = true;
            title = cfg.title;
            n_samples = cfg.samples;
            verbose = cfg.verbose;
        }

        void do_warmup_start() override {
            if(verbose) progress_stream() << "warming up\n";
        }
        void do_estimate_clock_resolution_start() override {
            if(verbose) progress_stream() << "estimating clock resolution\n";
        }
        void do_estimate_clock_cost_start() override {
            if(verbose) progress_stream() << "estimating cost of a clock call\n";
        }

        void do_benchmark_start(std::string const& name) override {
            if(verbose) progress_stream() << "\nbenchmarking " << name << "\n";
            current = name;
        }

        void do_measurement_start(execution_plan<fp_seconds> plan) override {
            progress_stream() << std::setprecision(7);
            progress_stream().unsetf(std::ios::floatfield);
            if(verbose) progress_stream() << "collecting " << n_samples << " samples, " << plan.iterations_per_sample << " iterations each, in estimated " << detail::pretty_duration(plan.estimated_duration) << "\n";
        }
        void do_measurement_complete(std::vector<fp_seconds> const& samples) override {
            data[current] = samples;
        }

        void do_benchmark_failure(std::exception_ptr) override {
            error_stream() << current << " failed to run successfully\n";
        }

        void do_suite_complete() override {
            if(verbose) progress_stream() << "\ngenerating HTML report\n";

            auto&& templ = template_string();

            auto magnitude = ideal_magnitude();

            cpptempl::data_map map;
            map["title"] = escape(title);
            map["units"] = detail::units_for_magnitude(magnitude);
            for(auto d : data) {
                cpptempl::data_map item;
                item["name"] = escape(d.first);
                for(auto e : d.second) {
                    item["times"].push_back(truncate(e.count() * magnitude));
                }

                map["benchmarks"].push_back(item);
            }

            cpptempl::parse(report_stream(), templ, map);

            report_stream() << std::flush;

            if(verbose) progress_stream() << "done\n";
        }

        static double truncate(double x) {
            return int(x * 1000.) / 1000.;
        }

        double ideal_magnitude() const {
            std::vector<fp_seconds> mins;
            mins.reserve(data.size());
            for(auto d : data) {
                mins.push_back(*std::min_element(d.second.begin(), d.second.end()));
            }
            auto min = *std::min_element(mins.begin(), mins.end());
            return detail::get_magnitude(min);
        }

        static std::string escape(std::string const& source) {
            static const std::unordered_map<char, std::string> escapes {
                { '\'', "&apos;" },
                { '"',  "&quot;" },
                { '<',  "&lt;"   },
                { '>',  "&gt;"   },
                { '&',  "&amp;"  },
                { '\\',  "\\\\"  },
            };
            return detail::escape(source, escapes);
        }

        int n_samples;
        bool verbose;
        std::string title;
        std::string current;
        std::unordered_map<std::string, std::vector<fp_seconds>> data;
    };

    NONIUS_REPORTER("html", html_reporter);
} // namespace nonius

#endif // NONIUS_DISABLE_HTML_REPORTER
#endif // NONIUS_DISABLE_EXTRA_REPORTERS

#endif // NONIUS_HPP

#ifdef NONIUS_RUNNER
// #included from: main.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Executable building kit

#define NONIUS_MAIN_HPP

// #included from: detail/argparse.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Command-line argument parsing

#define NONIUS_ARGPARSE_HPP

// #included from: detail/mismatch.h++
// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// mismatch algorithm

#define NONIUS_DETAIL_MISMATCH_HPP

#include <utility>

namespace nonius {
    namespace detail {
        template <typename InputIt1, typename InputIt2, typename BinaryPredicate>
        std::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, BinaryPredicate p) {
            while(first1 != last1 && first2 != last2 && p(*first1, *first2)) {
                ++first1, ++first2;
            }
            return std::make_pair(first1, first2);
        }
    } // namespace detail
} // namespace nonius

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <ostream>
#include <iomanip>
#include <tuple>
#include <functional>

namespace nonius {
    namespace detail {
        struct option {
            bool matches_short(std::string const& s) const {
                return s == ("-" + short_form);
            }
            std::tuple<bool, std::string::const_iterator> long_separator(std::string const& s) const {
                auto l = "--" + long_form;
                auto its = detail::mismatch(s.begin(), s.end(), l.begin(), l.end(), [](char a, char b) { return a == b; });
                return std::make_tuple(its.second == l.end(), its.first);
            }
            bool matches_long(std::string const& s) const {
                return std::get<0>(long_separator(s));
            }
            bool matches_long(std::string const& s, std::string& argument) const {
                bool match; std::string::const_iterator it;
                std::tie(match, it) = long_separator(s);
                if(match && it != s.end()) {
                    if(*it == '=') argument.assign(it+1, s.end());
                    else return false;
                }
                return match;
            }

            option(std::string long_form, std::string short_form, std::string description, std::string argument = std::string(), bool multiple = false)
            : long_form(std::move(long_form)), short_form(std::move(short_form)), description(std::move(description)), argument(std::move(argument)), multiple(multiple) {}

            std::string long_form;
            std::string short_form;
            std::string description;
            std::string argument;
            bool multiple;
        };

        using option_set = std::vector<option>;

        struct help_text {
            help_text(std::string name, option_set const& options) : name(std::move(name)), options(options) {}
            std::string name;
            option_set const& options;
        };

        template <typename Iterator, typename Projection>
        int get_max_width(Iterator first, Iterator last, Projection proj) {
            auto it = std::max_element(first, last, [&proj](option const& a, option const& b) { return proj(a) < proj(b); });
            return static_cast<int>(proj(*it));
        }

        inline std::ostream& operator<<(std::ostream& os, help_text h) {
            os << "Usage: " << h.name << " [OPTIONS]\n\n";

            auto lwidth = 2 + get_max_width(h.options.begin(), h.options.end(), [](option const& o) { return 2 + o.long_form.size() + 1 + o.argument.size(); });
            auto swidth = 2 + get_max_width(h.options.begin(), h.options.end(), [](option const& o) { return 1 + o.short_form.size() + 1 + o.argument.size(); });

            os << std::left;
            for(auto o : h.options) {
                auto l = "--" + o.long_form;
                auto s = "-" + o.short_form;
                if(!o.argument.empty()) {
                    l += "=" + o.argument;
                    s += " " + o.argument;
                }
                os << std::setw(lwidth) << l
                   << std::setw(swidth) << s
                   << o.description
                   << '\n';
            }
            return os;
        }

        using arguments = std::unordered_map<std::string, std::string>;

        struct argument_error {
            virtual ~argument_error() = default;
        };

        template <typename Iterator>
        void parse_short(option const& o, arguments& args, Iterator& first, Iterator last) {
            if(!o.argument.empty()) {
                if(++first != last) {
                    args.emplace(o.long_form, *first);
                } else {
                    throw argument_error();
                }
            } else {
                args.emplace(o.long_form, "");
            }
        }
        inline void parse_long(option const& o, arguments& args, std::string&& arg) {
            args.emplace(o.long_form, std::move(arg));
        }

        template <typename Iterator>
        arguments parse_arguments(option_set const& options, Iterator first, Iterator last) {
            arguments args;
            for(; first != last; ++first) {
                bool parsed = false;
                for(auto&& option : options) {
                    if(option.matches_short(*first)) {
                        parse_short(option, args, first, last);
                        parsed = true;
                        break;
                    } else {
                        std::string arg;
                        if(option.matches_long(*first, arg)) {
                            parse_long(option, args, std::move(arg));
                            parsed = true;
                            break;
                        }
                    }
                }
                if(!parsed) {
                    throw argument_error();
                }
            }
            return args;
        }
    } // namespace detail
} // namespace nonius

#include <vector>
#include <string>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <iomanip>
#include <utility>

namespace nonius {
    namespace detail {
        template <typename T>
        struct parser;
        template <>
        struct parser<int> {
            static int parse(std::string const& s) { return std::stoi(s); }
        };
        template <>
        struct parser<double> {
            static double parse(std::string const& s) { return std::stod(s); }
        };
        template <>
        struct parser<std::string> {
            static std::string parse(std::string const& s) { return s; }
        };
        template <>
        struct parser<bool> {
            static bool parse(std::string const&) { return true; }
        };
        template <typename T, typename Predicate>
        void parse(T& variable, detail::arguments& args, std::string const& option, Predicate&& is_valid) {
            auto it = args.find(option);
            if(it != args.end()) {
                auto value = parser<T>::parse(it->second);
                if(is_valid(value)) {
                    variable = value;
                } else {
                    throw argument_error();
                }
            }
        }
        template <typename T>
        void parse(T& variable, detail::arguments& args, std::string const& option) {
            return parse(variable, args, option, [](T const&) { return true; });
        }

        inline detail::option_set const& command_line_options() {
            static detail::option_set the_options {
                detail::option("help", "h", "show this help message"),
                detail::option("samples", "s", "number of samples to collect (default: 100)", "SAMPLES"),
                detail::option("resamples", "rs", "number of resamples for the bootstrap (default: 100000)", "RESAMPLES"),
                detail::option("confidence-interval", "ci", "confidence interval for the bootstrap (between 0 and 1, default: 0.95)", "INTERVAL"),
                detail::option("output", "o", "output file (default: <stdout>)", "FILE"),
                detail::option("reporter", "r", "reporter to use (default: standard)", "REPORTER"),
                detail::option("title", "t", "set report title", "TITLE"),
                detail::option("no-analysis", "A", "perform only measurements; do not perform any analysis"),
                detail::option("list", "l", "list benchmarks"),
                detail::option("list-reporters", "lr", "list available reporters"),
                detail::option("verbose", "v", "show verbose output (mutually exclusive with -q)"),
                detail::option("summary", "q", "show summary output (mutually exclusive with -v)"),
            };
            return the_options;
        }

        template <typename Iterator>
        configuration parse_args(std::string const& name, Iterator first, Iterator last) {
            try {
                auto args = detail::parse_arguments(command_line_options(), first, last);

                configuration cfg;

                auto is_positive = [](int x) { return x > 0; };
                auto is_ci = [](double x) { return x > 0 && x < 1; };
                auto is_reporter = [](std::string const x) { return global_reporter_registry().count(x) > 0; };

                parse(cfg.help, args, "help");
                parse(cfg.samples, args, "samples", is_positive);
                parse(cfg.resamples, args, "resamples", is_positive);
                parse(cfg.confidence_interval, args, "confidence-interval", is_ci);
                parse(cfg.output_file, args, "output");
                parse(cfg.reporter, args, "reporter", is_reporter);
                parse(cfg.no_analysis, args, "no-analysis");
                parse(cfg.list_benchmarks, args, "list");
                parse(cfg.list_reporters, args, "list-reporters");
                parse(cfg.verbose, args, "verbose");
                parse(cfg.summary, args, "summary");
                parse(cfg.title, args, "title");
                if(cfg.verbose && cfg.summary) throw argument_error();

                return cfg;
            } catch(...) {
                std::cout << help_text(name, command_line_options());
                throw argument_error();
            }
        }
    } // namespace detail

    inline int print_help(std::string const& name) {
        std::cout << detail::help_text(name, detail::command_line_options());
        return 0;
    }
    inline int list_benchmarks() {
        std::cout << "All available benchmarks:\n";
        for(auto&& b : global_benchmark_registry()) {
            std::cout << "  " << b.name << "\n";
        }
        std::cout << global_benchmark_registry().size() << " benchmarks\n\n";
        return 0;
    }
    inline int list_reporters() {
        using reporter_entry_ref = decltype(*global_reporter_registry().begin());
        auto cmp = [](reporter_entry_ref a, reporter_entry_ref b) { return a.first.size() < b.first.size(); };
        auto width = 2 + std::max_element(global_reporter_registry().begin(), global_reporter_registry().end(), cmp)->first.size();

        std::cout << "Available reporters:\n";
        std::cout << std::left;
        for(auto&& r : global_reporter_registry()) {
            if(!r.first.empty()) {
                std::cout << "  " << std::setw(width) << r.first << r.second->description() << "\n";
            }
        }
        std::cout << '\n';
        return 0;
    }
    inline int run_it(configuration cfg) {
        try {
            nonius::go(cfg);
        } catch(...) {
            std::cerr << "PANIC: clock is on fire\n";
            try {
                throw;
            } catch(std::exception const& e) {
                std::cerr << "  " << e.what() << "\n";
            } catch(...) {}
            return 23;
        }
        return 0;
    }

    template <typename Iterator>
    int main(std::string const& name, Iterator first, Iterator last) {
        configuration cfg;

        try {
            cfg = detail::parse_args(name, first, last);
        } catch(detail::argument_error const&) {
            return 17;
        }

        if(cfg.help) return print_help(name);
        else if(cfg.list_benchmarks) return list_benchmarks();
        else if(cfg.list_reporters) return list_reporters();
        else return run_it(cfg);
    }
    inline int main(int argc, char** argv) {
        std::string name(argv[0]);
        std::vector<std::string> args(argv+1, argv+argc);
        return main(name, args.begin(), args.end());
    }
}

int main(int argc, char** argv) {
    return nonius::main(argc, argv);
}

#endif // NONIUS_RUNNER
#endif // NONIUS_SINGLE_INCLUDE_HPP
