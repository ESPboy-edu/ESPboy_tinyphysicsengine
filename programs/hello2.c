/** Simple demo showing 2 bodies thrown inside a room, the world is rendered
  as a simple ASCII side view. */

#include "../tinyphysicsengine.h"
#include <stdio.h>

#define ROOM_SIZE (20 * TPE_F)

TPE_Vec3 environmentDistance(TPE_Vec3 point, TPE_Unit maxDistance)
{
  // our environemnt: just a simple room
  return TPE_envAABoxInside(point,TPE_vec3(ROOM_SIZE / 2,ROOM_SIZE / 2,0),
    TPE_vec3(ROOM_SIZE,ROOM_SIZE,ROOM_SIZE));
}

// the following functions are just for ASCII drawing the world

#define SCREEN_W 32
#define SCREEN_H 16

char screen[SCREEN_W * SCREEN_H];

void clearScreen(void)
{
  for (int i = 0; i < SCREEN_W * SCREEN_H; ++i)
    screen[i] = (i < SCREEN_W || i >= SCREEN_W * (SCREEN_H - 1)) ? '-' :
      ((i % SCREEN_W) == 0 || (i % SCREEN_W) == SCREEN_W - 1 ? '|' : ' ');
}

void setPixel(int x, int y, char c)
{
  if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H)
    return;

  y = SCREEN_H - 1 - y;

  screen[y * SCREEN_W + x] = c;
}

void printScreen(void)
{
  for (int i = 0; i < 20; ++i)
    putchar('\n');

  for (int y = 0; y < SCREEN_H; ++y)
  {
    for (int x = 0; x < SCREEN_W; ++x)
      putchar(screen[y * SCREEN_W + x]);

    putchar('\n');
  }
}

int main(void)
{
  TPE_Body bodies[2];             // we'll have two bodies
  TPE_World world;

  TPE_Joint joints[32];           // joint buffer
  TPE_Connection connections[64]; // connection buffer

  /* we'll create the first body "by hand", just two joints (spheres) with one
     connection: */
  joints[0] = TPE_joint(TPE_vec3(3 * ROOM_SIZE / 4,ROOM_SIZE / 2,0),TPE_F);
  joints[1] = 
    TPE_joint(TPE_vec3(3 * ROOM_SIZE / 4 + TPE_F * 4,ROOM_SIZE / 2,0),TPE_F);

  connections[0].joint1 = 0;
  connections[0].joint2 = 1;

  TPE_bodyInit(&bodies[0],joints,2,connections,1,TPE_F);

  /* the other (a "box" approximated by spheres) will be made by the library
     function: */
  TPE_makeBox(joints + 2,connections + 1,2 * TPE_F, 2 * TPE_F, 2 * TPE_F,TPE_F);
  TPE_bodyInit(&bodies[1],joints + 2,8,connections + 1,16,TPE_F);
  TPE_bodyMoveTo(&bodies[1],TPE_vec3(ROOM_SIZE / 2,ROOM_SIZE / 2,0));

  TPE_worldInit(&world,bodies,2,environmentDistance);

  // give some initial velocities and spins to the bodies:

  TPE_bodyAccelerate(&world.bodies[0],TPE_vec3(-1 * TPE_F / 8,TPE_F / 3,0));
  TPE_bodySpin(&world.bodies[0],TPE_vec3(0,0,-1 * TPE_F / 25));
  TPE_bodyAccelerate(&world.bodies[1],TPE_vec3(-1 * TPE_F / 2,50,0));
  TPE_bodySpin(&world.bodies[1],TPE_vec3(0,0,TPE_F / 23));

#define FRAMES 200

  for (int i = 0; i <= FRAMES; ++i) // simulate 200 steps
  {
    if (i % 10 == 0) // draw the world every 10 frames
    {
      clearScreen();

      for (int j = 0; j < world.bodyCount; ++j)
      {
        // draw body joints:
        for (int k = 0; k < world.bodies[j].jointCount; ++k)
        {
          TPE_Vec3 pos = world.bodies[j].joints[k].position;

          setPixel((pos.x * SCREEN_W) / ROOM_SIZE,
            (pos.y * SCREEN_H) / ROOM_SIZE,'.');
        }

        // draw the body center:
        TPE_Vec3 pos = TPE_bodyGetCenterOfMass(&world.bodies[j]);
          
        setPixel((pos.x * SCREEN_W) / ROOM_SIZE,(pos.y * SCREEN_H) / ROOM_SIZE,
          'A' + j);
      }

      printScreen();
      printf("frame %d/%d\n",i,FRAMES);
      puts("press return to step");
      getchar();
    }

    TPE_worldStep(&world); // simulate next tick

    for (int j = 0; j < world.bodyCount; ++j)
      TPE_bodyApplyGravity(&world.bodies[j],TPE_F / 100);
  }

  return 0;
}
