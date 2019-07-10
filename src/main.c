#include <ecs_collisions.h>

void SetColor(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsCollision2D, collision, 1);
    ECS_COLUMN_COMPONENT(rows, EcsColor, 2);

    for (int i = 0; i < rows->count; i ++) {
        /* Set or override (if collided for the first time) the entity color */
        ecs_set(rows->world, collision[i].entity_1, EcsColor, {255, 50, 100, 255});
        ecs_set(rows->world, collision[i].entity_2, EcsColor, {255, 50, 100, 255});
    }
}

void ResetColor(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsColor, color, 1);

    if (color && !ecs_is_shared(rows, 1)) {
        for (int i = 0; color && i < rows->count; i ++) {
            color[i] = (EcsColor){color[i].r * 0.97, 50, 100, 255}; /* Fade to blue */
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsComponentsGraphics, ECS_2D);  /* EcsCanvas2D, EcsColor */
    ECS_IMPORT(world, FlecsComponentsTransform, ECS_2D); /* EcsPosition2D */
    ECS_IMPORT(world, FlecsComponentsPhysics, ECS_2D);   /* EcsAngularSpeed */
    ECS_IMPORT(world, FlecsComponentsGeometry, ECS_2D);  /* EcsCircle, EcsSquare, EcsRectangle */
    ECS_IMPORT(world, FlecsSystemsPhysics, ECS_2D);      /* Collision detection, movement */
    ECS_IMPORT(world, FlecsSystemsSdl2, ECS_2D);         /* Rendering */

    /* Define prefabs for collidable circles and squares with a color */
    ECS_PREFAB(world, Shape, EcsColor, EcsCollider);
    ECS_PREFAB(world, Circle, INSTANCEOF | Shape, EcsCircle);
    ECS_PREFAB(world, Square, INSTANCEOF | Shape, EcsSquare);
    ecs_set(world, Shape, EcsColor, {0, 50, 100, 255}); /* Initially shapes all share the same color */
    ecs_set(world, Circle, EcsCircle, {.radius = 24});
    ecs_set(world, Square, EcsSquare, {.size = 24});

    /* Systems that implement logic for coloring entities */
    ECS_SYSTEM(world, ResetColor, EcsOnUpdate, EcsColor);
    ECS_SYSTEM(world, SetColor, EcsOnSet, EcsCollision2D, .EcsColor);

    /* Create canvas (used by SDL to create window) */
    ecs_set(world, 0, EcsCanvas2D, { .window = {.width = 800, .height = 600} });

    /* Create rotating rectangle and array of shapes it will collide with */
    ecs_entity_t e = ecs_set(world, 0, EcsPosition2D, {0, 0});
    ecs_set(world, e, EcsRectangle, {.width = 250, .height = 12});
    ecs_set(world, e, EcsAngularSpeed, {.value = 1.5});
    ecs_add(world, e, EcsCollider);

    EcsPosition2D pos[] = {{-100, 100}, {100, 100}, {-100, -100}, {100, -100},
       {-50, -100}, {0, -100}, {50, -100}, {-50, 100}, {0, 100}, {50, 100},
       {-100, -50}, {-100, 0}, {-100, 50}, {100, -50}, {100, 0}, {100, 50}};

    for (int i = 0; i < 16; i ++) {
        e = _ecs_new_instance(world, i < 4 ? Circle : Square, 0);
        ecs_set(world, e, EcsPosition2D, {pos[i].x, pos[i].y});
    }

    /* Main loop */
    ecs_set_target_fps(world, 60);

    while (ecs_progress(world, 0)) { }

    return ecs_fini(world);
}
