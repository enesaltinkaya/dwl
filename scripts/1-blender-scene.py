import json
import os
import sys
import bpy.ops
from idprop.types import IDPropertyGroup

argv = sys.argv
argv = argv[argv.index("--") + 1:]  # get all args after "--"
outfile = argv[0]


keyword = "export".lower()

# Collect all objects that should be removed
objs_to_remove = []

for obj in bpy.data.objects:
    # Check if the object is in ANY collection containing "export"
    in_export_collection = any(
        keyword in col.name.lower()
        for col in obj.users_collection
    )

    # If not in any export collection → mark for deletion
    if not in_export_collection:
        objs_to_remove.append(obj)

# Remove objects
for obj in objs_to_remove:
    # Unlink from all collections first
    for col in obj.users_collection:
        col.objects.unlink(obj)

    # Remove object completely
    bpy.data.objects.remove(obj, do_unlink=True)



bpy.ops.export_scene.gltf(filepath=outfile,
                            export_format="GLB",
                            export_yup=True,
                            export_texcoords=True,
                            export_normals=True,
                            export_tangents=True,
                            export_materials='EXPORT',
                            export_unused_textures=False,
                            export_unused_images=False,
                            export_all_vertex_colors=False,
                            export_attributes=False,
                            use_visible=True,
                            use_renderable=True,
                            use_active_scene=True,
                            export_apply=True,
                            export_extras=True,
                            export_animations=True,
                            export_force_sampling=False,
                            export_animation_mode="ACTIONS",
                            export_optimize_animation_size=True,
                            export_optimize_animation_keep_anim_armature=True,
                            export_optimize_animation_keep_anim_object=True,
                            export_anim_slide_to_zero=True,
                            export_lights=True,
                            export_cameras=True,
                            collection="export")