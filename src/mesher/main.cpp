//
// Created by kr2 on 12/25/21.
//

#include "../common.hpp"
#include "VDBMesher.hpp"
#include "common.hpp"

#include <fstream>

std::vector<vec3> data, mesh;
std::vector<uint> indices;

bool
isFileExist(const char *fileName)
{
  std::ifstream file(fileName);
  return file.good();
}

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    fprintf(stderr, "Usage: %s filename", argv[0]);
    exit(EXIT_FAILURE);
  }

  std::string filename = argv[1];
  if (!isFileExist(filename.c_str())) {
    std::cerr << "File do not exists" << std::endl;
    exit(EXIT_FAILURE);
  }

  FILE *f = fopen(filename.c_str(), "r");
  float scale = 1.0f;
  if (argc == 3) { scale = atof(argv[2]); }

  int nSize;
  fscanf(f, "%d", &nSize);
  data.resize(nSize);
  for (int i = 0; i < nSize; ++i) {
    fscanf(f, "\np %f %f %f", &data[i].x, &data[i].y, &data[i].z);
    data[i].x *= scale;
    data[i].y *= scale;
    data[i].z *= scale;
  }

  std::vector<openvdb::Vec3s> points;
  std::vector<openvdb::Vec3I> triangles;
  std::vector<openvdb::Vec4I> quads;

  particleToMesh(data, points, triangles, quads);
  std::cout << "n points: " << points.size() << std::endl;
  std::cout << "n triangles: " << triangles.size() << std::endl;
  std::cout << "n quads: " << quads.size() << std::endl;

  static auto conv_to_vec3 = [&](openvdb::Vec3s item) -> vec3 {
    return { item.x(), item.y(), item.z() };
  };

  std::for_each(triangles.begin(), triangles.end(), [&](const auto &i) {
    indices.push_back(i.x());
    indices.push_back(i.y());
    indices.push_back(i.z());
  });

  std::for_each(quads.begin(), quads.end(), [&](const auto &i) {
    // Select the first three points
    uint _a, _b, _c, _o;
    const uint *points_index = i.asPointer();
    _a = points_index[0], _b = points_index[1];
    _c = points_index[2], _o = points_index[3];

    for (uint a = 1; a <= 3; ++a) {
      for (uint b = a + 1; b <= 3; ++b) {
        // select the none-select point
        if (a == b) continue;
        const uint c = 6 - a - b;
        _a = points_index[a], _b = points_index[b], _c = points_index[c],
        _o = points_index[0];
        const vec3 o2c = conv_to_vec3(points[_c] - points[_o]),
                   o2a = conv_to_vec3(points[_a] - points[_o]),
                   o2b = conv_to_vec3(points[_b] - points[_o]);
        const vec3 cro_a = glm::cross(o2c, o2a), cro_b = glm::cross(o2c, o2b);
        if (glm::dot(cro_a, cro_b) < 0) goto _set;
      }
    }

    // assert(false);
  _set: // a, b, c, o is set
    indices.push_back(_o);
    indices.push_back(_a);
    indices.push_back(_b);

    indices.push_back(_c);
    indices.push_back(_a);
    indices.push_back(_b);
  });

  for (auto &point : points) {
    mesh.emplace_back(point.x(), point.y(), point.z());
  }

  std::ofstream of;
  of.open("fluid.obj");
  for (uint i = 0; i < mesh.size(); ++i) {
    of << "v " << mesh[i].x << " " << mesh[i].y << " " << mesh[i].z
       << std::endl;
  }

  assert(indices.size() % 3 == 0);
  for (uint i = 0; i < indices.size(); i += 3) {
    of << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " "
       << indices[i + 2] + 1 << std::endl;
  }

  of.close();

  return 0;
}