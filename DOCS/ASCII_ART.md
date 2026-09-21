# ASCII Art TUI Library

Custom library for SHADOWDEEP Zv1 - draws all characters using colored ASCII letters only, no emojis.

## Goals

- No emojis anywhere, only colored ASCII letters for glyphs
- Draw all characters using ascii art with own library
- Handle colors, resize, terminal drawing
- Sprites for monsters and items
- Works on Windows, Linux, macOS, BSD, Raspberry Pi

## Design

### AsciiSprite

- `lines`: vector of strings, each line is ASCII art using only ASCII chars like `/ \ | - _ . o O V ^ etc`
- `fg`, `bg`: Color from terminal/color.hpp (8/16/256/true-color fallback handled by ScreenBuffer)
- `bold`: bold flag
- `width`, `height`: computed from lines

### AsciiArtLibrary

Singleton via `instance()`.

- `initMonsters()`: 85 monsters, each has sprite 3-6 lines tall, colored per family
  - beast brown/gray, humanoid green/yellow, undead white/cyan/red, demon red/magenta, dragon gold/yellow, construct steel, aberration purple
  - Examples: giant_rat `/\_/\ ( o.o) > ^ <`, goblin `.--. ( o o) ( V )`, dragon multi-line with `/\` wings
  - All use only ASCII letters, no Unicode emoji

- `initItems()`: 50+ explicit sprites + generic fallbacks covering 159 item templates
  - Weapons: dagger `/`, longsword `/| / |`, battle_axe `.--. / \`
  - Ranged: longbow `/} ( }`, crossbow `--+--`
  - Armor: `.----. | [] | \____/`, plate `.====.`
  - Shields, helmets, rings `.---. ( o )`, amulets, potions `.--. | ++ |`, food, scrolls, bombs, keys, guide fragments, lore, gold `( $$ )`, ruby `/\`
  - Fallback logic in `getItemSprite` maps stableId substrings to generic sprites

- `initTiles()`: tile glyphs for wall `#`, floor `.`, stairs `> <`, altar `A`, fountain `F`, chest `C`

- `initBanners()`: biome banners using ASCII block letters
  - stone, fungal, crystal, infernal, void, title (SHADOWDEEP)

### Drawing Functions - ascii_draw namespace

- `drawSprite(screen, x, y, sprite)`: draws sprite lines at x,y with fg/bold, uses ScreenBuffer::text which handles clipping and UTF-8 width
- `drawSpriteClipped(screen, x, y, sprite, maxW, maxH)`: clipped version for side panels
- `drawBoxStyled(screen, x, y, w, h, border, style, bold)`: draws boxes
  - Styles: Single `+ - |`, Double `# =`, Thick `#`, Ascii `+ - |`, Rounded `. ' - |`
  - Used for inventory, codex, side panels, message boxes
  - Handles resize: checks w<2 or h<2 returns, screen.text clips
- `drawProgressBar(screen, x, y, w, current, max, fill, empty, border)`: HP bars using `#` filled and `-` empty, centered text, color lerped via healthColor
- `drawVerticalBar(screen, x, y, h, current, max, fill, empty)`: vertical bar
- `drawCenteredText`, `drawWrappedText`: text layout with word wrap, respects w/h
- `drawLine(screen, x0,y0,x1,y1, ch, c)`: Bresenham line for connections
- `drawGradientText`, `drawBorderedText`: text effects using color lerp
- `clearArea`, `fillArea`: area ops
- `drawShadowBox`, `drawDoubleBuffer`: shadow and double border effects

### Color Handling

- All sprites use `Color` enum from terminal/color.hpp
- ScreenBuffer maps Color to ANSI 8/16/256/true-color based on terminal capabilities (fallback ASCII/8/16/256/true-color required by spec)
- healthColor: >66% green, >33% yellow, else red
- rarityColor: 0 white, 1 bright green, 2 bright cyan, 3 purple, 4 gold, 5 bright yellow

### Resize Handling

- ScreenBuffer::resize(cols, rows) called every frame from terminal backend size
- All draw functions check bounds via ScreenBuffer::set/text which clips out-of-bounds
- drawBoxStyled returns early if w<2 or h<2
- drawSpriteClipped enforces maxW/maxH for side panels (inventory width changes)
- Gameplay checks `if (w < 90 || h < 30)` shows warning, otherwise draws map 80x24 viewport inside 160x80 map

### Terminal Drawing

- Uses ScreenBuffer which does differential rendering (only changed cells sent)
- toAnsi() generates ANSI escape sequences for colors, bold
- TerminalBackend abstraction: Windows Console API vs POSIX termios, behind platform/
- No scattered #ifdef in gameplay, only in platform/

### Integration Points

- `App::renderHome`: titleArt via getTitleArt(), goblin and dragon sprites as deco, boxes via drawBoxStyled
- `App::renderGameplay`: side panel box via drawBoxStyled, nearby monster sprite + health bar via drawSprite + drawProgressBar, inventory list colored ASCII letters
- `App::renderInventory`: shadow box, item sprite in box, rarity color, progress bar
- `App::renderCodex`: guide fragment sprites, lore sprites, biome banners, bestiary shows all 85 monsters with sprites, stats, depth info
- Bestiary: scrollable list of all monster templates sorted alphabetically, each with sprite box, name, HP/Atk/Def/XP, depth range, boss flag
- Items: fallback ensures every stableId has at least generic sprite, so inventory always shows ASCII art

### No Emojis Guarantee

- All sprites use only ASCII characters: letters a-z A-Z, punctuation `/ \ | - _ . : ; ( ) [ ] { } < > + = * # $ % & etc`
- No Unicode emoji codepoints
- Glyphs are colored ASCII letters: monsters use `g s o O D` etc but rendered as single char in map, with full sprite in side panel
- Verified via grep for emoji ranges - none

### Files

- `include/shadowdeep/tui/ascii_art.hpp`: public API, no comments per project rule
- `src/tui/ascii_art.cpp`: implementation, 85 monster sprites, 50+ item sprites + fallbacks, 6 banners, drawing functions
- `src/tui/screen.cpp`: ScreenBuffer handles colors, resize, differential rendering
- `src/app/app.cpp`: integration uses drawSprite, drawBoxStyled, drawProgressBar, drawShadowBox

### Future Enhancements

- Could add animation frames for monsters (e.g., fire elemental flicker)
- Could add larger sprites for bosses (elder dragon 7 lines)
- Could add item enchantment overlay (e.g., `*` for enchanted)
- Currently covers all 85 monsters and 159 items via explicit + fallback
