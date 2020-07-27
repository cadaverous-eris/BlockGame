#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/exponential.hpp>
#include <glm/gtx/compatibility.hpp>


// DEBUGGING
#include <iostream>
#include <glm/gtx/io.hpp>

namespace eng {

	class Camera {
	private:
		glm::vec3 position, prevPosition;
		glm::vec3 look;
		inline static const glm::vec3 up = { 0, 1, 0 };
		glm::vec3 forward, right;
		// yaw and pitch in degrees
		float yaw = -90.0f, pitch = 0.0f;

	public:
		Camera() {
			recalculateLook();
		}

		const glm::vec3& getPosition() const {
			return position;
		}
		const glm::vec3& getLookVec() const {
			return look;
		}
		void setPosition(const glm::vec3& pos) {
			prevPosition = position = pos;
		}
		void setPosition(float x, float y, float z) {
			position.x = x;
			position.y = y;
			position.z = z;
			prevPosition = position;
		}
		void setRotation(float yaw, float pitch) {
			this->yaw = glm::mod(yaw, 360.0f);
			this->pitch = glm::clamp(pitch, -85.0f, 85.0f);
			recalculateLook();
		}
		void setLookVec(glm::vec3 lookVec) {
			look = glm::normalize(lookVec);
			recalculateRotation();
		}
		void lookAt(glm::vec3 target) {
			look = normalize(target - position);
			recalculateRotation();
		}
		void lookAt(float x, float y, float z) {
			 lookAt(glm::vec3(x, y, z));
		}

		void translate(const glm::vec3& d) {
			position += d;
		}
		void translate(float dx, float dy, float dz) {
			position.x += dx;
			position.y += dy;
			position.z += dz;
		}
		void rotate(float dYaw, float dPitch) {
			yaw = glm::mod(yaw + dYaw, 360.0f);
			pitch = glm::clamp(pitch - dPitch, -89.5f, 89.5f);
			recalculateLook();
		}

		// moves the camera relative to it's orientation
		void move(const glm::vec3& d) {
			move(d.x, d.y, d.z);
		}
		void move(float dx, float dy, float dz) {
			// vertical movement
			position.y += dy;
			// horizontal movement
			position += (forward * dz);
			position += (right * dx);
		}

		glm::mat4 getViewMatrix(float partialTicks) const {
			glm::vec3 lerpedPos = glm::lerp(prevPosition, position, partialTicks);
			return glm::lookAt(lerpedPos, lerpedPos + look, up);
			//glm::vec3 slerpedLook(look);
			//if (prevLook != look) slerpedLook = glm::slerp(prevLook, look, partialTicks);
			//if (glm::isnan(slerpedLook.x)) std::cout << ((prevLook == look) ? "equal" : "not equal") << std::endl;
			//return glm::lookAt(lerpedPos, lerpedPos + slerpedLook, up);
		}

		void update() {
			prevPosition = position;
		}

		void log() const {
			std::cout << "Look: " << look << std::endl;
			std::cout << "Yaw: " << yaw << ", Pitch: " << pitch << std::endl;
			std::cout << "Pos: " << position << std::endl;
			std::cout << std::endl;
		}

	private:
		// recalculate yaw and pitch from look vector
		void recalculateRotation() {
			float yawRadians = glm::atan(look.z, look.x);
			float pitchRadians = glm::atan(look.y, glm::sqrt((look.x * look.x) + (look.z * look.z)));


			yaw = glm::mod(glm::degrees(yawRadians), 360.0f);
			pitch = glm::clamp(glm::degrees(pitchRadians), -89.9f, 89.9f);

			// recalculate forward and right vectors
			forward = glm::normalize(glm::vec3(look.x, 0.0f, look.z));
			right = glm::normalize(glm::cross(forward, up));
		}
		// recalculate look vector from yaw and pitch
		void recalculateLook() {
			float yawRadians = glm::radians(yaw);
			float pitchRadians = glm::radians(pitch);

			look.x = cos(yawRadians) * cos(pitchRadians);
			look.y = sin(pitchRadians);
			look.z = sin(yawRadians) * cos(pitchRadians);

			//const auto l = (look.x * look.x) + (look.y * look.y) + (look.z * look.z);
			//if (l != 1.0f) std::cout << l << "\n";

			// recalculate forward and right vectors
			forward = glm::normalize(glm::vec3(look.x, 0.0f, look.z));
			right = glm::normalize(glm::cross(forward, up));
		}

	};

}