optional
========

This is an implemention of `optional from the standard library`. If it detects that a proper optional exists, it will attempt to use it. This is mostly an implementation detail, used in the :ref:`stack::check_get<stack-check-get>` and :ref:`stack::get\<optional\<T>><stack-get>` and ``optional<T> maybe_value = table["arf"];`` impementations for additional safety reasons.