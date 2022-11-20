/** Demo showing a wrecking ball hitting a few boxes. */

#include "helper.h"

#define ROOM_SIZE (14 * TPE_F)
#define CUBE_SIZE (3 * TPE_F / 2)
#define GRAVITY (TPE_F / 100)

TPE_Vec3 environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  return TPE_envAABoxInside(p,TPE_vec3(0,ROOM_SIZE / 4,0),
    TPE_vec3(ROOM_SIZE,ROOM_SIZE / 2,ROOM_SIZE));
}

TPE_Vec3 cubeOrientations[6];
TPE_Vec3 cubePositions[6];

TPE_Joint ballJoints[4];  // for the wrecking ball body
TPE_Connection ballConnections[3];

/** Updates the orientation and position of the ith cube's 3D model accordint to
  the physics model. This will only be done for active bodies to help
  performance a bit. */
void updateOrientPos(int i)
{
  cubeOrientations[i] = TPE_bodyGetRotation(&tpe_world.bodies[i],0,2,1);
  cubePositions[i] = TPE_bodyGetCenterOfMass(&tpe_world.bodies[i]);
}

int main(void)
{
  helper_init();

  tpe_world.environmentFunction = environmentDistance;

  s3l_scene.camera.transform.translation.z -= ROOM_SIZE / 2;
  s3l_scene.camera.transform.translation.y += ROOM_SIZE / 3;
  s3l_scene.camera.transform.translation.x -= ROOM_SIZE / 4;
  s3l_scene.camera.transform.rotation.y = -1 * TPE_F / 16;

  for (int i = 0; i < 6; ++i) // add 6 cubes
  {
    helper_addBox(CUBE_SIZE / 2,CUBE_SIZE / 2,CUBE_SIZE / 2,CUBE_SIZE / 4,TPE_F / 5);
    helper_lastBody.friction = TPE_F / 20; // decrease friction for fun
  }

#define move(i,x,y) \
  TPE_bodyMoveBy(&tpe_world.bodies[i],TPE_vec3((CUBE_SIZE / 2 + 10) * x,10 + CUBE_SIZE / 2 + y * (CUBE_SIZE + 10),0));

  move(0,0,0)
  move(1,-2,0)
  move(2,2,0)
  move(3,-1,1)
  move(4,1,1)
  move(5,0,2)

#undef move
  
  for (int i = 0; i < 6; ++i)
  {
    updateOrientPos(i);

    /* normally the cubes wouldn't stand on each other as they're really made of
       spheres, so we deactivate them to keep them in place until the wrecking
       ball hits them: */
    TPE_bodyDeactivate(&tpe_bodies[i]);
  }

  // now create the wrecking ball body:

  ballJoints[0] = TPE_joint(TPE_vec3(0,ROOM_SIZE / 2,0),0);
  ballJoints[1] = TPE_joint(TPE_vec3(0,ROOM_SIZE / 2 - TPE_F / 5,-1 * TPE_F),0);
  ballJoints[2] = TPE_joint(TPE_vec3(0,ROOM_SIZE / 2 - TPE_F / 2,-2 * TPE_F),0);
  ballJoints[3] = TPE_joint(TPE_vec3(0,ROOM_SIZE / 2 - TPE_F,-4 * TPE_F),TPE_F);
  ballJoints[3].velocity[1] = -1 * TPE_F / 2;
  ballJoints[3].velocity[2] = -1 * TPE_F / 2;

  ballConnections[0].joint1 = 0; ballConnections[0].joint2 = 1;
  ballConnections[1].joint1 = 1; ballConnections[1].joint2 = 2;
  ballConnections[2].joint1 = 2; ballConnections[2].joint2 = 3;

  TPE_Body *ballBody = &tpe_world.bodies[6];

  TPE_bodyInit(ballBody,ballJoints,4,ballConnections,3,2 * TPE_F);

  ballBody->flags |= TPE_BODY_FLAG_SIMPLE_CONN;
  ballBody->flags |= TPE_BODY_FLAG_ALWAYS_ACTIVE;
  ballBody->friction = 0;
  ballBody->elasticity = TPE_F;

  tpe_world.bodyCount++;
    
  while (helper_running)
  {
    helper_frameStart();

    helper_cameraFreeMovement();

    if (helper_frame % 16 == 0)
    {
      helper_printCPU();

      if (sdl_keyboard[SDL_SCANCODE_L]) // pressing L explodes the cubes :)
        for (int i = 0; i < 6; ++i)
        {
          TPE_bodyActivate(&tpe_world.bodies[i]);
          TPE_bodyAccelerate(&tpe_world.bodies[i],
          TPE_vec3Plus(TPE_vec3(0,TPE_F / 5,0),TPE_vec3Times(cubePositions[i],TPE_F / 4)));
        }
    }

    // pin the top point of the wrecking ball to the ceiling:
    TPE_jointPin(&ballBody->joints[0],TPE_vec3(0,ROOM_SIZE / 2 - TPE_F / 100,0));

    TPE_worldStep(&tpe_world);

    helper_set3DColor(170,170,170);
    helper_draw3DBoxInside(TPE_vec3(0,ROOM_SIZE / 4,0),TPE_vec3(ROOM_SIZE,ROOM_SIZE / 2,ROOM_SIZE),TPE_vec3(0,0,0));

    helper_set3DColor(200,10,10);

    for (int i = 0; i < 6; ++i)
    {
      TPE_bodyApplyGravity(&tpe_world.bodies[i],GRAVITY);

      if (TPE_bodyIsActive(&tpe_world.bodies[i]))
        updateOrientPos(i);

      helper_draw3DBox(cubePositions[i],TPE_vec3(CUBE_SIZE,CUBE_SIZE,CUBE_SIZE),
        cubeOrientations[i]);
    }

    ballBody->joints[3].velocity[1] -= GRAVITY; // apply gravity only to one joint

    helper_draw3DSphere(ballBody->joints[3].position,TPE_vec3(400,400,400),TPE_vec3(0,0,0));

    for (int i = 0; i < 3; ++i)
      helper_drawLine3D(
        ballBody->joints[ballBody->connections[i].joint1].position,
        ballBody->joints[ballBody->connections[i].joint2].position,
        255,0,0);

    if (helper_debugDrawOn)
      helper_debugDraw(1);

    helper_frameEnd();
  }

  helper_end();

  return 0;
}
