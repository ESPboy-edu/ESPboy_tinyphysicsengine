# tinyphysicsengine

![](tpe1.gif)![](tpe2.gif)![](tpe3.gif)![](tpe4.gif)![](tpe5.gif)![](tpe6.gif)![](tpe7.gif)

This is tinyphysicsengine (TPE), a small, completely public domain KISS/suckless, fixed point physically inaccurate pure C header only 3D physics engine (or rather a library) mostly for entertainment purposes that's supposed to run even on tiny computers such as embedded, even **bare metal**. It's written in the same style/philosophy as [small3dlib](https://codeberg.org/drummyfish/small3dlib), [raycastlib](https://codeberg.org/drummyfish/raycastlib) etc.

TPE is NOT a "robust framework" and it is **NOT physically accurate**; basic things follow physics equations but a lot of other things are empirical approximations, the main goal is to achieve SIMPLE behavior that LOOKS LIKE real world physics. TPE can be used to **fake** many things (even such as e.g. car physics) just as in computer graphics we fake things such as reflections because in games we simply don't really notice they're inaccurate. This approach has been chosen on purpose after trying and failing to create a traditional physically accurate engine which is archived in the *old* folder; I already had physically correct collision detections and responses of rigid bodies programmed but eventually failed on dealing with the complexity of handling imprecisions of fixed point in very low/high energy cases. At that point I started over with a completely new approach: just use soft bodies made of spheres connected with springs and fake what is possible to fake.

**The basic principles in short**: TPE uses soft body physics, bodies are modelled as **spheres connected by springs** but the springs can be made stiff so that the bodies behave almost like rigid bodies, so you can simulate (*fake*) both soft and rigid physics. Environment in which bodies are placed is modelled by **distance functions**, i.e. you can in theory create any environment as long as you can create a function that for any point in space returns the closest point to the environment (functions for basic and some more complex shapes are included in TPE).

**Why does this exist?** Because all other engines suck, they are either trivial or bloated, have licensing conditions, dependencies, require floating point unit, complex build systems, bad languages, PhD level math etcetc. TPE is a keep it simple engine for people who just want to add simple physics to their tiny game without being bothered by bullshit.

## features

- KISS, **suckless**, pure C99, header only
- **compatible with small3dlib**, easy integration (same data types, conventions, ...)
- **no dependencies**, not even standard library (except for stdint header)
- **no build system**
- **no dynamic allocation** (malloc), not using files etc.
- completely **public domain** free software, no legal worries and burdens, do whatever you want
- **no floating point**, only 32 bit integer (fixed point) math
- **nice performance** for smaller simulations, runs even on embedded devices such as Pokitto (32 kB RAM, 48 MHz CPU)
- **discrete collision detection** with **simple acceleration** by bounding volumes that use **no precomputation**
- **soft/stiff body** physics that can be used to also **fake rigid body** physics, built-in functions for constructing bodies
- bodies are **spheres connected by springs** with simple attributes (mass, stiffness, elasticity, friction, ...), can be **soft or stiff**
- environments are **distance functions** (can possibly be animated, i.e. dynamic, no precomputation is needed), support for any mathematically definable environment (even e.g. 3D fractals), predefined functions for:
  - **sphere, plane, line segment, ...**
  - **box** (axis-aligned and arbitrary rotation)
  - **cylinder** and **cone** (arbitrary rotation)
  - **heightmap**
  - trivial **unions of shapes**
  - axis-aligned **triangular prism** (ramp)
  - **simple bounding sphere/box acceleration**
- functions for **rotations**, mostly in Euler angles (no quaternions)
- **ray casting** support (both against bodies and environments)
- **simple deactivation of bodies** that don't move much for a while
- **debug render**, a function that renders the 3D view of the world with a provided pixel drawing function (independent of any rendering system)
- **optional non-rotating bodies**, e.g. for representing the player
- **faking ball rotation** (to make single joint bodies look as if they rotate even though internally they don't)
- built-in **vector math**, trigonometric functions, gravity etc.
- **compile time options** to tune in specific parameters (such as body deactivation time or use distance approximation for better performance)
- **world hash**
- may in theory also be used for 2D physics in a limited way
- simple, well commented code and examples, just a simple math, you can easily make any changes if you want

You can probably use this to:

- make simple to mid-complex games and entertainment visualizations such as:
  - a first person game such as a shooter or adventure
  - maybe a simple racing game
  - marble racing?
  - probably even a simple 2D game
  - camera collisions in 3D visualizations
  - etc.
- spark your game with effects like flying objects after explosions, waving water surface, maybe even a super simple cloth and ragdoll physics
- impress girls by showing them 3D physics running on embedded devices
- etc.

## limitations

- generally **NOT physically correct**, for God's sake do not use this for computation of your space rocket's trajectory!
- even though **performance** is good for simple simulations, it is not best possible and **won't scale** very much because the soft body model requires to make more computations and also takes more RAM (however the code is still a reasonably optimized C that should run alright)
- yes, bodies sometimes vibrate and do weird things
- **fixed point is not extremely precise** of course
- **NOT a robust framework** that solves everything for you, more of a library with a set of tools that will help you easily create simple physics, you may need to handle many things yourself
- the library **does NOT really consider correct energies** as bodies don't store information about their angular velocity etc., usually some approximation such as a sum of speed/velocities of all joints is used to e.g. determine if a body should be deactivated
- **do NOT create extreme situations** such as extremely big or extremely small bodies or extreme velocities (probably also positions far away from the origin), fixed point will overflow or lose precision, this is not handled at all
- **body joint and connection count should be kept low**, do not create bodies with dozens or even hundreds of joints, that will be very slow
- made for a time tick length corresponding to **about 30 FPS**, hugely different ticks (like 200 or 2) lengths may behave weird or be unusable, half and double should probably be workable, you can try to apply interpolation etc.
- there is **no neat UBER integration method**, only a "dirty" empirical algorithm that may fail in weird situations but is tested to behave nicely and stably in most common situations (only current joint positions are considered, connections of joints create basically just a constant acceleration after a threshold of their tension is passed, some basic friction and cancelling of forces is applied). Tricks are used to help stability, such as force-reshaping stiff bodies when their shape diverges too much from their ideal shape.
- quantities such as energy and momentum are generally **NOT conserved**, they shouldn't go up but will probably go down, however you can force conservation of a quantity if you want (see the examples)
- **bodies are only approximated with spheres** (so your 3D models won't collide EXACTLY at their bounds), other shapes (boxes, cylinders, ...) are usable only for the environment
- **deactivation of bodies is imperfect** and may sometimes lead to e.g. a body freezing mid air, you need to handle this yourself in case it shows to be a problem for you
- things such as stable contacts are not handled at all, you may encounter issues with stacking object etc., again handle this yourself (there are functions provided for smoothing our movement etc., these are usually enough to mostly eliminate shaking)
- **body parameters**, mostly elasticity, may sometimes not work as expected because they only define the parameters of joints, however added connections also add elasticity etc., keep this in mind
- the **environment distance functions are not signed** (no SDFs) so as to make them easier to build, i.e. inside the solid environment they don't provide any information about where to escape -- this is mostly okay but can sometimes cause trouble (a body stuck deep in environment won't get out by itself, rays casted from within a solid environment won't work accurately etc.)
- the library **doesn't try to be too smart**, it is up to you to decide if you e.g. need smoothing of movement and apply it yourself (however helper functions for this are provided)
- just simple discrete collision detection and resolution

## how to

For a basic use see the `hello.c` example program and `hello2.c` as the next, then take a look at the more complex ones. Also see the library file itself, it is highly commented and is supposed to serve as its own documentation.

Very basic sum up: TODO, coming soon :)

## usage rights

tl;dr: CC0 public domain, I hate intellectual property, do whatever you want

I (drummyfish) have made this project completely myself from scratch. Everything in this repository is available under CC0 1.0 (public domain, https://creativecommons.org/publicdomain/zero/1.0/) + a waiver of all other IP rights (including patents and trademarks).

This project is made out of love and to be truly helpful to everyone, not for any self interest. I want it to forever stay completely in the public domain, not owned by anyone.

This is not mandatory but please consider supporting free software and free culture by using free licenses and/or waivers.

If you'd like to support me or just read something about me and my projects, visit my site: www.tastyfish.cz.

You can also choose to use this under the following waiver which is here to just ensure more legal safety and waiving of additional IP such as patents:

The intent of this waiver is to ensure that this work will never be encumbered by any exclusive intellectual property rights and will always be in the public domain world-wide, i.e. not putting any restrictions on its use.

Each contributor to this work agrees that they waive any exclusive rights, including but not limited to copyright, patents, trademark, trade dress, industrial design, plant varieties and trade secrets, to any and all ideas, concepts, processes, discoveries, improvements and inventions conceived, discovered, made, designed, researched or developed by the contributor either solely or jointly with others, which relate to this work or result from this work. Should any waiver of such right be judged legally invalid or ineffective under applicable law, the contributor hereby grants to each affected person a royalty-free, non transferable, non sublicensable, non exclusive, irrevocable and unconditional license to this right.
