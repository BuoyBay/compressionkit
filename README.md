# Stream gzip and zlib data with explicit resource bounds

Incremental gzip and zlib streams for Buoy, with explicit input/output budgets,
checksummed completion and scoped native ownership. Copy the sealed
`packages/compressionkit-0.1.0.buipkg` into a consumer's packages directory, then
`Import compressionkit`. The archive includes native bridge slices listed below;
zlib 1.3.2 is statically compiled into each bridge, so no system zlib installation
is required. Buoy's bundled zip package remains suitable for whole-buffer helpers.

Format names are exact lowercase `gzip` or `zlib`. The codec maintains only its
compression state and the current call's bounded output, rather than retaining
the whole input. Returned Bytes are independent of later calls or Close. Each
stream is confined to one thread; separate streams can operate independently.
Destructors release native state, but call Close explicitly when prompt release
matters. Repeated Finish and operations after Finish/Close are invalid arguments.

## Install

From a Buoy consumer project with BPM installed:

```sh
bpm add compressionkit --git https://github.com/BuoyBay/compressionkit.git --rev refs/heads/main
```

BPM records the resolved commit in its lockfile. Keep that lockfile with the
consumer project for reproducible installs.

## API entrypoints

- `Import compressionkit`
- `New StreamCompressor(formatName = "gzip", level = -1, maxInputBytes = 67108864, maxOutputBytes = 67108864, maxChunkBytes = 1048576, maxCallOutputBytes = 16777216)`
- `New StreamDecompressor(formatName = "gzip", maxInputBytes = 67108864, maxOutputBytes = 67108864, maxChunkBytes = 1048576, maxCallOutputBytes = 16777216)`
- `Write(data As Bytes) As Bytes`
- `compressor.Flush() As Bytes`

## Build and API reference

Source is in `modules/`, with native/runtime inputs where required. Use the
[package guide](docs/API.md) for backend-specific build prerequisites, ownership
rules and supported targets. Rebuilding a native backend requires that target's
compiler and SDK; sealing an archive alone does not rebuild the backend.

## Local Buoy setup

On your own machine, install Buoy and run:

```sh
buoy ai init --both
```

The generated agent files and editor settings stay local and are ignored by Git.

## Contributing

[Contributing](CONTRIBUTING.md) · [License](LICENSE)
