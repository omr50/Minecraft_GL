# Minecraft Game 

A Minecraft inspired game with infinite world generation, multiple biomes, and world saving.
Created using C++, OpenGL, and SDL2

![screenshot 1](textures/in_game_screenshot1.png)

## Features
- Infinitely generated world using Perlin Noise
- Multiple biomes (plains, desert, mountains, forest)
- Saving your progress / builds (persistent worlds basically)
- World saves are compressed using custom made Huffman encoding to decrease world size on disk
- Thread pool used to efficiently compute terrain, create meshes and vertices for multiple chunks in the background

![screenshot 2](textures/in_game_screenshot3.png)

![screenshot 3](textures/in_game_screenshot2.png)
