#define CAMERA_STEP (TPE_F / 2)
#define JOINT_SIZE (TPE_F / 4)
#define BALL_SIZE (5 * TPE_F / 4)

#define HEIGHTMAP_3D_RESOLUTION 8
#define HEIGHTMAP_3D_STEP (TPE_F * 2)

#include "helper.h"

#define ROOM_SIZE (HEIGHTMAP_3D_RESOLUTION * HEIGHTMAP_3D_STEP + JOINT_SIZE)

TPE_Vec3 environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  return TPE_envAABoxInside(p,TPE_vec3(0,0,0),TPE_vec3(ROOM_SIZE,ROOM_SIZE,ROOM_SIZE));
}

#define WATER_JOINTS (HEIGHTMAP_3D_RESOLUTION * HEIGHTMAP_3D_RESOLUTION)
#define WATER_CONNECTIONS (2 * ((HEIGHTMAP_3D_RESOLUTION - 1) * HEIGHTMAP_3D_RESOLUTION))

TPE_Joint joints[WATER_JOINTS + 1];
TPE_Connection connections[WATER_CONNECTIONS];
TPE_Body bodies[2];

int main(void)
{
  helper_init();

  puts("WSAD, XC: move the ball");

  s3l_scene.camera.transform.translation.z = -6 * TPE_F;
  s3l_scene.camera.transform.translation.y = 4 * TPE_F;
  s3l_scene.camera.transform.translation.x = 0;
  s3l_scene.camera.transform.rotation.y = TPE_F / 16;

  // build the water body:

  for (int i = 0; i < HEIGHTMAP_3D_POINTS; ++i)
    joints[i] = TPE_joint(helper_heightmapPointLocation(i),JOINT_SIZE);

  int index = 0;

  for (int j = 0; j < HEIGHTMAP_3D_RESOLUTION; ++j)
    for (int i = 0; i < HEIGHTMAP_3D_RESOLUTION - 1; ++i)
    {
      connections[index].joint1 = j * HEIGHTMAP_3D_RESOLUTION + i;
      connections[index].joint2 = connections[index].joint1 + 1;

      index++;

      connections[index].joint1 = i * HEIGHTMAP_3D_RESOLUTION + j;
      connections[index].joint2 = connections[index].joint1 + HEIGHTMAP_3D_RESOLUTION;

      index++;
    }

  TPE_bodyInit(&bodies[0],joints,WATER_JOINTS,connections,WATER_CONNECTIONS,
    2 * TPE_F);

  bodies[0].flags |= TPE_BODY_FLAG_SOFT;
  bodies[0].flags |= TPE_BODY_FLAG_ALWAYS_ACTIVE;

  // create the ball body:
  joints[WATER_JOINTS] = TPE_joint(TPE_vec3(0,0,ROOM_SIZE / 4),BALL_SIZE);
  TPE_bodyInit(&bodies[1],joints + WATER_JOINTS,1,connections,0,200);

  bodies[1].flags |= TPE_BODY_FLAG_ALWAYS_ACTIVE;

  TPE_worldInit(&tpe_world,bodies,2,environmentDistance);

  while (helper_running)
  {
    helper_frameStart();

    helper_cameraFreeMovement();

    // update the 3D model vertex positions:

    S3L_Unit *v = heightmapVertices;

    for (int i = 0; i < WATER_JOINTS; ++i)
    {
      *v = joints[i].position.x;
      v++;
      *v = joints[i].position.y;
      v++;
      *v = joints[i].position.z;
      v++;
    }

    // pin the joints at the edges of the grid:

    for (int index = 0; index < WATER_JOINTS; ++index)
      if (index % HEIGHTMAP_3D_RESOLUTION == 0 || index % HEIGHTMAP_3D_RESOLUTION == HEIGHTMAP_3D_RESOLUTION - 1 ||
        index / HEIGHTMAP_3D_RESOLUTION == 0 || index / HEIGHTMAP_3D_RESOLUTION == HEIGHTMAP_3D_RESOLUTION - 1)
        TPE_jointPin(&joints[index],helper_heightmapPointLocation(index));

    TPE_worldStep(&tpe_world);

#define G ((5 * 30) / FPS)
    TPE_bodyApplyGravity(&tpe_world.bodies[1],
      bodies[1].joints[0].position.y > 0 ? G : (-2 * G));

#define ACC ((25 * 30) / FPS )
    if (sdl_keyboard[SDL_SCANCODE_W])
      TPE_bodyAccelerate(&bodies[1],TPE_vec3(0,0,ACC));
    else if (sdl_keyboard[SDL_SCANCODE_S])
      TPE_bodyAccelerate(&bodies[1],TPE_vec3(0,0,-1 * ACC));
    else if (sdl_keyboard[SDL_SCANCODE_D])
      TPE_bodyAccelerate(&bodies[1],TPE_vec3(ACC,0,0));
    else if (sdl_keyboard[SDL_SCANCODE_A])
      TPE_bodyAccelerate(&bodies[1],TPE_vec3(-1 * ACC,0,0));
    else if (sdl_keyboard[SDL_SCANCODE_C])
      TPE_bodyAccelerate(&bodies[1],TPE_vec3(0,ACC,0));
    else if (sdl_keyboard[SDL_SCANCODE_X])
      TPE_bodyAccelerate(&bodies[1],TPE_vec3(0,-1 * ACC,0));

    helper_set3DColor(255,0,0);
    helper_draw3DSphere(bodies[1].joints[0].position,TPE_vec3(BALL_SIZE,BALL_SIZE,BALL_SIZE),TPE_vec3(0,0,0));
    helper_set3DColor(0,100,255);
    helper_drawModel(&heightmapModel,TPE_vec3(0,0,0),TPE_vec3(TPE_F,TPE_F,TPE_F),TPE_vec3(0,0,0));

    if (helper_debugDrawOn)
      helper_debugDraw(1);

    helper_frameEnd();
  }

  helper_end();

  return 0;
}
