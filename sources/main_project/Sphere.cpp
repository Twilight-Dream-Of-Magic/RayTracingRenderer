#include "Sphere.hpp"
#include "SceneObject.hpp"

Sphere::Sphere( SceneObject* owner, float radius ) : Primitive(owner), size_radius(radius)
{

}

/*
 * ============================================================
 *  Baby‑level Sphere Intersection — let the computer “see” algebraically
 * ============================================================
 *
 *  We have a sphere sitting somewhere in world space.
 *  The sphere has a center and a radius.
 *  We also have a ray that starts at some point and shoots off
 *  in some direction. We want to know:
 *    Does the ray hit the sphere?
 *    If yes, where, and what's the surface normal there?
 *
 *  The tricky part: computers don't have eyes. They can't just
 *  look at a 3D scene and “see” whether a line touches a ball.
 *  They only understand numbers and equations.
 *  So our job is to translate the geometric question —
 *  “does this line poke through that ball?” — into something
 *  a computer can solve with pure arithmetic.
 *
 * -----------------------------------------------------------
 *  Step 1 – Make the sphere the simplest possible shape
 * -----------------------------------------------------------
 *
 *  In general, the sphere is at an arbitrary position in world space.
 *  A sphere equation in that form looks messy:
 *      |P - C|² = R²
 *  (C is the sphere center, R is its radius.)
 *  That's already a little bit of algebra, but the ray is also
 *  in world space, so we'd have to mix everything together.
 *
 *  Can we make our lives easier? YES!
 *  We can move the whole problem so that the sphere sits perfectly
 *  at the origin (0,0,0) with radius R. How? By applying the
 *  inverse of the sphere's world matrix to the ray.
 *  Now the sphere is just a perfect unit sphere at the center
 *  of the universe. Beautiful and simple.
 *
 *  Transforming the ray into the sphere's local space is the
 *  first thing we do:
 *      transformed_ray = world_from_object * ray
 *  Now everything happens in a coordinate system where the sphere
 *  equation is as clean as it gets:
 *      x² + y² + z² = R²
 *  or, in vector form:
 *      dot(P, P) = R²
 *  (Because the center is at the origin, we just measure distance
 *   directly from the origin.)
 *
 * -----------------------------------------------------------
 *  Step 2 – Turn the ray into an algebraic equation
 * -----------------------------------------------------------
 *
 *  A ray is a straight line. Any point along it can be written as:
 *      P(t) = O + t * D
 *  where:
 *      O = ray origin (in local space now)
 *      D = ray direction (a vector, not necessarily unit length)
 *      t = how far we walk from O along D (the “grid scaling” factor)
 *
 *  The computer doesn't understand “walk along this line until
 *  you bump into something”. It does understand:
 *      “Is there a number t such that plugging P(t) into the
 *       sphere equation makes it true?”
 *
 * -----------------------------------------------------------
 *  Step 3 – Substitute and get a quadratic equation
 * -----------------------------------------------------------
 *
 *  We demand that P(t) lies exactly on the sphere:
 *      dot( P(t), P(t) ) = R²
 *  Substitute P(t) = O + t*D:
 *      dot( O + t*D, O + t*D ) = R²
 *
 *  Expand the dot product (just like (a+b)² = a² + 2ab + b²):
 *      dot(O,O) + 2*t*dot(O,D) + t²*dot(D,D) = R²
 *
 *  Move R² to the left:
 *      t²*dot(D,D) + 2*t*dot(O,D) + dot(O,O) - R² = 0
 *
 *  This is a quadratic equation in t:
 *      A*t² + B*t + C = 0
 *  where:
 *      A = dot(D, D)       (always ≥ 0, zero only if the ray is a point)
 *      B = 2 * dot(O, D)
 *      C = dot(O, O) - R²
 *
 *  Look at that! We've turned a geometric “does it hit?” question
 *  into something a computer loves: a simple formula with numbers
 *  A, B, C that it can compute in a few clock cycles.
 *
 * -----------------------------------------------------------
 *  Step 4 – Oh! We know how to solve this! The quadratic formula!
 * -----------------------------------------------------------
 *
 *  Any quadratic equation A*t² + B*t + C = 0 can be solved with
 *  the quadratic formula:
 *      t = [ -B ± sqrt( B² - 4*A*C ) ] / (2*A)
 *
 *  The part inside the square root, Δ = B² - 4*A*C, is called the
 *  discriminant. It tells us how many real solutions exist:
 *    Δ < 0  →  no real solution (ray misses the sphere completely)
 *    Δ = 0  →  one solution (ray just grazes the surface)
 *    Δ > 0  →  two solutions (ray enters and exits the sphere)
 *
 *  The computer just computes Δ, checks its sign, and if it's
 *  non‑negative, it finds the actual t values.
 *  No 3D “vision” needed — just basic arithmetic.
 *
 * -----------------------------------------------------------
 *  Step 5 – Pick the right t (which “hit” do we care about?)
 * -----------------------------------------------------------
 *
 *  If the discriminant is positive, we get two t values:
 *      t1 = (-B - sqrt(Δ)) / (2*A)
 *      t2 = (-B + sqrt(Δ)) / (2*A)
 *  These are the distances along the ray where it pierces the sphere.
 *  The smaller one (t1) is the entry point, the larger one (t2) is
 *  the exit point.
 *
 *  We want the closest intersection that is still in front of the
 *  ray origin and within our allowed range (min_limited to max_limited).
 *  So we check:
 *    - Is t2 before min_limited? If yes, the whole sphere is behind us.
 *    - Is t1 after max_limited? If yes, the sphere is too far away.
 *    - Otherwise, try t1 first. If t1 is too close (less than min_limited),
 *      then try t2. If t2 is also out of range, no valid hit.
 *
 *  This is exactly the “segment filter” you see in the code.
 *
 * -----------------------------------------------------------
 *  Step 6 – Compute the hit information in local space
 * -----------------------------------------------------------
 *
 *  Once we have a valid t (we call it result_distance), the hit
 *  position in the sphere's local space is simply:
 *      local_hit = O + t * D
 *
 *  Because the sphere is centered at the origin, the surface normal
 *  at any point is just the direction from the center to that point.
 *  The center is (0,0,0), so the normal vector is the point itself!
 *  Normalize it, and we have a perfect unit normal:
 *      local_normal = normalize( local_hit )
 *
 * -----------------------------------------------------------
 *  Step 7 – Transform back to world space
 * -----------------------------------------------------------
 *
 *  We solved everything in the sphere's local coordinate system,
 *  but the rest of the program expects answers in world space.
 *  So we transform the hit position and normal back:
 *    - Position is a point, so we use the full matrix with translation:
 *        world_hit = object_to_world * local_hit
 *    - Normal is a direction, so we use the matrix without translation:
 *        world_normal = normalize( object_to_world * local_normal )
 *
 *  And that's it! We fill in the Intersection struct with the
 *  world‑space position, normal, and the distance (we can keep
 *  the distance as t because it's the same in both spaces if the
 *  transformation is rigid; for scaling transforms a more careful
 *  approach would be needed, but for our typical sphere it's fine).
 *
 * ============================================================
 *  Summary — how did we make the computer “see”?
 * ============================================================
 *
 *  1. Transform the problem to make the sphere a perfect origin ball.
 *  2. Write the ray as a parametric line.
 *  3. Plug the line into the sphere equation → quadratic in t.
 *  4. Recognize the quadratic and apply the formula we learned in school.
 *  5. Use the discriminant to decide if there's a real intersection.
 *  6. Pick the correct t that falls within our allowed segment.
 *  7. Read off the position and normal from the simple geometry,
 *     then transform them back to world space.
 *
 *  The computer never “looked” at the scene. It just solved an
 *  equation. And that's the magic of math in computer graphics.
 * ============================================================
 */
bool Sphere::Intersect( Ray ray, Intersection& intersect ) const
{
	SceneObject* onwer_scene_object = this->GetOwnerSceneObject();

	//Ray world glboal space to sphere local space
	Ray transformed_ray = onwer_scene_object->GetObjectFromWorld() * ray;

	//Solve for the parameters of the ray equation.
	float A = glm::dot(transformed_ray.line, transformed_ray.line);
	float B = 2.0f * glm::dot(transformed_ray.line, transformed_ray.origin);
	float C = glm::dot(transformed_ray.origin, transformed_ray.origin) - this->size_radius * this->size_radius;

	//For A,B,C parameters
	//Since it's a sphere, we use the quadratic formula to find the roots.
	float delta = B * B - 4.0f * A * C;
	if(delta < 0.0f)
		return false;

	//How many times larger should the grid be?
	float delta_sqrted = std::sqrt(delta);
	float value1 = (-B - delta_sqrted) / (2.0f * A);
	float value2 = (-B + delta_sqrted) / (2.0f * A);

	//Segment filter

	if(value2 < transformed_ray.min_limited)
		return false;

	if(value1 > transformed_ray.max_limited)
		return false;
	
	//Try point 1
	float result_distance = value1;
	if(result_distance < transformed_ray.min_limited)
	{
		//Try point 2
		result_distance = value2;
		if(result_distance > transformed_ray.max_limited)
			return false;
	}

	Vector3DFloat local_intersect_position = transformed_ray.origin + result_distance * transformed_ray.line;
	Vector3DFloat local_normal = glm::normalize(local_intersect_position);

	//World intersect position
	intersect.position = Vector3DFloat(onwer_scene_object->GetObjectToWorld() * Vector4DFloat(local_intersect_position, 1.0f));
	//World intersect normal
	intersect.normal = glm::normalize(Vector3DFloat(onwer_scene_object->GetObjectToWorld() * Vector4DFloat(local_normal, 0.0f)));
	intersect.distance = result_distance;

	return true;
}




