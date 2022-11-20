/** Boring demo, shows how to accelerate environment functions with bounding
  volume checks. */

#include "helper.h"

#define ACCELERATE 1 // if you turn this off, FPS will drastically drop

TPE_Vec3 environmentDistance(TPE_Vec3 p, TPE_Unit maxD)
{
  /* this is our highly complex environment function that find the closest point
     against  a grid of shape chunks -- with acceleration we apply bounding
     volume checks to the big chunks and each smaller subchunk */

  TPE_ENV_START( TPE_envAABoxInside(p,TPE_vec3(0,0,0),TPE_vec3(30000,30000,30000)), p )

  for (int bigZ = -1; bigZ < 1; ++bigZ) // big chunks
    for (int bigY = -1; bigY < 1; ++bigY)
      for (int bigX = -1; bigX < 1; ++bigX)
      {
        TPE_Vec3 bigCenter =
        TPE_vec3(bigX * 20 * TPE_F,bigY * 20 * TPE_F,bigZ * 20 * TPE_F);

#if ACCELERATE
        // bouding volume check for the big chunks of environment
        if (TPE_ENV_BCUBE_TEST(p,maxD,bigCenter,20 * TPE_F))
#endif
          for (int smallZ = 0; smallZ < 2; ++smallZ) // smaller chunks
            for (int smallY = 0; smallY < 2; ++smallY)
              for (int smallX = 0; smallX < 2; ++smallX)
              {
                TPE_Vec3 smallCenter = TPE_vec3Plus(bigCenter,
                  TPE_vec3(smallX * 8 * TPE_F,smallY * 8 * TPE_F,smallZ * 8 * TPE_F));

#if ACCELERATE
                // bouding volume check for the smaller subchanks of environment
                if (TPE_ENV_BSPHERE_TEST(p,maxD,smallCenter,8 * TPE_F))
#endif
                {
                  TPE_ENV_NEXT( TPE_envBox(p,smallCenter,TPE_vec3(2 * TPE_F,4 * TPE_F / 3,4 * TPE_F / 3),
                    TPE_vec3(TPE_F / 100,TPE_F / 20,TPE_F / 15)), p );

                  TPE_ENV_NEXT( TPE_envCylinder(p,smallCenter,TPE_vec3(TPE_F * 4,TPE_F * 3,3 * TPE_F / 4),3 * TPE_F / 4), p);
                  TPE_ENV_NEXT( TPE_envSphere(p,TPE_vec3Minus(smallCenter,TPE_vec3(-3 * TPE_F / 4,TPE_F,0)), 2 * TPE_F  )  , p );
                }
              }
      }

  TPE_ENV_END
}

int main(void)
{
  helper_init();

  helper_debugDrawOn = 1;
  tpe_world.environmentFunction = environmentDistance;
  s3l_scene.camera.transform.translation.z -= TPE_F * 4;

  while (helper_running)
  {
    helper_frameStart();
    helper_cameraFreeMovement();
    TPE_worldStep(&tpe_world);

    if (helper_frame % 32 == 0)
      helper_printCPU();

    if (helper_debugDrawOn)
      helper_debugDraw(1);

    helper_frameEnd();
  }

  helper_end();

  return 0;
}
