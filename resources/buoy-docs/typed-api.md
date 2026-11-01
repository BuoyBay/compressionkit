---
title: "compressionkit typed API"
summary: "Source-derived public signatures, properties, parameters, defaults, and return values."
category: "Reference"
order: 21
types: ["CompressionException", "StreamCompressor", "StreamDecompressor"]
dependencies: []
symbols: ["CompressionException", "StreamCompressor", "StreamDecompressor"]
related: ["api.md", "getting-started.md", "index.md"]
---

# compressionkit typed API

Generated from the package source declarations. Platform guards are shown where present; inherited members are documented on the base type. See the [API contracts](api.md) for behavior and limitations.

## CompressionException

`Public Class CompressionException Inherits Exception`

Codec failures permanently close the stream. Earlier decoder chunks are provisional until Finish.

### Constructor

`Public Sub Constructor(message As String)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `message` | `String` | `ByVal` | Required |

Returns: no value.

## StreamCompressor

`Public Class StreamCompressor`

Incremental single-stream gzip/zlib encoder. Confine each instance to one thread.

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `IsClosed` | `Boolean` | Yes | No | — | — |
| `IsFinished` | `Boolean` | Yes | No | — | — |
| `InputBytes` | `Integer` | Yes | No | — | — |
| `OutputBytes` | `Integer` | Yes | No | — | — |

### Constructor

`Public Sub Constructor(formatName As String = "gzip", level As Integer = -1, maxInputBytes As Integer = 67108864, maxOutputBytes As Integer = 67108864, maxChunkBytes As Integer = 1048576, maxCallOutputBytes As Integer = 16777216)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `formatName` | `String` | `ByVal` | `"gzip"` |
| `level` | `Integer` | `ByVal` | `-1` |
| `maxInputBytes` | `Integer` | `ByVal` | `67108864` |
| `maxOutputBytes` | `Integer` | `ByVal` | `67108864` |
| `maxChunkBytes` | `Integer` | `ByVal` | `1048576` |
| `maxCallOutputBytes` | `Integer` | `ByVal` | `16777216` |

Returns: no value.

### Write

`Public Function Write(data As Bytes) As Bytes`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `data` | `Bytes` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `Bytes` |

### Flush

`Public Function Flush() As Bytes`

Z_SYNC_FLUSH: emit pending data and an aligned empty block; stream remains open.

No parameters.

#### Returns

| Type |
| --- |
| `Bytes` |

### Finish

`Public Function Finish() As Bytes`

Emit final compressed bytes and checksum/footer. Must be called once before Close.

No parameters.

#### Returns

| Type |
| --- |
| `Bytes` |

### Close

`Public Sub Close()`

Idempotent abort/release. Close never silently finishes a stream.

No parameters.

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

## StreamDecompressor

`Public Class StreamDecompressor`

Incremental strict one-member decoder. All decoded chunks remain provisional until Finish.

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `IsClosed` | `Boolean` | Yes | No | — | — |
| `IsFinished` | `Boolean` | Yes | No | — | — |
| `IsStreamEnd` | `Boolean` | Yes | No | — | — |
| `InputBytes` | `Integer` | Yes | No | — | — |
| `OutputBytes` | `Integer` | Yes | No | — | — |

### Constructor

`Public Sub Constructor(formatName As String = "gzip", maxInputBytes As Integer = 67108864, maxOutputBytes As Integer = 67108864, maxChunkBytes As Integer = 1048576, maxCallOutputBytes As Integer = 16777216)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `formatName` | `String` | `ByVal` | `"gzip"` |
| `maxInputBytes` | `Integer` | `ByVal` | `67108864` |
| `maxOutputBytes` | `Integer` | `ByVal` | `67108864` |
| `maxChunkBytes` | `Integer` | `ByVal` | `1048576` |
| `maxCallOutputBytes` | `Integer` | `ByVal` | `16777216` |

Returns: no value.

### Write

`Public Function Write(data As Bytes) As Bytes`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `data` | `Bytes` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `Bytes` |

### Finish

`Public Function Finish() As Bytes`

Validate that a complete checksummed stream ended; truncated streams fail and close.

No parameters.

#### Returns

| Type |
| --- |
| `Bytes` |

### Close

`Public Sub Close()`

Idempotent abort/release; it does not validate or commit decoded output.

No parameters.

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.
