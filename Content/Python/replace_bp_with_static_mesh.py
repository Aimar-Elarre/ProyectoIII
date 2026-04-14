"""
Replace Blueprint actors (containing a single Static Mesh) with StaticMeshActors.

How to run
----------
1. Open the level you want to process in Unreal Editor.
2. Go to  Tools > Execute Python Script  (or use the Output Log Python console).
3. Browse to  Content/Python/replace_bp_with_static_mesh.py  and execute it.

What it does
------------
- Iterates all actors in the currently open level.
- Finds Blueprint actors (class name ends with "_C") whose root is a plain
  SceneComponent (DefaultSceneRoot) and that have exactly ONE child
  StaticMeshComponent with a valid mesh assigned.
- Spawns a new StaticMeshActor at the child mesh's world transform.
- Copies mesh, material overrides, mobility, actor label, and folder path.
- Deletes the original Blueprint actor.
- Prints a summary to the Output Log.

NOTE: Save your level before running!  The operation is wrapped in a single
undo transaction so you can Ctrl-Z if something goes wrong.
"""

import unreal


def get_static_mesh_components(actor):
    """Return StaticMeshComponents that have a mesh assigned."""
    components = actor.get_components_by_class(unreal.StaticMeshComponent)
    return [c for c in components if c.static_mesh is not None]


def is_child_mesh_blueprint(actor):
    """
    Return True if the actor matches the artist pattern:
      - Root component is a plain SceneComponent (DefaultSceneRoot), NOT a mesh.
      - Exactly one StaticMeshComponent exists as a child with a mesh assigned.
    """
    root = actor.root_component
    if root is None:
        return False

    # Root must be a plain SceneComponent, not a StaticMeshComponent itself
    if isinstance(root, unreal.StaticMeshComponent):
        return False

    sm_components = get_static_mesh_components(actor)
    return len(sm_components) == 1


def replace_blueprints_with_static_mesh_actors():
    editor_actor_sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    world = unreal.EditorLevelLibrary.get_editor_world()
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    # Collect candidates first (avoid mutating the list while iterating)
    candidates = []
    skipped = []

    for actor in all_actors:
        # Already a StaticMeshActor — nothing to do
        if isinstance(actor, unreal.StaticMeshActor):
            continue

        # Only process Blueprint-generated classes (their name always ends with "_C")
        if not actor.get_class().get_name().endswith("_C"):
            continue

        sm_components = get_static_mesh_components(actor)

        if len(sm_components) == 0:
            continue  # no mesh — probably a logic-only BP, skip silently

        if len(sm_components) > 1:
            skipped.append(
                f"  SKIPPED '{actor.get_actor_label()}' — "
                f"has {len(sm_components)} static-mesh components"
            )
            continue

        # Must match the child-mesh pattern: root is a plain SceneComponent
        root = actor.root_component
        if root is None:
            continue
        if isinstance(root, unreal.StaticMeshComponent):
            skipped.append(
                f"  SKIPPED '{actor.get_actor_label()}' — "
                f"StaticMesh is the root component, not a child"
            )
            continue

        candidates.append((actor, sm_components[0]))

    if not candidates:
        unreal.log("No Blueprint actors with a single StaticMesh found. Nothing to do.")
        if skipped:
            for msg in skipped:
                unreal.log(msg)
        return

    replaced_count = 0

    with unreal.ScopedEditorTransaction("Replace BPs with StaticMeshActors"):
        for actor, source_comp in candidates:
            source_mesh = source_comp.static_mesh
            actor_label = actor.get_actor_label()

            # Get the component's world-space transform (includes actor + relative offset)
            world_transform = source_comp.get_world_transform()
            location = world_transform.translation
            rotation = world_transform.rotation.rotator()
            scale = world_transform.scale3d

            # Collect material overrides
            num_materials = source_comp.get_num_materials()
            materials = [source_comp.get_material(i) for i in range(num_materials)]

            # Collect mobility
            mobility = source_comp.mobility

            # Collect folder path
            actor_folder = actor.get_folder_path()

            # Spawn the replacement
            new_actor = editor_actor_sub.spawn_actor_from_class(
                unreal.StaticMeshActor,
                location,
                rotation,
            )

            if new_actor is None:
                unreal.log_warning(
                    f"Could not spawn StaticMeshActor for '{actor_label}' — skipping."
                )
                continue

            # Apply scale (spawn_actor_from_class does not accept scale)
            new_actor.set_actor_scale3d(scale)

            # Set mesh
            new_smc = new_actor.static_mesh_component
            new_smc.set_static_mesh(source_mesh)

            # Apply material overrides
            for i, mat in enumerate(materials):
                if mat is not None:
                    new_smc.set_material(i, mat)

            # Match mobility (Static / Stationary / Movable)
            new_smc.set_mobility(mobility)

            # Preserve label and folder organisation
            new_actor.set_actor_label(actor_label)
            if actor_folder and str(actor_folder) not in ("", "None"):
                new_actor.set_folder_path(actor_folder)

            # Remove the original Blueprint actor
            editor_actor_sub.destroy_actor(actor)

            replaced_count += 1
            unreal.log(
                f"Replaced '{actor_label}' -> StaticMeshActor "
                f"(mesh: {source_mesh.get_name()})"
            )

    # ---- Summary ----
    unreal.log("=" * 60)
    unreal.log(f"Done.  Replaced {replaced_count} Blueprint actor(s).")
    if skipped:
        unreal.log(f"Skipped {len(skipped)} actor(s) with multiple meshes:")
        for msg in skipped:
            unreal.log(msg)
    unreal.log("=" * 60)


# Execute
replace_blueprints_with_static_mesh_actors()
