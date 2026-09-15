# ot-joinserver

`ot-joinserver` is a C++20 TCP server built on Boost.Asio coroutines, backed by a small in-memory data layer (tables + a table registry). It exposes that data layer over a line-oriented text protocol for creating tables, inserting rows, and running joins/set operations between two fixed tables, `A` and `B`.

## Features

- Coroutine-based async TCP server (`TcpServer`) — one coroutine per client connection, non-blocking accept loop
- Newline-delimited command protocol: read a line, respond with a (possibly multi-line) result
- Handles multiple concurrent clients
- In-memory `Table` storage: thread-safe (reader/writer lock) insert/remove/get/list by integer id
- `DbEngine`: create/drop/lookup named tables, list all tables
- `INNER`/`LEFT`/`RIGHT`/`FULL` joins and symmetric difference between two tables, keyed by integer id

## Requirements

- CMake 3.20+
- C++20 compiler (GCC/Clang/MSVC)
- Boost (headers; `Boost::boost`)
- POSIX threads
- Optional: GoogleTest (for unit tests)

## Build

```bash
cmake -S . -B build
cmake --build build
```

The server executable will be available at:

```bash
build/bin/joinserver
```

### Build with tests enabled

```bash
cmake -S . -B build -DWITH_GOOGLE_TEST=ON
cmake --build build
```

Note: if `WITH_GOOGLE_TEST` is `OFF` in your existing CMake cache, tests are not added.

## Quick Start

### Run the server

```bash
./build/bin/joinserver [port]
```

`port` is optional and defaults to `9000`.

### Talk to it with `nc`

```bash
nc 127.0.0.1 9000
```

Then type commands, one per line. Tables must be created before they're used, and the join/set commands always operate on tables named `A` and `B`:

```text
CREATE_TABLE A
OK

CREATE_TABLE B
OK

INSERT A 0 lean
OK

INSERT A 1 sweater
OK

INSERT B 0 harry
OK

PRINT_TABLE A
0,lean
1,sweater
OK

LEFT_JOIN
0,lean,harry
1,sweater,NULL
OK

INTERSECTION
0,lean,harry
OK

TRUNCATE A
OK
```

Non-interactive one-shot test with `printf`:

```bash
printf 'CREATE_TABLE A\nCREATE_TABLE B\nINSERT A 0 lean\nINSERT B 0 harry\nPRINT_TABLE A\nLEFT_JOIN\nFULL_JOIN\nSYMMETRIC_DIFFERENCE\n' | nc 127.0.0.1 9000
```

An unrecognized command or bad arguments returns an error on the same connection, without dropping it:

```bash
printf 'NOT_A_COMMAND\n' | nc 127.0.0.1 9000
# ERR unknown command: NOT_A_COMMAND
```

A join/set command run before both `A` and `B` exist, or `PRINT_TABLE`/`INSERT`/`TRUNCATE` against a table that hasn't been created, also errors instead of returning rows:

```bash
printf 'PRINT_TABLE C\n' | nc 127.0.0.1 9000
# ERR no such table: C
```

Multiple clients can connect at once; each connection is handled independently, and all share the same underlying tables.

## Protocol

One command per line, space-separated tokens.

| Command | Arguments | Effect |
| --- | --- | --- |
| `CREATE_TABLE` | `table` | Create a new empty table named `table` |
| `INSERT` | `table id name` | Insert row `(id, name)` into `table`; `id` is an integer |
| `TRUNCATE` | `table` | Remove all rows from `table` |
| `PRINT_TABLE` | `table` | List all rows of `table`, sorted by `id` |
| `INTERSECTION` | — | Inner join of `A` and `B` on `id` |
| `SYMMETRIC_DIFFERENCE` | — | Rows whose `id` exists in exactly one of `A`/`B` |
| `LEFT_JOIN` | — | Left join of `A` and `B` on `id` |
| `RIGHT_JOIN` | — | Right join of `A` and `B` on `id` |
| `FULL_JOIN` | — | Full outer join of `A` and `B` on `id` |

`CREATE_TABLE`/`INSERT`/`TRUNCATE`/`PRINT_TABLE` accept any table name; the five join/set commands always operate on the tables named `A` and `B` specifically, so those two must be created (and populated) first.

### Responses

Every response ends with a blank line, so a client can tell where it ends even when it spans multiple lines, and every successful response ends with an `OK` line right before that blank line — including `PRINT_TABLE`/join/set responses, after any result rows:

- `OK` alone — a mutation (`CREATE_TABLE`/`INSERT`/`TRUNCATE`) succeeded with no rows to report.
- `ERR <message>` — the command failed: unknown command, wrong number of arguments, non-integer id, missing table, duplicate id, or a join/set command run before both `A` and `B` exist. No trailing `OK` in this case.
- `PRINT_TABLE`: zero or more `id,name` lines (sorted by `id`), then `OK`.
- A join/set command: zero or more `id,name_a,name_b` lines, one per result row, where a missing side is rendered as `NULL`, then `OK`.

## Architecture

Project layout:

- `src/app/join-server`: executable entry point (`main.cpp`) — owns the `DbEngine`, wires `TcpServer` to a per-connection handler that parses and dispatches commands
- `src/lib/network/server`: `TcpServer` — Boost.Asio coroutine-based TCP acceptor and session dispatcher (protocol-agnostic)
- `src/lib/network/protocol`: wire format only — `Command` variant, `parseCommand()`, and `formatOk`/`formatError`/`formatRows` (overloaded for table rows and join rows) response builders; no `DbEngine` dependency
- `src/lib/network/command-handler`: `CommandHandler` — executes a parsed `Command` against a `DbEngine`/the join functions and returns a formatted response
- `src/lib/data/storage`: `Table`/`Record` — thread-safe in-memory key/value table (`int` id → name)
- `src/lib/data/db-engine`: `DbEngine` — owns and manages a set of named `Table`s
- `src/lib/data/join`: free functions implementing `innerJoin`/`leftJoin`/`rightJoin`/`fullJoin`/`symmetricDifference` over two `Table`s

Key components:

- `TcpServer`
	- Accepts connections on a configured port
	- Spawns a caller-supplied coroutine (`SessionHandler`) per accepted socket
	- Runs on a caller-owned `boost::asio::io_context`

- `parseCommand` / `Command`
	- Tokenizes a line and validates it against the command's expected arity/types
	- Produces a closed `std::variant` of command structs, or a `ParseResult` with an error message

- `CommandHandler`
	- One `operator()` overload per `Command` alternative, dispatched via `std::visit` (compiler-enforced exhaustive)
	- Looks up tables through `DbEngine`, calls into `Table`/the join functions, and formats the result

- `Table`
	- Stores records keyed by integer id under a `std::shared_mutex`
	- `insert` / `remove` / `truncate` / `get` / `getAll` / `size`
	- Exposes raw data and its mutex for callers that need custom locking (e.g. cross-table joins)

- `DbEngine`
	- Creates, drops, and looks up `Table`s by name
	- Guards its table registry with its own `std::shared_mutex`
	- `listTables()` returns all registered table names

Adding a new command means adding one struct to the `Command` variant, one arm in `parseCommand`, and one `CommandHandler::operator()` overload — the compiler flags any variant alternative left unhandled.

## Testing

When built with `-DWITH_GOOGLE_TEST=ON`:

```bash
ctest --test-dir build --output-on-failure
```

- `protocol_tests`: `parseCommand` arity/type validation and response formatting, in isolation from `DbEngine`
- `command_handler_tests`: each command executed against a real `DbEngine`, including error paths (missing table, duplicate id, joins before both tables exist)

## Packaging

The project includes CPack settings for generating a Debian package:

```bash
cpack --config build/CPackConfig.cmake
```

## Notes

- The protocol is line-based: each command must end with `\n`, and every response ends with a blank line.
- The server runs single-threaded (`io_context::run()` on the main thread); concurrency comes from coroutines, not OS threads. `DbEngine`/`Table` still guard themselves with `std::shared_mutex` for when that changes.
