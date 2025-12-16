;; SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
;;
;; SPDX-License-Identifier: GPL-3.0-or-later

(define-module (yoctofetch-package)
	       #:use-module (guix packages)
	       #:use-module (guix build-system gnu)
	       #:use-module (guix git)
	       #:use-module (guix git-download)
	       #:use-module (guix utils)
	       #:use-module (guix gexp)
	       #:use-module ((guix licenses) #:prefix license:))

(define vcs-file?
  (or (git-predicate (string-append (current-source-directory) "/../.."))
      (const #t)))

(define-public yoctofetch
  (package
    (name "yoctofetch")
    (version "1.1.0-git")
    (source
     (local-file "../.."
                 "yoctofetch-checkout"
                 #:recursive? #t
                 #:select? vcs-file?))
    (supported-systems '("x86_64-linux" "aarch64-linux"))
    (build-system gnu-build-system)
    (arguments
     (list
      #:tests? #f))
    (synopsis "Fast(est) system fetch tool for Linux with a low footprint")
    (description
     "System fetch tool like neofetch, nanofetch etc.  It is very fast and very
     lightwheigt, but also very limited in features.")
    (home-page "https://codeberg.org/Phosphenius/yoctofetch")
    (license license:agpl3+)))

yoctofetch
