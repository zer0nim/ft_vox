# ft_vox [[42](https://www.42.fr/) project] ![](https://github.com/zer0nim/ft_vox/workflows/make/badge.svg) ![](https://github.com/zer0nim/ft_vox/workflows/linter/badge.svg) ![](https://github.com/zer0nim/ft_vox/workflows/ft_vox/badge.svg)

## ⚠️ NOT WORKING ! WORK IN PROGRESS ⚠️

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

	```./ft_vox --name <map_name:string>```

	```./ft_vox --seed <seed:uint32_t>```

## Debug mode

Run in debug mode (this command recompile the project if needed)
``` sh
make DEBUG=1 -j8
```

Run with fps analitics
``` sh
./debug/checkFPS.sh
./debug/checkFPS.sh re  # if you want to recompile (needed for first use)
```

Change world generation algorithm
``` sh
make fclean
make DEBUG=1 DEFINE="NOISE_USED=0" -j8  # perlin noise generation (default)
make DEBUG=1 DEFINE="NOISE_USED=1" -j8  # simplex noise generation
```

Change Chunk generation object (see all Chunk objects in [ft_vox.hpp](includes/ft_vox.hpp))
``` sh
make fclean
make DEBUG=1 DEFINE="CHUNK=Chunk" -j8  # use Chunk object
```

---

See more on the school [subject](ft_vox.fr.pdf).
