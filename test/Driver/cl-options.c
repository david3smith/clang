// Don't attempt slash switches on msys bash.
// REQUIRES: shell-preserves-root

// Note: %s must be preceded by --, otherwise it may be interpreted as a
// command-line option, e.g. on Mac where %s is commonly under /Users.


// Alias options:

// RUN: %clang_cl /c -### -- %s 2>&1 | FileCheck -check-prefix=C %s
// C: -c

// RUN: %clang_cl /Dfoo=bar -### -- %s 2>&1 | FileCheck -check-prefix=D %s
// RUN: %clang_cl /D foo=bar -### -- %s 2>&1 | FileCheck -check-prefix=D %s
// D: "-D" "foo=bar"

// RUN: %clang_cl /E -### -- %s 2>&1 | FileCheck -check-prefix=E %s
// E: "-E"
// E: "-o" "-"

// RTTI is on by default; just check that we don't error.
// RUN: %clang_cl /Zs /GR -- %s 2>&1

// RUN: %clang_cl /GR- -### -- %s 2>&1 | FileCheck -check-prefix=GR_ %s
// GR_: -fno-rtti

// RUN: %clang_cl /Imyincludedir -### -- %s 2>&1 | FileCheck -check-prefix=SLASH_I %s
// RUN: %clang_cl /I myincludedir -### -- %s 2>&1 | FileCheck -check-prefix=SLASH_I %s
// SLASH_I: "-I" "myincludedir"

// RUN: %clang_cl /J -### -- %s 2>&1 | FileCheck -check-prefix=J %s
// J: -fno-signed-char

// RUN: %clang_cl /Ofoo -### -- %s 2>&1 | FileCheck -check-prefix=O %s
// O: -Ofoo

// RUN: %clang_cl /Ob0 -### -- %s 2>&1 | FileCheck -check-prefix=Ob0 %s
// Ob0: -fno-inline

// RUN: %clang_cl /Od -### -- %s 2>&1 | FileCheck -check-prefix=Od %s
// Od: -O0

// RUN: %clang_cl /Oi- /Oi -### -- %s 2>&1 | FileCheck -check-prefix=Oi %s
// Oi-NOT: -fno-builtin

// RUN: %clang_cl /Oi- -### -- %s 2>&1 | FileCheck -check-prefix=Oi_ %s
// Oi_: -fno-builtin

// RUN: %clang_cl /Os -### -- %s 2>&1 | FileCheck -check-prefix=Os %s
// Os: -Os

// RUN: %clang_cl /Ot -### -- %s 2>&1 | FileCheck -check-prefix=Ot %s
// Ot: -O2

// RUN: %clang_cl /Ox -### -- %s 2>&1 | FileCheck -check-prefix=Ox %s
// Ox: -O3

// RUN: %clang_cl /Zs /Oy -- %s 2>&1

// RUN: %clang_cl /Oy- -### -- %s 2>&1 | FileCheck -check-prefix=Oy_ %s
// Oy_: -mdisable-fp-elim

// RUN: %clang_cl /showIncludes -### -- %s 2>&1 | FileCheck -check-prefix=showIncludes %s
// showIncludes: --show-includes

// RUN: %clang_cl /Umymacro -### -- %s 2>&1 | FileCheck -check-prefix=U %s
// RUN: %clang_cl /U mymacro -### -- %s 2>&1 | FileCheck -check-prefix=U %s
// U: "-U" "mymacro"

// RUN: %clang_cl /W0 -### -- %s 2>&1 | FileCheck -check-prefix=W0 %s
// W0: -w

// RUN: %clang_cl /W1 -### -- %s 2>&1 | FileCheck -check-prefix=W1 %s
// RUN: %clang_cl /W2 -### -- %s 2>&1 | FileCheck -check-prefix=W1 %s
// RUN: %clang_cl /W3 -### -- %s 2>&1 | FileCheck -check-prefix=W1 %s
// RUN: %clang_cl /W4 -### -- %s 2>&1 | FileCheck -check-prefix=W1 %s
// RUN: %clang_cl /Wall -### -- %s 2>&1 | FileCheck -check-prefix=W1 %s
// W1: -Wall

// RUN: %clang_cl /WX -### -- %s 2>&1 | FileCheck -check-prefix=WX %s
// WX: -Werror

// RUN: %clang_cl /WX- -### -- %s 2>&1 | FileCheck -check-prefix=WX_ %s
// WX_: -Wno-error

// RUN: %clang_cl /w -### -- %s 2>&1 | FileCheck -check-prefix=w %s
// w: -w

// RUN: %clang_cl /Zs -### -- %s 2>&1 | FileCheck -check-prefix=Zs %s
// Zs: -fsyntax-only

// RUN: %clang_cl /FIasdf.h -### -- %s 2>&1 | FileCheck -check-prefix=FI %s
// FI: "-include" "asdf.h"

// RUN: %clang_cl /FI asdf.h -### -- %s 2>&1 | FileCheck -check-prefix=FI_ %s
// FI_: "-include" "asdf.h"

// We forward any unrecognized -W diagnostic options to cc1.
// RUN: %clang_cl -Wunused-pragmas -### -- %s 2>&1 | FileCheck -check-prefix=WJoined %s
// WJoined: "-cc1"
// WJoined: "-Wunused-pragmas"


// Ignored options. Check that we don't get "unused during compilation" errors.
// (/Zs is for syntax-only, /WX is for -Werror)
// RUN: %clang_cl /Zs /WX \
// RUN:    /analyze- \
// RUN:    /errorReport:foo \
// RUN:    /FS \
// RUN:    /GF \
// RUN:    /GS- \
// RUN:    /kernel- \
// RUN:    /nologo \
// RUN:    /Ob1 \
// RUN:    /Ob2 \
// RUN:    /RTC1 \
// RUN:    /sdl \
// RUN:    /sdl- \
// RUN:    /vmg \
// RUN:    /w12345 \
// RUN:    /wd1234 \
// RUN:    /Zc:forScope \
// RUN:    /Zc:wchar_t \
// RUN:    -- %s

// Ignored options and compile-only options are ignored for link jobs.
// RUN: touch %t.obj
// RUN: %clang_cl /nologo -### -- %t.obj 2>&1 | FileCheck -check-prefix=LINKUNUSED %s
// RUN: %clang_cl /Dfoo -### -- %t.obj 2>&1 | FileCheck -check-prefix=LINKUNUSED %s
// RUN: %clang_cl /MD -### -- %t.obj 2>&1 | FileCheck -check-prefix=LINKUNUSED %s
// LINKUNUSED-NOT: argument unused during compilation

// Support ignoring warnings about unused arguments.
// RUN: %clang_cl /Abracadabra -Qunused-arguments -### -- %s 2>&1 | FileCheck -check-prefix=UNUSED %s
// UNUSED-NOT: warning

// Unsupported but parsed options. Check that we don't error on them.
// (/Zs is for syntax-only)
// RUN: %clang_cl /Zs \
// RUN:     /AIfoo \
// RUN:     /arch:sse2 \
// RUN:     /clr:pure \
// RUN:     /docname \
// RUN:     /EHsc \
// RUN:     /EP \
// RUN:     /F \
// RUN:     /FA \
// RUN:     /FAc \
// RUN:     /Fafilename \
// RUN:     /FAs \
// RUN:     /FAu \
// RUN:     /favor:blend \
// RUN:     /FC \
// RUN:     /Fdfoo \
// RUN:     /Fifoo \
// RUN:     /Fmfoo \
// RUN:     /FpDebug\main.pch \
// RUN:     /fp:precise \
// RUN:     /Frfoo \
// RUN:     /FRfoo \
// RUN:     /FU foo \
// RUN:     /Fx \
// RUN:     /G1 \
// RUN:     /G2 \
// RUN:     /GA \
// RUN:     /Gd \
// RUN:     /Ge \
// RUN:     /Gh \
// RUN:     /GH \
// RUN:     /GL \
// RUN:     /GL- \
// RUN:     /Gm \
// RUN:     /Gm- \
// RUN:     /Gr \
// RUN:     /GS \
// RUN:     /Gs1000 \
// RUN:     /GT \
// RUN:     /GX \
// RUN:     /Gy \
// RUN:     /Gy- \
// RUN:     /Gz \
// RUN:     /GZ \
// RUN:     /H \
// RUN:     /homeparams \
// RUN:     /hotpatch \
// RUN:     /kernel \
// RUN:     /LN \
// RUN:     /MP \
// RUN:     /o foo.obj \
// RUN:     /ofoo.obj \
// RUN:     /openmp \
// RUN:     /Qfast_transcendentals \
// RUN:     /QIfist \
// RUN:     /Qimprecise_fwaits \
// RUN:     /Qpar \
// RUN:     /Qvec-report:2 \
// RUN:     /u \
// RUN:     /V \
// RUN:     /vd2 \
// RUN:     /vmb \
// RUN:     /vmm \
// RUN:     /vms \
// RUN:     /vmv \
// RUN:     /volatile \
// RUN:     /wfoo \
// RUN:     /WL \
// RUN:     /Wp64 \
// RUN:     /X \
// RUN:     /Y- \
// RUN:     /Yc \
// RUN:     /Ycstdafx.h \
// RUN:     /Yd \
// RUN:     /Yl- \
// RUN:     /Ylfoo \
// RUN:     /Yustdafx.h \
// RUN:     /Z7 \
// RUN:     /Za \
// RUN:     /Zc:auto \
// RUN:     /Zc:wchar_t- \
// RUN:     /Ze \
// RUN:     /Zg \
// RUN:     /Zi \
// RUN:     /ZI \
// RUN:     /Zl \
// RUN:     /Zp \
// RUN:     /Zp1 \
// RUN:     /ZW:nostdlib \
// RUN:     -- %s 2>&1

// We support -Xclang for forwarding options to cc1.
// RUN: %clang_cl -Xclang hellocc1 -### -- %s 2>&1 | FileCheck -check-prefix=Xclang %s
// Xclang: "-cc1"
// Xclang: "hellocc1"

// We support -m32 and -m64.
// RUN: %clang_cl /Zs /WX -m32 -m64 -- %s


void f() { }
