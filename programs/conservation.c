/** Example that fakes the momentum/energy conservation. We simply keep the
  record of total speed in the system (as an approximation of
  momentum/energy/whatever) and if it changes too much, we multiply them
  accordingly to get them back :) */

#include "helper.h"

#define SPHERE_R (7 * TPE_F)

TPE_Vec3 environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  TPE_ENV_START( TPE_envSphereInside(p,TPE_vec3(0,0,0),SPHERE_R),p )
  TPE_ENV_END
}

int main(void)
{
  helper_init();

  tpe_world.environmentFunction = environmentDistance;

  s3l_scene.camera.transform.translation.z = -1 * SPHERE_R - 2 * TPE_F;

  for (int i = 0; i < 4; ++i) // add bodies
  {
    switch (i)
    {
      case 0: helper_addBox(5 * TPE_F / 4,5 * TPE_F / 4,5 * TPE_F / 4,TPE_F,2 * TPE_F); break;
      case 1: helper_addBall(TPE_F,TPE_F / 4); break;
      case 2: helper_addRect(5 * TPE_F / 4,5 * TPE_F / 4,TPE_F,2 * TPE_F); break;
      case 3: helper_add2Line(2 * TPE_F,TPE_F / 2,TPE_F); break;
      default: break;
    }

    TPE_bodyMoveBy(&helper_lastBody,TPE_vec3((i - 2) * TPE_F * 2,-1 * TPE_F * i,TPE_F / 2));

    /* We don't want any energy losses due to friction and non-elastic collision,
    so we'll turn them off. This alone won't be enough though as numeric errors
    will still change the total energy. */

    helper_lastBody.friction = 0;
    helper_lastBody.elasticity = TPE_FRACTIONS_PER_UNIT;
    helper_lastBody.flags |= TPE_BODY_FLAG_ALWAYS_ACTIVE;

    TPE_bodyAccelerate(&helper_lastBody, // give some initial velocity
      TPE_vec3Plus(TPE_vec3Times(TPE_bodyGetCenterOfMass(&helper_lastBody),TPE_F / 10),TPE_vec3(0,TPE_F / 100,0)));
  }

  TPE_Unit sTotal = TPE_worldGetNetSpeed(&tpe_world);
  // ^ keep the record of total speed

  while (helper_running)
  {
    helper_frameStart();

    helper_cameraFreeMovement();

    TPE_worldStep(&tpe_world);

    TPE_Unit s = TPE_worldGetNetSpeed(&tpe_world);
    TPE_Unit ratio = (sTotal * TPE_FRACTIONS_PER_UNIT) / TPE_nonZero(s);

    if (ratio < (4 * TPE_FRACTIONS_PER_UNIT) / 5 ||
      ratio > (6 * TPE_FRACTIONS_PER_UNIT) / 5)
    {
      // if total speed changed by more than 1/5, we adjust it

      printf("net speed is now %d but needs to be %d, correcting!\n",s,sTotal);

      for (int i = 0; i < tpe_world.bodyCount; ++i)
        TPE_bodyMultiplyNetSpeed(&tpe_world.bodies[i],ratio);
    }

    helper_set3DColor(200,10,10); 

    for (int i = 0; i < tpe_world.bodyCount; ++i) // draw the bodies
    {
      TPE_Joint *joints = tpe_world.bodies[i].joints;
      TPE_Vec3 pos = TPE_bodyGetCenterOfMass(&tpe_world.bodies[i]);
      TPE_Vec3 right = TPE_vec3(TPE_F,0,0);
      TPE_Vec3 forw = TPE_vec3(0,0,TPE_F);

      if (i != 1) // ugly code to get the correct orientation :)
      {
        if (i != 3)
        {
          forw = TPE_vec3Minus(joints[2].position,joints[0].position);
          right = TPE_vec3Minus(joints[1].position,joints[0].position);
        }
        else
          forw = TPE_vec3Minus(joints[1].position,joints[0].position);
      }

      TPE_Vec3 orient = TPE_rotationFromVecs(forw,right);

      switch (i % 5) // and draw the correct shape
      {
        case 0: helper_draw3DBox(pos,TPE_vec3(1200,1200,1200),orient); break;
        case 1: helper_draw3DSphere(pos,TPE_vec3(500,500,500),orient); break;
        case 2: helper_draw3DBox(pos,TPE_vec3(1200,400,1200),orient); break; 
        case 3: helper_draw3DBox(pos,TPE_vec3(200,200,1200),orient); break;
        default: break;
      }
    }

    helper_set3DColor(200,200,200); 

    helper_draw3DSphereInside(TPE_vec3(0,0,0),
      TPE_vec3(SPHERE_R,SPHERE_R,SPHERE_R),TPE_vec3(0,0,0));

    if (helper_debugDrawOn)
      helper_debugDraw(1);

    helper_frameEnd();
  }

  helper_end();

  return 0;
}
