codecvt + std::(w/u16/u32)string support
========================================
because this is surprisingly hard using standard C++
----------------------------------------------------

Individuals using Visual Studio 2015, or on Windows with the VC++ and MinGW compilers (possibly Clang++ on Windows as well) have ``<codecvt>`` headers, and thusly Sol will attempt to include it. Individuals on GCC 4.9.x, Clang 3.5.x, Clang 3.6.x do not seem to have ``<codecvt>`` shipped with the standard library that comes with installation of these compilers. If you want ``std::wstring``, ``std::u16string``, ``std::u32string`` automatic handling then you need to make sure you have those headers and then define ``SOL_CODECVT_SUPPORT`` on unsupported compilers.

.. _codecvt-deprecation:

Visual C++, in Visual Studio versions 15.5+ (``_MSC_VER >= 1912``), introduced deprecation macros when you are in ``/std:c++latest``, or similar. In order to get rid of codecvt issues, you need to define the silencing macros they ask for (``_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING``).

It must be defined because despite the committee deprecating it, there is no useful standard alternative to perform these conversions (without, saying, including ``<Windows.h>`` on Windows machines, which is unacceptable).

.. note::

	The `standard`_ states the following:

	| “this library component should be retired to Annex D, along side , 
	| until a suitable replacement is standardized.”


In other words, it is totally fine! Just define the silencing macro and leave it at that. sol2 keeps up with the standard, so the moment a suitable replacement is standardized it will be added to sol2 posthaste. For the time being, you will be okay.

.. _standard: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0618r0.html
