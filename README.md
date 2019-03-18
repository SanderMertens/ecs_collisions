# ecs_collisions
This is an example that shows how to do collision detection with flecs ECS (https://github.com/SanderMertens/flecs). The example code itself doesn't do much besides creating the objects and running the main loop, while the heavy lifting is done by the `flecs.systems.physics` module (https://github.com/SanderMertens/flecs-systems-physics).

## Getting Started
Flecs uses the bake build system (https://github.com/SanderMertens/bake). To install bake on Linux and MacOS, do:

```
git clone https://github.com/SanderMertens/bake
make -C bake/build-$(uname)
bake/bake setup
```

Before you can run the demo, make sure to have SDL2 installed.

On MacOS:

```
brew install sdl2
```

On Debian/Ubuntu:

```
sudo apt-get install sdl2
```

To install and run the demo, do:

```
bake clone SanderMertens/ecs_collisions
bake run ecs_collisions
```

## Implementation
This example demonstrates basic usage of the flecs 2D physics engine. A number of circle, square and rectangle objects are created. The rectangle is given angular momentum which makes it spin. The spinning rectangle collides with the other entities, which triggers collision events.

For the physics engine to detect collisions, objects must have a collider component. Flecs physics supports two types of colliders, Circle and Polygon. In this example we do not manually create colliders, but instead let the physics framework derive the colliders from the geometry data. We can do this by adding the `EcsCollider` component. This component does not actually contain collider data, but rather tells the physics engine that a collider must be added to this entity.

Whenever the physics engine detects a collision, it will create a new entity with the `EcsCollision2D` component. This component contains details of the collision, like the collision vector, how much the entities have collided into each other, and which entities it concerns. To run logic when a collision is detected, an application can simply create a system that subscribes for the `EcsCollision2D` component.

In the example, the `SetColor` system subscribes for EcsCollision2D components, and colors collided entities red. The `ResetColor` system resets the color to white. Note how `ResetColor` is defined _before_ `SetColor`. This is important, as system declaration order determines in which order they will be executed.

The physics system will generate one collision entity for every two entities that collide. After the frame is processed, collision entities are automatically cleaned up. Therefore, an application has only one chance to catch the collision.

Note that the physics systems can be used in non-GUI applications, by not importing the SDL2 module.
