/** Completely basic program showing an initialization of a body consisting of
a single joint and dropping it on the floor, then plotting the body vertical
position over time. */

#include "../tinyphysicsengine.h"
#include <stdio.h>

TPE_Vec3 environmentDistance(TPE_Vec3 point, TPE_Unit maxDistance)
{
  return TPE_envGround(point,0); // just an infinite flat plane
}

int main(void)
{
  TPE_Body body;
  TPE_World world;
  TPE_Joint joint;
  int frame = 0;

  joint = TPE_joint(TPE_vec3(0,TPE_F * 8,0),TPE_F);
  TPE_bodyInit(&body,&joint,1,0,0,2 * TPE_F);
  TPE_worldInit(&world,&body,1,environmentDistance);

  while (TPE_bodyIsActive(&body))
  {
    if (frame % 6 == 0) // print once in 6 frames
    {
      TPE_Unit height = TPE_bodyGetCenterOfMass(&body).y;

      for (int i = 0; i < (height * 4) / TPE_F; ++i)
        putchar(' ');

      puts("*");
    }

    TPE_bodyApplyGravity(&body,TPE_F / 100);
    TPE_worldStep(&world);
    frame++;
  }

  puts("body deactivated");

  return 0;
}
