import json
import os
import sys
import bpy
import bpy.ops
from idprop.types import IDPropertyGroup

argv = sys.argv
argv = argv[argv.index("--") + 1:]  # get all args after "--"
outfile = argv[0]

# apply modifiers in "export" collection
# collection_name = "export"
# target_collection = bpy.data.collections.get(collection_name)
# if target_collection:
#     for obj in target_collection.all_objects:
#         if obj.type == 'MESH':
#             bpy.context.view_layer.objects.active = obj
#             obj.select_set(True)
#             if bpy.context.object.mode != 'OBJECT':
#                 bpy.ops.object.mode_set(mode='OBJECT')
#             for mod in obj.modifiers:
#                 bpy.ops.object.modifier_apply(modifier=mod.name)
#             obj.select_set(False)
            

def collect_splat_data_from_object(obj):
    color_inputs = ["red", "green", "blue", "alpha"]
    output_data = {}

    if not obj.data or not hasattr(obj.data, "materials"):
        return output_data

    for mat in obj.data.materials:
        if not mat or not mat.use_nodes:
            continue

        for node in mat.node_tree.nodes:
            if node.type == 'GROUP' and "_Splat_" in node.label:
                label = node.label
                splatColorInput = node.inputs["SplatColor"]
                label = splatColorInput.links[0].from_node.image.name
                
                output_data[label] = {}
                for input_name in color_inputs:
                    inp = next(
                        (s for s in node.inputs if s.name.lower() == input_name),
                        None
                    )

                    image_name = None
                    if inp and inp.links:
                        frm = inp.links[0].from_node
                        if frm.type == 'TEX_IMAGE' and frm.image:
                            image_name = frm.image.name

                    output_data[label][input_name] = image_name

    return output_data

export_collection = bpy.data.collections.get("export")
if not export_collection:
    print("Export collection not found.")
    exit(1)

terrain_objects = [
    obj for obj in export_collection.objects
    if "terrain" in obj.name.lower()
]

if not terrain_objects:
    print("No terrain objects found.")
    exit(1)

for terrain in terrain_objects:
    splat_data = collect_splat_data_from_object(terrain)
    if splat_data:
        terrain["splatInfo"] = splat_data


for img in list(bpy.data.images):
    if "splat" not in img.name.lower():
        bpy.data.images.remove(img)



bpy.ops.export_scene.gltf(filepath=outfile,
                            export_format="GLB",
                            export_yup=True,
                            export_extras=True,
                            export_normals=True,
                            export_texcoords=False,
                            export_tangents=False,
                            export_materials='EXPORT',
                            export_unused_images=True,
                            use_active_scene=True,
                            export_apply=False,
                            collection="export")

                            # export_active_vertex_color_when_no_material=True,
                            # export_all_vertex_colors=True,
                            # export_keep_originals=True,
                            # export_image_format="NONE",