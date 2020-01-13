#pragma once

#include "Camera.hpp"
#include "ft_vox.hpp"

#define SURV_MOVEMENT_SPEED 8.0f
#define SURV_RUN_FACTOR		1.5f
#define SURV_GRAVITY		18.0f
#define SURV_JUMP_HEIGHT	1.4f
#define SURV_JUMP_SPEED		7.0f

// size of player
#define SURV_HEIGHT			1.8f
#define SURV_RADIUS			0.4f

#define MOVING_COLISION_PRECISION	0.1f  // when moving with more than x, check colision each x block

class CameraSurvival : public Camera {
	public:
		CameraSurvival(tWinUser * winU, CAMERA_VEC3 pos = CAMERA_VEC3(0.0f, 0.0f, 0.0f), CAMERA_VEC3 up = \
		CAMERA_VEC3(0.0f, 1.0f, 0.0f), CAMERA_FLOAT yaw = -90.0f, CAMERA_FLOAT pitch = 0.0f);
		CameraSurvival(CameraSurvival const &src);
		virtual ~CameraSurvival();

		CameraSurvival &operator=(CameraSurvival const &rhs);

		virtual void run(CAMERA_FLOAT dtTime);  // call  this function each frame
		virtual void processKeyboard(CamMovement direction, CAMERA_FLOAT dtTime, bool isRun = false);
		virtual void resetPosition();  // this function does nothing

		bool isOnBlock(wordIVec3 blockPos) const;

		CAMERA_FLOAT	gravity;
		CAMERA_FLOAT	jumpHeight;
		CAMERA_FLOAT	jumpSpeed;
		CAMERA_FLOAT	height;
		CAMERA_FLOAT	eyeHeight;
		CAMERA_FLOAT	radius;

	private:
		struct Constraints {  // true == unable to move
			bool	positiveX;
			bool	positiveY;
			bool	positiveZ;
			bool	negativeX;
			bool	negativeY;
			bool	negativeZ;

			Constraints() : positiveX(false), positiveY(false), positiveZ(false),
				negativeX(false), negativeY(false), negativeZ(false) {}
		};

		CameraSurvival();
		void _move(CAMERA_VEC3 dest, bool _underMove = false);
		Constraints _getConstraints(CAMERA_VEC3 dest);

		tWinUser *	_winU;

		CAMERA_FLOAT		_curJumping;  // current jumping height -> if _curJumping < 0: no jump
};
