threading
=========

Lua has no thread safety. sol2 does not force thread safety bottlenecks anywhere.

Assume any access or any call on Lua affects the whole global state (because it does, in a fair bit of cases). Therefore, every call to a state should be blocked off in C++ with some kind of access control. When you start hitting the same state from multiple threads, race conditions (data or instruction) can happen. Individual Lua coroutines might be able to run on separate C++-created threads without tanking the state utterly, since each Lua coroutine has the capability to run on an independent Lua thread which has its own stack, as well as some other associated bits and pieces that won't quite interfere with the global state.

To handle multithreaded environments, it is encouraged to either spawn mutliple Lua states for each thread you are working with and keep inter-state communication to synchronized serialization points. Using coroutines and Lua's threads might also buy you some concurrency and parallelism, but remember that Lua's threading technique is ultimately cooperative and requires explicit yielding and resuming (simplified as function calls for :doc:`sol::coroutine<api/coroutine>`).
