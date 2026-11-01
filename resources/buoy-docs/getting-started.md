---
title: "compressionkit Getting started"
summary: "Install, import, and explore the public compressionkit API."
category: "Guide"
symbols: []
related: ["index.md", "api.md", "typed-api.md"]
---

# Getting started

Install the compressionkit package into your project’s packages directory. Use `Import compressionkit`. Create a `StreamCompressor("gzip")`, feed input bytes through `Write`, and append the bytes returned by `Finish` to the encoded output. Always call `Close` in a `Finally` block.

To decode, create a `StreamDecompressor("gzip")` and feed every encoded chunk through `Write`. Keep decoded output private until `Finish` succeeds: checksum errors and truncated streams can be detected after plaintext has been emitted. Call `Close` in `Finally` for the decoder too.

Start with the [API contracts](api.md) for usage, constraints, and platform limitations. The [typed API](typed-api.md) lists constructors, each parameter and its default, properties, and return values directly from source.

[Package overview](index.md)
