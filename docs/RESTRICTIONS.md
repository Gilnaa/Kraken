# Kraken Restrictions #
Kraken's code must follow the restrictions listed in this file.
Some are specified due to performance reasons, but most are due to over-paranoid standards set by my superiors:

 - No implicit dynamic allocation.
 - Classes that hold system resources must **not** have copy-constructors.
 - Code should not use standard C++ headers, nor be linked with libstdc++. (The only exception is the `type_traits` header).
 - Absolutely no exceptions, anywhere.

Some guidelines that are implied by these restrictions:
 - Constructors cannot fail. If the code in the constructor will have thrown an exception in regular code, move it to an `Init` so it could return an error value.
 - Use `errno`. Since most of the functions are thin wrappers around syscalls they return `-errno` on error.