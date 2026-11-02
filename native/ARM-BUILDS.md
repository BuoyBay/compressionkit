# ARM64 native backend requirements

CompressionKit's scalar C bridge uses the included checksum-pinned zlib source
listed in [sources.json](sources.json). Rebuild both the codec and `bridge.c` for
the consumer's target ABI before sealing a package.

## Target ABIs

- Linux ARM64 uses the AArch64 GNU ABI with 64-bit pointers and `libc.so.6`.
  Native codec symbols must remain hidden from the executable's global namespace.
- Windows ARM64 requires an ARM64 PE DLL and matching COFF import library usable
  by Buoy's target linker. Runtime dependencies should be limited to the expected
  Windows OS/UCRT imports.

C `long` and zlib's `z_stream` layout differ across these ABIs. Match the actual
target headers and export only the public `bck_*` bridge interface. Compile and
link the consuming application against its architecture-matching runtime.

[Zig](https://ziglang.org/) can provide cross-target C toolchains; native target
SDK/toolchain requirements and the toolchain's component licenses still apply.
A cross-linked consumer is not evidence of successful execution on ARM hardware.

The [zlib notice](../resources/zlib-LICENSE.txt) and retained toolchain notices
remain applicable to their respective source/runtime components.

[Package API](../docs/API.md)
