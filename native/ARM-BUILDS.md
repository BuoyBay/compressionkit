# CompressionKit ARM64 builds

`tools/build_compressionkit_arm.py` adds Linux ARM64 and Windows ARM64 payloads
without changing the original builder or scalar bridge. The existing builder
still extracts the checked-in zlib 1.3.2 archive, verifies its pinned hash,
selects the same ten core C files, and writes its current-source receipt.
The helper invokes that builder through its supported `--cc` interface.

Run from the repository root on Linux x86-64 with Python 3.12 or newer:

```sh
python3 tools/build_compressionkit_arm.py
  --report tests/compressionkit_arm_evidence/verification.json
```

The verifier needs `llvm-readobj` or `llvm-readobj-19`; `--readobj PATH` selects
another installed LLVM inspector. `--native-only` builds without sealing;
`--target arm64-linux` or `--target arm64-windows` selects one slice.
The verifier's `--no-seal` inspects the existing package without rewriting it.

The helper downloads the official Zig **0.14.1 Linux x86-64** distribution from
<https://ziglang.org/download/0.14.1/zig-x86_64-linux-0.14.1.tar.xz> and checks
SHA256 `24aeeec8af16c381934a6cd7d95c807a8cb2cf7df9fa40d359aa884195c4716c`
before extraction. The pin comes from <https://ziglang.org/download/index.json>.
The verified distribution archive and compiler cache remain in
`~/.cache/buoy-compressionkit-arm`, overridable with `COMPRESSIONKIT_ARM_CACHE`.
Each compiler invocation extracts that verified archive into a fresh temporary
directory and removes it afterward; an old extracted compiler tree is never
executed. There are no global compiler or sysroot installations. The compiler
cache must not be shared with untrusted writers.

Zig is MIT licensed; bundled LLVM uses Apache-2.0 with the LLVM exception.
The distribution carries component licenses, including `lib/libc/mingw/COPYING`
and `lib/libc/glibc/LICENSES`. Copies of the Zig and MinGW notices are retained
under `tests/compressionkit_arm_evidence/`. zlib retains its existing package
license. The toolchain archive is a build dependency and is not packaged.

Linux targets `aarch64-linux-gnu.2.17`, with ELF hidden visibility and a dynamic
`libc.so.6` dependency. Windows targets `aarch64-windows-gnu`, producing a genuine
ARM64 PE DLL and ARM64 COFF import `.lib` compatible with Buoy's MSVC target
linker. This does not require a MinGW runtime DLL: only Windows OS and UCRT
imports remain. Zig's compiler-rt replaces the existing builder's inapplicable
`-static-libgcc` switch. Debug information is stripped to avoid build-directory
paths and sidecar PDBs. No assembly optimizations are added.

The helper compiles assertions for 8-byte pointers/int64_t, 4-byte C int, and
platform C long/z_stream sizes (Linux 8/112; Windows 4/88). Each new payload
has the original `.source-sha256` receipt plus supplemental `.toolchain.json`
provenance containing the download pin, helper hash, ABI assertions and binary
hash. The original receipt intentionally retains the original builder's digest
contract, preserving the four existing slices' receipts.

Verification checks ELF/PE AArch64 identity, 64-bit addresses, the import library,
exactly eight public `bck_*` exports, dependency allowlists, six current slices
inside the sealed archive, and archive payload equality. No vendor zlib symbols
are public, so they cannot interpose on an application's zlib. It syntax-checks
and cross-links the existing full streaming `.bui` consumer against the sealed
package for both ARM targets, then checks the resulting executable architecture.
`--rebuild` additionally requires byte-identical `.so`, `.dll` and `.lib` output
from two separate temporary source extractions/builds.
Verifier checks use explicit exceptions and remain active under `python -O`.

## Execution limits

The evidence records compilation, ABI assertions, binary inspection, sealing
and cross-linking. **Neither ARM64 Linux nor ARM64 Windows code was executed.**
The available Linux host and Windows VM are x86-64. The available Apple ARM64
machine cannot natively execute Linux ELF or Windows PE binaries. No QEMU result
is represented as native execution; no emulated execution was performed.
Native streaming/interoperability regressions on actual Linux ARM64 and Windows
ARM64 hosts remain release validation work.

[Package overview](../docs/API.md)

Test suites and fixtures are maintainer-local and are not part of this public repository.
