# ft_vox [[42](https://www.42.fr/) project] ![](https://github.com/zer0nim/ft_vox/workflows/make/badge.svg) ![](https://github.com/zer0nim/ft_vox/workflows/linter/badge.svg) ![](https://github.com/zer0nim/ft_vox/workflows/ft_vox/badge.svg)

## Project
A Simple Voxel Engine in Opengl, with custom procedural generation

## Explore an almost infinite world with fly mode

![moving](/assets/GIFs/moving.gif)
## Traverse caves with walking mode
![caves](/assets/GIFs/survival-caves.gif)
## Observe diverse biomes and try multiples types of generations
![biome-preview](/assets/imgs/biome-preview.png)
## Discover the generated structures
![struct-preview](/assets/imgs/struct-preview.png)
## Try multiples texture packs or create yours
![texture-pack-1](/assets/imgs/smooth-preview.png)
![texture-pack-2](/assets/imgs/texture-blocks.png)
## Experience day and night exploration
![day-night-cycle](/assets/imgs/day-night-cycle.png)

## Installation

We used `glfw3` API for creating windows and manage input and events,
and `glm` to manage matrix/vector/quaternion.

- Install depencies (osx)

	```./configure```

- Add precommit rules (for dev only)

	```make init```

- Compile the project

	```make -j8```

	```make help  # get info about make commands```
- Run the project

	```./ft_vox --usage```

	```./ft_vox --name <map_name:string>  # open or create 'map_name'```

	```./ft_vox --seed <seed:uint32_t>  # create world with given seed```

## The project

### Gamemode

In ft_vox, you have 2 gamemodes: *survival* and *creative*.

Change current gamemode with `g`.

#### Survival
You can walk in the world but can't walk through blocks, the gravity is enabled.

#### Creative
You can fly and you don't have any collision detection with the world.

In both modes, you can add or destroy blocks in the world.

> If you want to save the world, don't forget to set a world name (`./ft_vox --name worldName`).

### Moving

You can move with `arrows` or `wasd` keys.
To run, just press `shift`.

To fly up (creative) or jump (survival): `space` / `e`.
To fly down (creative): `ctrl` or `q`.

### World interaction

Add blocks on the world with `right-click` and destroy thems with `left-click`.

To change the selected block, `scroll-up`, `scroll-down`, `[`, `]` or select the block under cursor with `middle-click`.

### Quit game

Save and quit the game by pressing `escape`.

## Settings

Feel free to change world settings in [assets/settings.json](assets/settings.json).

In the file, you can configure:
- FPS
- Render distance
- Texture pack selection
- Window size / fullscreen option
- Speed / mouse sensitivity
- Destroy / add blocks speed
- Enable / disable fog
- Map seed (0 for random seed)
- Generation type (0 -> void world, 1 -> flat world, 2 -> classic generation)
- For classic generation, enable / disable cavern
- For classic generation, enable / disable tree
- For classic generation, enable / disable ores
- Starting position of the player
- Fully personalised flatMap generation
- Gamemode
- Days duration
- ...

You have more than 50 parameters to discover !

## Texture packs

To change the texture pack, edit the settings file: (`global->files->textures`).

You can set one of the two textures pack provided:
- `"./assets/textures/textures-default.png"`
- `"./assets/textures/textures-smooth.png"`

#### Or you can also build your own 😎.

You will need to create 32x32px textures and merge them in one texture atlas.

If your textures are not in the right dimension (32x32px), you can resize them [here](https://www.birme.net/?target_width=32&target_height=32&quality=0).

You can design your own textures on [this](https://www.pixilart.com/draw?ref=home-page) website.

You can merge them with [this](http://free-tex-packer.com/app/) website (look at the settings in the image below).

You need to put blocs in the right order (example in [assets/textures](assets/textures)).

![free-tex-settings](/assets/imgs/free-tex-settings.png)

## Debug mode

Run in debug mode (this command recompile the project if needed)
``` sh
make DEBUG=1 -j8
```

Run with fps analitics
``` sh
./debug/checkFPS.sh
./debug/checkFPS.sh re  # if you want to recompile (needed in first use)
```

You can toogle debug panel with `F3`.
To show the full list of commands: `F3 + C`.
![F3-1](/assets/imgs/F3-1.png)

---

See more on the school [subject](ft_vox.fr.pdf).
