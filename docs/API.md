# CompressionKit 0.1.0

Incremental gzip and zlib streams for Buoy, with explicit input/output budgets,
checksummed completion and scoped native ownership. Copy the sealed
`packages/compressionkit-0.1.0.buipkg` into a consumer's packages directory, then
`Import compressionkit`. The archive includes native bridge slices listed below;
zlib 1.3.2 is statically compiled into each bridge, so no system zlib installation
is required. Buoy's bundled zip package remains suitable for whole-buffer helpers.

| API | Behavior |
| --- | --- |
| `New StreamCompressor(formatName = "gzip", level = -1, maxInputBytes = 67108864, maxOutputBytes = 67108864, maxChunkBytes = 1048576, maxCallOutputBytes = 16777216)` | Create one gzip or zlib encoder; level is -1/default or 0..9 |
| `New StreamDecompressor(formatName = "gzip", maxInputBytes = 67108864, maxOutputBytes = 67108864, maxChunkBytes = 1048576, maxCallOutputBytes = 16777216)` | Create a strict decoder for exactly one member/stream |
| `Write(data As Bytes) As Bytes` | Borrow input during the call, consume the entire chunk, return newly emitted owned bytes |
| `compressor.Flush() As Bytes` | Z_SYNC_FLUSH; emit pending data and an aligned empty block while leaving the stream open |
| `Finish() As Bytes` | Encoder: emit final bytes/footer. Decoder: require a complete validated end marker. Call exactly once. |
| `Close()` | Idempotent abort/release; never implicitly finishes or validates a stream |
| `IsClosed`, `IsFinished` | Lifecycle flags retained after Close |
| `InputBytes`, `OutputBytes` | Counts at the last successful operation; never include a discarded failing-call result |
| `decoder.IsStreamEnd` | A complete checksummed member was encountered; caller must still Finish after supplying all input |

Format names are exact lowercase `gzip` or `zlib`. The codec maintains only its
compression state and the current call's bounded output, rather than retaining
the whole input. Returned Bytes are independent of later calls or Close. Each
stream is confined to one thread; separate streams can operate independently.
Destructors release native state, but call Close explicitly when prompt release
matters. Repeated Finish and operations after Finish/Close are invalid arguments.

## Completion, strictness and provisional output

**Decoded chunks remain provisional until Finish succeeds.** A stream can emit
plaintext before a later checksum, length or trailer failure is discovered.
Previously returned Bytes cannot be recalled. Consumers needing all-or-nothing
publication should stage chunks privately and commit only after successful Finish.

Truncated input fails at Finish. Corrupt headers, deflate data, checksum/length
mismatches, preset dictionaries and any trailing bytes fail. Both concatenated
gzip members and concatenated zlib streams are rejected, whether they arrive in
one Write or later Writes. There is no gzip auto-detection or partial/truncation
recovery mode. Gzip optional headers are interpreted by zlib. Numeric byte offsets,
header timestamps or compressed bytes are not promised identical across platforms.

Codec errors throw CompressionException and permanently close the stream. The
failing call returns no output. Nil input and bad constructor arguments throw
InvalidArgumentException; a Nil Write does not alter an otherwise active stream.
Closing an unfinished encoder abandons it, and closing a decoder does not validate
the data. Finish cannot be replaced by resource cleanup.

## Resource limits

Lifetime input/output limits are nonnegative 64-bit byte counts. Input means
uncompressed bytes for an encoder and compressed bytes for a decoder; output is
the converse, including compressed headers and footers. maxChunkBytes and
maxCallOutputBytes are each 1..16777216. No call can allocate a native output
buffer larger than its configured call output limit, even with a very large
lifetime budget. The returned Bytes add a bounded copy of that call's output.
The zlib working state and the caller's input buffer are additional storage.

Exceeding any budget is a terminal CompressionException, not a partial success.
For a highly expanding input, feed smaller chunks or deliberately choose a larger
call output budget within the cap. A zero output budget can decode a valid empty
stream; even an empty encoder needs room for its compressed header/footer.
Counters are advanced only after a successful public operation. Flush can add
compressed bytes and degrade compression ratio; avoid flushing after every small
chunk unless the consumer needs the boundary immediately.

## Build and verify

```sh
python3 tools/build_compressionkit.py
python3 tools/build_compressionkit.py --target x86_64-windows --cc x86_64-w64-mingw32-gcc --native-only
python3 tools/build_compressionkit.py --seal-only
```

The build extracts the pinned upstream source archive after checking SHA-256,
compiles its core codec sources plus a scalar ABI bridge, and records a source
receipt (including the build recipe) beside each runtime. Vendored symbols are
hidden on ELF/Mach-O so other imported zlib libraries cannot replace the pinned
codec through symbol interposition. Sealing includes only slices matching the current
source digest. Windows import libraries are included. All six target payloads are
provided: Linux x86_64/ARM64, macOS ARM64/Intel, and Windows x86_64/ARM64.
Linux ARM64 and Windows ARM64 are cross-built, ABI/export/dependency inspected,
and sealed-consumer cross-linked; native execution on those two targets remains
unverified. See [ARM build provenance and reproduction](../native/ARM-BUILDS.md).
Compiler checks and cross-links are recorded separately from native execution.

The verifier compiler-checks the module and sealed consumer on six desktop
targets. It executes strict regressions and 36 two-way Python zlib/gzip oracles
over empty, binary/NUL, Unicode, random and compressible payloads with one-byte,
seven-byte and large chunk boundaries. Tests cover sync flush, exact/zero limits,
over-budget output/input/chunks, corruption, truncation, later-call trailing data,
concatenation, dictionaries, terminal failure, repeated close and explicit abort.
The same strict consumer and oracles pass native macOS ARM64 and Windows x86_64;
macOS Intel consumers also link, with Intel execution untested. Reproduce native
runs with `tests/standard_library_macos.py` and `tests/standard_library_windows.py`.
BUOY and BPM_PACKAGER select toolchain commands. The builder uses GCC/Clang-style
C flags and Python with tarfile's data extraction filter (Python 3.12+).

The [upstream source record](../native/sources.json) and
[zlib license](../resources/zlib-LICENSE.txt) accompany this independent bridge.
Zig and MinGW component notices are also included under package resources for
the ARM build toolchain.
No declaresub implementation is copied.

Test suites and fixtures are maintainer-local and are not part of this public repository.
