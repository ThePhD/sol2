supported compilers, binary size, compile time
==============================================
getting good final product out of sol2
--------------------------------------

supported compilers
-------------------

GCC 7.x is now out alongside Visual Studio 2017. This means that Sol v. 2.17.5 will be the last version of the code targeted at the older compilers. Newer code will be targeted at working with the following compilers and leveraging their features, possibly taking advantage of whatever C++17 features are made available by the compilers and standard libraries bundled by-default with them:

* VC++
	- Visual Studio 2017
	- Visual Studio 2015 (with latest updates)
* GCC (includes MinGW)
	- v7.x
	- v6.x
	- v5.x 
* Clang
	- v4.x
	- v3.9.x
	- v3.8.x
	- v3.7.x
	- Note: this applies to XCode's Apple Clang as well, but that compiler packs its own deficiencies and problems as well

Note that Visual Studio's 2017 Community Edition is absolutely free now, and installs faster and easier than ever before. It also removes a lot of hacky work arounds and formally supports decltype SFINAE.

MinGW's GCC version 7.x of the compiler fixes a long-standing derp in the <codecvt> header that swapped the endianness of utf16 and utf32 strings.

Clang 3.4, 3.5 and 3.6 have many bugs we have run into when developing sol2 and that have negatively impacted users for a long time now.

We encourage all users to upgrade immediately. If you need old code, use `sol2 release v2.17.5`_: otherwise, always grab sol2's latest.

"compiler out of heap space"
----------------------------

Typical of Visual Studio, the compiler will complain that it is out of heap space because Visual Studio defaults to using the x86 (32-bit) version of itself (it will still compile x86 or x64 or ARM binaries, just the compiler **itself** is a 32-bit executable). In order to get around heap space requirements, add the following statement in your ``.vcoxproj`` files under the ``<Import .../>`` statement, as instructed by `OrfeasZ in this issue`_::

	<PropertyGroup>
		<PreferredToolArchitecture>x64</PreferredToolArchitecture>
	</PropertyGroup>


This should use the 64-bit tools by default, and increase your maximum heap space to whatever a 64-bit windows machine can handle.

binary sizes
------------

For individiauls who use :doc:`usertypes<api/usertype>` a lot, they can find their compilation times increase. This is due to C++11 and C++14 not having very good facilities for handling template parameters and variadic template parameters. There are a few things in cutting-edge C++17 and C++Next that sol can use, but the problem is many people cannot work with the latest and greatest: therefore, we have to use older techniques that result in a fair amount of redundant function specializations that can be subject to the pickiness of the compiler's inlining and other such techniques.

what to do
----------

Here are some notes on achieving better compile-times without sacrificing too much performance:

* When you bind lots of usertypes, put them all in a *single* translation unit (one C++ file) so that it is not recompiled multiple times over, only to be discarded later by the linker.
	- Remember that the usertype binding ends up being serialized into the Lua state, so you never need them to appear in a header and cause that same compilation overhead for every compiled unit in your project.
* Consider placing groups of bindings in multiple different translation units (multiple C++ source files) so that only part of the bindings are recompiled when you have to change the bindings.
	- Avoid putting your bindings into headers: it *will* slow down your compilation
* For extremely large usertypes, consider using :doc:`simple_usertype<api/simple_usertype>`.
	- It performs much more work at runtime rather than compile-time, and should still give comparative performance (but it loses out in some cases for variable bindings or when you bind all functions to a usertype).
* If you are developing a shared library, restrict your overall surface area by specifically and explicitly marking functions as visible and exported and leaving everything else as hidden or invisible by default


next steps
----------

The next step for Sol from a developer standpoint is to formally make the library a C++17 one. This would mean using Fold Expressions and several other things which will reduce compilation time drastically. Unfortunately, that means also boosting compiler requirements. While most wouldn't care, others are very slow to upgrade: finding the balance is difficult, and often we have to opt for backwards compatibility and fixes for bad / older compilers (of which there are many in the codebase already).

Hopefully, as things progress, we move things forward.


.. _sol2 release v2.17.5: https://github.com/ThePhD/sol2/releases/tag/v2.17.5
.. _OrfeasZ in this issue: https://github.com/ThePhD/sol2/issues/329#issuecomment-276824983
