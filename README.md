# Kraken #

Kraken is a very minimal replacement for a standard library for C++ with a very minimal feature set.

It is designed for use on embedded Linux system for specific use cases.

Nothing in this library is new or exciting, it just adheres to a very specific set of rules.

## Restricttions ##
Kraken is designed for use in security aware embedded Linux devices, and it doesn't use any of the following:
 - libstdc++
 - Exceptions
 - STL (except for the `type_traits`)

The main focus of the library is to provide a thin object-oriented wrapper for Linux file handling.
Most of the functionality will probably work for any POSIX compliant system, but it was not tested against any such system.

Given these restrictions, Kraken takes an unusual approach for C++ that many might not like.

__For example__:
Constructors are not allowed to contain code that fails, because there's no return value and no exceptions can be used. 
Thus, most classes contain an `Init` or an `Open` methods.

Destructors are allowed to try (and potentially fail) to dispose of the object in order to maintain RAII.

For more information refer to the `docs/RESTRICTIONS.md` file.

## Features ##
The library contains (or will contain in the near future), the following wrappers:
- [x] `File` - A wrapper for the basic and most common Linux File operations:
  - [x] `Open` & `Close`.
  - [x] `Read` & `Write` (+ at an offset).
  - [ ] `VectorRead` & `VectorWrite` (+ at an offset).
  - [x] `IOControl`
- [x] `Socket` - A generic wrapper around the `socket` syscall. The domain & type of the socket are given to the `Init` method.
- Wrappers around posix Socket-Addresses horrible interface. (Kraken's implementation is horrible as well, but the user-facing interface is quite nice):
  - [x] Unix addresses (both file paths and abstract)
  - [x] IPv4 addresses
  - [x] IPv6 addresses
  - [ ] Raw Ethernet
- [ ] `Event` - eventfd wrapper.
- [ ] `Timer` - timerfd wrapper.
- [x] `EPoll`, `IEPollable` - Generic epoll wrappers.
- Collections:
  - [x] `slice` - A simple struct that holds the address and size of the buffer. (probably quite useless on its own).
  - [x] `slice_adapter`- A weird way to bridge between third-party collections and `slice`.
  - [x] `array` - An almost-copy of `std::array` that can be implicitly converted to a `slice`.

## About slices ##
Slices are a compromise that allows the user to avoid passing raw-pointers and their size.
Although the slice itself is just that - a pointer and an integer - it allows sending varous buffer-like object directly to Kraken functions.

Each IO function is written first by receiving a `void *, size_t` pair, and then an overload is added that receives a slice instead.
  
All the Kraken collections that represent buffers are implicitly converted to a slice.

Even though Kraken itself does not use stdc++ or have any dependencies (except for libc), we do not want to prevent you
from using any library for its collections.
For that reason, there's an ugly and convoluted way to extend third-party objects to support implicit slice conversion.

Refer to `docs/SLICES.md` for more information.