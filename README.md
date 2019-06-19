# libcashaccount

Small and efficient library for working with Cash Accounts. Cash Accounts is a
a naming system that can be used alongside regular bitcoin addresses to
simplify the process of sharing payment information.

This library is written in C++, but publishes a C interface to ease creating
bindings to ther languages.

To test bindings creation, this repo also builds example Rust bindings.

# Build

This library uses `scons` to build.

```
scons
```

Builds library and rust bindings.

```
scons lib
```

Builds only the library.

# Doxygen
To build documentation, run `scons docs`

# Formatting
To format files, run `scons format`
