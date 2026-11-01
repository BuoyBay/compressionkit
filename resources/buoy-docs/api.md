---
title: "CompressionKit API"
summary: "Stream lifecycle, strictness, limits and provisional decoding contracts."
category: "Reference"
order: 20
keywords: ["gzip", "zlib", "finish", "flush", "budgets"]
symbols: ["StreamCompressor", "StreamDecompressor", "CompressionException", "StreamCompressor.Write", "StreamCompressor.Flush", "StreamCompressor.Finish", "StreamDecompressor.Write", "StreamDecompressor.Finish"]
related: ["index.md", "getting-started.md", "typed-api.md"]
---

# API reference

StreamCompressor takes formatName = "gzip", level = -1, maxInputBytes = 67108864,
maxOutputBytes = 67108864, maxChunkBytes = 1048576 and maxCallOutputBytes = 16777216.
StreamDecompressor has the same arguments except level. Exact format names are
gzip/zlib. Levels are -1 or 0..9.

Write(Bytes) returns newly emitted owned Bytes. Compressor Flush() performs
Z_SYNC_FLUSH while leaving the stream active. Finish() returns final encoder
bytes or validates a complete decoder end marker; repeated Finish is invalid.
Close() is idempotent and never implicitly finishes or validates. Destructors
release resources. Keep each instance on one thread.

Read-only properties are IsClosed, IsFinished, InputBytes and OutputBytes; decoder
also exposes IsStreamEnd. Counters reflect the last successful operation, retaining
their values after Close. A decoder reaching a member end must still receive all
input and pass Finish. Trailing data, concatenated members, dictionaries,
corruption and truncation are rejected. Earlier decoder chunks remain provisional
until Finish succeeds.

Lifetime input/output limits are nonnegative. Both per-call bounds are 1..16777216.
Exceeding a budget or codec failure throws CompressionException, returns no output
from that call and permanently closes the stream. Previously returned output is
not recalled. Nil data, invalid constructor arguments and operations after
Finish/Close throw InvalidArgumentException. Input/output buffers remain bounded
by the configured per-call limits even when lifetime budgets are much larger.

[Overview](index.md)


[Full typed API: properties, parameters, defaults, and returns](typed-api.md)
