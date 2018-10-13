#pragma once

#include <glm/glm.hpp>

class Water
{



public:
	static float heightAt(const glm::vec3& pos, float time);

	static glm::vec3 velocityAt(const glm::vec3& pos, float time);
};