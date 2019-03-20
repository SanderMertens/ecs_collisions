#include <include/ecs_collisions.h>

void SetColor(EcsRows *rows) {
    EcsCollision2D *collision = ecs_column(rows, EcsCollision2D, 1);
    EcsType TEcsColor = ecs_column_type(rows, 2);

    for (int i = 0; i < rows->count; i ++) {
        /* Set or override (if collided for the first time) the entity color */
        ecs_set(rows->world, collision[i].entity_1, EcsColor, {255, 50, 100, 255});
        ecs_set(rows->world, collision[i].entity_2, EcsColor, {255, 50, 100, 255});
    }
}

void ResetColor(EcsRows *rows) {
    /* Little trick to ensure the color is only set if entity owns EcsColor:
     * ecs_column_test returns NULL if the column is a shared component. */
    EcsColor *color = ecs_column_test(rows, EcsColor, 1);

    for (int i = 0; color && i < rows->count; i ++) {
        color[i] = (EcsColor){color[i].r * 0.97, 50, 100, 255}; /* Gradually fade to blue */
    }
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    ECS_IMPORT(world, EcsComponentsGraphics, ECS_2D);  /* EcsCanvas2D, EcsColor */
    ECS_IMPORT(world, EcsComponentsTransform, ECS_2D); /* EcsPosition2D */
    ECS_IMPORT(world, EcsComponentsPhysics, ECS_2D);   /* EcsAngularSpeed */
    ECS_IMPORT(world, EcsComponentsGeometry, ECS_2D);  /* EcsCircle, EcsSquare, EcsRectangle */
    ECS_IMPORT(world, EcsSystemsPhysics, ECS_2D);      /* Collision detection, movement */
    ECS_IMPORT(world, EcsSystemsSdl2, ECS_2D);         /* Rendering */

    /* Define prefabs for collidable circles and squares with a color */
    ECS_PREFAB(world, Shape, EcsColor, EcsCollider);
    ECS_PREFAB(world, Circle, Shape, EcsCircle);
    ECS_PREFAB(world, Square, Shape, EcsSquare);
    ecs_set(world, Shape, EcsColor, {0, 50, 100, 255}); /* Initially shapes all share the same color */
    ecs_set(world, Circle, EcsCircle, {.radius = 24});
    ecs_set(world, Square, EcsSquare, {.size = 24});

    /* Systems that implement logic for coloring entities */
    ECS_SYSTEM(world, ResetColor, EcsOnFrame, EcsColor);
    ECS_SYSTEM(world, SetColor, EcsOnSet, EcsCollision2D, ID.EcsColor);

    /* Create canvas (used by SDL to create window) */
    ecs_set(world, 0, EcsCanvas2D, { .window = {.width = 800, .height = 600} });

    /* Create rotating rectangle and array of shapes it will collide with */
    EcsEntity e = ecs_set(world, 0, EcsPosition2D, {0, 0});
    ecs_set(world, e, EcsRectangle, {.width = 250, .height = 12});
    ecs_set(world, e, EcsAngularSpeed, {.value = 1.5});
    ecs_add(world, e, EcsCollider);

    EcsPosition2D pos[] = {{-100, 100}, {100, 100}, {-100, -100}, {100, -100},
       {-50, -100}, {0, -100}, {50, -100}, {-50, 100}, {0, 100}, {50, 100},
       {-100, -50}, {-100, 0}, {-100, 50}, {100, -50}, {100, 0}, {100, 50}};

    for (int i = 0; i < 16; i ++) {
        e = _ecs_new(world, i < 4 ? TCircle : TSquare);
        ecs_set(world, e, EcsPosition2D, {pos[i].x, pos[i].y});
    }

    /* Main loop */
    ecs_set_target_fps(world, 60);
    while (ecs_progress(world, 0));

    return ecs_fini(world);
}
