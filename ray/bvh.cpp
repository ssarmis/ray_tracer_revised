#include "bvh.hpp"
#include "scene.hpp"
#include "ray.hpp"

using namespace Math;
#define RENDER_DEBUG_SPHERES

v3 defaultStartingPosition(0, 0, 3);
v3 defaultStartingSize(20, 20, 20);
int numberOfLeafs = 0;
r32 offsetDivider = 4.0f;
BVHNode* BVH::GenerateNode(std::vector<Object*>& objects, BVHNode* parent, v3 position, int currentDepth) {
	if (currentDepth > mDepth) {
		return nullptr;
	}

	bool isLeaf = currentDepth == mDepth;
	BVHNode* node = new BVHNode(position, parent->size / 2.0f, parent);
	node->isLeaf = isLeaf;

	bool intersected = false;// SPONGE
	std::vector<Triangle*> triangles;
	for (auto& obj : objects) {
		if (obj->IntersectsBox(node->position, node->size)) {
			if (obj->mIsMesh) {
				MeshObject* s = dynamic_cast<MeshObject*>(obj);
				triangles = s->IntersectedTriangles(node->position, node->size);
				if (triangles.size()) {
					intersected = true;
					if (isLeaf) {
						auto* m = &(node->triangleContents[obj]);
						m->insert(m->end(), triangles.begin(), triangles.end());
					}
				}
			} else {
				if (isLeaf) {
					node->objectContents.push_back(obj);
				}
				intersected = true;
			}
			if (!isLeaf) {
				break;
			}
		}
	}
#ifdef RENDER_DEBUG_SPHERES
	//if (node->objectContents.size()) {
	//}
	if (node->triangleContents.size()) {
		Sphere* s1 = new Sphere(node->position, 0.1f);
		s1->SetColor(v3(0, 0, 1));
		s1->SetRoughness(1);
		//s1->SetEmission(v3(1, 0, 0));
		mScene->AddObject(s1);
	} else {
		r32 p = ((r32)currentDepth / (r32)mDepth) * 0.5f;
		Sphere* s1 = new Sphere(node->position, 0.1f);
		s1->SetColor(v3(1, 0, 0) * p);
		s1->SetRoughness(1);
		//s1->SetEmission(v3(1, 0, 0));
		mScene->AddObject(s1);
	}
#endif

	if (intersected && isLeaf) {
		++numberOfLeafs;
	}

	if (intersected && !isLeaf) {
		v3 offset[8] = {
			v3(-1, 1, 1),
			v3(-1, -1, 1),
			v3(1, 1, 1),
			v3(1, -1, 1),

			v3(-1, 1, -1),
			v3(-1, -1, -1),
			v3(1, 1, -1),
			v3(1, -1, -1)
		};
		for (int i = 0; i < 8; ++i) {
			node->children[i] = GenerateNode(objects, node, 
				node->position + v3::Hadamard(node->size / offsetDivider, offset[i]),
				currentDepth + 1);
		}
	}

	return node;
}

BVH BVH::BuildFromScene(Scene* scene, int depth) {
	BVH result;
	result.mScene = scene;
	result.mDepth = depth;
	// TODO Just call generate node
	result.mRoot = new BVHNode(defaultStartingPosition, defaultStartingSize, nullptr);
	v3 offset[8] = {
		v3(-1, 1, 1),
		v3(-1, -1, 1),
		v3(1, 1, 1),
		v3(1, -1, 1),

		v3(-1, 1, -1),
		v3(-1, -1, -1),
		v3(1, 1, -1),
		v3(1, -1, -1)
	};
#ifdef RENDER_DEBUG_SPHERES
	Sphere* s1 = new Sphere(result.mRoot->position, 0.1f);
	s1->SetColor(v3(0, 1, 0));
	s1->SetRoughness(1);
	//s1->SetEmission(v3(1, 0, 0));
	result.mScene->AddObject(s1);
#endif
	for (int i = 0; i < 8; ++i) {
		result.mRoot->children[i] = result.GenerateNode(scene->mObjects, result.mRoot, 
			result.mRoot->position + v3::Hadamard(result.mRoot->size / offsetDivider, offset[i]),
			0);
	}

	std::cout << "Number of leafs: " << numberOfLeafs << std::endl;

	return result;
}

void BVH::SelectHitLeafsFromRay(Ray& ray, BVHNode* node, std::vector<BVHNode*>& result) {
	BVHNode* currentNode = node;
	if (!currentNode) {
		return;
	}

	BoundingBox b(currentNode->position - (currentNode->size / 2.0),
		currentNode->position + (currentNode->size / 2.0));

	if (Intersections::RayAABB(b, ray)) {
		if (currentNode->isLeaf) {
			if (currentNode->objectContents.size() || currentNode->triangleContents.size()) {
				result.push_back(currentNode);
			}
		} else {
			for (int i = 0; i < 8; ++i) {
				SelectHitLeafsFromRay(ray, currentNode->children[i], result);
			}
		}
	}
}
