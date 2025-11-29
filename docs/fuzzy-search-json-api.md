# fuzzy-search — JSON-RPC API (stdin/stdout)

## Summary

- The `fuzzy-search` binary exposes a simple JSON-RPC–style, line‑oriented API over `stdin`/`stdout`.
- It supports both **incremental searches** (typing characters / backspace / arrow control) and **search-string queries** (sending a complete query or seeding via CLI).
- Implementation: `fzf::JSONRPCInterface` (see `src/fuzzy-search/JSONRPCInterface.h` / `.cpp`).

## General rules

- One JSON object per line (newline-terminated).
- Requests are sent from client → server using `method: "input"` and `params` describing the input event.
- Server → client notifications include `results`, `progress`, and `finalResult`.

---

## Client → Server: input request

Send an input event as a single JSON object with `method: "input"` and a `params` object describing the event type.

Supported event types include:

- `PrintableChar` — user typed a printable character (provide `character`).
- `Backspace` — user deleted a character.
- `UpArrow` — move selection up.
- `DownArrow` — move selection down.
- `Newline` — submit/accept the current selection.
- `SearchString` — submit or replace the entire current search string atomically (provide `searchString`).

Example: typing the letter `a`

```json
{
  "jsonrpc": "2.0",
  "method": "input",
  "params": {
    "type": "PrintableChar",
    "character": "a"
  }
}
```

Example: backspace

```json
{
  "jsonrpc": "2.0",
  "method": "input",
  "params": {
    "type": "Backspace"
  }
}
```

Example: submit selection (Enter)

```json
{
  "jsonrpc": "2.0",
  "method": "input",
  "params": {
    "type": "Newline"
  }
}
```

Example: send a complete search string in one message using `SearchString`

```json
{
  "jsonrpc": "2.0",
  "method": "input",
  "params": {
    "type": "SearchString",
    "searchString": "path/to/foo"
  }
}
```

---

## Server → Client notifications

1) `results`

- Method: `results`
- Params: object built from the internal `fzf::Results` structure.
- Key fields:
  - `searchString` (string) — current search text.
  - `totalResults` (number) — total matches.
  - `range` (array) — `[first, last]` indices for this page.
  - `results` (array) — result objects (index, line, selected, score, ...).

Example (simplified):

```json
{
  "jsonrpc": "2.0",
  "method": "results",
  "params": {
    "searchString": "foo",
    "totalResults": 123,
    "range": [0, 10],
    "results": [
      {"index": 0, "line": "/path/to/foo", "selected": true, "score": 42.5},
      {"index": 1, "line": "/path/to/food", "selected": false, "score": 31.0}
    ]
  }
}
```

2) `progress`

- Method: `progress`
- Params: `{ "count": <number> }` — indicates processed line count or progress steps.

Example:

```json
{"jsonrpc":"2.0","method":"progress","params":{"count":512}}
```

3) `finalResult`

- Method: `finalResult`
- Params: `{ "result": "<selected-line-or-empty>" }` — emitted when the run finishes.

Example:

```json
{"jsonrpc":"2.0","method":"finalResult","params":{"result":"/path/to/selected"}}
```

---

## Incremental searches vs search-string queries

- Incremental: send successive `PrintableChar` and `Backspace` events to build or edit the search string. The server treats each event as an incremental update and streams `results` notifications as the search evolves.
- Query-mode / search-string queries: you may seed the server with a complete initial search value (for example via a CLI `--search` flag) or send a complete query as a short sequence of `PrintableChar` events. The server responds with results the same way; you can then use `Newline` to accept a selection.

Behavioral notes:

- The same JSON API and event flow are used for both interactive incremental typing and one-shot query submissions.
- Clients that prefer a single-shot request can send the full query (or preseed it via the CLI) and then wait for `results` and `finalResult`.

---

## Client examples and pointers

- A reference client that demonstrates this protocol is `fzf-client.py` in the repository root. It shows how to send `input` events, read `results` / `progress` / `finalResult`, and wait for the final selection.
- Implementation details live in `src/fuzzy-search/JSONRPCInterface.{h,cpp}` and data structures in `src/fuzzy-search/InputInterface.h`.
- For line-based clients: ensure each JSON object you write is newline-terminated (one object per line).

## References

- `src/fuzzy-search/JSONRPCInterface.h` — JSON interface header
- `src/fuzzy-search/JSONRPCInterface.cpp` — input parsing and writers (`getNextEvent`, `writeResults`, `updateProgress`, `writeFinalResult`)
- `src/fuzzy-search/InputInterface.h` — `fzf::Results`, `fzf::Result`, `fzf::InputType`, `fzf::InputEvent`
- `fzf-client.py` — example client that speaks this JSON API
