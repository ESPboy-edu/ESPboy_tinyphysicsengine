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

#define MAP_BOUND 15000

//#define TPE_RESHAPE_ITERATIONS 2
#define TPE_APPROXIMATE_NET_SPEED 1

#define ACCELERATION (TPE_F / 13)
#define TURN_RATE (3 * TPE_F / 4)
#define TURN_FRICTION (3 * TPE_F / 4)
#define FORW_FRICTION (TPE_F / 14)

#include "tinyphysicsengine.h"
#include "small3dlib.h"
#include "carModel.h"

TPE_World tpe_world;
TPE_Joint tpe_joints[5];
TPE_Connection tpe_connections[10];
TPE_Body tpe_body;

S3L_Scene s3l_scene;

uint8_t debugDraw = 0;

#define PYRAMID_VERTICES 5
const S3L_Unit pyramidVertices[PYRAMID_VERTICES * 3] =
{
  512,0,-512,
  512,0,512,
  -512,0,512,
  -512,0,-512,
  0,512,0
};

#define PYRAMID_TRIANGLES 4
const S3L_Index pyramidTriangles[PYRAMID_TRIANGLES * 3] =
{
  0,1,4,
  1,2,4,
  2,3,4,
  3,0,4
};

#define RAMP_VERTICES 4
const S3L_Unit rampVertices[RAMP_VERTICES * 3] =
{
  512,0,-512,
  512,512,512,
  -512,512,512,
  -512,0,-512
};

#define RAMP_TRIANGLES 4
const S3L_Index rampTriangles[RAMP_TRIANGLES * 3] =
{
  0,1,2,
  2,3,0,
  1,0,2,
  3,2,0
};

S3L_Model3D models[4];

void tpe_debugDrawPixel(uint16_t x, uint16_t y, uint8_t color)
{
  SAF_drawPixel(x,y,(color + 2) * 32 + color);
}

const TPE_Unit rampPoits[6] = { 0,0, -2400,1400, -2400,0 };

TPE_Vec3 tpe_environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  TPE_ENV_START (TPE_envGround(p,0), p)
  TPE_ENV_NEXT (TPE_envSphere(p,TPE_vec3(0,0,-4000),1000), p)
  TPE_ENV_NEXT( TPE_envAATriPrism(p,TPE_vec3(-6500,0,0),rampPoits,5000,2),p )
  TPE_ENV_END
}

void s3l_drawPixel(S3L_PixelInfo *p)
{
  SAF_drawPixel(p->x,p->y,(p->triangleIndex + 100) % 256);
}

uint8_t steering = 0;
uint8_t jointCollisions;
uint8_t jointCollisionsPrev;
TPE_Vec3 carForw, carSide, carUp, carPos, carRot;

uint8_t collisionCallback(uint16_t b1, uint16_t j1, uint16_t b2, uint16_t j2,
  TPE_Vec3 p)
{
  if (b1 == 0 && b1 == b2 && j1 < 4)
    jointCollisions |= 0x01 << j1;

  return 1;
}

void carReset(void)
{
  TPE_bodyStop(&tpe_body);

  tpe_joints[3].position = TPE_vec3(0,3000,10000);
  tpe_joints[2].position = TPE_vec3(512,3000,9500);
  tpe_joints[1].position = TPE_vec3(-512,3000,9500);
  tpe_joints[0].position = TPE_vec3(512,3000,10500);
  tpe_joints[4].position = TPE_vec3(512,3500,10500);

  s3l_scene.camera.transform.translation.z = tpe_body.joints[4].position.z + 3000;
}

void SAF_init(void)
{
  carModelInit();

  models[0] = carModel;

  S3L_model3DInit(pyramidVertices,PYRAMID_VERTICES,pyramidTriangles,PYRAMID_TRIANGLES,&models[1]);

  models[1].transform.translation.z = -4000;
  models[1].transform.scale.x = 1300;
  models[1].transform.scale.y = models[1].transform.scale.x;
  models[1].transform.scale.z = models[1].transform.scale.x;

  S3L_model3DInit(rampVertices,RAMP_VERTICES,rampTriangles,RAMP_TRIANGLES,&models[2]);

  models[2].transform.translation.x = -6500;
  models[2].transform.translation.z = -800;
  models[2].transform.scale.x = 2500;
  models[2].transform.scale.y = 1300;
  models[2].transform.scale.z = 1200;
  models[2].transform.rotation.y = 255;

  S3L_sceneInit(models,3,&s3l_scene);

  s3l_scene.camera.transform.rotation.x -= TPE_F / 14;

  TPE_makeCenterRectFull(tpe_joints,tpe_connections,1000,1800,400);

  tpe_joints[4].position.y += 600;
  tpe_joints[4].sizeDivided *= 4;
  tpe_joints[4].sizeDivided /= 3;

  TPE_bodyInit(&tpe_body,tpe_joints,5,tpe_connections,10,TPE_F / 2);

  tpe_body.elasticity = TPE_F / 100; 
  tpe_body.friction = FORW_FRICTION;
  tpe_body.flags |= TPE_BODY_FLAG_ALWAYS_ACTIVE;

  carReset();

  TPE_worldInit(&tpe_world,&tpe_body,1,tpe_environmentDistance);
  tpe_world.collisionCallback = collisionCallback;
}

uint8_t SAF_loop(void)
{
  SAF_clearScreen(SAF_COLOR_GRAY);

  for (uint8_t i = 0; i < 4; ++i)
  {
    S3L_Vec4 boundPoint, screenPoint;
    S3L_vec4Set(&boundPoint,
      i % 2 == 0 ? -1 * MAP_BOUND : MAP_BOUND,0,
      i / 2 == 0 ? -1 * MAP_BOUND : MAP_BOUND,1024);

    S3L_project3DPointToScreen(boundPoint,s3l_scene.camera,&screenPoint);

    if (screenPoint.w != 0)
      SAF_drawPixel(screenPoint.x,screenPoint.y,SAF_COLOR_RED);
  }
    
  jointCollisions = 0;

  if (SAF_buttonPressed(SAF_BUTTON_RIGHT))
    steering = 1;
  else if (SAF_buttonPressed(SAF_BUTTON_LEFT))
    steering = 2;
  else
    steering = 0;

  TPE_worldStep(&tpe_world);

  TPE_bodyApplyGravity(&tpe_body,TPE_F / 50);
    
  carPos = TPE_bodyGetCenterOfMass(&tpe_body); 

  carForw = TPE_vec3Normalized(
    TPE_vec3Minus(tpe_body.joints[2].position,tpe_body.joints[0].position));

  carSide = TPE_vec3Normalized(
      TPE_vec3Minus(tpe_body.joints[1].position,tpe_body.joints[0].position));

  carUp = TPE_vec3Cross(carForw,carSide);
    
  for (int i = 0; i < 4; ++i)
    if (jointCollisions & (0x01 << i))
    {
      TPE_Vec3 jv = TPE_vec3(
          tpe_body.joints[i].velocity[0],   
          tpe_body.joints[i].velocity[1],   
          tpe_body.joints[i].velocity[2]);   

      TPE_Vec3 ja = carSide;

      if (i >= 2 && steering)
      {
        if (steering == 2)
          ja = TPE_vec3Plus(TPE_vec3Times(carForw,TURN_RATE),carSide);
        else
          ja = TPE_vec3Minus(TPE_vec3Times(carForw,TURN_RATE),carSide);

        ja = TPE_vec3Normalized(ja);
      }

      TPE_Vec3 fric = TPE_vec3Times(ja,(TPE_vec3Dot(ja,jv) * TURN_FRICTION)
        / TPE_F);

      jv = TPE_vec3Minus(jv,fric);

      tpe_body.joints[i].velocity[0] = jv.x;
      tpe_body.joints[i].velocity[1] = jv.y;
      tpe_body.joints[i].velocity[2] = jv.z;
    }
    
  if (TPE_vec3Dot(carUp,TPE_vec3Minus(tpe_body.joints[4].position,
      tpe_body.joints[0].position)) < 0)
    tpe_body.joints[4].position = TPE_vec3Plus(TPE_vec3Times(carUp,300),
      tpe_body.joints[0].position);
    
  if ((jointCollisions | jointCollisionsPrev) & 0x03) // back wheels on ground?
  {
    if (SAF_buttonPressed(SAF_BUTTON_UP))
    {
      tpe_body.joints[0].velocity[0] += (carForw.x * ACCELERATION) / TPE_F;
      tpe_body.joints[0].velocity[1] += (carForw.y * ACCELERATION) / TPE_F;
      tpe_body.joints[0].velocity[2] += (carForw.z * ACCELERATION) / TPE_F;
      tpe_body.joints[1].velocity[0] += (carForw.x * ACCELERATION) / TPE_F;
      tpe_body.joints[1].velocity[1] += (carForw.y * ACCELERATION) / TPE_F;
      tpe_body.joints[1].velocity[2] += (carForw.z * ACCELERATION) / TPE_F;
    }
    else if (SAF_buttonPressed(SAF_BUTTON_DOWN))
    {
      tpe_body.joints[0].velocity[0] -= (carForw.x * ACCELERATION) / TPE_F;
      tpe_body.joints[0].velocity[1] -= (carForw.y * ACCELERATION) / TPE_F;
      tpe_body.joints[0].velocity[2] -= (carForw.z * ACCELERATION) / TPE_F;
      tpe_body.joints[1].velocity[0] -= (carForw.x * ACCELERATION) / TPE_F;
      tpe_body.joints[1].velocity[1] -= (carForw.y * ACCELERATION) / TPE_F;
      tpe_body.joints[1].velocity[2] -= (carForw.z * ACCELERATION) / TPE_F;
    }
  }
    
  if (SAF_buttonJustPressed(SAF_BUTTON_B))
    carReset();

  jointCollisionsPrev = jointCollisions;

  carRot = TPE_bodyGetRotation(&tpe_body,0,2,1);

  S3L_newFrame();

  models[0].transform.translation.x = carPos.x;
  models[0].transform.translation.y = carPos.y;
  models[0].transform.translation.z = carPos.z;

  models[0].transform.rotation.x = carRot.x;
  models[0].transform.rotation.y = carRot.y;
  models[0].transform.rotation.z = carRot.z;

  if (carPos.x > MAP_BOUND)
  {
    TPE_bodyMoveBy(&tpe_body,TPE_vec3(-2 * MAP_BOUND,0,0));
    s3l_scene.camera.transform.translation.x -= 2 * MAP_BOUND;
  }
  else if (carPos.x < -1 * MAP_BOUND)
  {
    TPE_bodyMoveBy(&tpe_body,TPE_vec3(2 * MAP_BOUND,0,0));
    s3l_scene.camera.transform.translation.x += 2 * MAP_BOUND;
  }

  if (carPos.z > MAP_BOUND)
  {
    TPE_bodyMoveBy(&tpe_body,TPE_vec3(0,0,-2 * MAP_BOUND));
    s3l_scene.camera.transform.translation.z -= 2 * MAP_BOUND;
  }
  else if (carPos.z < -1 * MAP_BOUND)
  {
    TPE_bodyMoveBy(&tpe_body,TPE_vec3(0,0,2 * MAP_BOUND));
    s3l_scene.camera.transform.translation.z += 2 * MAP_BOUND;
  }

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

  s3l_scene.camera.transform.translation.y = carPos.y + 900;

  TPE_Vec3 v = TPE_vec3KeepWithinDistanceBand(
    TPE_vec3(
        s3l_scene.camera.transform.translation.x,
        s3l_scene.camera.transform.translation.y,
        s3l_scene.camera.transform.translation.z
      ),tpe_body.joints[4].position,2 * TPE_F,4 * TPE_F);

  s3l_scene.camera.transform.translation.x = v.x;
  s3l_scene.camera.transform.translation.y = v.y;
  s3l_scene.camera.transform.translation.z = v.z;

  S3L_Vec4 toCar;

  toCar.x = carPos.x - s3l_scene.camera.transform.translation.x;
  toCar.y = carPos.y - s3l_scene.camera.transform.translation.y;
  toCar.z = carPos.z - s3l_scene.camera.transform.translation.z;
  toCar.w = 0;

  TPE_Unit angleDiff = s3l_scene.camera.transform.rotation.y -
    (TPE_vec2Angle(toCar.x,toCar.z) - 128);

  s3l_scene.camera.transform.rotation.y -= 
    (angleDiff < 100 && angleDiff > -100) ? angleDiff / 2 : angleDiff;

  return 1;
}
