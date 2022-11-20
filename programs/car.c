#define S3L_NEAR_CROSS_STRATEGY 2
#define S3L_PERSPECTIVE_CORRECTION 2

#include "helper.h"
#include "carArenaModel.h"
#include "carModel.h"

#define ACCELERATION (TPE_F / 14)
#define TURN_RATE (3 * TPE_F / 4)
#define TURN_FRICTION (3 * TPE_F / 4) // wheel side friction
#define FORW_FRICTION (TPE_F / 14)    // wheel forward friction

TPE_Unit rampPoits[6] = { 0,0, -2400,1400, -2400,0 };

TPE_Vec3 environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  TPE_ENV_START( TPE_envGround(p,0),p )
  TPE_ENV_NEXT( TPE_envSphereInside(p,TPE_vec3(0,10000,0),20000),p )
  TPE_ENV_NEXT( TPE_envAABox(p,TPE_vec3(-8700,100,-800),TPE_vec3(2200,1000,800)),p )
  TPE_ENV_NEXT( TPE_envAATriPrism(p,TPE_vec3(8700,0,0),rampPoits,5000,2),p )
  TPE_ENV_NEXT( TPE_envSphere(p,TPE_vec3(0,-200,0),1700),p )
  TPE_ENV_END
}

uint8_t steering = 0; // 0: none, 1: right, 2: left
uint8_t jointCollisions; /* bit mask of colliding joints (i.e. wheels that are 
                            currently touching the ground). */
uint8_t jointCollisionsPrev; // for averaging

TPE_Body *carBody;

TPE_Vec3 carForw, carSide, carUp, carPos, carRot;

uint8_t collisionCallback(uint16_t b1, uint16_t j1, uint16_t b2, uint16_t j2,
  TPE_Vec3 p)
{
  // here we record which wheels (joints) are currently touching the ground

  if (b1 == 1 && b1 == b2 && j1 < 4)
    jointCollisions |= 0x01 << j1;

  return 1;
}

int main(void)
{
  arenaModelInit();
  carModelInit();

  helper_init();

  carPos = TPE_vec3(0,0,0);

  tpe_world.environmentFunction = environmentDistance;
  tpe_world.collisionCallback = collisionCallback;

  // add an interactive body:

  helper_addRect(6 * TPE_F / 5,6 * TPE_F / 5,6 * TPE_F / 5,TPE_F / 2);
  TPE_bodyMoveBy(&helper_lastBody,TPE_vec3(2000,1000,3000));
  helper_lastBody.friction = TPE_F / 5;

  // create the car body, start with a "center rect":

  helper_addCenterRectFull(1000,1800,400,2000);

  carBody = &helper_lastBody;

  // scale and shift the middle joint a bit
  carBody->joints[4].position.y += 600;
  carBody->joints[4].sizeDivided *= 3;
  carBody->joints[4].sizeDivided /= 2;

  // we need to reinit the body so that it recomputes its connection lengths
  TPE_bodyInit(carBody,carBody->joints,carBody->jointCount,carBody->connections,
    carBody->connectionCount,TPE_F / 2);

  TPE_bodyMoveBy(carBody,TPE_vec3(6 * TPE_F,2 * TPE_F,0));
  carBody->elasticity = TPE_F / 100; 
  carBody->friction = FORW_FRICTION;
  carBody->flags |= TPE_BODY_FLAG_ALWAYS_ACTIVE;
  
  s3l_scene.camera.transform.rotation.x = -35;

  while (helper_running)
  {
    helper_frameStart();

    jointCollisions = 0;

    if (sdl_keyboard[SDL_SCANCODE_RIGHT])
      steering = 1;
    else if (sdl_keyboard[SDL_SCANCODE_LEFT])
      steering = 2;
    else
      steering = 0;

    TPE_worldStep(&tpe_world);

    // get and smooth the car position:

    carPos = TPE_vec3KeepWithinBox(carPos,carBody->joints[4].position,
      TPE_vec3(TPE_F / 50,TPE_F / 50,TPE_F / 50));

    // compute the car direction vectors from positions of its joints:

    carForw = TPE_vec3Normalized(TPE_vec3Plus(
      TPE_vec3Minus(carBody->joints[2].position,carBody->joints[0].position),
      TPE_vec3Minus(carBody->joints[3].position,carBody->joints[1].position)));

    carSide = TPE_vec3Normalized(TPE_vec3Plus(
      TPE_vec3Minus(carBody->joints[1].position,carBody->joints[0].position),
      TPE_vec3Minus(carBody->joints[3].position,carBody->joints[2].position)));

    carUp = TPE_vec3Cross(carForw,carSide);

    /* now we'll check each joint separately and if it is touching the ground,
       we apply directional friction (friction that's dependent on the direction
       of the wheel which e.g. allows turning); TPE has only non-directional
       friction programmed in so we do it ourselves */
    for (int i = 0; i < 4; ++i)
      if (jointCollisions & (0x01 << i)) // wheel touches the ground?
      {
        TPE_Vec3 jv = TPE_vec3( // joint velocity 
            carBody->joints[i].velocity[0],   
            carBody->joints[i].velocity[1],   
            carBody->joints[i].velocity[2]);   

        TPE_Vec3 ja = carSide; // wheel axis of rotation

        if (i >= 2 && steering)
        {
          // for front wheels with turning we tilt the wheel axis 45 degrees

          if (steering == 2)
            ja = TPE_vec3Plus(TPE_vec3Times(carForw,TURN_RATE),carSide);
          else
            ja = TPE_vec3Minus(TPE_vec3Times(carForw,TURN_RATE),carSide);

          ja = TPE_vec3Normalized(ja);
        }

        /* friction is in the direction if the axis and its magnitude is
          determined by the dot product (angle) of the axis and velocity */
        TPE_Vec3 fric = TPE_vec3Times(ja,(TPE_vec3Dot(ja,jv) * TURN_FRICTION)
          / TPE_F);

        jv = TPE_vec3Minus(jv,fric); // subtract the friction

        carBody->joints[i].velocity[0] = jv.x;
        carBody->joints[i].velocity[1] = jv.y;
        carBody->joints[i].velocity[2] = jv.z;
      }
  
    if (TPE_vec3Dot(carUp,TPE_vec3Minus(carBody->joints[4].position,
      carBody->joints[0].position)) < 0)
    {
      /* if the car falls on its roof the center joint may flip to the other
         side, here we fix it */

      puts("car geometry flipped over, fixing...");

      carBody->joints[4].position = TPE_vec3Plus(TPE_vec3Times(carUp,300),
        carBody->joints[0].position);
    }

    for (int i = 0; i < tpe_world.bodyCount; ++i)
      TPE_bodyApplyGravity(&tpe_world.bodies[i],5);

    if ((jointCollisions | jointCollisionsPrev) & 0x03) // back wheels on ground?
    {
      if (sdl_keyboard[SDL_SCANCODE_UP])
      {
        carBody->joints[0].velocity[0] += (carForw.x * ACCELERATION) / TPE_F;
        carBody->joints[0].velocity[1] += (carForw.y * ACCELERATION) / TPE_F;
        carBody->joints[0].velocity[2] += (carForw.z * ACCELERATION) / TPE_F;
        carBody->joints[1].velocity[0] += (carForw.x * ACCELERATION) / TPE_F;
        carBody->joints[1].velocity[1] += (carForw.y * ACCELERATION) / TPE_F;
        carBody->joints[1].velocity[2] += (carForw.z * ACCELERATION) / TPE_F;
      }
      else if (sdl_keyboard[SDL_SCANCODE_DOWN])
      {
        carBody->joints[0].velocity[0] -= (carForw.x * ACCELERATION) / TPE_F;
        carBody->joints[0].velocity[1] -= (carForw.y * ACCELERATION) / TPE_F;
        carBody->joints[0].velocity[2] -= (carForw.z * ACCELERATION) / TPE_F;
        carBody->joints[1].velocity[0] -= (carForw.x * ACCELERATION) / TPE_F;
        carBody->joints[1].velocity[1] -= (carForw.y * ACCELERATION) / TPE_F;
        carBody->joints[1].velocity[2] -= (carForw.z * ACCELERATION) / TPE_F;
      }
    }

    jointCollisionsPrev = jointCollisions;

    carRot = TPE_bodyGetRotation(carBody,0,2,1);

    // draw:

    helper_set3DColor(20,150,150);

    helper_drawModel(&arenaModel,TPE_vec3(0,0,0),TPE_vec3(512 * 32,512 * 32,512 * 32), 
      TPE_vec3(0,0,0));

    helper_set3DColor(20,50,250);

    helper_draw3DBox(TPE_bodyGetCenterOfMass(&tpe_world.bodies[0]),
      TPE_vec3(1000,800,1000),TPE_bodyGetRotation(&tpe_world.bodies[0],0,2,1));

    S3L_zBufferClear();
  
    helper_set3DColor(200,200,200);

    helper_drawModel(&carModel,TPE_vec3Minus(carPos,TPE_vec3Times(carUp,400)),
      TPE_vec3(600,600,600),carRot);

    if (helper_debugDrawOn)
      helper_debugDraw(1);

    // handle camera:

    s3l_scene.camera.transform.translation.y = carPos.y + 800;

    TPE_Vec3 cPos = TPE_vec3KeepWithinDistanceBand(
      TPE_vec3(
        s3l_scene.camera.transform.translation.x,
        s3l_scene.camera.transform.translation.y,
        s3l_scene.camera.transform.translation.z
      ),carBody->joints[4].position,4 * TPE_F,6 * TPE_F);

    s3l_scene.camera.transform.translation.x = cPos.x;
    s3l_scene.camera.transform.translation.y = cPos.y;
    s3l_scene.camera.transform.translation.z = cPos.z;

    S3L_Vec4 toCar;

    toCar.x = carPos.x - s3l_scene.camera.transform.translation.x;
    toCar.y = carPos.y - s3l_scene.camera.transform.translation.y;
    toCar.z = carPos.z - s3l_scene.camera.transform.translation.z;
    toCar.w = 0;

    TPE_Unit angleDiff = s3l_scene.camera.transform.rotation.y -
      (TPE_vec2Angle(toCar.x,toCar.z) - 128);

    s3l_scene.camera.transform.rotation.y -= 
      (angleDiff < 100 && angleDiff > -100) ? angleDiff / 2 : angleDiff;

    helper_frameEnd();
  }

  helper_end();

  return 0;
}
