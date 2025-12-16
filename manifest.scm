;;; SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
;;;
;;; SPDX-License-Identifier: GPL-3.0-or-later

; TODO: Can we make a gcc-toolchain without libc?

(specifications->manifest (list "reuse"
                                "cppcheck"
                                "gdb"
                                "strace"
                                "qemu"
				"kcachegrind"
				"graphviz"
                                "make"
                                "bmake"
                                "universal-ctags"
                                "shellcheck"
                                "valgrind"
                                "hyperfine"
                                "clang"
                                "gcc-toolchain"))
