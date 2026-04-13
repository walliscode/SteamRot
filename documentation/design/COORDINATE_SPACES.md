# Coordinate Spaces: Screen Space vs World Space

## Overview

Any game that renders to a window operates in at least two distinct coordinate
spaces simultaneously. Mixing them up silently produces wrong results — hit
detection that drifts with zoom, objects that snap to the wrong position, or
collision checks that work at scale 1.0 but break the moment the camera zooms
in or pans. This document explains the two spaces, the transform that connects
them, and the rules that prevent accidental mixing.

---

## The Two Spaces

### Screen Space

Screen space (also called *window space* or *pixel space*) is the 2D coordinate
system of the display surface itself.

| Property | Value |
|---|---|
| Origin | top-left corner of the window/render texture |
| Axes | X grows right, Y grows down |
| Unit | one pixel on the display surface |
| Type in SFML | `sf::Vector2i` (integer pixels) |

Everything the operating system and windowing layer reports is in screen space:
mouse cursor position, touch coordinates, window resize events, UI element
bounding boxes that have been positioned relative to the window edge.

**Key insight**: screen space is *fixed* — zooming or panning the camera never
moves a screen-space coordinate. If the mouse cursor is at pixel (400, 300) it
is at (400, 300) regardless of how far the camera has zoomed.

### World Space

World space is the logical coordinate system of your game simulation. Objects
live at positions they were assigned during construction or simulation —
completely independent of where the camera is pointing or how far it has zoomed.

| Property | Value |
|---|---|
| Origin | defined by your game design (often map centre or (0,0)) |
| Axes | X grows right, Y grows down (in SFML's default convention) |
| Unit | one logical unit (metres, tiles, arbitrary) |
| Type in SFML | `sf::Vector2f` (float) |

Fragments, joints, scaffolds, and the placement ghost all live in world space.
Their positions are meaningful to the simulation; they do not change when the
window is resized or the camera zooms.

---

## The Camera Transform: Connecting the Two Spaces

A *camera* (in SFML: an `sf::View`) defines the rectangle of world space that
is currently projected onto the screen. Conceptually it answers: "which world
coordinates map to which screen pixels right now?"

### How SFML Views Work

When you draw with an active view, SFML applies a transform that maps world
coordinates into the normalised device coordinates of the render target:

```
world position → (view transform) → screen pixel
```

The inverse — converting a screen pixel back to the world coordinate it
represents — is `sf::RenderTarget::mapPixelToCoords()`.

### What Zoom Does

Zoom scales the view rectangle around its centre. If zoom level is `z`:

- A world unit that previously filled 10 pixels now fills 10/z pixels
- A pixel that was 1/10 of a world unit now represents z/10 of a world unit

This means a pixel (px, py) at zoom 1.0 represents a *different* world position
than (px, py) at zoom 2.0. **If you use a screen-space coordinate directly where
a world-space coordinate is expected, the error grows linearly with zoom.**

### The Conversion Formula

```cpp
// Screen pixel → world position
sf::Vector2f world_pos = render_texture.mapPixelToCoords(screen_pixel, view);

// World position → screen pixel
sf::Vector2i screen_px = render_texture.mapCoordsToPixel(world_pos, view);
```

Both functions require the *same* `sf::View` that is active during rendering —
if rendering uses a zoomed world view, the same zoomed view must be used for
coordinate mapping. Using the wrong view is a common source of subtle bugs.

---

## Key Conceptual Points

### 1. Types Are Your Best Ally

SFML encodes the distinction in C++ types:
- Screen space: `sf::Vector2i` (integer pixels)
- World space: `sf::Vector2f` (float world units)

If a function parameter is `sf::Vector2f` it expects world coordinates. If it is
`sf::Vector2i` it expects screen pixels. An implicit cast from `sf::Vector2i` to
`sf::Vector2f` compiles silently and is almost always a bug at zoom ≠ 1.0.

### 2. Convert Exactly Once, At the Boundary

The best architecture converts the raw screen-space input (mouse position) to
world space exactly once per tick, as early as possible, and stores the result
in a well-named field. Every downstream system then reads from that field rather
than redoing the conversion independently. Benefits:

- **Consistency**: all systems see the same world position for a given tick
- **Single point of failure**: if the conversion is wrong it is wrong in one place
- **Self-documenting**: code that reads `world_mouse_position` is unambiguous

### 3. Different Systems Belong to Different Spaces — Keep Them Separate

| System type | Correct space | Why |
|---|---|---|
| UI layout and hover detection | Screen space | UI is positioned relative to the window; it does not move with the camera |
| Physics, collision (world objects) | World space | Objects live in the simulation, not on the screen |
| Ghost / placement preview | World space | The preview must align with world-space objects |
| Minimap or HUD overlays | Screen space | Fixed positions on the screen |

Mixing spaces in one system (e.g. checking a world-space socket against a
screen-space mouse pixel) produces results that are correct only when zoom = 1.0
and the view has not panned — a fragile coincidence, not correctness.

### 4. The View Must Match the Rendering Pass

When a scene renders to a `sf::RenderTexture` using a zoomed world view, mouse
position must be mapped through that *same zoomed world view* to get a correct
world position. If the scene also renders UI using the texture's default view
(no zoom), that default view must never be used to map mouse-to-world for
world-space systems.

### 5. Non-Mouse Screen→World Conversions Are Separate Concerns

Sometimes a *canvas pixel* (a fixed position on screen like the centre of a
crafting area) needs to be converted to world space for placement purposes. This
is a distinct conversion from the mouse position conversion. It should be done
explicitly at the call site with a comment explaining why — it is not the
"mouse-to-world" concern.

---

## Summary

| Question | Answer |
|---|---|
| What space does the OS give mouse coordinates in? | Screen space (`sf::Vector2i`) |
| What space do game objects live in? | World space (`sf::Vector2f`) |
| What changes the mapping between them? | Camera zoom and pan (the active `sf::View`) |
| When should conversion happen? | Once per tick, at the top of the update loop |
| How do you avoid mixing spaces? | Keep them in separate typed fields; never implicitly cast `Vector2i` to `Vector2f` |
