std::(w/u16/u32)string support
==============================
because this is surprisingly hard using standard C++
----------------------------------------------------

Individuals using Visual Studio 2015, or on Windows with the VC++ and MinGW compilers (possibly Clang++ on Windows as well) have ``<codecvt>`` headers, and thusly Sol will attempt to include it. Individuals on GCC 4.9.x, Clang 3.5.x, Clang 3.6.x do not seem to have ``<codecvt>`` shipped with the standard library that comes with installation of these compilers. If you want ``std::wstring``, ``std::u16string``, ``std::u32string`` automatic handling then you need to make sure you have those headers and then define ``SOL_CODECVT_SUPPORT`` on unsupported compilers.

.. _codecvt-deprecation:

.. note::

	Visual C++, in Visual Studio versions 15.5+ (``_MSC_VER >= 1912``), introduced deprecation macros when you are in ``/std:c++latest``. In order to get rid of codecvt issues, you need to define the silencing macros they ask for. It must be defined because despite the committee deprecating it, there is no useful standard alternative to perform these conversions (without, saying, including ``<Windows.h>`` on Windows machines, which is unacceptable).
