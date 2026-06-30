#include "Triangle.hpp"
#include "SceneObject.hpp"

Triangle::Triangle(SceneObject* owner, const Vector3DFloat& point1, const Vector3DFloat& point2, const Vector3DFloat& point3 ) : Primitive(owner)
{
	SceneObject* onwer_scene_object = this->GetOwnerSceneObject();
	Matrix4D world_matrix = onwer_scene_object->GetObjectToWorld();

	this->vertices[0] = Vector3DFloat(world_matrix * Vector4DFloat(point1, 1.0f));
	this->vertices[1] = Vector3DFloat(world_matrix * Vector4DFloat(point2, 1.0f));
	this->vertices[2] = Vector3DFloat(world_matrix * Vector4DFloat(point3, 1.0f));

	Vector3DFloat edge1 = this->vertices[1] - this->vertices[0];
	Vector3DFloat edge2 = this->vertices[2] - this->vertices[0];
	this->normal = glm::normalize(glm::cross(edge1, edge2));
}

/**
 * ============================================================
 *  Ray–Triangle Intersection — starting from zero
 * ============================================================
 *
 *  Picture this:
 *    You have a laser pointer. Its tip is at point O.
 *    You press the button and the beam shoots out in direction D.
 *    Floating in the air is a triangular piece of glass with
 *    vertices P0, P1, P2.
 *    We want to know:
 *      1. Does the beam hit the glass?
 *      2. If it hits, where exactly?
 *      3. How far is that point from the tip?
 *
 * ============================================================
 *  Step 1 – Write the “glass point” and the “ray point”
 * ============================================================
 *
 *  Any point on the flat triangle can be described with
 *  barycentric coordinates (b0, b1, b2):
 *      P = b0*P0 + b1*P1 + b2*P2
 *  and the weights must satisfy:
 *      b0 + b1 + b2 = 1       (covers the whole plane of the triangle)
 *      b0, b1, b2 >= 0        (the point stays inside the triangle)
 *
 *  Any point along the ray can be described with a distance t:
 *      P = O + t*D
 *      t is exactly what you meant by
 *      “How many times larger should the grid be?”
 *      — you scale the direction vector D by t to reach that point.
 *
 * ============================================================
 *  Step 2 – If the beam hits, those two P's are the same point
 * ============================================================
 *
 *  Hitting means:
 *      b0*P0 + b1*P1 + b2*P2 = O + t*D
 *
 *  Because b0 + b1 + b2 = 1, we replace b0 with 1 - b1 - b2:
 *      (1 - b1 - b2)*P0 + b1*P1 + b2*P2 = O + t*D
 *
 * ============================================================
 *  Step 3 – Move unknowns to the left, knowns to the right
 * ============================================================
 *
 *  Expand:
 *      P0 - b1*P0 - b2*P0 + b1*P1 + b2*P2 = O + t*D
 *  Put terms with b1, b2, t on the left, constants on the right:
 *      b1*(P1 - P0) + b2*(P2 - P0) - t*D = O - P0
 *
 * ============================================================
 *  Step 4 – Give the vectors simple names
 * ============================================================
 *
 *  Let:
 *      E1 = P1 - P0        (one edge of the triangle)
 *      E2 = P2 - P0        (the other edge)
 *      S  = O - P0         (from the triangle's corner to the ray origin)
 *
 *  The equation becomes:
 *      b1*E1 + b2*E2 - t*D = S
 *
 * ============================================================
 *  Step 5 – Write it as a matrix, and see why it's solvable
 * ============================================================
 *
 *  This vector equation can be packed into a matrix times a vector:
 *
 *				   ┌    ┐
 *				   | b1 |
 *	| E1 E2 -D | × | b2 | = S
 *				   | t  |
 *				   └    ┘
 *
 *  Here | E1 E2 -D | means the 3×3 matrix whose columns are the
 *  vectors E1, E2, and -D.  (We use vertical bars just like we
 *  will later use for determinants – they look the same because
 *  the matrix is built from those three columns.)
 *
 *  【Key insight】 Although we wrote only one line of matrix
 *  equation, every vector E1, E2, D, S has three components (x,y,z).
 *  So this single line actually hides three ordinary equations:
 *       E1.x * b1 + E2.x * b2 - D.x * t = S.x
 *       E1.y * b1 + E2.y * b2 - D.y * t = S.y
 *       E1.z * b1 + E2.z * b2 - D.z * t = S.z
 *  Three equations, three unknowns (b1, b2, t) – exactly solvable!
 *
 * ============================================================
 *  Step 6 – Apply Cramer's Rule: each unknown is a “volume ratio”
 * ============================================================
 *
 *  Let the columns of the coefficient matrix be A1=E1, A2=E2, A3=-D.
 *  Cramer's Rule tells us that the unknowns are ratios of
 *  determinants (“volumes” of the parallelepipeds spanned by the vectors):
 *
 *           | S  E2  -D |          | E1  S  -D |          | E1  E2  S |
 *    b1 = --------------- , b2 = --------------- , t = ---------------
 *           | E1  E2  -D |        | E1  E2  -D |        | E1  E2  -D |
 *
 *  The bars | ... | here are the determinant of the 3×3 matrix
 *  formed by the three vectors written inside.
 *
 * ============================================================
 *  Step 7 – Why do we need the scalar triple product?
 * ============================================================
 *
 *  Computers can compute cross products and dot products much faster
 *  than generic determinants, and the geometry is much clearer.
 *
 *  The scalar triple product (also called the box product) is
 *  exactly the determinant of three vectors:
 *      | a  b  c | = (a × b) · c
 *  That is, “cross the first two vectors, then dot with the third”.
 *  This value gives the signed volume of the skewed box they span.
 *
 *  It has a wonderful cyclic property:
 *      (a × b) · c = (b × c) · a = (c × a) · b
 *  You can rotate the three vectors as long as you keep the
 *  cross‑and‑dot order — the value stays exactly the same.
 *  This gives us huge freedom to simplify every determinant!
 *
 *  Now we simplify each “volume” using this property.
 *
 *  --- The denominator (shared by all) ---
 *  det = | E1  E2  -D | = (E1 × E2) · (-D)
 *  We want to reshape it into something containing D × E2.
 *  Using the cyclic property:
 *      (E1 × E2) · (-D) = (E2 × (-D)) · E1
 *                       = E1 · (D × E2)
 *                       = (D × E2) · E1
 *  So the denominator becomes:
 *      det = (D × E2) · E1
 *  We'll call this det — it is the heart of the whole calculation.
 *
 *  --- Numerator for b1 ---
 *  | S  E2  -D | = (S × E2) · (-D)
 *                = S · (D × E2)
 *                = (D × E2) · S
 *  Thus  b1 = ( (D × E2) · S ) ÷ det
 *
 *  --- Numerator for b2 ---
 *  | E1  S  -D | = (E1 × S) · (-D)
 *                = (S × E1) · D        (cyclic shift)
 *  Thus  b2 = ( (S × E1) · D ) ÷ det
 *
 *  --- Numerator for t ---
 *  | E1  E2  S | = (E1 × E2) · S
 *                = (S × E1) · E2       (cyclic shift)
 *  Thus  t  = ( (S × E1) · E2 ) ÷ det
 *
 *  Notice what just happened: every single numerator and the
 *  denominator now use only two cross products:
 *      D × E2   and   S × E1
 *  All the scary 3×3 determinants turned into easy cross‑and‑dot
 *  operations. That's exactly WHY we need the scalar triple product!
 *
 * ============================================================
 *  Step 8 – Introduce helper vectors, get the final formulas
 * ============================================================
 *
 *  For convenience, define two helper vectors:
 *      S1 = D × E2      (perpendicular to both ray direction and edge2)
 *      S2 = S × E1      (perpendicular to both ray‑to‑vertex offset and edge1)
 *
 *  Substituting them into the results above:
 *      b1 = ( S1 · S  ) ÷ ( S1 · E1 )
 *      b2 = ( S2 · D  ) ÷ ( S1 · E1 )
 *      t  = ( S2 · E2 ) ÷ ( S1 · E1 )
 *
 *  These are exactly the formulas we use in the code!
 *  In the code:
 *      S1  is  perpendicular_to_ray_and_edge2
 *      S2  is  perpendicular_to_vertical_and_edge1
 *      det is  S1 · E1
 *
 * ============================================================
 *  Step 9 – Check if the intersection is actually valid
 * ============================================================
 *
 *  After we have b1, b2, t, we must pass these checks:
 *
 *   1. Is the denominator det zero (or extremely close to zero)?
 *      → If yes, the ray is parallel to the triangle plane — no hit.
 *
 *   2. Is t within the allowed range? (ray.min_limited ~ ray.max_limited)
 *      → This keeps only intersections in front of the ray and within
 *        the maximum distance we care about.
 *
 *   3. Are all barycentric coordinates non‑negative?
 *      b1 >= 0
 *      b2 >= 0
 *      b0 = 1 - b1 - b2 >= 0
 *      → Only when all three are ≥ 0 does the hit truly lie inside the triangle.
 *
 *  All checks passed? Congratulations — we hit the glass!
 *      Hit position = O + t · D
 *      Normal       = the pre‑computed triangle normal
 *      Distance     = t
 *
 * ============================================================
 *  Summary – what did we just do?
 * ============================================================
 *
 *  Starting from the simplest idea — “the ray point and the
 *  triangle point must coincide” — we used barycentric coordinates
 *  to eliminate b0, giving a vector equation with three unknowns.
 *  We noticed that this vector equation is actually three scalar
 *  equations, exactly enough to solve for b1, b2, t.
 *
 *  Cramer's Rule gave us the solution as ratios of 3×3 determinants.
 *  That's where we called in the scalar triple product (box product).
 *  Because of its cyclic property, every determinant collapsed into
 *  a cross product and a dot product.  All formulas ended up sharing
 *  the same denominator and using only two helper vectors:
 *      S1 = D × E2
 *      S2 = S × E1
 *
 *  The result is fast, geometrically clear, and free of any heavy
 *  matrix solving.  This is the full baby‑step derivation that
 *  leaves nothing hidden.
 * ============================================================
 */
bool Triangle::Intersect( Ray ray, Intersection& intersect ) const
{
	Vector3DFloat vertex0 = this->vertices[ 0 ];
	Vector3DFloat vertex1 = this->vertices[ 1 ];
	Vector3DFloat vertex2 = this->vertices[ 2 ];

	Vector3DFloat edge1 = vertex1 - vertex0;
	Vector3DFloat edge2 = vertex2 - vertex0;
	Vector3DFloat ray_origin_to_point1 = ray.origin - vertex0;

	Vector3DFloat perpendicular_to_ray_and_edge2 = cross( ray.line, edge2 );
	Vector3DFloat perpendicular_to_vertical_and_edge1 = cross( ray_origin_to_point1, edge1 );

	float determinant = dot( perpendicular_to_ray_and_edge2, edge1 );
	//Ray is parallel to the triangle plane
	if ( std::abs( determinant ) < std::numeric_limits<float>::epsilon() )
		return false;

	float inverse_determinant = 1.0f / determinant;

	float barycentric_1 = dot( perpendicular_to_ray_and_edge2, ray_origin_to_point1 ) * inverse_determinant;
	float barycentric_2 = dot( perpendicular_to_vertical_and_edge1, ray.line ) * inverse_determinant;

	//How many times larger should the grid be?
	float value = dot( perpendicular_to_vertical_and_edge1, edge2 ) * inverse_determinant;

	//Intersection position is in the world space ray range?
	if ( value < ray.min_limited || value > ray.max_limited )
		return false;

	//Third barycentric coordinate: barycentric_0 = 1 - barycentric_1 - barycentric_2
	float barycentric_0 = 1.0f - barycentric_1 - barycentric_2;

	//Is the world space ray intersection position inside the triangle?
	if ( barycentric_0 < 0.0f || barycentric_1 < 0.0f || barycentric_2 < 0.0f )
		return false;

	intersect.position = ray.origin + value * ray.line;
	intersect.normal = this->normal;
	intersect.distance = value;
	return true;
}