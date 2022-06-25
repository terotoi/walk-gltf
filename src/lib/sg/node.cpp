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
#include "node.h"

using namespace cst;

Node::Node(mesh_ptr mesh, std::string const &name) : name(name) {
  meshes.push_back(mesh);
}

Node::Node(node_ptr node)
    : name(node->name), meshes(node->meshes), local(node->local),
      global(node->global), children(node->children) {}

void Node::updateGlobalTransform(mat4 const &p) {
  std::scoped_lock lock(mutex());
  global = p * local;
  for (node_ptr &ch : children)
    ch->updateGlobalTransform(global);
}

void Node::move(vec3 const &rel, bool limitY) {
  mat4 const &rot = getLocalRotation();
  vec3 dir = rot * rel;
  if (limitY)
    dir.d[1] = 0.0f;
  setLocalPosition(getLocalPosition() + dir);
}

AABB const &Node::getAABB() const {
  if (!aabbIsUpToDate)
    calcAABB();
  return aaBB;
}

void Node::calcAABB() const {
  std::scoped_lock lock(mutex());
  AABB aabb;
  if (meshes.size() > 0) {
    for (auto const &m : meshes)
      aabb.extend(m->getAABB());
  }

  for (auto const &ch : children)
    aabb.extend(ch->getLocalTransform() * ch->getAABB());

  aaBB = aabb;
  aabbIsUpToDate = true;
}

bool Node::isCulled(mat4 const &projView) const {
  AABB const &aabb = getAABB() * getGlobalTransform();
  return !aabb.isVisible(projView);
}

node_ptr Node::addChild(node_ptr child) {
  children.push_back(child);
  return child;
}

node_ptr Node::find(std::string const &name) const {
  for (node_ptr const &ch : children) {
    if (ch->name == name)
      return ch;

    node_ptr r = ch->find(name);
    if (r != nullptr)
      return r;
  }

  return nullptr;
}

void Node::forEach(std::function<void(node_ptr)> f, bool recursive) {
  for (node_ptr &ch : children) {
    f(ch);
    if (recursive)
      ch->forEach(f, recursive);
  }
}

void Node::mapChildren(std::function<node_ptr(node_ptr)> f) {
  std::scoped_lock lock(mutex());

  for (node_ptr &ch : children) {
    // std::scoped_lock lock(ch->mutex());
    ch->mapChildren(f);
    ch = f(ch);
  }
}

void Node::forMeshes(std::function<void(mesh_ptr)> const &f) const {
  for (mesh_ptr const &m : meshes) {
    // std::scoped_lock lock(m->mutex());
    f(m);
  }

  // Recurse into children.
#if 0
  for (node_ptr const &ch : children)
    ch->forMeshes(f);
#endif
}

void Node::mapMeshes(std::function<mesh_ptr(mesh_ptr)> f) {
  for (mesh_ptr &m : meshes) {
    // std::scoped_lock lock(m->mutex());
    m = f(m);
  }
}

void Node::collectStaged(std::vector<node_ptr> *nodes) {
  forEach(
      [nodes](node_ptr n) {
        if (n->isStaged() && n->isVisual())
          nodes->push_back(n);
      },
      true);
}