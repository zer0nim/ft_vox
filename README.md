# ft_vox [[42](https://www.42.fr/) project] ![](https://github.com/zer0nim/ft_vox/workflows/make/badge.svg) ![](https://github.com/zer0nim/ft_vox/workflows/linter/badge.svg) ![](https://github.com/zer0nim/ft_vox/workflows/ft_vox/badge.svg)

## Project
A little Opengl Voxel World engine

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

![biome-preview](/assets/imgs/biome-preview.png)
![struct-preview](/assets/imgs/struct-preview.png)

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
