# ot-bayan

`ot-bayan` is a C++20 command-line utility for finding duplicate files.

It scans one or more directories, filters candidate files, and groups files with identical content using block hashing.

## Features

- Recursive directory scanning with configurable depth
- Excluding directories from scan
- File-name mask filtering (`*` and `?`, case-insensitive)
- Minimum file size filter
- Configurable block size for hashing
- Two hash backends: `crc32` and `md5`
- Duplicate grouping by content, not by file name
- Verbose mode with a parameter summary and a per-group file/size table
- Print paths as full (canonical) by default, or as-passed with `--relative`

## Requirements

- CMake 3.20+
- C++20 compiler (GCC/Clang/MSVC)
- Boost components:
	- `filesystem`
	- `program_options`
	- `system`
- Optional: GoogleTest (for unit tests)

## Build

### Release/Default build

```bash
cmake -S . -B build
cmake --build build
```

The executable will be available at:

```bash
build/bin/bayan
```

### Build with tests enabled

```bash
cmake -S . -B build -DWITH_GOOGLE_TEST=ON
cmake --build build
```

Note: if `WITH_GOOGLE_TEST` is `OFF` in your existing CMake cache, tests are not added.

## Quick Start

### Show help

```bash
./build/bin/bayan --help
```

### Show version

```bash
./build/bin/bayan --version
```

### Find duplicates in a directory

```bash
./build/bin/bayan --scan test-dir --depth 5 --min-size 1 --relative
```

Example output:

```text
  test-dir/subdir1/sub-sub-dir/subsub.md
  test-dir/subdir2/subsub.md
  test-dir/subdir2/sub2.md
```

Note: by default paths are printed fully resolved (canonical); pass `--relative` to print them as given on the command line.

## CLI Reference

Current options:

```text
	-h [ --help ]                   Produce help message
	-s [ --scan ] arg               Directories to scan
	-e [ --exclude ] arg            Directories to exclude
	-d [ --depth ] arg (=0)         Scan depth (0 - current dir only)
	-m [ --min-size ] arg (=1)      Minimum file size in bytes
	--mask arg                      Filename masks (case-insensitive)
	-b [ --block-size ] arg (=4096) Block size bytes for hashing (default: 4096)
	--hash arg (=crc32)             Hash algorithm (crc32, md5)
	-r [ --relative ]               Print paths as passed (relative) instead of full paths
	-V [ --verbose ]                Verbose output: print used parameters and a table with file paths and sizes
	-v [ --version ]                Show version information
```

## Usage Examples

### 1) Scan multiple directories

```bash
./build/bin/bayan \
	--scan /data/photos /data/backups \
	--depth 6 \
	--min-size 1024
```

### 2) Use masks to restrict file types

```bash
./build/bin/bayan \
	--scan /data \
	--depth 8 \
	--mask "*.jpg" "*.png" "*.jpeg"
```

### 3) Exclude known directories

```bash
./build/bin/bayan \
	--scan /data \
	--exclude /data/.git /data/cache \
	--depth 8
```

### 4) Switch hash algorithm and block size

```bash
./build/bin/bayan \
	--scan /data \
	--hash md5 \
	--block-size 8192 \
	--depth 8
```

### 5) Verbose output with relative paths

```bash
./build/bin/bayan \
	--scan /data \
	--depth 8 \
	--relative \
	--verbose
```

Verbose mode prints the resolved parameters and, for each duplicate group, a table of file paths and sizes instead of a plain path list.

## How It Works

Duplicate detection pipeline:

```mermaid
flowchart LR
	A[CLI args] --> B[CliParser]
	B --> C[Bayan::extractOptions]
	C --> D[FileFinder::Find]
	D --> E[FileObj list]
	E --> F[DuplicateFinder::Find]
	F --> G[Group by file size]
	G --> H[Refine by block hash]
	H --> I[Duplicate groups]
	I --> J[Printed paths]
```

Core idea:

- Files are first grouped by size.
- Only groups with at least 2 files continue.
- Those groups are refined block-by-block using hashes.
- Groups that still match after all blocks are reported as duplicates.

This avoids full byte-by-byte comparisons for most non-duplicates and discards differences early.

## Architecture

Project layout:

- `src/app/bayan`: executable entry point (`main.cpp`)
- `src/lib/bayan/bayan`: application orchestration (`Bayan`)
- `src/lib/bayan/cli-parser`: CLI parsing and help
- `src/lib/bayan/filesystem-helper`: scanning (`FileFinder`) and file abstraction (`FileObj`)
- `src/lib/bayan/duplicate-finder`: duplicate grouping/refinement logic
- `src/lib/bayan/hash`: hash algorithms and factory (`crc32`, `md5`)
- `src/lib/bayan/version`: generated version constants and helpers
- `tests/units`: hash unit tests

Key components:

- `Bayan`
	- Parses options
	- Validates required inputs
	- Configures `FileFinder`
	- Runs `DuplicateFinder`
	- Prints duplicate groups (plain list, or a table with sizes in verbose mode)
	- Resolves printed paths as canonical (default) or as-passed (`--relative`)

- `FileFinder`
	- Walks scan roots recursively
	- Applies depth and exclusion checks
	- Applies mask and min-size filters
	- Creates `FileObj` entries with chosen hash function

- `FileObj`
	- Stores file metadata
	- Reads file content block-by-block lazily
	- Caches computed block hashes

- `DuplicateFinder`
	- Buckets by file size
	- Iteratively partitions groups by hash of block `i`
	- Returns only groups with cardinality > 1

## Testing

When built with `-DWITH_GOOGLE_TEST=ON`:

```bash
ctest --test-dir build --output-on-failure
```

Current unit tests cover:

- CRC32 determinism and known test vector
- MD5 determinism and known test vector

## Packaging

The project includes CPack settings for generating a Debian package:

```bash
cpack --config build/CPackConfig.cmake
```

## Notes

- If no duplicates are found, output is empty.
- `--scan` is required.
- `--depth 0` scans only the top-level of each scan directory.
- Printed paths are fully resolved (canonical) by default; use `--relative` to print them as passed.
