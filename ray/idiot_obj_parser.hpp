#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "math.hpp"

namespace Import {

using namespace Math;

class OBJImporter {
public:
	static std::vector<Triangle> LoadFile(const std::string& path) {
		std::vector<Triangle> result;
		std::vector<v3> positions;

		std::ifstream file(path);
		std::string line;
		while (std::getline(file, line)) {
			std::stringstream s(line);

			char c;
			s >> c;
			switch (c) {
				case 'v': {
					v3 position;
					s >> position.x >> position.y >> position.z;
					positions.push_back(position);
					//std::cout << position << std::endl;
					break;
				}

				case 'f': {
					int i0;
					int i1;
					int i2;

					s >> i0 >> i1 >> i2;

					result.push_back(
						Triangle(
							positions[i0 - 1], positions[i1 - 1], positions[i2 - 1]
						)
					);
					break;
				}

				case '#': {
					break;
				}
			}
		}
		file.close();
		return result;
	}
};

}