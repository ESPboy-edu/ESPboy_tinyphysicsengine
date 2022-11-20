/** A bigger testing playground, just to see that everyhing works OK. The code
  is not very nice :) */

#define CAMERA_STEP 200

#include "helper.h"

TPE_Unit
  ramp1[6] = { 5000,0, 5000,5000, 0,0 },
  ramp2[6] = { 5000,0, 5000,3000, 0,0 },
  ramp3[6] = { 5000,0, 5000,1500, 0,0 },
  impale[6] = { 1500,0, 0,6000, -1500,0 };

TPE_Vec3 environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  TPE_ENV_START( TPE_envGround(p,0), p)
  TPE_ENV_NEXT ( TPE_envAATriPrism(p,TPE_vec3(1000,0,0),ramp1,5000,2), p)
  TPE_ENV_NEXT ( TPE_envAATriPrism(p,TPE_vec3(-4000,0,0),ramp2,3500,2), p)
  TPE_ENV_NEXT ( TPE_envAATriPrism(p,TPE_vec3(-7500,0,0),ramp3,3500,2), p)
  TPE_ENV_NEXT ( TPE_envAATriPrism(p,TPE_vec3(8000,0,-2400),impale,3500,2), p)
  TPE_ENV_END
}

uint8_t simulating = 0;

TPE_Body *controlledBody;

int main(void)
{
  helper_init();

  puts("press P to start");

  helper_debugDrawOn = 1;

  tpe_world.environmentFunction = environmentDistance;

  s3l_scene.camera.transform.translation.z = -3000;
  s3l_scene.camera.transform.translation.y = 2000;
  s3l_scene.camera.transform.translation.x = 0;
  s3l_scene.camera.transform.rotation.y = TPE_F / 16;

#define addBody(x,y,z,f) \
  helper_addBox(700,700,700,300,500); \
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(x,y,z)); \
  helper_lastBody.elasticity = 255; \
  helper_lastBody.friction = f;

  // cubes on ramps:

  addBody(1800,5200,4000,0)
  addBody(400,5200,4000,128)
  addBody(-1100,5200,4000,511)
  addBody(-2800,3600,4000,300)
  addBody(-4300,3600,4000,10)
  addBody(-6700,2000,4000,300)
  addBody(-8400,2000,4000,10)

  addBody(7500,6200,-2400,10) // impaled cube

#define addBody(x,y,z,e) \
  helper_addRect(700,700,300,500); \
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(x,y,z)); \
  helper_lastBody.elasticity = e;

  // falling bodies

  addBody(7000,5000,0,511)
  addBody(8800,5000,0,255)
  addBody(10600,5000,0,0)

  helper_addBox(5000,5000,5000,2000,2000); // big box
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(-20000,10000,8000));
  TPE_bodySpin(&helper_lastBody,TPE_vec3(100,200,-20));

  // two colliding spheres:

  helper_addBall(800,2000);
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(200,4000,-4800));
  helper_lastBody.elasticity = 255;
  helper_lastBody.friction = 255;
  helper_lastBody.joints[0].velocity[0] = 10;
  
  helper_addBall(800,200);
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(3200,3800,-4800));
  helper_lastBody.elasticity = 255;
  helper_lastBody.friction = 255;
  helper_lastBody.joints[0].velocity[0] = -300;

  helper_addBall(1000,1000);
  controlledBody = &helper_lastBody;
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(200,2000,-3000));

  // two colliding bodies:

  helper_addCenterBox(700,700,700,300,500);
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(-4000,5000,-3000));
  helper_lastBody.elasticity = 255;
  helper_lastBody.friction = 255;
  //helper_lastBody.flags |= TPE_BODY_FLAG_SOFT;
  TPE_bodyAccelerate(&helper_lastBody,TPE_vec3(-300,0,0));

  helper_addCenterBox(700,700,700,300,500);
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(-8000,5000,-3000));
  helper_lastBody.elasticity = 255;
  helper_lastBody.friction = 255;
  helper_lastBody.flags |= TPE_BODY_FLAG_SOFT;
  TPE_bodyAccelerate(&helper_lastBody,TPE_vec3(300,0,0));

  while (helper_running)
  {
    helper_frameStart();

    helper_cameraFreeMovement();

    if (simulating)
    {
      TPE_worldStep(&tpe_world);

      for (int i = 0; i < 12; ++i)
        TPE_bodyApplyGravity(&tpe_world.bodies[i],5);
    }

    if (helper_debugDrawOn)
      helper_debugDraw(1);

    if (sdl_keyboard[SDL_SCANCODE_P])
      simulating = 1;

    TPE_bodyMultiplyNetSpeed(controlledBody,255);

#define ACC 100
    if (sdl_keyboard[SDL_SCANCODE_W])
      TPE_bodyAccelerate(controlledBody,TPE_vec3(0,0,ACC));
    else if (sdl_keyboard[SDL_SCANCODE_S])
      TPE_bodyAccelerate(controlledBody,TPE_vec3(0,0,-1 * ACC));
    else if (sdl_keyboard[SDL_SCANCODE_D])
      TPE_bodyAccelerate(controlledBody,TPE_vec3(ACC,0,0));
    else if (sdl_keyboard[SDL_SCANCODE_A])
      TPE_bodyAccelerate(controlledBody,TPE_vec3(-1 * ACC,0,0));
    else if (sdl_keyboard[SDL_SCANCODE_C])
      TPE_bodyAccelerate(controlledBody,TPE_vec3(0,ACC,0));
    else if (sdl_keyboard[SDL_SCANCODE_X])
      TPE_bodyAccelerate(controlledBody,TPE_vec3(0,-1 * ACC,0));
#undef ACC

    helper_frameEnd();
  }

  helper_end();

  return 0;
}
