## Sol

Sol is a C++ library binding to Lua. It currently supports Lua 5.2. Sol aims to be easy to use and easy to add to a project.
At this time, the library is header-only for easy integration with projects.

## Caveats

Due to how this library is used compared to the C API, the Lua Stack is completely abstracted away. Not only that, but all 
Lua errors are thrown as exceptions instead. This allows you to handle the errors gracefully without being forced to exit.

It should be noted that the library itself depends on `lua.hpp` to be found by your compiler. It uses angle brackets, e.g.
`#include <lua.hpp>`. 

## Contributing

If you want to contribute, please check CONTRIBUTING.md for details. Thank you!

## Examples

Examples are provided in the examples directory.

## License

Sol is distributed with an MIT License. You can see LICENSE.txt for more info.

## Supported Compilers

Sol makes use of C++11 features. GCC 4.7 and Clang 3.3 or higher should be able to compile without problems. Visual Studio 
2013 with the November CTP should be able to support this as well.

## TODO

- Possibly document functions and classes via doxygen.
- Provide more examples to showcase uses.
