compatibility.hpp
=================
Lua 5.3/5.2 compatibility for Lua 5.1/LuaJIT
--------------------------------------------

This is a detail header used to maintain compatability with the 5.2 and 5.3 APIs. It contains code from the MIT-Licensed `Lua<http://www.Lua.org/>` code in some places and also from the `lua-compat<https://github.com/keplerproject/lua-compat-5.3>` repository by KeplerProject.

It is not fully documented as this header's only purpose is for internal use to make sure Sol compiles across all platforms / distributions with no errors or missing Lua functionality. If you think there's some compatibility features we are missing or if you are running into redefinition errors, please make an `issue in the issue tracker<https://github.com/ThePhD/sol2/issues/>`.

For the licenses, see :doc:`here<../licenses>`