;;; SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
;;;
;;; SPDX-License-Identifier: GPL-3.0-or-later

(list (channel
        (name 'guix)
        (url "https://git.guix.gnu.org/guix.git")
        (branch "master")
        (commit
          "48fa6de73684b544d904f66e7de50860cec95ee3")
        (introduction
          (make-channel-introduction
            "9edb3f66fd807b096b48283debdcddccfea34bad"
            (openpgp-fingerprint
              "BBB0 2DDF 2CEA F6A8 0D1D  E643 A2A0 6DF2 A33A 54FA")))))
