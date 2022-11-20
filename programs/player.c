/** Demo showing a simple first person movement. */

#define S3L_NEAR_CROSS_STRATEGY 2
#define S3L_PERSPECTIVE_CORRECTION 2

#include "helper.h"
#include "levelModel.h"

TPE_Unit elevatorHeight;
TPE_Unit ramp[6] = { 1600,0, -500,1400, -700,0 };
TPE_Unit ramp2[6] = { 2000,-5000, 1500,1700, -5000,-500 };

TPE_Vec3 environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  // manually created environment to match the 3D model of it
  TPE_ENV_START( TPE_envAABoxInside(p,TPE_vec3(0,2450,-2100),TPE_vec3(12600,5000,10800)),p )
  TPE_ENV_NEXT( TPE_envAABox(p,TPE_vec3(-5693,0,-6580),TPE_vec3(4307,20000,3420)),p )
  TPE_ENV_NEXT( TPE_envAABox(p,TPE_vec3(-10000,-1000,-10000),TPE_vec3(11085,2500,9295)),p )
  TPE_ENV_NEXT( TPE_envAATriPrism(p,TPE_vec3(-5400,0,0),ramp,3000,2), p)
  TPE_ENV_NEXT( TPE_envAATriPrism(p,TPE_vec3(2076,651,-6780),ramp2,3000,0), p)
  TPE_ENV_NEXT( TPE_envAABox(p,TPE_vec3(7000,0,-8500),TPE_vec3(3405,2400,3183)),p )
  TPE_ENV_NEXT( TPE_envSphere(p,TPE_vec3(2521,-100,-3799),1200),p )
  TPE_ENV_NEXT( TPE_envAABox(p,TPE_vec3(5300,elevatorHeight,-4400),TPE_vec3(1000,elevatorHeight,1000)),p )
  TPE_ENV_NEXT( TPE_envHalfPlane(p,TPE_vec3(5051,0,1802),TPE_vec3(-255,0,-255)),p )
  TPE_ENV_NEXT( TPE_envInfiniteCylinder(p,TPE_vec3(320,0,170),TPE_vec3(0,255,0),530),p )
  TPE_ENV_END
}

int jumpCountdown = 0, onGround = 0;
TPE_Unit playerRotation = 0, groundDist;
TPE_Vec3 ballRot, ballPreviousPos, playerDirectionVec;
TPE_Body *playerBody = 0;

void updateDirection(void) // updates player direction vector
{
  playerDirectionVec.x = TPE_sin(playerRotation);
  playerDirectionVec.z = TPE_cos(playerRotation);
  playerDirectionVec.y = 0;
}

int main(void)
{
  helper_init();
  levelModelInit();
  updateDirection();

  ballRot = TPE_vec3(0,0,0);

  tpe_world.environmentFunction = environmentDistance;

  /* normally player bodies are approximated with capsules -- since we don't
  have these, we'll use a body consisting of two spheres: */
  helper_add2Line(400,300,400);

  playerBody = &(tpe_world.bodies[0]);

  TPE_bodyMoveBy(&tpe_world.bodies[0],TPE_vec3(1000,1000,1500));
  TPE_bodyRotateByAxis(&tpe_world.bodies[0],TPE_vec3(0,0,TPE_F / 4));
  playerBody->elasticity = 0;
  playerBody->friction = 0;   
  playerBody->flags |= TPE_BODY_FLAG_NONROTATING; // make it always upright
  playerBody->flags |= TPE_BODY_FLAG_ALWAYS_ACTIVE;
  groundDist = TPE_JOINT_SIZE(playerBody->joints[0]) + 30;

  // add two interactive bodies:

  helper_addBall(1000,100);
  TPE_bodyMoveBy(&tpe_world.bodies[1],TPE_vec3(-1000,1000,0));
  tpe_world.bodies[1].elasticity = 400;
  tpe_world.bodies[1].friction = 100;
  ballPreviousPos = tpe_world.bodies[1].joints[0].position;

  helper_addCenterRect(600,600,400,50);
  TPE_bodyMoveBy(&tpe_world.bodies[2],TPE_vec3(-3000,1000,2000));
  tpe_world.bodies[2].elasticity = 100;
  tpe_world.bodies[2].friction = 50;
 
  while (helper_running)
  {
    helper_frameStart();

    TPE_worldStep(&tpe_world);

    if (jumpCountdown > 0)
      jumpCountdown--;

    TPE_Vec3 groundPoint =
      environmentDistance(playerBody->joints[0].position,groundDist);

    onGround = (playerBody->flags & TPE_BODY_FLAG_DEACTIVATED) ||
     (TPE_DISTANCE(playerBody->joints[0].position,groundPoint)
     <= groundDist && groundPoint.y < playerBody->joints[0].position.y - groundDist / 2);

    if (!onGround)
    {
      /* it's possible that the closest point is e.g. was a perpend wall so also
         additionally check directly below */

      onGround = TPE_DISTANCE( playerBody->joints[0].position,
        TPE_castEnvironmentRay(playerBody->joints[0].position,
        TPE_vec3(0,-1 * TPE_F,0),tpe_world.environmentFunction, 
        128,512,512)) <= groundDist;
    }

    elevatorHeight = (1250 * (TPE_sin(helper_frame * 4) + TPE_F)) / (2 * TPE_F);

    s3l_scene.camera.transform.translation.x = playerBody->joints[0].position.x;
    s3l_scene.camera.transform.translation.z = playerBody->joints[0].position.z;
    s3l_scene.camera.transform.translation.y = TPE_keepInRange(
      s3l_scene.camera.transform.translation.y,
      playerBody->joints[1].position.y,
      playerBody->joints[1].position.y + 10);

    TPE_bodyMultiplyNetSpeed(&tpe_world.bodies[0],onGround ? 300 : 505);

    s3l_scene.camera.transform.rotation.y = -1 * playerRotation;

    // fake the sphere rotation (since a single joint doesn't rotate itself):
    TPE_Vec3 ballRoll = TPE_fakeSphereRotation(ballPreviousPos,
      tpe_world.bodies[1].joints[0].position,1000);

    ballRot = TPE_rotationRotateByAxis(ballRot,ballRoll);

    ballPreviousPos = tpe_world.bodies[1].joints[0].position;

    for (int i = 0; i < tpe_world.bodyCount; ++i)
      TPE_bodyApplyGravity(&tpe_world.bodies[i],5);

    if (onGround)
    {
      if (sdl_keyboard[SDL_SCANCODE_SPACE] && jumpCountdown == 0)
      {
        playerBody->joints[0].velocity[1] = 90;
        jumpCountdown = 8;
      }

#define D 15 // just some vector divisor to make the speed slower
      if (sdl_keyboard[SDL_SCANCODE_UP] || sdl_keyboard[SDL_SCANCODE_W])
      {
        playerBody->joints[0].velocity[0] += playerDirectionVec.x / D;
        playerBody->joints[0].velocity[2] += playerDirectionVec.z / D;
      }
      else if (sdl_keyboard[SDL_SCANCODE_DOWN] || sdl_keyboard[SDL_SCANCODE_S])
      {
        playerBody->joints[0].velocity[0] -= playerDirectionVec.x / D;
        playerBody->joints[0].velocity[2] -= playerDirectionVec.z / D;
      }

      if (sdl_keyboard[SDL_SCANCODE_A])
      {
        playerBody->joints[0].velocity[2] += playerDirectionVec.x / D;
        playerBody->joints[0].velocity[0] -= playerDirectionVec.z / D;
      }
      else if (sdl_keyboard[SDL_SCANCODE_D])
      {
        playerBody->joints[0].velocity[2] -= playerDirectionVec.x / D;
        playerBody->joints[0].velocity[0] += playerDirectionVec.z / D;
      }
#undef D
    }
    
    updateDirection();

    if (sdl_keyboard[SDL_SCANCODE_LEFT])
      playerRotation -= 8;
    else if (sdl_keyboard[SDL_SCANCODE_RIGHT])
      playerRotation += 8;

    if (helper_frame % 64 == 0)
    {
      helper_printCPU();
      helper_printCamera();
    }

    // draw the 3D environment

    helper_set3DColor(200,200,200);

    helper_drawModel(&levelModel,TPE_vec3(0,0,0),TPE_vec3(600,600,600), 
      TPE_vec3(0,0,0));

    helper_draw3DBox(TPE_vec3(5300,elevatorHeight,-4400),
      TPE_vec3(2000,2 * elevatorHeight,2000),TPE_vec3(0,0,0));

    helper_set3DColor(200,50,0);

    helper_draw3DBox(TPE_bodyGetCenterOfMass(&tpe_world.bodies[2]),
      TPE_vec3(1200,800,1200),TPE_bodyGetRotation(&tpe_world.bodies[2],0,2,1));

    helper_draw3DSphere(tpe_world.bodies[1].joints[0].position,
      TPE_vec3(1000,1000,1000),ballRot);

    if (helper_debugDrawOn)
      helper_debugDraw(1);

    helper_frameEnd();
  }

  helper_end();

  return 0;
}
