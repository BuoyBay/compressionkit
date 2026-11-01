---
title: "CompressionKit"
summary: "Bounded incremental gzip/zlib streams with explicit finish and native ownership."
category: "Overview"
order: 0
keywords: ["compression", "gzip", "zlib", "streaming", "limits"]
related: ["getting-started.md", "api.md", "typed-api.md"]
---

# CompressionKit

Import compressionkit for incremental gzip/zlib encoders and strict one-member
decoders. Write returns owned output chunks, encoder Flush emits pending bytes,
and Finish completes or validates the stream. Close releases resources without
implicitly finishing. Streams have lifetime input/output budgets and independent
per-call input/output bounds.

Decoded chunks remain provisional until Finish succeeds. Later corruption or a
missing trailer can invalidate earlier output; stage privately before publication
when rollback is required. Codec failures permanently close the stream.

Vendored zlib is compiled into the included native bridge slices; no system zlib
installation is needed. See the [API reference](api.md).

Payloads cover all six desktop targets. Linux ARM64 and Windows ARM64 are
cross-built, ABI/export/dependency inspected and sealed-consumer cross-linked;
native execution on those two targets remains unverified. Native stream tests
cover Linux x86_64, macOS ARM64 and Windows x86_64. macOS Intel consumers link,
with Intel execution untested.
