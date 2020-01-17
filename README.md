# ft_vox [[42](https://www.42.fr/) project] ![](https://github.com/zer0nim/ft_vox/workflows/make/badge.svg) ![](https://github.com/zer0nim/ft_vox/workflows/linter/badge.svg) ![](https://github.com/zer0nim/ft_vox/workflows/ft_vox/badge.svg)

## Project
A little Opengl Voxel World engine

## Explore an "infinite" world with fly mode
![moving](/assets/GIFs/moving.gif)
## Explore caves with walking mode
![caves](/assets/GIFs/survival-caves.gif)
## Try differents generations with multiples biomes
![biome-preview](/assets/imgs/biome-preview.png)
## Explore and find all differents structure
![struct-preview](/assets/imgs/struct-preview.png)
## Try multiples texture packs or create yours
![texture-pack-1](/assets/imgs/smooth-preview.png)
![texture-pack-2](/assets/imgs/texture-blocks.png)
## Explore day and night
![day-night-cycle](/assets/imgs/day-night-cycle.png)

We used `glfw3` API for creating windows and manage input and events,
and `glm` to manage matrix/vector/quaternion.

## Installation

- Install depencies (osx)

	```./configure```

- Add precommit rules

	```make init```

- Compile the project

	```make -j8```

	```make help  # get info about make commands```
- Run the project

	```./ft_vox --usage```

	```./ft_vox --name <map_name:string>  # open or create 'map_name'```

	```./ft_vox --seed <seed:uint32_t>  # create world with given seed```

## The project

You can toogle debug panel with `F3`.
To show the full list of commands: `F3 + C`.

### Gamemode

Change current gamemode with `g`.
In ft_vox, you have 2 gamemodes: survival and creative.
In survival mode, you can walk in the world. You can't walk through blocks, the gravity is enabled.
In creative mode, you can fly and you don't have any collision detection with the world.
In both mode, you can add or destroy blocks in the world. If you want to save the world, don't forget to set a world name (`./ft_vox --name worldName`).

### Moving

You can move with arrows or with `wasd`. To run, just press `shift`.
To fly up (creative) or jump (survival): `space` or `e`, to fly down (creative): `ctrl` or `q`.

### World interaction

You can add blocks on the world with `right-click` and you can destroy blocks with `left-click`.
To change the selected block, you can `scroll-up`, `scroll-down`, `[`, `]` or select the block under cursor with `middle-click`.

### Quit game

You can save and quit the game by pressing `escape`.

## Settings

You can change all world settings in [assets/settings.json](assets/settings.json).

You can configure the game with this file:
- FPS
- Render distance
- Texture pack selection
- Window size / fullscreen option
- Speed / mouse sensitivity
- Destroy / add blocks speed
- Fog
- Map seed (0 for random seed)
- Generation type (0 -> void world, 1 -> flat world, 2 -> classic generation)
- For classic generation, enable / disable tree
- For classic generation, enable / disable ores
- Starting position of the player
- Fully personalised flatMap generation
- Gamemode
- Days duration
- ...

You have more than 50 parameters to discover !

## Texture packs
You can also change the textures by creating a texture atlas [assets/textures/textures-default.png](assets/textures/textures-default.png).
You can create this file with [this](http://free-tex-packer.com/app/) website (show settings in the image bellow).
If the images are not in the right size (32px), you can resize all your images in [this](https://www.birme.net/?target_width=32&target_height=32&quality=0) website.
You can also create your texture on [this](https://www.pixilart.com/draw?ref=home-page) website.
You need to put blocs in the right order (example in [assets/textures](assets/textures)).

There is 2 default texture packs: `textures-default` and `textures-smooth`.
You can select your texture pack in settings (`global->files->textures`: `"./assets/textures/textures-default.png"`, `"./assets/textures/textures-smooth.png"` or your texture file).

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

In game, you can use `F3` to show debug menu.
![F3-1](/assets/imgs/F3-1.png)

---

See more on the school [subject](ft_vox.fr.pdf).
