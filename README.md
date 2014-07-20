# DumbShell

This is my excuse to write C. (What's yours?)

Features:
 - it can run ` ls   ; ls -la `
 - BUT, it can't run ` ; ls`

### Running

```sh
cmake .
make
./dsh
```

### Tests

Requires version 0.9.13 of the [check](http://check.sourceforge.net/) unit-testing framework.

```sh
cmake .
make check
```

### Screenshot

```
$ (5) ls -l
total 5232
-rw-r--r--  1 abesto staff   13533 Jul 15 12:58 CMakeCache.txt
drwxr-xr-x 22 abesto staff     748 Jul 17 18:36 CMakeFiles
-rw-r--r--  1 abesto staff     488 Jul 17 16:41 CMakeLists.txt
-rw-r--r--  1 abesto staff     311 Jul 15 12:49 CTestTestfile.cmake
-rw-r--r--  1 abesto staff    9903 Jul 17 17:04 Makefile
-rw-r--r--  1 abesto staff     258 Jul 17 16:41 README.md
-rwxr-xr-x  1 abesto staff   26456 Jul 17 18:36 all_tests
-rw-r--r--  1 abesto staff    1251 Jul 15 12:20 cmake_install.cmake
-rwxr-xr-x  1 abesto staff   10664 Jul 15 11:58 coverage
drwxr-xr-x  2 abesto staff      68 Jul 15 08:46 doc
-rwxr-xr-x  1 abesto staff   16732 Jul 17 18:36 dsh
-rw-r--r--  1 abesto staff   20184 Jul 17 18:36 libdsh_lib.a
-rw-r--r--  1 abesto staff 5211827 Jul 15 21:27 out
-rwxr-xr-x  1 abesto staff   14008 Jul 15 12:47 run_checks
drwxr-xr-x 12 abesto staff     408 Jul 17 18:36 src
drwxr-xr-x  7 abesto staff     238 Jul 17 18:25 tests
```

----

Shell cat:

![shell cat](http://s3.favim.com/orig/47/black-and-white-cat-cute-shell-Favim.com-438008.jpg)
