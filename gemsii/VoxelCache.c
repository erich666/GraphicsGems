/*
/* When Spawning a Refraction Ray:
/* Mask = 0x01 << Spawning_ray_level;
/* path = path | Mask;               /* Turn on correct bit. */
/* trace( / refraction ray / );
/* path = path & ~Mask;
/* 
/* When Spawning Reflection Ray:
/* Mask = 0x01 << Spawning_ray_level;
/* path = path & ~Mask;              /* Turn off correct bit. */
/* trace( / reflection ray / );
/**/

typedef struct _stree {
    TRIANGLE_REC   *last_object;
    TRIANGLE_REC  **last_voxel;
    struct _stree  *refraction_ray;
    struct _stree  *reflection_ray;
} SHADOW_TREE;

float check_shadowing(ray, light, path, Spawning_ray_level)
RAY_REC   *ray;   /* ray from shading point to light source */
LIGHT_REC *light; /* the light source we are interested in */
int        path;  /* bit table describing current position in vision ray tree */
int        Spawning_ray_level; /* level of the ray spawning this shadow ray */
{
    unsigned int  Mask;
    SHADOW_TREE  *cache;
    int i, hit ;
    float shadow_percent;
    /* user needs to define cache, object, voxel structures */

    cache = light->cache_tree;
    Mask = 0x01;
    /* If the spawning ray's level is 0 (primary ray), then we */
    /* use the head of the cache_tree. */
    for (i = 0; i < Spawning_ray_level; ++i) {
        if (Mask & path) cache = cache->refraction_ray;
        else             cache = cache->reflection_ray;
        Mask = Mask << 1; /* Shift mask left 1 bit */
    }

    if (cache->last_object != NULL) {
        /* intersect_object() marks object as having been */
        /* intersected by this ray. */
        hit = intersect_object( ray, cache->last_object, &object);

        if (hit) {
            return(1.0); /* full shadowing */
        }
        cache->last_object = NULL; /* object was not hit */

        if (cache->last_voxel != NULL) { /* implied !hit */

            /* intersect_object_in_voxel_for_shadows() returns hit = TRUE */
            /* on first affirmed intersection with an opaque object. */
            /* It ignores transparent objects altogether. */
            hit = intersect_objects_in_voxel_for_shadows( ray,
                                         cache->last_voxel, &object);
            if (hit) {
                cache->last_object = object;
                return(1.0);
            }
            cache->last_voxel = NULL; /* voxel did not supply a hit */
        }
    }

    /* traverse_voxels_for_shadows() DOES intersect transparent objects and */
    /* sorts the intersections for proper attenuation of the light          */
    /* intensity. If multiple objects are hit, then one of the              */
    /* intersections must be transparent, and the object returned is the    */
    /* transparent one. Tracing of the shadow ray halts once the light      */
    /* source has been reached. */
    hit = traverse_voxels_for_shadows(ray, &object, &voxel, &shadow_percent);

    if (!hit) {
        cache->last_object = NULL;
        cache->last_voxel  = NULL;
        return(0.0); /* No shadowing was found. */
    }
    if (object->transparency_value > 0.0) {
        /* the object is transparent */
        cache->last_object = NULL;
        cache->last_voxel  = NULL;
    }
    else {
        /* The object was NOT transparent, cache the info. */
        cache->last_object = object;
        cache->last_voxel  = voxel;
    }
    return ( shadow_percent );
}

/*
- Andrew Pearce, Alias, someplace in Toronto - pearce@alias.com
*/
