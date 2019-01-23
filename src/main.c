#include <include/ecs_collisions.h>

void Collision(EcsRows *rows) {
    EcsEntity EcsColor_h = ecs_component(rows, 1);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsCollision2D *collision = ecs_data(rows, row, 0);
        ecs_set(rows->world, collision->entity_1, EcsColor, {255, 0, 0, 255});
        ecs_set(rows->world, collision->entity_2, EcsColor, {255, 0, 0, 255});
    }
}

void ResetColor(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        *(EcsColor*)ecs_data(rows, row, 0) = (EcsColor){255, 255, 255, 255};
    }
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    /* Import modules */
    ECS_IMPORT(world, EcsComponentsGraphics, ECS_2D);
    ECS_IMPORT(world, EcsComponentsTransform, ECS_2D);
    ECS_IMPORT(world, EcsComponentsPhysics, ECS_2D);
    ECS_IMPORT(world, EcsComponentsGeometry, ECS_2D);
    ECS_IMPORT(world, EcsSystemsPhysics, ECS_2D);
    ECS_IMPORT(world, EcsSystemsSdl2, ECS_2D);

    /* Define prefab for circle and square shapes */
    ECS_PREFAB(world, Circle, EcsCircle, EcsCollider);
    ECS_PREFAB(world, Square, EcsSquare, EcsCollider);
    ecs_set(world, Circle_h, EcsCircle, {.radius = 12});
    ecs_set(world, Square_h, EcsSquare, {.size = 24});

    /* Systems that set color based on collision */
    ECS_SYSTEM(world, ResetColor, EcsOnFrame, EcsColor);
    ECS_SYSTEM(world, Collision, EcsOnFrame, EcsCollision2D, ID.EcsColor);

    /* Drawing canvas */
    EcsEntity e, canvas = ecs_set(world, 0, EcsCanvas2D, {
        .window = {.width = 800, .height = 600}
    });

    /* Create shapes */
    e = ecs_new(world, canvas);
    ecs_set(world, e, EcsPosition2D, {0, 0});
    ecs_set(world, e, EcsRectangle, {.width = 272, .height = 12});
    ecs_set(world, e, EcsAngularSpeed, {.value = 1.0});
    ecs_add(world, e, EcsCollider_h);

    EcsPosition2D cpos[] = {{-100, 100}, {100, 100}, {-100, -100}, {100, -100}};
    EcsPosition2D spos[] = {{0, 100}, {0, -100}, {-100, 0}, {100, 0}};
    int i;
    for (i = 0; i < 4; i ++) {
        e = ecs_new(world, Circle_h);
        ecs_set(world, e, EcsPosition2D, {cpos[i].x, cpos[i].y});
        ecs_add(world, e, canvas);

        e = ecs_new(world, Square_h);
        ecs_set(world, e, EcsPosition2D, {spos[i].x, spos[i].y});
        ecs_add(world, e, canvas);
    }

    /* Main loop */
    ecs_set_target_fps(world, 60);
    while (ecs_progress(world, 0));
    return ecs_fini(world);
}
