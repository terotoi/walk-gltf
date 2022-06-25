/*
 Copyright (c) 2022 Tero Oinas

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#ifndef _CST_LIB_SG_NODE_H
#define _CST_LIB_SG_NODE_H

#include "core/lockable.h"
#include "math/mat4.h"
#include "mesh.h"

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace cst {

class Node;
typedef std::shared_ptr<Node> node_ptr;

class Renderer;

/**
 * Node is a scene graph node.
 */
class Node : public Lockable {
public:
  Node(std::string const &name = "") : name(name) {}
  Node(mesh_ptr mesh, std::string const &name = "");
  Node(std::vector<mesh_ptr> const &meshes, std::string const &name = "")
      : name(name), meshes(meshes) {}
  Node(node_ptr node);
  virtual ~Node() {}

  std::string const &getName() const { return name; }

  // Returns the visible flag of this node. Non-visible objects
  // exist in the scene graph but are not drawn.
  bool isVisible() const { return visible; }

  // Set the visible flag of this node.
  void setVisible(bool vis) { visible = vis; }

  // Returns the local transform of this node. Global transform
  // of this node is the product of its local transform and the
  // global transform of its parent.
  mat4 const &getLocalTransform() const { return local; }

  // Sets the local transform. updateGlobalTransform() must be
  // called before this takes into effect.
  void setLocalTransform(mat4 const &m) { local = m; }

  // Global transform is the effective world-space object transform.
  mat4 const &getGlobalTransform() const { return global; }

  // Subclass might override to copy to UBOs etc.
  virtual void updateGlobalTransform(mat4 const &p);

  // Sets the translation part of the local transform.
  vec3 getLocalPosition() const {
    return vec3(local.m[12], local.m[13], local.m[14]);
  }

  mat4 getLocalRotation() const { return local.getRotation(); }

  // Sets the translation part of the local transform.
  void setLocalPosition(vec3 const &p) {
    local.m[12] = p.d[0];
    local.m[13] = p.d[1];
    local.m[14] = p.d[2];
  }

  // Returns the position in global space.
  vec3 getGlobalPosition() const {
    return vec3(global.m[12], global.m[13], global.m[14]);
  }

  // Returns the rotation part of the global transform as mat4.
  // The translation part is set to zero.
  mat4 getGlobalRotation() const { return global.getRotation(); }

  // Move to node by local transform * rel.
  void move(vec3 const &rel, bool limitY = false);

  // Returns true if this node is staged.
  virtual bool isStaged() const { return false; }

  // Returns true if this node is a visual node that can be staged.
  virtual bool isVisual() const { return true; }

  // Returns the bounding box of this node. It is cached and recalculated when
  // needed. The box is in local space.
  AABB const &getAABB() const;

  // Returns true if this node is culled by the given projection * view matrix.
  bool isCulled(mat4 const &projView) const;

  // Add a child node. Returns the added node.
  node_ptr addChild(node_ptr child);

  // Finds a child node by name recursively. Does not compare itself.
  node_ptr find(std::string const &name) const;

  // Returns the number of meshes.
  size_t numMeshes() const { return meshes.size(); }

  // Iterates all meshes of this node, calling f on each.
  // Does not recurse into children.
  void forMeshes(std::function<void(mesh_ptr)> const &f) const;

  // Calls the function for every child of this node.
  void forEach(std::function<void(node_ptr)> f, bool recursive);

  // Maps a function to every child of this node recursively.
  // The children are replaced in place with the results of the function.
  void mapChildren(std::function<node_ptr(node_ptr)> f);

  // Maps a function to every mesh of this node.
  // The children are replaced in place with the results of the function.
  void mapMeshes(std::function<mesh_ptr(mesh_ptr)> f);

  // Collects all staged nodes into a vector.
  void collectStaged(std::vector<node_ptr> *nodes);

  // Updates the state of this mesh to the renderer.
  virtual void update(Renderer *renderer) {}

private:
  // Calculates AABB of this node and its children and stores it into aaBB
  // variable.
  void calcAABB() const;

  std::string name;
  bool visible = true;
  std::vector<mesh_ptr> meshes;
  mat4 local;
  mat4 global;
  mutable AABB aaBB;
  mutable bool aabbIsUpToDate = false;
  std::vector<node_ptr> children;
};

} // namespace cst

#endif // _CST_LIB_SG_NODE_H
