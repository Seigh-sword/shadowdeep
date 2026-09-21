# SHADOWDEEP Save Format

## Goals

- Structured storage, compactness, validation, versioning
- Less casual text editing, but not impossible to modify (user owns their computer)
- Cross-platform, endian-fixed, no raw struct dumps
- Atomic saving, backups, migration
- Treat files as untrusted input

## Container

- Magic: 0x53484450 ASCII "SHDP" little-endian u32
- Header (fixed size, little-endian):
  - magic u32
  - containerVersion u16
  - saveSchema u16
  - entryUuid u128 (16 bytes)
  - creationTimestamp u64 (system_clock)
  - lastSaveTimestamp u64
  - payloadSize u64
  - compression u8 (0=None, 1=Zstd)
  - reserved[3] u8
  - headerCrc u32
  - payloadCrc u32

## Payload

- After header, payload is chunk-based TLV:
  - chunkId 4cc u32 (e.g., 'META', 'PLAY', 'WORL', 'FLOR', 'RNG ', 'HIST')
  - length u32
  - data bytes
  - crc u32

- Unknown optional chunks can be skipped by length.
- Mandatory chunks must be validated.

Potential chunks:

- META: EntryMetadata (entry name, character name, class, level, depth, region, play time, seed, difficulty, status, gold, rubies, hasAmulet)
- PLAYER: Player stats, position, inventory, equipment, effects
- WORLD: World seed, RNG state, region info
- FLOORS: Array of floors, each with dungeon tiles, explored, monsters, items
- RNG: RNG seed and state
- HISTORY: Run history, kills, gold earned
- QUESTS: Quest states
- NPCS: NPC states
- ECON: Shop states
- DISCOVERY: Bestiary, Codex

## Serialization

- Use BinaryWriter/BinaryReader with fixed-width integers: u8, u16, u32, u64, i32, i64, bool, string (u32 len + UTF8 bytes)
- Hard limits: string 4096, collections 100k, floors 64, entities 10k, payload 100MB
- All integers little-endian
- Validate all incoming sizes, indexes, conversions
- No execution of content from save files

## Compression

- Optional Zstd after serialization, before checksum
- Compression ID in header

## Atomic Saving

1. Serialize to memory
2. Validate
3. Write temp file `*.tmp`
4. Flush and fsync where practical
5. Close
6. Atomically replace destination via rename (POSIX) or ReplaceFileW (Windows)
7. Maintain rotating backups (10)

## Migration

- Game version Zv1 stays constant, save schema increments independently
- Save schema 1 is initial for new system
- MigrationManager: chain of migrations N -> N+1 in memory
- Process:
  1. Read safely
  2. Validate
  3. Preserve original
  4. Create backup
  5. Migrate in memory
  6. Validate migrated state
  7. Write temp save
  8. Flush
  9. Atomically replace
  10. Retain backup
- If older exe sees newer schema: refuse to modify, display "This Entry was saved by a newer SHADOWDEEP save format. Update SHADOWDEEP to open it."

## Entry Metadata

Displayed in Entries screen:

- Entry name, character name, class, level, region, depth, play time, last played, difficulty, seed, status, save compatibility, file size

## Paths

- Windows: %APPDATA%\shadowdeep\saves\<id>.sav
- Linux: $XDG_DATA_HOME/shadowdeep/saves/ or ~/.local/share/shadowdeep/saves/
- macOS: ~/Library/Application Support/shadowdeep/saves/

Backups: save_backups/, trash/, exports/

## Import/Export

- Export creates portable .sav copy
- Import validates magic, size, schema, no path traversal (reject "..")
- Detect corruption, unsupported future schema, duplicate IDs
- Generate new UUID on import to avoid collisions
