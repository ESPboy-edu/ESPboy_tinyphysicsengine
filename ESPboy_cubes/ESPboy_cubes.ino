#define SAF_PROGRAM_NAME "tpe1"
#define SAF_PLATFORM_ESPBOY
#define SAF_SETTING_ENABLE_SOUND 0
#define SAF_SETTING_ENABLE_SAVES 0

#define S3L_RESOLUTION_X SAF_SCREEN_WIDTH
#define S3L_RESOLUTION_Y SAF_SCREEN_HEIGHT
#define S3L_PIXEL_FUNCTION s3l_drawPixel

#include "saf.h"

#define S3L_Z_BUFFER 0
#define S3L_SORT 1
#define S3L_FLAT 1
#define S3L_NEAR_CROSS_STRATEGY 1

#define ROOM_SIZE (30 * TPE_F)

#define TPE_RESHAPE_ITERATIONS 2
#define TPE_APPROXIMATE_NET_SPEED 1

#include "tinyphysicsengine.h"
#include "small3dlib.h"

TPE_World tpe_world;
TPE_Joint tpe_joints[30];
TPE_Connection tpe_connections[35];
TPE_Body tpe_bodies[4];

static const S3L_Unit s3l_cubeVertices[S3L_CUBE_VERTEX_COUNT * 3] = { S3L_CUBE_VERTICES(S3L_FRACTIONS_PER_UNIT) };
static const S3L_Index s3l_cubeTriangles[S3L_CUBE_TRIANGLE_COUNT * 3] = { S3L_CUBE_TRIANGLES };

S3L_Model3D s3l_models[3];
S3L_Scene s3l_scene;

uint8_t debugDraw = 0;

void tpe_debugDrawPixel(uint16_t x, uint16_t y, uint8_t color)
{
  SAF_drawPixel(x,y,(color + 2) * 32 + color);
}

TPE_Vec3 tpe_environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  return TPE_envAABoxInside(p,TPE_vec3(0,0,0),
    TPE_vec3(ROOM_SIZE,ROOM_SIZE,ROOM_SIZE));
}

static uint8_t sideColors[12] =
  { 32, 32, 64, 64, 96, 96, 128, 128, 160, 160, 192, 192 };

uint8_t colorAdd;

void s3l_drawPixel(S3L_PixelInfo *p)
{
  SAF_drawPixel(p->x,p->y,sideColors[p->triangleIndex] + colorAdd);
}

void SAF_init(void)
{
  S3L_model3DInit(s3l_cubeVertices,S3L_CUBE_VERTEX_COUNT,s3l_cubeTriangles,S3L_CUBE_TRIANGLE_COUNT,&s3l_models[0]);

  s3l_models[0].transform.scale.x = 3 * S3L_FRACTIONS_PER_UNIT;
  s3l_models[0].transform.scale.y = s3l_models[0].transform.scale.x;
  s3l_models[0].transform.scale.z = s3l_models[0].transform.scale.x;

  s3l_models[1] = s3l_models[0];
  s3l_models[2] = s3l_models[0];

  s3l_models[2].transform.scale.x = ROOM_SIZE;
  s3l_models[2].transform.scale.y = ROOM_SIZE;
  s3l_models[2].transform.scale.z = ROOM_SIZE;
  s3l_models[2].config.backfaceCulling = 1;

  s3l_models[2].triangleCount -= 2;

  S3L_sceneInit(s3l_models,2,&s3l_scene);

  s3l_scene.camera.transform.rotation.x -= TPE_F / 16;

  TPE_makeBox(tpe_joints,tpe_connections,
    2 * TPE_F,2 * TPE_F,2 * TPE_F,TPE_F);

  TPE_bodyInit(&tpe_bodies[0],tpe_joints,8,tpe_connections,16,TPE_F);

  TPE_makeBox(tpe_joints + 8,tpe_connections + 16,
    2 * TPE_F,2 * TPE_F,2 * TPE_F,TPE_F);

  TPE_bodyInit(&tpe_bodies[1],tpe_joints + 8,8,tpe_connections + 16,16,TPE_F);

  TPE_bodyMoveBy(&tpe_bodies[1],TPE_vec3(4 * TPE_F,0,0));

  TPE_worldInit(&tpe_world,tpe_bodies,2,tpe_environmentDistance);
}

uint8_t SAF_loop(void)
{
  SAF_clearScreen(SAF_COLOR_GRAY);

  TPE_worldStep(&tpe_world);

  for (uint8_t i = 0; i < tpe_world.bodyCount; ++i)
    TPE_bodyApplyGravity(&tpe_world.bodies[i],TPE_F / 50);

  for (uint8_t i = 0; i < 2; ++i)
  {
    s3l_models[i].transform.translation.x = 
      (tpe_bodies[i].joints[0].position.x + 
      tpe_bodies[i].joints[7].position.x) / 2;

    s3l_models[i].transform.translation.y = 
      (tpe_bodies[i].joints[0].position.y + 
      tpe_bodies[i].joints[7].position.y) / 2;

    s3l_models[i].transform.translation.z = 
      (tpe_bodies[i].joints[0].position.z + 
      tpe_bodies[i].joints[7].position.z) / 2;

    TPE_Vec3 orient = TPE_bodyGetRotation(&tpe_bodies[i],0,1,2);

    s3l_models[i].transform.rotation.x = orient.x;
    s3l_models[i].transform.rotation.y = orient.y;
    s3l_models[i].transform.rotation.z = orient.z;
  }

  S3L_newFrame();
  colorAdd = 19;
  s3l_scene.models = s3l_models + 2;
  s3l_scene.modelCount = 1;
  S3L_drawScene(s3l_scene);

  colorAdd = 0;
  S3L_newFrame();
  s3l_scene.models = s3l_models;
  s3l_scene.modelCount = 2;
  S3L_drawScene(s3l_scene);

  if (SAF_buttonJustPressed(SAF_BUTTON_C))
    debugDraw = !debugDraw;

  if (debugDraw)
    TPE_worldDebugDraw(&tpe_world,tpe_debugDrawPixel,
      TPE_vec3(
        s3l_scene.camera.transform.translation.x,
        s3l_scene.camera.transform.translation.y,
        s3l_scene.camera.transform.translation.z),
      TPE_vec3(s3l_scene.camera.transform.rotation.x,s3l_scene.camera.transform.rotation.y,0),
      TPE_vec3(SAF_SCREEN_WIDTH,SAF_SCREEN_HEIGHT,TPE_F),4,TPE_F);

  s3l_scene.camera.transform.translation = s3l_models[0].transform.translation;
  s3l_scene.camera.transform.translation.y += 3 * TPE_F; 
  s3l_scene.camera.transform.translation.z -= 5 * TPE_F; 

#define ACC 20
  if (SAF_buttonPressed(SAF_BUTTON_LEFT))
    TPE_bodyAccelerate(&tpe_world.bodies[0],TPE_vec3(-1 * ACC,0,0));
  else if (SAF_buttonPressed(SAF_BUTTON_RIGHT))
    TPE_bodyAccelerate(&tpe_world.bodies[0],TPE_vec3(ACC,0,0));
  else if (SAF_buttonPressed(SAF_BUTTON_UP))
    TPE_bodyAccelerate(&tpe_world.bodies[0],TPE_vec3(0,0,ACC));
  else if (SAF_buttonPressed(SAF_BUTTON_DOWN))
    TPE_bodyAccelerate(&tpe_world.bodies[0],TPE_vec3(0,0,-1 * ACC));

  if (SAF_buttonJustPressed(SAF_BUTTON_A))
    TPE_bodyAccelerate(&tpe_world.bodies[0],TPE_vec3(0,20 * ACC,0));
#undef ACC

  return 1;
}
