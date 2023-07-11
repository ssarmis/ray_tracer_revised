#pragma once

#include <vector>
#include <map>

#include "object.hpp"
#include "math.hpp"

class Scene;
class Ray;

struct BVHNode {
	bool isLeaf;
	Math::v3 position;
	Math::v3 size;
	std::vector<Object*> objectContents;
	std::map<Object*, std::vector<Math::Triangle*>> triangleContents;
	BVHNode* parent;
	BVHNode* children[8];

	BVHNode() {};
	BVHNode(Math::v3 p, Math::v3 s, BVHNode* pa) : position(p), size(s), children{}, parent(pa), isLeaf(false){}
};

class BVH {
private:
	Scene* mScene; // SPONGE

	int mDepth;

	BVHNode* GenerateNode(std::vector<Object*>& objects, BVHNode* parent, Math::v3 position, int currentDepth);
public:
	BVHNode* mRoot;

	BVH() :mDepth(0), mRoot(nullptr) {};

	void SelectHitLeafsFromRay(Ray& ray, BVHNode* node, std::vector<BVHNode*>& result);
	static BVH BuildFromScene(Scene* scene, int depth);
};